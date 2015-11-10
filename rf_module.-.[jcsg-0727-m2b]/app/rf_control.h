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

/******************�Ϲ⹦�Ų���**********************/
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

//������ѡ��ʹ�õ�ADCͨ��
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
__RF_CONTROL_EXT__	S16 gCurRfTemp;				//��ǰ�¶�ֵ
__RF_CONTROL_EXT__	S16 gPreRfTemp;				//��һ���¶�ֵ
__RF_CONTROL_EXT__	S16 gainCo;
/****debug****/
__RF_CONTROL_EXT__	S16 gTestRfTemp;			//�����¶�ֵ
__RF_CONTROL_EXT__	U8 	gTestflag;				//���������־��0Ϊ����״̬��1Ϊ����״̬
__RF_CONTROL_EXT__	U8  gTempgain;				//1�ֽڣ��²���׼ֵ�趨
__RF_CONTROL_EXT__	U8 	gBaseTemp;				//�¶Ȼ�׼ֵ
/****gain*****/
__RF_CONTROL_EXT__	U8  gAtteVal;				//0x0001      	//1�ֽڣ������趨ֵ
__RF_CONTROL_EXT__	U8  gGainOffset;			//0x0002		//1�ֽڣ��������ֵ
__RF_CONTROL_EXT__	U8  gAtt1;					//0x0003		//1�ֽڣ�1#˥������1U5
__RF_CONTROL_EXT__	U8  gAtt2;					//0x0003		//1�ֽڣ�1#˥������1U5
/****PA******/
__RF_CONTROL_EXT__	U16 gDAoutA;				//0x0034		//2�ֽڣ���������޷�ֵ
__RF_CONTROL_EXT__	U16 gDAoutB;				//0x0036		//2�ֽڣ���������޷�ֵ
__RF_CONTROL_EXT__	U16 gDAoutC;				//0x0038		//2�ֽڣ���������޷�ֵ
__RF_CONTROL_EXT__	U16 gDAoutD;				//0x003A		//2�ֽڣ���������޷�ֵ
__RF_CONTROL_EXT__	U16 gPALim;					//0x0020		//2�ֽڣ���������޷�ֵ
__RF_CONTROL_EXT__ 	U16 gOutPwrVal;            	//0x0022		//2�ֽڣ�������ʶ���ֵ
__RF_CONTROL_EXT__	U16 gOutPwrSlopeVal;		//0x0024		//2�ֽڣ���������趨ֵ����
__RF_CONTROL_EXT__	U16 gRefPwrVal;		        //0x0026		//2�ֽڣ����书�ʶ���
__RF_CONTROL_EXT__	U16 gRefPwrSlopeVal; 		//0x0028		//2�ֽڣ����书��б��ֵ
__RF_CONTROL_EXT__	U8  gOutPwrShutVal;			//0x002e		//1�ֽڣ�������ʹر�����
__RF_CONTROL_EXT__ 	S8  gPALimNumer;		    //0x0075		//1�ֽڣ���������޷���������	    
__RF_CONTROL_EXT__ 	S8  gPReadADNumer;	        //0x0076		//1�ֽڣ�������ʶ�ȡ��������	    
__RF_CONTROL_EXT__ 	S8  gPequDenomi;	        //0x0077		//1�ֽڣ����ʲ�����ĸ	  
/***frequency**	*/
__RF_CONTROL_EXT__	U32 gPBmFreq;				//0x0078		//4�ֽڣ���׼Ƶ��
/***frequency***/
__RF_CONTROL_EXT__	U32 gCenFreq;				//0x0040		//4�ֽڣ�����Ƶ�� KHzΪ��λ
__RF_CONTROL_EXT__	U32 gBandWidth;				//0x0044		//4�ֽڣ�����
__RF_CONTROL_EXT__	S16 gIFOffset;				//0x0048		//2�ֽڣ���Ƶ����ֵ
__RF_CONTROL_EXT__	U16 gFreqStep;				//0x004A		//2�ֽڣ�Ƶ�ʲ���ֵ
__RF_CONTROL_EXT__	U32 gIFFreq;				//0x004C		//4�ֽڣ���ƵƵ��ֵ
__RF_CONTROL_EXT__	U32 gFreq;					//0x0050		//4�ֽڣ�����Ƶ��
__RF_CONTROL_EXT__	U32 gRefFreq;				//0x0054		//4�ֽڣ��ο�Ƶ��
/*****current*****/
__RF_CONTROL_EXT__	U16 gCurCo;					//0x0080		//2�ֽڣ�����б��
__RF_CONTROL_EXT__	U16 gCurOffset;				//0x0082		//2�ֽڣ�����������ֵ
__RF_CONTROL_EXT__	U16 gCurHlim;				//0x0084		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
__RF_CONTROL_EXT__	U16 gCurLlim;				//0x0086		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
/*****temperature********/
__RF_CONTROL_EXT__	U8  gGainNumer;				//0x00A2		//1�ֽڣ������²�ϵ������
__RF_CONTROL_EXT__	U8  gGainDenomi;			//0x00A3		//1�ֽڣ������²�ϵ����ĸ
__RF_CONTROL_EXT__	S8  gPAProtecttLim;			//0x00A4		//1�ֽڣ����ű����¶�����
__RF_CONTROL_EXT__	S8  gPAResetLim;			//0x00A5		//1�ֽڣ����Żظ��¶�
__RF_CONTROL_EXT__ 	U8  gGain;
/*****module*******/
__RF_CONTROL_EXT__	U8  gRF_No;					//0x00C0		//1�ֽڣ���Ƶģ����
__RF_CONTROL_EXT__	U8  gPASW;					//0x00C1      	//1�ֽڣ����ſ���
__RF_CONTROL_EXT__	U8  gRFSW;					//0x00C2      	//1�ֽڣ���Ƶ����
/****************other parameter********************/
__RF_CONTROL_EXT__ 	U8  gAtttempval;			//�¶Ȳ���ֵ		
__RF_CONTROL_EXT__ 	U8 	gRFModify;	   			//��Ƶ�����޸ı�־(0:��Ч��1����Ч)//4��
__RF_CONTROL_EXT__ 	U16 gFWCheck;				//����У���
__RF_CONTROL_EXT__ 	U8  gDownFlag;				//0x01FF		//1�ֽڣ�������־
__RF_CONTROL_EXT__  U8  gModuleNo[10];			//0x00E4		//10�ֽڣ�ģ�����к�
/*********apparatus parameter************/
__RF_CONTROL_EXT__	U8  PaAdjEquModulus;        //0x00F0  	    //1�ֽ�, ���ŵ�У׼���̵Ĳ���ϵ��
__RF_CONTROL_EXT__	U16 PaLastAdjDate_year;	    //0x00F1	    //2�ֽڣ��ŵ�����У׼����(��)
__RF_CONTROL_EXT__	U16 PaLastAdjDate_month;	//0x00F3	    //1�ֽڣ��ŵ�����У׼����(��)
__RF_CONTROL_EXT__	U16 PaLastAdjDate_day;	    //0x00F1	    //1�ֽڣ��ŵ�����У׼����(��)
__RF_CONTROL_EXT__	U16 PaUpLimit;		        //0x00F5		//2�ֽڣ���������
__RF_CONTROL_EXT__	U8  AppModel;	            //0x00F7	    //1�ֽڣ��Ǳ��ͺ�
/******Signal Generator********/
__RF_CONTROL_EXT__	U8  gSigAtt;				//1�ֽڣ��ź�˥������
__RF_CONTROL_EXT__	U16 gSigPower;				//2�ֽڣ��������
__RF_CONTROL_EXT__	U32 gSigPowerOffset;		//4�ֽڣ�������ʼ�����ֵ
/****************�������崥������********************/
__RF_CONTROL_EXT__	U8 	gPlusSwitchState;     	//���忪��״̬����
__RF_CONTROL_EXT__	U8  gPlusValue[4];
__RF_CONTROL_EXT__	S8	gTempValue[6];			//�²���������   
__RF_CONTROL_EXT__	U8	gFreqLim[6];
//���Ʋ���
__RF_CONTROL_EXT__ U8	gRFSrcSel;				//��Դѡ�����
__RF_CONTROL_EXT__ U8	gRFSrcRef;				//�ο�ѡ�����
__RF_CONTROL_EXT__ U8 	gLNAState;				//����Ź���״̬
__RF_CONTROL_EXT__ U8 	gLimState;				//�޷�״̬
__RF_CONTROL_EXT__ U8 	gPLLLock;				//��Ƶʧ��״̬
__RF_CONTROL_EXT__ U16  gPACurrent;				//���ŵ���
__RF_CONTROL_EXT__ U16  gPwrOut;				//�������
__RF_CONTROL_EXT__ U16  gPwrRef;				//���书��
__RF_CONTROL_EXT__ U16  gPwrIn;					//���빦��
/* Public function prototypes ------------------------------------------------*/	
#define TaskControlEnter		((Procf)(BANK_CTRL_START_ADDR|0x1UL))
#define InitTaskCtrl()			TaskControlEnter(NULL,1)

__RF_CONTROL_EXT_FUNC__ int TaskControl(int*argv[],int argc);
__RF_CONTROL_EXT_FUNC__ void InitTaskControl(void);
__RF_CONTROL_EXT_FUNC__ JC_COMMAND*GetTable(void);
__RF_CONTROL_EXT_FUNC__ int GetTableMebCnt(void);
#endif/*__RF_CONTROL_H__*/
/********************************END OF FILE***********************************/
