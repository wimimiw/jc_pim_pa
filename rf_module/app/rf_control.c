/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_control.c
 * @author	:mashuai
 * @version	:v1.0 
 * @date	:2012.4.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 	20130116:
	   V0x32:section of .constdata
	   V0x40:1)The bug in the data initialize.when modify the gloabe or static data
	    	 in the rf_control.c,the code in the head of firmware will change by 
			 the data initialize process.
			 2)Add the temperature compensate.
			 3)Modify that upgrade can only active in the same hardware version.
			 4)hotStandby modification.
	   V0x50:Optimize the upgrade function
	   V0x51:modify	the hotStandby rollback
	   V0x52:Optimize the Reset Temperature Max&Min process.
	   V0x60:11.0592MHz(HSE_VALUE) -> 8.000000MHz(HSI_VALUE)
		 V0x61:temperature perhapes will be 0xFF,so to screen this situation.
 ******************************************************************************/
#ifndef __RF_CONTROL_C__
#define __RF_CONTROL_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/                                          	     	                                                       
/* Private macro -------------------------------------------------------------*/
const U8 BootloaderV 	__attribute__((at(ADDR_BYTE_BOOTLOADERV))) = 0x11;
const U8 SoftwareV 		__attribute__((at(ADDR_BYTE_SOFTWAREV  ))) = 0x62;
const U8 HardwareV 		__attribute__((at(ADDR_BYTE_HARDWAREV  ))) = 0x50;
/* Private variables ---------------------------------------------------------*/
const STABLE_COMMAND tabInfo[] = {	
/****gain*****/
	{0,EE_AtteVal		,0,(U8*)&gAtteVal,sizeof(gAtteVal)				,0,0,NULL},//0x0001     //1�ֽڣ������趨ֵ
	{0,EE_GainOffset	,0,(U8*)&gGainOffset,sizeof(gGainOffset)		,0,0,NULL},//0x0002		//1�ֽڣ��������ֵ
	{0,EE_Att1			,0,(U8*)&gAtt1,sizeof(gAtt1)					,0,0,NULL},//0x0003		//1�ֽڣ�1#˥������1U5  	
/**DAchannel**/
	{0,EE_DA_CHANNEL_A	,0,(U8*)&gDAoutA,sizeof(gDAoutA)				,0,0,NULL},//0x0034		//2�ֽڣ���������޷�ֵ
	{0,EE_DA_CHANNEL_B	,0,(U8*)&gDAoutB,sizeof(gDAoutB)				,0,0,NULL},//0x0036		//2�ֽڣ���������޷�ֵ
	{0,EE_DA_CHANNEL_C	,0,(U8*)&gDAoutC,sizeof(gDAoutC)				,0,0,NULL},//0x0038		//2�ֽڣ���������޷�ֵ
	{0,EE_DA_CHANNEL_D	,0,(U8*)&gDAoutD,sizeof(gDAoutD)				,0,0,NULL},//0x003A		//2�ֽڣ���������޷�ֵ
/****PA******/	
	{0,EE_gPALim		,0,(U8*)&gPALim,sizeof(gPALim)					,0,0,NULL},//0x0020		//2�ֽڣ���������޷�ֵ
	{0,EE_gOutPwrVal	,0,(U8*)&gOutPwrVal,sizeof(gOutPwrVal)			,0,0,NULL},//0x0022		//2�ֽڣ�������ʶ���ֵ
	{0,EE_gOutPwrSlopeVal,0,(U8*)&gOutPwrSlopeVal,sizeof(gOutPwrSlopeVal),0,0,NULL},//0x0024		//2�ֽڣ���������趨ֵ����
	{0,EE_gRefPwrVal	,0,(U8*)&gRefPwrVal,sizeof(gRefPwrVal)			,0,0,NULL},//0x0026		//2�ֽڣ����书�ʶ���
	{0,EE_gRefPwrSlopeVal,0,(U8*)&gRefPwrSlopeVal,sizeof(gRefPwrSlopeVal),0,0,NULL},//0x0028		//2�ֽڣ����书��б��ֵ
	{0,EE_gInPwrVal		,0,(U8*)&gOutPwrShutVal,sizeof(gOutPwrShutVal)	,0,0,NULL},//0x002e		//1�ֽڣ�������ʹر�����
	{0,EE_gPALimNumer	,0,(U8*)&gPALimNumer,sizeof(gPALimNumer)		,0,0,NULL},//0x0075		//1�ֽڣ���������޷���������
	{0,EE_gPReadADNumer	,0,(U8*)&gPReadADNumer,sizeof(gPReadADNumer)	,0,0,NULL},//0x0076		//1�ֽڣ�������ʶ�ȡ��������	  
	{0,EE_gPequDenomi	,0,(U8*)&gPequDenomi,sizeof(gPequDenomi)		,0,0,NULL},//0x0077		//1�ֽڣ����ʲ�����ĸ	
/***frequency***/
	{0,EE_gPBmFreq		,0,(U8*)&gPBmFreq,sizeof(gPBmFreq)				,0,0,NULL},//0x0078		//4�ֽڣ���׼Ƶ��
	{0,EE_CenFreq		,0,(U8*)&gCenFreq,sizeof(gCenFreq)				,0,0,NULL},//0x0040		//4�ֽڣ�����Ƶ�� KHzΪ��λ
	{0,EE_BandWidth		,0,(U8*)&gBandWidth,sizeof(gBandWidth)			,0,0,NULL},//0x0044		//4�ֽڣ�����
	{0,EE_IFOffset		,0,(U8*)&gIFOffset,sizeof(gIFOffset)			,0,0,NULL},//0x0048		//2�ֽڣ���Ƶ����ֵ
	{0,EE_FreqStep		,0,(U8*)&gFreqStep,sizeof(gFreqStep)			,0,0,NULL},//0x004A		//2�ֽڣ�Ƶ�ʲ���ֵ
	{0,EE_IFFreq		,0,(U8*)&gIFFreq,sizeof(gIFFreq)				,0,0,NULL},//0x004C		//4�ֽڣ���ƵƵ��ֵ
	{0,EE_Freq			,0,(U8*)&gFreq,sizeof(gFreq)					,0,0,NULL},//0x0050		//4�ֽڣ�����Ƶ��
	{0,EE_RefFreq		,0,(U8*)&gRefFreq,sizeof(gRefFreq)				,0,0,NULL},//0x0054		//4�ֽڣ��ο�Ƶ��
/*****current*****/
	{0,EE_CurCo			,0,(U8*)&gCurCo,sizeof(gCurCo)					,0,0,NULL},//0x0080		//2�ֽڣ�����б��
	{0,EE_CurOffset		,0,(U8*)&gCurOffset,sizeof(gCurOffset)			,0,0,NULL},//0x0082		//2�ֽڣ�����������ֵ
	{0,EE_CurHlim		,0,(U8*)&gCurHlim,sizeof(gCurHlim)				,0,0,NULL},//0x0084		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
	{0,EE_CurLlim		,0,(U8*)&gCurLlim,sizeof(gCurLlim)				,0,0,NULL},//0x0086		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
/*****temperature********/		
	{0,EE_GainNumer		,0,(U8*)&gGainNumer,sizeof(gGainNumer)			,0,0,NULL},//0x00A2		//1�ֽڣ������²�ϵ������		
	{0,EE_GainDenomi	,0,(U8*)&gGainDenomi,sizeof(gGainDenomi)		,0,0,NULL},//0x00A3		//1�ֽڣ������²�ϵ����ĸ		
	{0,EE_PAProtectLim	,0,(U8*)&gPAProtecttLim,sizeof(gPAProtecttLim)	,0,0,NULL},//0x00A4		//1�ֽڣ����ű����¶�����		
	{0,EE_PAResetLim	,0,(U8*)&gPAResetLim,sizeof(gPAResetLim)		,0,0,NULL},//0x00A5		//1�ֽڣ����Żظ��¶�		
/*****module*******/
	{0,EE_RF_No			,0,(U8*)&gRF_No,sizeof(gRF_No)					,0,0,NULL},//0x00C0		//1�ֽڣ���Ƶģ����		
	{0,EE_PASW			,0,(U8*)&gPASW,sizeof(gPASW)					,0,0,NULL},//0x00C1     //1�ֽڣ����ſ���		
	{0,EE_RFSW			,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,NULL},//0x00C2     //1�ֽڣ���Ƶ����		
/****************other parameter********************/
	{0,0,0,(U8*)&gAtttempval,sizeof(gAtttempval)		,0,0,NULL},//�¶Ȳ���ֵ	
	{0,0,0,(U8*)&rfPramModified,sizeof(rfPramModified)	,0,0,NULL},//��Ƶ�����޸ı�־(0:��Ч��1����Ч)//4��
	{0,0,0,(U8*)&all_checksum,sizeof(all_checksum)		,0,0,NULL},//����У���
	{0,0,0,(U8*)&gDownFlag,sizeof(gDownFlag)			,0,0,NULL},//0x01FF		//1�ֽڣ�������־			
/*********apparatus parameter************/
	{0,0,0,(U8*)&PaAdjEquModulus,sizeof(PaAdjEquModulus),0,0,NULL},//0x00F0  	//1�ֽ�, ���ŵ�У׼���̵Ĳ���ϵ��
	{0,0,0,(U8*)&PaLastAdjDate_year,sizeof(PaLastAdjDate_year)	,0,0,NULL},//0x00F1	    //2�ֽڣ��ŵ�����У׼����(��)
	{0,0,0,(U8*)&PaLastAdjDate_month,sizeof(PaLastAdjDate_month),0,0,NULL},//0x00F3	    //1�ֽڣ��ŵ�����У׼����(��)
	{0,0,0,(U8*)&PaLastAdjDate_day,sizeof(PaLastAdjDate_day)	,0,0,NULL},//0x00F1	    //1�ֽڣ��ŵ�����У׼����(��)
	{0,0,0,(U8*)&PaUpLimit,sizeof(PaUpLimit)			,0,0,NULL},//0x00F5		//2�ֽڣ���������
	{0,0,0,(U8*)&AppModel,sizeof(AppModel)				,0,0,NULL},//0x00F7	    //1�ֽڣ��Ǳ��ͺ�            
/****************�������崥������********************/
	{0,0,0,(U8*)&__PlusSwitchState,sizeof(__PlusSwitchState),0,0,NULL},//���忪��״̬����
	{0,0,0,(U8*)&__PlusReqAddr,sizeof(__PlusReqAddr)		,0,0,NULL},//�����ַ
	{0,0,0,(U8*)&__PlusReqPower,sizeof(__PlusReqPower)		,0,0,NULL},//������ֵ ��λdB	
	{0,0,0,(U8*)&__PlusReqFreq,sizeof(__PlusReqFreq)		,0,0,NULL},//����Ƶ��ֵ ��λMHz			
};

