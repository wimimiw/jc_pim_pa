/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_control.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __RF_CONTROL_H__
#define __RF_CONTROL_H__

#ifndef __RF_CONTROL_C__
#define	__RF_CONTROL_EXT__ 	 	extern volatile
#define __RF_CONTROL_EXT_FUNC__	extern	
#else
#define	__RF_CONTROL_EXT__	 	volatile
#define __RF_CONTROL_EXT_FUNC__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/	
typedef struct
{
	U8 cmd;
	U16 sub;
	U8 type;
	U8 *var;
	U8 varLen;
	U16 min;
	U16 max;
	BOOL (*proc)(U8,U8*,U16,U16*);
}JC_COMMAND;
/* Public define -------------------------------------------------------------*/
#define ID_SET_MODE						0x10
#define ID_GET_MODE                     0x11
#define ID_SET_POWER                    0x12
#define ID_GET_POWER                    0x13
#define ID_SET_VI_LIMIT            		0x14
#define ID_GET_VI_LIMIT            		0x15
#define ID_GET_VI          				0x16
#define ID_SET_ATT_GAIN                 0x17
#define ID_GET_ATT_GAIN                 0x18
#define ID_GET_TEMP_CUR                 0x19
#define ID_GET_TEMP_SCOPE               0x1A
#define ID_SET_TEMP_SCOPE_CUR 	        0x1B
#define ID_GET_ALARM                    0x1C
#define ID_SET_SWITCH                   0x1D
#define ID_GET_SWITCH					0x1E
#define ID_RESERVED1                    0x1F
#define ID_ADDR_WD                      0x20
#define ID_ADDR_RD                      0x21
#define ID_SET_TEMP_ALARM_VALUE			0x22
#define ID_GET_TEMP_ALARM_VALUE			0x23
#define ID_RESET                        0x60
#define ID_LOAD_START                   0x61
#define ID_LOADING                      0x62
#define ID_LOAD_END                     0x63
#define ID_INQUIRE_STATE                0x64
#define ID_SET_INFO                     0x65
#define ID_GET_INFO                     0x66

#define ID_UPGRADEALL_START				0xF1
#define ID_UPGRADEALL_DOING				0xF2
#define ID_UPGRADEALL_END				0xF3

/*Alarm Detection*/
#define ALARM_CURRENT_OVER_15	(U16)(1<<0)
#define ALARM_CURRENT_UNDER_15	(U16)(1<<1)
#define ALARM_CURRENT_OVER_25	(U16)(1<<2)
#define ALARM_CURRENT_UNDER_25	(U16)(1<<3)
#define ALARM_CURRENT_OVER_19	(U16)(1<<4)
#define ALARM_CURRENT_UNDER_19	(U16)(1<<5)
#define ALARM_CURRENT_OVER_29	(U16)(1<<6)
#define ALARM_CURRENT_UNDER_29	(U16)(1<<7)

#define ALARM_VOLTAGE_OVER_12	(U16)(1<<8)
#define ALARM_VOLTAGE_UNDER_12	(U16)(1<<9)
#define ALARM_VOLTAGE_OVER_09	(U16)(1<<10)
#define ALARM_VOLTAGE_UNDER_09	(U16)(1<<11)
#define ALARM_VOLTAGE_OVER_05	(U16)(1<<12)
#define ALARM_VOLTAGE_UNDER_05	(U16)(1<<13)

#define ALARM_TEMPERATURE_HIGH	(U16)(1<<14)
#define ALARM_TEMPERATURE_LOW	(U16)(1<<15)

#define NORMAL_STATE			0x00
#define HOT_STANDBY				0x03

/******************紫光功放参数**********************/
#define	UNDO			0x00		
#define	DOING			0x01
#define	DONE			0x10

#define FALSE 			0
#define TRUE 			1

#define CLOSE 			0
#define OPEN 			1

#define SRC_INTERNAL	0
#define SRC_EXTERNAL 	1

#define	FW_VERSION_H  	0x06
#define FW_VERSION_L  	0x01

//在这里选择使用的ADC通道
#define CHANNEL_0		0x01
#define CHANNEL_1		0x02
#define CHANNEL_2		0x04
#define CHANNEL_3		0x08
#define CHANNEL_4		0x10
#define CHANNEL_5		0x20
#define CHANNEL_6		0x40
#define CHANNEL_7		0x80

