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
#define AD4350_PWR_LIM			(1)   //-1dBm
#define IS_ALARM_TEMPERATURE()	(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) != Bit_SET)   	//�¶Ⱦ���
#define IS_ALARM_CURRENT()		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) != Bit_SET)	//��������
#define IS_ALARM_VSWR()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) != Bit_SET)	//פ������
#define IS_VCO_LOCK()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == Bit_SET)	//VCO����
#define IS_ALC_LOCK()			(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == Bit_SET)	//ALC����
#define IS_ADD_REF()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13)== Bit_SET)	//�ο�����
//VCOƬѡʹ��
#define VCO_CE(x)				(GPIO_WriteBit(GPIOB,GPIO_Pin_0,(x) == TRUE ? Bit_SET : Bit_RESET))
//ǰ���ʼ첨ʹ��
#define FWR_DET_ENABLE(x)		(GPIO_WriteBit(GPIOA,GPIO_Pin_12,(x) != TRUE ? Bit_SET : Bit_RESET))
//ǰ���ʼ�����ο�
#define FWR_DET_ADD_REF(x)		(GPIO_WriteBit(GPIOA,GPIO_Pin_13,(x) != TRUE ? Bit_SET : Bit_RESET))
//��Դ�л�
#define SOURCE_SWITCH(x)		do{GPIO_WriteBit(GPIOA,GPIO_Pin_7,(x == SRC_INTERNAL)?Bit_SET:Bit_RESET);\
								   GPIO_WriteBit(GPIOA,GPIO_Pin_6,(x != SRC_INTERNAL)?Bit_SET:Bit_RESET);}while(0)

#define PA_POWER_SWITCH(x)		(GPIO_WriteBit(GPIOB,GPIO_Pin_8,(x) == TRUE ? Bit_SET : Bit_RESET))
#define PA_RESET(X)				(GPIO_WriteBit(GPIOB,GPIO_Pin_9,(x) == TRUE ? Bit_SET : Bit_RESET))

