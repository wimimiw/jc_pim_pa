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
#include "stdio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define AD4350_PWR_LIM			(1)   //-1dBm
#define IS_ALARM_TEMPERATURE()	(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) != Bit_SET)   	//温度警报
#define IS_ALARM_CURRENT()		(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) != Bit_SET)	//电流警报
#define IS_ALARM_VSWR()			(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != Bit_SET)	//驻波警报
#define IS_VCO_LOCK()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13)== Bit_SET)	//VCO锁定
#define IS_ALC_LOCK()			(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)== Bit_SET)	//ALC锁定
//VCO片选使能
#define VCO_CE(x)				(GPIO_WriteBit(GPIOB,GPIO_Pin_14,(x) == TRUE ? Bit_SET : Bit_RESET))
//前向功率检波选择				高电平(1) = 电平输入 >< 低电(0) = 射频输入
#define PWR_DET_SELECT(x)		(GPIO_WriteBit(GPIOA,GPIO_Pin_0,(x) == TRUE ? Bit_SET : Bit_RESET))
//前向功率检波控制
#define IS_OVER_3V()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == Bit_SET)
//前向功率参考读取
#define PWRDTREF_RO(x)			(ReadAdcValue(ADC_Channel_3,&(x)))
//前向功率电平读取
#define PWRDTADJ_RO(x)			(ReadAdcValue(ADC_Channel_1,&(x)))
//信源切换
#define SOURCE_SWITCH(x)		do{GPIO_WriteBit(GPIOA,GPIO_Pin_14,(x == SRC_INTERNAL)?Bit_SET:Bit_RESET);\
								   GPIO_WriteBit(GPIOA,GPIO_Pin_15,(x != SRC_INTERNAL)?Bit_SET:Bit_RESET);}while(0)

#define PA_POWER_SWITCH(x)		(GPIO_WriteBit(GPIOB,GPIO_Pin_8,(x) == TRUE ? Bit_SET : Bit_RESET))
#define PA_RESET(x)				(GPIO_WriteBit(GPIOB,GPIO_Pin_9,(x) == TRUE ? Bit_SET : Bit_RESET))