/* Private function prototypes -----------------------------------------------*/
STABLE_COMMAND*GetTable(void)
{
	return (STABLE_COMMAND*)tabInfo;
}

int GetTableMebCnt(void)
{	
	return sizeof(tabInfo)/sizeof(STABLE_COMMAND);
}

void WritePLL(u32 freq,u8 power,BOOL enable)
{
	#define R0_INIT		0x00000000L
	//
	//DB31             ~                  DB4 DB3 DB2 DB1 DB0
	//0    0    0    0    0    0    0          0
	//0                ~                  0   0   0   0   0
	#define R1_INIT		0x08008001L
	//			  PRESCALER
	//DB31~DB28  DB27       DB26 DB25 DB24 DB23  ~ DB16 DB15 DB14 DB13 DB12 DB11 ~ DB4 DB3 DB2 DB1 DB0
	//0			  8			 	   			0     0		  8					  0    0      1
	//0   ~0     1			 0    0    0    0    ~   0   1    0    0    0    0    ~ 0	  0	  0   0   1 
	#define R2_INIT		0x18004EC2L
	//     NOISE-MODE[1:0] MUXOUT[2:0]	   REF-DOU RDIV2    					 DOUB-BUFF	CHARGE-PUMP[3:0]  LDF LDP PD-POLA PD  CP-THR  COUNT-RESET
	//DB31 DB30 DB29       DB28 DB27 DB26  DB25    DB24   DB23 ~ DB16 DB15 DB14 DB13       DB12 DB11 B10 DB9 DB8 DB7 DB6     DB5 DB4     DB3         DB2 DB1 DB0
	//1							8						  0	   0	   4						 E				  C						  2
	//0    0    0          1    1    0     0       0      0    ~ 0    0    1    0			0	 1	  1	  1	  0	  1	  1	      0	  0		  0           0   1   0
	#define R3_INIT		0x000004B3L
	//					CSR		  CLK-DIV[1:0]
	//DB31 ~ DB20 DB19 DB18 DB17 DB16 DB15    DB14 DB13 DB12 DB11 DB10 DB9 DB8 DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0  0  0	   0                   0                      4  				B				3
	//0    ~ 0    0    0    0    0    0       0    0    0    0	   1    0   0   1   0   1   1   0   0   1   1 	
	#define R4_INIT		0x00850404L				//0x00850414
	//				 FEEDBACK DIV-SEL[2:0]	     BAND-SELECT-CLOCK-DIVIDER-VALUE[7:0]        VCO-POW  MTLD AUXOUT-SEL AUXOUT-EN	AUXOUT-POW[1:0]	RFOUT-EN OUTPOW[1:0]
	//DB31 ~ DB24   DB23     DB22 DB21 DB20     DB19 DB18 DB17 DB16 DB15 DB14 DB13 DB12     DB11     B10  DB9        DB8       DB7 DB6         DB5      DB4 DB3     DB2 DB1 DB0
	//0    0		 8							 5					 0						 4									1							 C
	//0    ~ 0      1        0    0    0        0    1    0    1    0    0    0    0		 0		  1	   0		  0			0	0			0		 0	 0		 1	 0	 0
	#define R5_INIT		0x00580005L
	//				 LD-PIN[1:0]											 
	//DB31 ~ DB24   DB23 DB22   DB21 DB20 DB19 DB18 DB17 DB16 DB15 ~ DB4 DB3 DB2 DB1 DB0
	//0    0		 5					   8                   0 0 0       5
	//0    ~ 0      0    1    	 0	  1	   1    0    0    0    0    ~ 0   0   1   0   1	  
	u32 counterTemp,counterN,divSel,refFreq;
	u16 freqStep;
	u8 value,i;
	SPI_TYPE spiType;

	spiType.len   = 32;
	spiType.order = MSB_FIRST;
	spiType.type  = SPI_LEVEL_LOW;
	spiType.mask  = 0x80000000;	
  
	counterN 	= freq;
	freqStep 	= 100;
	refFreq 	= 10000;//10MHz�ο�Ƶ��	
	value 		= 4400000 / counterN;

	for(i=7;i>0;i--)
	{
		if((value & 0x80)==0x80)
		{
			break;
		}
		value <<= 1;
	} 
	 
	divSel = i;	
	counterN <<= divSel;							
	freqStep <<= divSel;
	
	//R5
	WriteSpiOneWord(&attBus[0],&spiType,R5_INIT);

	counterTemp = divSel << 20;
	counterTemp|= R4_INIT|(enable << 5)|(power<<3);
	//R4
	WriteSpiOneWord(&attBus[0],&spiType,counterTemp);
	//R3
	WriteSpiOneWord(&attBus[0],&spiType,R3_INIT);
	//R2
	WriteSpiOneWord(&attBus[0],&spiType,R2_INIT);
						  						
	counterTemp = refFreq;
	counterTemp /= (u32)freqStep;
	counterTemp <<= 3;
	counterTemp |= R1_INIT;
	//R1
	WriteSpiOneWord(&attBus[0],&spiType,counterTemp);
			
	counterTemp  = counterN % refFreq;
	counterTemp /= (u32)freqStep;
	counterTemp<<= 3;
	counterN 		/= refFreq;							
	counterN 	 <<= 15;
	counterTemp |= counterN;	
	//R0
	WriteSpiOneWord(&attBus[0],&spiType,counterTemp);
}