#define setAtt(att)     		(WritePe4302(&attBus[1],(att)))
#define setALCRef(x)			WriteAD5324((x),'B')
/* Private macro -------------------------------------------------------------*/
const U8 BootloaderV 	__attribute__((at(ADDR_BYTE_BOOTLOADERV))) = 0x11;
const U8 SoftwareV 		__attribute__((at(ADDR_BYTE_SOFTWAREV  ))) = 0x62;
const U8 HardwareV 		__attribute__((at(ADDR_BYTE_HARDWAREV  ))) = 0x50;
/* Private variables ---------------------------------------------------------*/
BOOL execAtt1Set(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execSwitchSource(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execALC(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execVCO(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execVCOLim(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execPASW(U8 flag,U8 *buf,U16 rxLen,U16*txLen);									   
								   
const JC_COMMAND tabInfo[] = {	
//��Դѡ��
	{ID_FCT_PARAM_WR,EE_SOURCE_SELECT	,0,(U8*)&gRFSrcSel,sizeof(gRFSrcSel)			,0,0,execSwitchSource	},
/****gain*****/
	{ID_FCT_PARAM_WR,EE_TEMPER_BUCHANG	,0,(U8*)&gTempValue,sizeof(gTempValue)			,0,0,NULL				},
	{ID_FCT_PARAM_WR,EE_AtteVal			,0,(U8*)&gAtteVal,sizeof(gAtteVal)				,0,0,NULL				},//0x0001     //1�ֽڣ������趨ֵ
	{ID_FCT_PARAM_WR,EE_GainOffset		,0,(U8*)&gGainOffset,sizeof(gGainOffset)		,0,0,NULL				},//0x0002		//1�ֽڣ��������ֵ
	{ID_FCT_PARAM_WR,EE_Att1			,0,(U8*)&gAtt1,sizeof(gAtt1)					,0,0,execAtt1Set		},//0x0003		//1�ֽڣ�1#˥������1U5  	
	{ID_FCT_PARAM_WR,EE_Att2			,0,(U8*)&gAtt2,sizeof(gAtt2)					,0,0,NULL				},//0x0003		//1�ֽڣ�2#˥������1U5  
/**DAchannel**/
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_A	,0,(U8*)&gDAoutA,sizeof(gDAoutA)				,0,0,NULL},//0x0034		//2�ֽڣ���������޷�ֵ
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_B	,0,(U8*)&gDAoutB,sizeof(gDAoutB)				,0,0,NULL},//0x0036		//2�ֽڣ���������޷�ֵ
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_C	,0,(U8*)&gDAoutC,sizeof(gDAoutC)				,0,0,NULL},//0x0038		//2�ֽڣ���������޷�ֵ
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_D	,0,(U8*)&gDAoutD,sizeof(gDAoutD)				,0,0,NULL},//0x003A		//2�ֽڣ���������޷�ֵ
/****PA******/	
	{ID_FCT_PARAM_WR,EE_gPALim			,0,(U8*)&gPALim,sizeof(gPALim)					,0,0,execALC			},//0x0020		//2�ֽڣ���������޷�ֵ
	{ID_FCT_PARAM_WR,EE_gOutPwrVal		,0,(U8*)&gOutPwrVal,sizeof(gOutPwrVal)			,0,0,NULL},//0x0022		//2�ֽڣ�������ʶ���ֵ
	{ID_FCT_PARAM_WR,EE_gOutPwrSlopeVal	,0,(U8*)&gOutPwrSlopeVal,sizeof(gOutPwrSlopeVal),0,0,NULL},//0x0024		//2�ֽڣ���������趨ֵ����
	{ID_FCT_PARAM_WR,EE_gRefPwrVal		,0,(U8*)&gRefPwrVal,sizeof(gRefPwrVal)			,0,0,NULL},//0x0026		//2�ֽڣ����书�ʶ���
	{ID_FCT_PARAM_WR,EE_gRefPwrSlopeVal	,0,(U8*)&gRefPwrSlopeVal,sizeof(gRefPwrSlopeVal),0,0,NULL},//0x0028		//2�ֽڣ����书��б��ֵ
	{ID_FCT_PARAM_WR,EE_gInPwrVal		,0,(U8*)&gOutPwrShutVal,sizeof(gOutPwrShutVal)	,0,0,NULL},//0x002e		//1�ֽڣ�������ʹر�����
	{ID_FCT_PARAM_WR,EE_gPALimNumer		,0,(U8*)&gPALimNumer,sizeof(gPALimNumer)		,0,0,NULL},//0x0075		//1�ֽڣ���������޷���������
	{ID_FCT_PARAM_WR,EE_gPReadADNumer	,0,(U8*)&gPReadADNumer,sizeof(gPReadADNumer)	,0,0,NULL},//0x0076		//1�ֽڣ�������ʶ�ȡ��������	  
	{ID_FCT_PARAM_WR,EE_gPequDenomi		,0,(U8*)&gPequDenomi,sizeof(gPequDenomi)		,0,0,NULL},//0x0077		//1�ֽڣ����ʲ�����ĸ	
/***frequency***/
	{ID_FCT_PARAM_WR,EE_gPBmFreq		,0,(U8*)&gPBmFreq,sizeof(gPBmFreq)				,0,0,NULL},//0x0078		//4�ֽڣ���׼Ƶ��
	{ID_FCT_PARAM_WR,EE_CenFreq			,0,(U8*)&gCenFreq,sizeof(gCenFreq)				,0,0,execVCO		    },//0x0040		//4�ֽڣ�����Ƶ�� KHzΪ��λ
	{ID_FCT_PARAM_WR,EE_BandWidth		,0,(U8*)&gBandWidth,sizeof(gBandWidth)			,0,0,NULL},//0x0044		//4�ֽڣ�����
	{ID_FCT_PARAM_WR,EE_IFOffset		,0,(U8*)&gIFOffset,sizeof(gIFOffset)			,0,0,NULL},//0x0048		//2�ֽڣ���Ƶ����ֵ
	{ID_FCT_PARAM_WR,EE_FreqStep		,0,(U8*)&gFreqStep,sizeof(gFreqStep)			,0,0,NULL},//0x004A		//2�ֽڣ�Ƶ�ʲ���ֵ
	{ID_FCT_PARAM_WR,EE_IFFreq			,0,(U8*)&gIFFreq,sizeof(gIFFreq)				,0,0,NULL},//0x004C		//4�ֽڣ���ƵƵ��ֵ
	{ID_FCT_PARAM_WR,EE_Freq			,0,(U8*)&gFreq,sizeof(gFreq)					,0,0,NULL},//0x0050		//4�ֽڣ�����Ƶ��
	{ID_FCT_PARAM_WR,EE_RefFreq			,0,(U8*)&gRefFreq,sizeof(gRefFreq)				,0,0,NULL},//0x0054		//4�ֽڣ��ο�Ƶ?
	
	{ID_FCT_PARAM_WR,EE_FREQ_ADD_POWEWR	,0,(U8*)&gFreqLim[0],sizeof(gFreqLim)			,0,0,execVCOLim			},//0x0020		//6�ֽڣ�����Ƶ��+�޷�ֵ
/*****current*****/
	{ID_FCT_PARAM_WR,EE_CURRENT_TEMP	,0,(U8*)&gCurRfTemp,sizeof(gCurRfTemp)			,0,0,NULL},//0x0080		//2�ֽڣ���ǰ����
	{ID_FCT_PARAM_WR,EE_CurCo			,0,(U8*)&gCurCo,sizeof(gCurCo)					,0,0,NULL},//0x0080		//2�ֽڣ�����б��
	{ID_FCT_PARAM_WR,EE_CurOffset		,0,(U8*)&gCurOffset,sizeof(gCurOffset)			,0,0,NULL},//0x0082		//2�ֽڣ�����������ֵ
	{ID_FCT_PARAM_WR,EE_CurHlim			,0,(U8*)&gCurHlim,sizeof(gCurHlim)				,0,0,NULL},//0x0084		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
	{ID_FCT_PARAM_WR,EE_CurLlim			,0,(U8*)&gCurLlim,sizeof(gCurLlim)				,0,0,NULL},//0x0086		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
/*****temperature********/		
	{ID_FCT_PARAM_WR,EE_GainNumer		,0,(U8*)&gGainNumer,sizeof(gGainNumer)			,0,0,NULL},//0x00A2		//1�ֽڣ������²�ϵ������		
	{ID_FCT_PARAM_WR,EE_GainDenomi		,0,(U8*)&gGainDenomi,sizeof(gGainDenomi)		,0,0,NULL},//0x00A3		//1�ֽڣ������²�ϵ����ĸ		
	{ID_FCT_PARAM_WR,EE_PAProtectLim	,0,(U8*)&gPAProtecttLim,sizeof(gPAProtecttLim)	,0,0,NULL},//0x00A4		//1�ֽڣ����ű����¶�����		
	{ID_FCT_PARAM_WR,EE_PAResetLim		,0,(U8*)&gPAResetLim,sizeof(gPAResetLim)		,0,0,NULL},//0x00A5		//1�ֽڣ����Żظ��¶�
	{ID_FCT_PARAM_WR,EE_TEMP_VALUE		,0,(U8*)&gPAResetLim,sizeof(gPAResetLim)		,0,0,NULL},//0x00A5		//1�ֽڣ����Żظ��¶�
/*****module*******/
	{ID_FCT_PARAM_WR,EE_RF_No			,0,(U8*)&gRF_No,sizeof(gRF_No)					,0,0,NULL},//0x00C0		//1�ֽڣ���Ƶģ����		
	{ID_FCT_PARAM_WR,EE_PASW			,0,(U8*)&gPASW,sizeof(gPASW)					,0,0,execPASW			},//0x00C1     //1�ֽڣ����ſ���		
	{ID_FCT_PARAM_WR,EE_RFSW			,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,NULL},//0x00C2     //1�ֽڣ���Ƶ��?
	{ID_FCT_PARAM_WR,EE_TEST_MARK		,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,NULL},//0x00C2     //1�ֽڣ���Ƶ����?	
/****************other parameter********************/
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&gAtttempval,sizeof(gAtttempval)		,0,0,NULL},//�¶Ȳ���ֵ	
	{ID_FCT_PARAM_WR,EE_ALL_CHECKSUM	,0,(U8*)&gFWCheck,sizeof(gFWCheck)				,0,0,NULL},//����У���
	{ID_FCT_PARAM_WR,EE_DOWN_FLAG		,0,(U8*)&gDownFlag,sizeof(gDownFlag)			,0,0,NULL},//0x01FF		//1�ֽڣ�������־
	{ID_FCT_PARAM_WR,EE_MODULE_No		,0,(U8*)&gModuleNo,sizeof(gModuleNo)			,0,0,NULL},//0x01FF		//1�ֽڣ�������־		
/*********apparatus parameter************/
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaAdjEquModulus,sizeof(PaAdjEquModulus),0,0,NULL},//0x00F0  	//1�ֽ�, ���ŵ�У׼���̵Ĳ���ϵ��
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaLastAdjDate_year,sizeof(PaLastAdjDate_year)	,0,0,NULL},//0x00F1	    //2�ֽڣ��ŵ�����У׼����(��)
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaLastAdjDate_month,sizeof(PaLastAdjDate_month),0,0,NULL},//0x00F3	    //1�ֽڣ��ŵ�����У׼����(��)
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaLastAdjDate_day,sizeof(PaLastAdjDate_day)	,0,0,NULL},//0x00F1	    //1�ֽڣ��ŵ�����У׼����(��)
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaUpLimit,sizeof(PaUpLimit)			,0,0,NULL},//0x00F5		//2�ֽڣ���������
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&AppModel,sizeof(AppModel)				,0,0,NULL},//0x00F7	    //1�ֽڣ��Ǳ��ͺ�            
/****************�������崥������********************/
	{ID_FCT_PARAM_WR,EE_PLUS_TRIGER		,0,(U8*)&gPlusValue,sizeof(gPlusValue),0,0,NULL},//���忪��״̬����
/****************�̼�����********************/	
	{ID_FCT_PARAM_SAMPLE		,0		,0,0,0,0,0,execRFParamQ},
	{ID_FIRMWARE_UPGRAD_ENTER	,0		,0,0,0,0,0,execFWEnter},
	{ID_FIRMWARE_UPGRAD_LOAD	,0		,0,0,0,0,0,execFWLoad},
	{ID_FIRMWARE_UPGRAD_REBOOT	,0		,0,0,0,0,0,execFWReboot},		
};

/* Private function prototypes -----------------------------------------------*/
JC_COMMAND*GetTable(void)
{
	return (JC_COMMAND*)tabInfo;
}

int GetTableMebCnt(void)
{	
	return sizeof(tabInfo)/sizeof(JC_COMMAND);
}

void WritePLL(u32 freq,u32 freqRef,u16 freqStep,u8 power,BOOL enable)
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
	#define R2_INIT		0x18004FC2L
	//     NOISE-MODE[1:0] MUXOUT[2:0]	   REF-DOU RDIV2    					 DOUB-BUFF	CHARGE-PUMP[3:0]  LDF LDP PD-POLA PD  CP-THR  COUNT-RESET
	//DB31 DB30 DB29       DB28 DB27 DB26  DB25    DB24   DB23 ~ DB16 DB15 DB14 DB13       DB12 DB11 B10 DB9 DB8 DB7 DB6     DB5 DB4     DB3         DB2 DB1 DB0
	//1							8						  0	   0	   4						 E				  C						  2
	//0    0    0          1    1    0     0       0      0    ~ 0    0    1    0			0	 1	  1	  1	  1	  1	  1	      0	  0		  0           0   1   0
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
	u32 counterTemp,counterN,divSel;
	u8 value,i;
	SPI_TYPE spiType;

	spiType.len   = 32;
	spiType.order = MSB_FIRST;
	spiType.type  = SPI_LEVEL_LOW;
	spiType.mask  = 0x80000000;	
  
	counterN = freq;
	value 	 = 4400000 / counterN;

	for(i=7;i>0;i--)
	{
		if((value & 0x80)==0x80)break;
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
						  						
	counterTemp = (freqRef/freqStep)<<3;
	counterTemp|= R1_INIT;
	//R1
	WriteSpiOneWord(&attBus[0],&spiType,counterTemp);
		
	counterTemp  = ((counterN%freqRef)/freqStep)<<3;
	counterN  	 = (counterN/freqRef)<<15;											
	counterTemp |= counterN;	
	//R0
	WriteSpiOneWord(&attBus[0],&spiType,counterTemp);
}