#define setAtt(att)     		(WritePe4302(&attBus[1],(att)))
#define setALCRef(x)			(WriteAD5324((x),'B'))
/* Private macro -------------------------------------------------------------*/
const U8 BootloaderV 	__attribute__((at(ADDR_BYTE_BOOTLOADERV))) = 0x11;
const U8 SoftwareV 		__attribute__((at(ADDR_BYTE_SOFTWAREV  ))) = 0x62;
const U8 HardwareV 		__attribute__((at(ADDR_BYTE_HARDWAREV  ))) = 0x50;
/* Private variables ---------------------------------------------------------*/
U16 __interPeriod = 0;
U16 __interVal = 0;
BOOL execAtt1Set(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execSwitchSource(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execALC(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execVCO(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execVCOLim(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
BOOL execRFSW(U8 flag,U8 *buf,U16 rxLen,U16*txLen);									   
								   
const JC_COMMAND tabInfo[] = {	
//信源选择
	{ID_FCT_PARAM_WR,EE_SOURCE_SELECT	,0,(U8*)&gRFSrcSel,sizeof(gRFSrcSel)			,0,0,execSwitchSource	},
/****gain*****/
	{ID_FCT_PARAM_WR,EE_TEMPER_BUCHANG	,0,(U8*)&gTempValue,sizeof(gTempValue)			,0,0,NULL				},
	{ID_FCT_PARAM_WR,EE_AtteVal			,0,(U8*)&gAtteVal,sizeof(gAtteVal)				,0,0,NULL				},//0x0001     //1字节，增益设定值
	{ID_FCT_PARAM_WR,EE_GainOffset		,0,(U8*)&gGainOffset,sizeof(gGainOffset)		,0,0,NULL				},//0x0002		//1字节，增益调整值
	{ID_FCT_PARAM_WR,EE_Att1			,0,(U8*)&gAtt1,sizeof(gAtt1)					,0,0,execAtt1Set		},//0x0003		//1字节，1#衰减器，1U5  	
	{ID_FCT_PARAM_WR,EE_Att2			,0,(U8*)&gAtt2,sizeof(gAtt2)					,0,0,NULL				},//0x0003		//1字节，2#衰减器，1U5  
/**DAchannel**/
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_A	,0,(U8*)&gDAoutA,sizeof(gDAoutA)				,0,0,NULL},//0x0034		//2字节，输出功率限幅值
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_B	,0,(U8*)&gDAoutB,sizeof(gDAoutB)				,0,0,NULL},//0x0036		//2字节，输出功率限幅值
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_C	,0,(U8*)&gDAoutC,sizeof(gDAoutC)				,0,0,NULL},//0x0038		//2字节，输出功率限幅值
	{ID_FCT_PARAM_WR,EE_DA_CHANNEL_D	,0,(U8*)&gDAoutD,sizeof(gDAoutD)				,0,0,NULL},//0x003A		//2字节，输出功率限幅值
/****PA******/	
	{ID_FCT_PARAM_WR,EE_gPALim			,0,(U8*)&gPALim,sizeof(gPALim)					,0,0,execALC			},//0x0020		//2字节，输出功率限幅值
	{ID_FCT_PARAM_WR,EE_gOutPwrVal		,0,(U8*)&gOutPwrVal,sizeof(gOutPwrVal)			,0,0,NULL},//0x0022		//2字节，输出功率定标值
	{ID_FCT_PARAM_WR,EE_gOutPwrSlopeVal	,0,(U8*)&gOutPwrSlopeVal,sizeof(gOutPwrSlopeVal),0,0,NULL},//0x0024		//2字节，输出功率设定值定标
	{ID_FCT_PARAM_WR,EE_gRefPwrVal		,0,(U8*)&gRefPwrVal,sizeof(gRefPwrVal)			,0,0,NULL},//0x0026		//2字节，反射功率定标
	{ID_FCT_PARAM_WR,EE_gRefPwrSlopeVal	,0,(U8*)&gRefPwrSlopeVal,sizeof(gRefPwrSlopeVal),0,0,NULL},//0x0028		//2字节，反射功率斜率值
	{ID_FCT_PARAM_WR,EE_gInPwrVal		,0,(U8*)&gOutPwrShutVal,sizeof(gOutPwrShutVal)	,0,0,NULL},//0x002e		//1字节，输出功率关闭门限
	{ID_FCT_PARAM_WR,EE_gPALimNumer		,0,(U8*)&gPALimNumer,sizeof(gPALimNumer)		,0,0,NULL},//0x0075		//1字节，输出功率限幅补偿分子
	{ID_FCT_PARAM_WR,EE_gPReadADNumer	,0,(U8*)&gPReadADNumer,sizeof(gPReadADNumer)	,0,0,NULL},//0x0076		//1字节，输出功率读取补偿分子	  
	{ID_FCT_PARAM_WR,EE_gPequDenomi		,0,(U8*)&gPequDenomi,sizeof(gPequDenomi)		,0,0,NULL},//0x0077		//1字节，功率补偿分母	
/***frequency***/
	{ID_FCT_PARAM_WR,EE_gPBmFreq		,0,(U8*)&gPBmFreq,sizeof(gPBmFreq)				,0,0,NULL},//0x0078		//4字节，基准频率
	{ID_FCT_PARAM_WR,EE_CenFreq			,0,(U8*)&gCenFreq,sizeof(gCenFreq)				,0,0,execVCO		    },//0x0040		//4字节，中心频率 KHz为单位
	{ID_FCT_PARAM_WR,EE_BandWidth		,0,(U8*)&gBandWidth,sizeof(gBandWidth)			,0,0,NULL},//0x0044		//4字节，带宽
	{ID_FCT_PARAM_WR,EE_IFOffset		,0,(U8*)&gIFOffset,sizeof(gIFOffset)			,0,0,NULL},//0x0048		//2字节，中频调整值
	{ID_FCT_PARAM_WR,EE_FreqStep		,0,(U8*)&gFreqStep,sizeof(gFreqStep)			,0,0,NULL},//0x004A		//2字节，频率步进值
	{ID_FCT_PARAM_WR,EE_IFFreq			,0,(U8*)&gIFFreq,sizeof(gIFFreq)				,0,0,NULL},//0x004C		//4字节，中频频率值
	{ID_FCT_PARAM_WR,EE_Freq			,0,(U8*)&gFreq,sizeof(gFreq)					,0,0,NULL},//0x0050		//4字节，本振频率
	{ID_FCT_PARAM_WR,EE_RefFreq			,0,(U8*)&gRefFreq,sizeof(gRefFreq)				,0,0,NULL},//0x0054		//4字节，参考频?
	
	{ID_FCT_PARAM_WR,EE_FREQ_ADD_POWEWR	,0,(U8*)&gFreqLim[0],sizeof(gFreqLim)			,0,0,execVCOLim			},//0x0020		//6字节，中心频率+限幅值
/*****current*****/
	{ID_FCT_PARAM_WR,EE_CURRENT_TEMP	,0,(U8*)&gCurRfTemp,sizeof(gCurRfTemp)			,0,0,NULL},//0x0080		//2字节，当前电流
	{ID_FCT_PARAM_WR,EE_CurCo			,0,(U8*)&gCurCo,sizeof(gCurCo)					,0,0,NULL},//0x0080		//2字节，电流斜率
	{ID_FCT_PARAM_WR,EE_CurOffset		,0,(U8*)&gCurOffset,sizeof(gCurOffset)			,0,0,NULL},//0x0082		//2字节，电流检测调整值
	{ID_FCT_PARAM_WR,EE_CurHlim			,0,(U8*)&gCurHlim,sizeof(gCurHlim)				,0,0,NULL},//0x0084		//2字节，功放电流上限,整数在前,小数在后
	{ID_FCT_PARAM_WR,EE_CurLlim			,0,(U8*)&gCurLlim,sizeof(gCurLlim)				,0,0,NULL},//0x0086		//2字节，功放电流下限,整数在前,小数在后
/*****temperature********/		
	{ID_FCT_PARAM_WR,EE_GainNumer		,0,(U8*)&gGainNumer,sizeof(gGainNumer)			,0,0,NULL},//0x00A2		//1字节，增益温补系数分子		
	{ID_FCT_PARAM_WR,EE_GainDenomi		,0,(U8*)&gGainDenomi,sizeof(gGainDenomi)		,0,0,NULL},//0x00A3		//1字节，增益温补系数分母		
	{ID_FCT_PARAM_WR,EE_PAProtectLim	,0,(U8*)&gPAProtecttLim,sizeof(gPAProtecttLim)	,0,0,NULL},//0x00A4		//1字节，功放保护温度门限		
	{ID_FCT_PARAM_WR,EE_PAResetLim		,0,(U8*)&gPAResetLim,sizeof(gPAResetLim)		,0,0,NULL},//0x00A5		//1字节，功放回复温度
	{ID_FCT_PARAM_WR,EE_TEMP_VALUE		,0,(U8*)&gPAResetLim,sizeof(gPAResetLim)		,0,0,NULL},//0x00A5		//1字节，功放回复温度
/*****module*******/
	{ID_FCT_PARAM_WR,EE_RF_No			,0,(U8*)&gRF_No,sizeof(gRF_No)					,0,0,NULL},//0x00C0		//1字节，射频模块编号		
	{ID_FCT_PARAM_WR,EE_PASW			,0,(U8*)&gPASW,sizeof(gPASW)					,0,0,NULL},//0x00C1     //1字节，功放开关		
	{ID_FCT_PARAM_WR,EE_RFSW			,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,execRFSW			},//0x00C2     //1字节，射频开?
	{ID_FCT_PARAM_WR,EE_TEST_MARK		,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,NULL},//0x00C2     //1字节，射频开关?	
/****************other parameter********************/
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&gAtttempval,sizeof(gAtttempval)		,0,0,NULL},//温度补偿值	
	{ID_FCT_PARAM_WR,EE_ALL_CHECKSUM	,0,(U8*)&gFWCheck,sizeof(gFWCheck)				,0,0,NULL},//程序校验和
	{ID_FCT_PARAM_WR,EE_DOWN_FLAG		,0,(U8*)&gDownFlag,sizeof(gDownFlag)			,0,0,NULL},//0x01FF		//1字节，升级标志
	{ID_FCT_PARAM_WR,EE_MODULE_No		,0,(U8*)&gModuleNo,sizeof(gModuleNo)			,0,0,NULL},//0x01FF		//1字节，升级标志		
/*********apparatus parameter************/
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaAdjEquModulus,sizeof(PaAdjEquModulus),0,0,NULL},//0x00F0  	//1字节, 功放的校准方程的补偿系数
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaLastAdjDate_year,sizeof(PaLastAdjDate_year)	,0,0,NULL},//0x00F1	    //2字节，放的最终校准日期(年)
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaLastAdjDate_month,sizeof(PaLastAdjDate_month),0,0,NULL},//0x00F3	    //1字节，放的最终校准日期(月)
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaLastAdjDate_day,sizeof(PaLastAdjDate_day)	,0,0,NULL},//0x00F1	    //1字节，放的最终校准日期(年)
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&PaUpLimit,sizeof(PaUpLimit)			,0,0,NULL},//0x00F5		//2字节，功放上限
//	{ID_FCT_PARAM_WR,0					,0,(U8*)&AppModel,sizeof(AppModel)				,0,0,NULL},//0x00F7	    //1字节，仪表型号            
/****************单次脉冲触发变量********************/
	{ID_FCT_PARAM_WR,EE_PLUS_TRIGER		,0,(U8*)&gPlusValue,sizeof(gPlusValue),0,0,NULL},//脉冲开关状态变量