void InitTaskControl(void)
{
	unsigned char temp[10];

	rfPramModified = FALSE;							//????????	
	//Task5S_time = getTime() + DELAY5S;				//???????????
	//Task30S_time= getTime() + 100;					//????????????1S?
	//Task100MS_time = getTime();

	//SrcSwitch = 1;
	//LD = 1;
	//LD2 = 0;
  //SM2 = 0;                                        //
	//softDownLoad = UNDO;							//?????????
	//ack_flag = NO_ERR;								//???????
	gtestflag = 0;		 							//???????
	gtestRfTemp = 50;								//????????
  gpreRfTemp = 50;					   			//???????
	gcurRfTemp = 50;
	gAtttempval = 0;

	__PlusSwitchState = CLOSE;
	
	re_Pwr = 0;
	PA_current = 0;
	OutputPwr = 0;
//	rfmute = 1;                      //	   ??ADF4350,?????
//	readE2promStr(1,EE_RFSW,&AutoSwitch);				//????
	AutoSwitch = CLOSE;                     //?????
	gRFSW = CLOSE;
	
	//switchRF(gRFSW);
	
	delay(1);
	watchdog();
	
	//???????
	readE2promStr(sizeof(__temp_que),EE_TEMPER_BUCHANG,__temp_que);
	
	//Source select
	readE2promStr(1,EE_SOURCE_SELECT,temp);
	//RFSrcSelect = (temp[0] == 0xFF)? SRC_INTERNAL:temp[0];
	
	readE2promStr(4,EE_CenFreq,temp);				//??????
	//swapBytes(temp,(unsigned char *)&gCenFreq,4);

	readE2promStr(1,EE_RF_No,&gRF_No);				//????????

	gPALimCompensate();
	writeAD5314(gPALim,DAPOWER_LIM_CHAN);

	watchdog();				
	writeLM75(0x90,0x00,2);							 //??????????

	readE2promStr(1,EE_PAProtectLim,&gPAProtecttLim);//?????????
	writeLM75(0x03,gPAProtecttLim,3);	
	
	readE2promStr(1,EE_PAResetLim,&gPAResetLim);	 //?????????
	writeLM75(0x02,gPAResetLim,3);

	readE2promStr(1,EE_BaseTemp_VALUE,&gBaseTemp); 			//???????????
	watchdog();	 
	readE2promStr(1,EE_Att1,&gGain);
	writeAtt1(gGain);
	writePLL();											
	//delay(1);									
	//readTemperatur();
	//execCheckSumQ();							   	 //????????		
}

int TaskControl(int*argv[],int argc)
{
	
	return 1;
}