void WriteAD5324(U16 value,U8 channel)
{	
	SPI_TYPE spiType;

	spiType.len   = 16;
	spiType.order = MSB_FIRST;
	spiType.type  = SPI_LEVEL_LOW;
	spiType.mask  = 0x8000;	
	
	value &= 0xFFFC;	
	
	if(channel=='A')	 value |= 0x2000;
	else if(channel=='B')value |= 0x6000;	
	else if(channel=='C')value |= 0xA000;	
	else 				 value |= 0xE000;	
	
	WriteSpiOneWord(&attBus[2],&spiType,value);
}

U16 ReadPowerADC(void)
{
	U16 adcValue,refValue = 0;

	FWR_DET_ADD_REF(FALSE);
	
	if(IS_ADD_REF())
	{		
		FWR_DET_ADD_REF(TRUE);
		ReadAdcValue(ADC_Channel_0,&refValue);
	}
	
	ReadAdcValue(ADC_Channel_1,&adcValue);
	
	adcValue += refValue;
	
	return adcValue/4;
}

BOOL execAtt1Set(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{	
	if(flag == TRUE)
	{
		//д˥����
		setAtt(gAtt1);	
	}
	return TRUE;
}

BOOL execSwitchSource(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	if(flag == TRUE)
	{	
		//�л���Դ
		SOURCE_SWITCH(gRFSrcSel);
		//������Դ
		VCO_CE(gRFSrcSel == SRC_INTERNAL && gRFSW == TRUE);	
	}
	return TRUE;
}

BOOL execALC(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		//ALC�����޷�
		setALCRef(gPALim * 3.3 / 1023.0 * 4095 / 5.0 );
	}
	return TRUE;
}