/****************固件操作********************/	
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
	
	PWRDTADJ_RO(adcValue);
	PWRDTREF_RO(refValue);
		
	if(IS_OVER_3V())
	{		
		PWRDTREF_RO(refValue);
		adcValue += refValue;
	}
	
	//归一化
	adcValue = 4096 * adcValue / ( 4096 + refValue );
		
	return adcValue/4;
}

BOOL execAtt1Set(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{	
	if(flag == TRUE)
	{
		//写衰减器
		setAtt(gAtt1);	
	}
	return TRUE;
}

BOOL execSwitchSource(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	if(flag == TRUE)
	{	
		//切换信源
		SOURCE_SWITCH(gRFSrcSel);
		//设置信源
		VCO_CE(gRFSrcSel == SRC_INTERNAL && gRFSW == TRUE);	
	}
	return TRUE;
}

BOOL execALC(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		//ALC参数限辐
		//setALCRef(gPALim * 3.3 / 1023.0 * 4095 / 5.0 );	
		setALCRef(gPALim*4);				
	}
	return TRUE;
}

BOOL execVCO(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		//写VCO
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
	return TRUE;
}

BOOL execRFSW(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	UserTimerDef Timer;
	
	if(flag == TRUE)
	{	
		//温度警报	电流警报 驻波警报
		if(IS_ALARM_TEMPERATURE()||IS_ALARM_CURRENT()||IS_ALARM_VSWR())
		{
			PA_RESET(TRUE);
			
			UserTimerReset(TIM2,&Timer);
			while(FALSE == UserTimerOver(TIM2,&Timer,USER_TIMER_10MS(5)));
			
			PA_RESET(FALSE);
		}

		VCO_CE(gRFSW);		
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSW);
		
		PA_POWER_SWITCH(gRFSW);
	}
	return TRUE;
}