#define TEMP_LOW  			__temp_que[0]
#define TEMP_LOW_VALUE  	__temp_que[1]
#define TEMP_NORMAL  		__temp_que[2]
#define TEMP_NORMAL_VALUE 	__temp_que[3]
#define TEMP_HIGH  			__temp_que[4]
#define TEMP_HIGH_VALUE  	__temp_que[5]
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/    
__RF_CONTROL_EXT__	S16 gCurRfTemp;				//当前温度值
__RF_CONTROL_EXT__	S16 gPreRfTemp;				//上一次温度值
__RF_CONTROL_EXT__	S16 gainCo;
/****debug****/
__RF_CONTROL_EXT__	S16 gTestRfTemp;			//测试温度值
__RF_CONTROL_EXT__	U8 	gTestflag;				//调试命令标志，0为正常状态，1为调试状态
__RF_CONTROL_EXT__	U8  gTempgain;				//1字节，温补基准值设定
__RF_CONTROL_EXT__	U8 	gBaseTemp;				//温度基准值
/****gain*****/
__RF_CONTROL_EXT__	U8  gAtteVal;				//0x0001      	//1字节，增益设定值
__RF_CONTROL_EXT__	U8  gGainOffset;			//0x0002		//1字节，增益调整值
__RF_CONTROL_EXT__	U8  gAtt1;					//0x0003		//1字节，1#衰减器，1U5
__RF_CONTROL_EXT__	U8  gAtt2;					//0x0003		//1字节，1#衰减器，1U5
/****PA******/
__RF_CONTROL_EXT__	U16 gDAoutA;				//0x0034		//2字节，输出功率限幅值
__RF_CONTROL_EXT__	U16 gDAoutB;				//0x0036		//2字节，输出功率限幅值
__RF_CONTROL_EXT__	U16 gDAoutC;				//0x0038		//2字节，输出功率限幅值
__RF_CONTROL_EXT__	U16 gDAoutD;				//0x003A		//2字节，输出功率限幅值
__RF_CONTROL_EXT__	U16 gPALim;					//0x0020		//2字节，输出功率限幅值
__RF_CONTROL_EXT__ 	U16 gOutPwrVal;            	//0x0022		//2字节，输出功率定标值
__RF_CONTROL_EXT__	U16 gOutPwrSlopeVal;		//0x0024		//2字节，输出功率设定值定标
__RF_CONTROL_EXT__	U16 gRefPwrVal;		        //0x0026		//2字节，反射功率定标
__RF_CONTROL_EXT__	U16 gRefPwrSlopeVal; 		//0x0028		//2字节，反射功率斜率值
__RF_CONTROL_EXT__	U8  gOutPwrShutVal;			//0x002e		//1字节，输出功率关闭门限
__RF_CONTROL_EXT__ 	S8  gPALimNumer;		    //0x0075		//1字节，输出功率限幅补偿分子	    
__RF_CONTROL_EXT__ 	S8  gPReadADNumer;	        //0x0076		//1字节，输出功率读取补偿分子	    
__RF_CONTROL_EXT__ 	S8  gPequDenomi;	        //0x0077		//1字节，功率补偿分母	  
/***frequency**	*/
__RF_CONTROL_EXT__	U32 gPBmFreq;				//0x0078		//4字节，基准频率
/***frequency***/
__RF_CONTROL_EXT__	U32 gCenFreq;				//0x0040		//4字节，中心频率 KHz为单位
__RF_CONTROL_EXT__	U32 gBandWidth;				//0x0044		//4字节，带宽
__RF_CONTROL_EXT__	S16 gIFOffset;				//0x0048		//2字节，中频调整值
__RF_CONTROL_EXT__	U16 gFreqStep;				//0x004A		//2字节，频率步进值
__RF_CONTROL_EXT__	U32 gIFFreq;				//0x004C		//4字节，中频频率值
__RF_CONTROL_EXT__	U32 gFreq;					//0x0050		//4字节，本振频率
__RF_CONTROL_EXT__	U32 gRefFreq;				//0x0054		//4字节，参考频率
/*****current*****/
__RF_CONTROL_EXT__	U16 gCurCo;					//0x0080		//2字节，电流斜率
__RF_CONTROL_EXT__	U16 gCurOffset;				//0x0082		//2字节，电流检测调整值
__RF_CONTROL_EXT__	U16 gCurHlim;				//0x0084		//2字节，功放电流上限,整数在前,小数在后
__RF_CONTROL_EXT__	U16 gCurLlim;				//0x0086		//2字节，功放电流下限,整数在前,小数在后
/*****temperature********/
__RF_CONTROL_EXT__	U8  gGainNumer;				//0x00A2		//1字节，增益温补系数分子
__RF_CONTROL_EXT__	U8  gGainDenomi;			//0x00A3		//1字节，增益温补系数分母
__RF_CONTROL_EXT__	S8  gPAProtecttLim;			//0x00A4		//1字节，功放保护温度门限
__RF_CONTROL_EXT__	S8  gPAResetLim;			//0x00A5		//1字节，功放回复温度
__RF_CONTROL_EXT__ 	U8  gGain;
/*****module*******/
__RF_CONTROL_EXT__	U8  gRF_No;					//0x00C0		//1字节，射频模块编号
__RF_CONTROL_EXT__	U8  gPASW;					//0x00C1      	//1字节，功放开关
__RF_CONTROL_EXT__	U8  gRFSW;					//0x00C2      	//1字节，射频开关
/****************other parameter********************/
__RF_CONTROL_EXT__ 	U8  gAtttempval;			//温度补偿值		
__RF_CONTROL_EXT__ 	U8 	gRFModify;	   			//射频参数修改标志(0:无效，1：有效)//4次
__RF_CONTROL_EXT__ 	U16 gFWCheck;				//程序校验和
__RF_CONTROL_EXT__ 	U8  gDownFlag;				//0x01FF		//1字节，升级标志
__RF_CONTROL_EXT__  U8  gModuleNo[10];			//0x00E4		//10字节，模块序列号
/*********apparatus parameter************/
__RF_CONTROL_EXT__	U8  PaAdjEquModulus;        //0x00F0  	    //1字节, 功放的校准方程的补偿系数
__RF_CONTROL_EXT__	U16 PaLastAdjDate_year;	    //0x00F1	    //2字节，放的最终校准日期(年)
__RF_CONTROL_EXT__	U16 PaLastAdjDate_month;	//0x00F3	    //1字节，放的最终校准日期(月)
__RF_CONTROL_EXT__	U16 PaLastAdjDate_day;	    //0x00F1	    //1字节，放的最终校准日期(年)
__RF_CONTROL_EXT__	U16 PaUpLimit;		        //0x00F5		//2字节，功放上限
__RF_CONTROL_EXT__	U8  AppModel;	            //0x00F7	    //1字节，仪表型号
/******Signal Generator********/
__RF_CONTROL_EXT__	U8  gSigAtt;				//1字节，信号衰减补偿
__RF_CONTROL_EXT__	U16 gSigPower;				//2字节，输出功率
__RF_CONTROL_EXT__	U32 gSigPowerOffset;		//4字节，输出功率及定标值
/****************单次脉冲触发变量********************/
__RF_CONTROL_EXT__	U8 	gPlusSwitchState;     	//脉冲开关状态变量
__RF_CONTROL_EXT__	U8  gPlusValue[4];
__RF_CONTROL_EXT__	S8	gTempValue[6];			//温补操作队列   
__RF_CONTROL_EXT__	U8	gFreqLim[6];
//控制参数
__RF_CONTROL_EXT__ U8	gRFSrcSel;				//信源选择参数
__RF_CONTROL_EXT__ U8	gRFSrcRef;				//参考选择参数
__RF_CONTROL_EXT__ U8 	gLNAState;				//低噪放故障状态
__RF_CONTROL_EXT__ U8 	gLimState;				//限幅状态
__RF_CONTROL_EXT__ U8 	gPLLLock;				//中频失锁状态
__RF_CONTROL_EXT__ U16  gPACurrent;				//功放电流
__RF_CONTROL_EXT__ U16  gPwrOut;				//输出功率
__RF_CONTROL_EXT__ U16  gPwrRef;				//反射功率
__RF_CONTROL_EXT__ U16  gPwrIn;					//输入功率
/* Public function prototypes ------------------------------------------------*/	
#define TaskControlEnter		((Procf)(BANK_CTRL_START_ADDR|0x1UL))
#define InitTaskCtrl()			TaskControlEnter(NULL,1)

__RF_CONTROL_EXT_FUNC__ int TaskControl(int*argv[],int argc);
__RF_CONTROL_EXT_FUNC__ void InitTaskControl(void);
__RF_CONTROL_EXT_FUNC__ JC_COMMAND*GetTable(void);
__RF_CONTROL_EXT_FUNC__ int GetTableMebCnt(void);
#endif/*__RF_CONTROL_H__*/
/********************************END OF FILE***********************************/