BOOL execVCO(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		//дVCO
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
	return TRUE;
}

BOOL execPASW(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		PA_POWER_SWITCH(gPASW);
	}
	return TRUE;
}

BOOL execVCOLim(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{	
	if(flag == TRUE)
	{	
		gCenFreq = *(U32*)&gFreqLim[0];
		gPALim = *(U16*)&gFreqLim[4];
		//ALC�����޷�
		setALCRef(gPALim*4);	
		//дVCO
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
	return TRUE;
}

void InitTaskControl(void)
{		
	gRFModify = FALSE;
	gPlusSwitchState = CLOSE;
	gPASW = CLOSE;
	gRFSW = CLOSE;
	gRFSrcSel = SRC_INTERNAL;
	
	FWR_DET_ENABLE(TRUE);
	
	SOURCE_SWITCH(gRFSrcSel);		
	
	PA_POWER_SWITCH(FALSE);
	
	setAtt(gAtt1);
	setALCRef(0);
	
	VCO_CE(TRUE);
	WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
}
		
int TaskControl(int*argv[],int argc)
{		
	//�޷�״̬
	gLimState = IS_ALC_LOCK();
	//VCO����״̬
	gPLLLock  = IS_VCO_LOCK();
	//����ֵԤ��	
	gPACurrent = 500;
	//��ǰ����
	gPwrOut = ReadPowerADC(); 
	gPwrRef	= gPwrOut/30;
	//��ȡ��ǰ�¶�
	gCurRfTemp = ReadInternalTemperatureSensor();	
	//�¶Ⱦ���	
	if(IS_ALARM_TEMPERATURE())
	{
		gCurRfTemp = 80*2;
		
		gPASW = FALSE;
		gRFSW = FALSE;
		PA_POWER_SWITCH(gPASW);
	}
	//��������
	if(IS_ALARM_CURRENT())
	{
		gPACurrent = 10000;
	}
	//פ������
	if(IS_ALARM_VSWR())
	{
		gPwrRef	= gPwrOut/2;
	}
	//�����仯���޸�
	if(TRUE == gRFModify)
	{		
		gRFModify = FALSE;
		//PA_POWER_SWITCH(gPASW);		
		//д˥����
		//setAtt(gAtt1);
		//�л���Դ
		//SOURCE_SWITCH(gRFSrcSel);
		//������Դ
		//VCO_CE(gRFSrcSel == SRC_INTERNAL && gRFSW == TRUE);		
		//ALC�����޷�
		//setALCRef(gPALim*4);		
		//WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
		
	return 1;
}