BOOL execVCOLim(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{	
	if(flag == TRUE)
	{	
		gCenFreq = *(U32*)&gFreqLim[0];
		WriteE2prom(EE_CenFreq,(U8*)&gCenFreq,sizeof(gCenFreq));
					
		if( buf[12] == 6 )
		{
			gPALim = *(U16*)&gFreqLim[4];	
			//ALC参数限辐
			setALCRef(gPALim*4);			
		}		
	
		//写VCO
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
	return TRUE;
}

void InitTaskSwitchMatrix()
{
	GPIO_Write(GPIOA,(U16)(1<<12));
	GPIO_Write(GPIOB,0);
	GPIO_Write(GPIOC,0);
	GPIO_Write(GPIOD,0);
	GPIO_Write(GPIOE,(U16)(1<<2));			
}

void InitTaskControl(void)
{		
	gRFModify = FALSE;
	gPlusSwitchState = CLOSE;
	gPASW = CLOSE;
	gRFSW = CLOSE;
	gRFSrcSel = SRC_INTERNAL;
	
	//前向功率检测切换至射频 输入模式
	PWR_DET_SELECT(0);
	
	SOURCE_SWITCH(gRFSrcSel);		
	
	PA_POWER_SWITCH(FALSE);
	
	setAtt(gAtt1);
	setALCRef(0);
	
	//VCO_CE(TRUE);
	//WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	VCO_CE(FALSE);
}

static int taskSwitchMatrix(U8 port)
{
#define DATA_LEN	12
	static U16 GpioInfo[5];	
	U8 i,check,*buf,addr,cmd,cmdLogic;
	U16 len;
	
	GetUartBufInfo(port,(U8**)&buf,&len);
	
	if(len == DATA_LEN)
	{		
		cmd = buf[0];
		
		if( cmd < 'A' )return 0;
		
		ResetUartBuf(port);
		
		check = 0;
		for(i=0;i<DATA_LEN-1;i++)
			check^=buf[i];	
		
		if(cmd > 'Z' && check == buf[DATA_LEN-1])
		{//包含地址信息	(91~255)
			cmdLogic = (cmd-'Z')%5;
			
			if(gAddr != (cmd-'Z')/5 && (cmd-'Z')/5 != 0)return 0;
			
			if( cmdLogic== 1)cmd = 'W';
			else if(cmdLogic == 2)cmd = 'R';
			else if(cmdLogic == 3)cmd = 'V';
			else if(cmdLogic == 4)cmd = 'Z';
		}
		else
		{			
			check = 0;
			for(i=1;i<DATA_LEN-1;i++)
				check^=buf[i];
		}
		
		if( check != buf[DATA_LEN-1] )
		{//校验
			buf[0] = 'X';//error			
		}	
		else if(cmd == 'W')
		{//写操作
			for(i=0;i<5;i++)
				GpioInfo[i] = *(U16*)&buf[i*2+1];
			
			GpioInfo[0] |= (1<<12);
			GpioInfo[4] |= (1<<2);
			
			GPIO_Write(GPIOA,GpioInfo[0]);
			GPIO_Write(GPIOB,GpioInfo[1]);
			GPIO_Write(GPIOC,GpioInfo[2]);
			GPIO_Write(GPIOD,GpioInfo[3]);
			GPIO_Write(GPIOE,GpioInfo[4]);					
		}
		else if (cmd == 'R')
		{//读操作
			for(i=0;i<5;i++)
				*(U16*)&buf[i*2+1] = GpioInfo[i];
			
			check = 0;
			for(i = 1;i<DATA_LEN-1;i++)
				check^=buf[i];
			
			buf[DATA_LEN-1] = check;					
		}
		else if (cmd == 'V')
		{//读版本
			sprintf((char*)&buf[1],"2.X");		
			
			check = 0;
			for(i = 1;i<DATA_LEN-1;i++)
				check^=buf[i];
			
			buf[DATA_LEN-1] = check;						
		}	
		else if (cmd == 'Z')
		{//读写地址
			if(buf[1] == 'A' && buf[2] == 'W')
			{
				gAddr = buf[3];
				WriteE2prom(EE_ADDR,(U8*)&gAddr,sizeof(gAddr));
			}
			else if(buf[1] == 'A' && buf[2] == 'R')
			{				
				buf[3] = gAddr;
			}				
			else if(buf[1] == 'P')
			{//脉冲
				__interVal = buf[2] + buf[3]*256;
				__interPeriod = buf[4] + buf[5]*256;

				WriteE2prom(EE_PLUS_VALUE,&buf[2],4);
				
				TIM_Cmd(TIM1,DISABLE);
				GPIO_Write(GPIOA,0xFFFF);
				GPIO_Write(GPIOB,0xFFFF);
				GPIO_Write(GPIOC,0xFFFF);
				GPIO_Write(GPIOD,0xFFFF);
				GPIO_Write(GPIOE,0xFFFF);
				TIM_Cmd(TIM1,ENABLE);									
			}	
			
			check = 0;
			for(i = 1;i<DATA_LEN-1;i++)
				check^=buf[i];
			
			buf[DATA_LEN-1] = check;						
		}			
		
		UartTxOpen(port,DATA_LEN);
	}
	
	return 0;
}

void TaskPlusDelegate()
{
	static U8 f =0;
	static U16 period1 = 0,period2 = 0;	
	
	if( __interPeriod == 0 )
	{
		if( --__interVal == 0 )
		{		
			GPIO_Write(GPIOA,0);
			GPIO_Write(GPIOB,0);
			GPIO_Write(GPIOC,0);
			GPIO_Write(GPIOD,0);
			GPIO_Write(GPIOE,0);	
			
			TIM_Cmd(TIM1,DISABLE);
		}	
	}	
	else 
	{		
		if( (period1++ % __interPeriod) == 0 )
		{
			f = 1;
			period2 = 0;
			GPIO_Write(GPIOA,0xFFFF);
			GPIO_Write(GPIOB,0xFFFF);
			GPIO_Write(GPIOC,0xFFFF);
			GPIO_Write(GPIOD,0xFFFF);
			GPIO_Write(GPIOE,0xFFFF);				
		}

		if( ++period2 >= __interVal && f == 1)
		{
			f = 0;
			GPIO_Write(GPIOA,0);
			GPIO_Write(GPIOB,0);
			GPIO_Write(GPIOC,0);
			GPIO_Write(GPIOD,0);
			GPIO_Write(GPIOE,0);
		}				
	}	
}

int TaskSwitchMatrix(int*argv[],int argc)
{
	Tim1RegisterDelegate(TaskPlusDelegate);
	taskSwitchMatrix(UART1);
	taskSwitchMatrix(UART2);
	
	return 0;
}

int TaskControl(int*argv[],int argc)
{		
	//限幅状态
	gLimState = IS_ALC_LOCK();
	//VCO锁定状态
	gPLLLock  = IS_VCO_LOCK();
	//电流值预设	
	gPACurrent = 500;
	//读前向功率
	gPwrOut = ReadPowerADC(); 
	gPwrRef	= gPwrOut/30;
	//获取当前温度
	gCurRfTemp = ReadInternalTemperatureSensor();	
	//温度警报	
	if(IS_ALARM_TEMPERATURE())
	{
		gCurRfTemp = 80*2;
		
		gPASW = FALSE;
		gRFSW = FALSE;
		PA_POWER_SWITCH(gRFSW);
	}
	//电流警报
	if(IS_ALARM_CURRENT())
	{
		gPACurrent = 10000;
	}
	//驻波警报
	if(IS_ALARM_VSWR())
	{
		gPwrRef	= gPwrOut/2;
	}
	//参数变化后修改
	if(TRUE == gRFModify)
	{		
		gRFModify = FALSE;
		//PA_POWER_SWITCH(gPASW);		
		//写衰减器
		//setAtt(gAtt1);
		//切换信源
		//SOURCE_SWITCH(gRFSrcSel);
		//设置信源
		//VCO_CE(gRFSrcSel == SRC_INTERNAL && gRFSW == TRUE);		
		//ALC参数限辐
		//setALCRef(gPALim*4);		
		//WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
	
	GPIO_WriteBit(GPIOB,GPIO_Pin_12,(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == Bit_RESET) ? Bit_SET : Bit_RESET );
		
	return 1;
}




