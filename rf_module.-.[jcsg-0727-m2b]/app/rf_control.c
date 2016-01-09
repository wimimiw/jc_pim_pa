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
typedef struct 
{
	S16 pwr1[11];	//0,1,2,3,4,5,6,7,8,9,10 dBm
	S16 pwr2[11];	//-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,-0.1 dBm
	S16 pwr3[11];	//-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-10.1 dBm
	//S16 pwr4[1];	//-80dBm
}ST_SIG_OFFSET;

const ST_SIG_OFFSET gSigOffset = {	{1000,900,800,700,600,500,400,300,200,100,0},
									{-10,-100,-200,-300,-400,-500,-600,-700,-800,-900,-1000},
									{-1010,-1100,-1200,-1300,-1400,-1500,-1600,-1700,-1800,-1900,-2000},};
									//{-8000}};
/* Private define ------------------------------------------------------------*/	
#define AD4350_PWR_LIM			(3)   //-3dBm
#define IS_ALARM_TEMPERATURE()	(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) != Bit_SET)   	//�¶Ⱦ���
#define IS_ALARM_CURRENT()		(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) != Bit_SET)	//��������
#define IS_ALARM_VSWR()			(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) != Bit_SET)	//פ������
#define IS_VCO_LOCK()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7 )== Bit_SET)	//VCO����
#define IS_ALC_LOCK()			(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)== Bit_SET)	//ALC����
//VCOƬѡʹ��
#define VCO_CE(x)				(GPIO_WriteBit(GPIOA,GPIO_Pin_3,(x) == TRUE ? Bit_SET : Bit_RESET))
//ǰ���ʼ첨ѡ��				�ߵ�ƽ(1) = ��ƽ���� >< �͵�(0) = ��Ƶ����
#define PWR_DET_SELECT(x)		(GPIO_WriteBit(GPIOA,GPIO_Pin_0,(x) == TRUE ? Bit_SET : Bit_RESET))
//ǰ���ʼ첨����
#define IS_OVER_3V()			(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == Bit_SET)
//ǰ���ʲο���ȡ
#define PWRDTREF_RO(x)			(x = 0)
//ǰ���ʵ�ƽ��ȡ
#define PWRDTADJ_RO(x)			(ReadAdcValue(ADC_Channel_1,&(x)))
//��Դ�л�
#define SOURCE_SWITCH(x)		do{GPIO_WriteBit(GPIOA,GPIO_Pin_14,(x == SRC_INTERNAL)?Bit_SET:Bit_RESET);\
								   GPIO_WriteBit(GPIOA,GPIO_Pin_15,(x != SRC_INTERNAL)?Bit_SET:Bit_RESET);}while(0)

#define PA_POWER_SWITCH(x)		(GPIO_WriteBit(GPIOB,GPIO_Pin_8,(x) == TRUE ? Bit_SET : Bit_RESET))
#define PA_RESET(x)				(GPIO_WriteBit(GPIOB,GPIO_Pin_9,(x) == TRUE ? Bit_SET : Bit_RESET))

//#define setAtt(att)     		(WritePe4302(&attBus[1],(att)))
#define setALCRef(x)			(WriteAD5324((x),'B'))
#define GET_SIG_TABLE_MEMCNT()	(sizeof(ST_SIG_OFFSET)/sizeof(S16))

#define CALIB_OUT_RANGEL		(-10001)
#define CALIB_ERR_LOW			(-10002)
#define CALIB_ERR_VLOW			(-10003)
#define CALIB_ERR_VHIGH			(-10004)

#define SIGNAL_POWER_LOW		(-8000)
/* Private macro -------------------------------------------------------------*/
const U8 BootloaderV 	__attribute__((at(ADDR_BYTE_BOOTLOADERV))) = 0x11;
const U8 SoftwareV 		__attribute__((at(ADDR_BYTE_SOFTWAREV  ))) = 0x62;
const U8 HardwareV 		__attribute__((at(ADDR_BYTE_HARDWAREV  ))) = 0x50;
/* Private variables ---------------------------------------------------------*/
static BOOL execAtt1Set(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execSwitchSource(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execSwitchReference(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execALC(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execVCO(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execVCOLim(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execRFSW(U8 flag,U8 *buf,U16 rxLen,U16*txLen);	
static BOOL execSigAtt(U8 flag,U8 *buf,U16 rxLen,U16*txLen);								   
static BOOL execSigPower(U8 flag,U8 *buf,U16 rxLen,U16*txLen);
static BOOL execSigCalibrate(U8 flag,U8 *buf,U16 rxLen,U16*txLen);
					   
const JC_COMMAND tabInfo[] = {	
//�ο�ѡ��	
	{ID_FCT_PARAM_WR,EE_SOURCE_REFERENCE,0,(U8*)&gRFSrcRef,sizeof(gRFSrcRef)			,0,0,execSwitchReference},
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
	{ID_FCT_PARAM_WR,EE_PASW			,0,(U8*)&gPASW,sizeof(gPASW)					,0,0,NULL},//0x00C1     //1�ֽڣ����ſ���		
	{ID_FCT_PARAM_WR,EE_RFSW			,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,execRFSW},//0x00C2     //1�ֽڣ���Ƶ��?
	{ID_FCT_PARAM_WR,EE_TEST_MARK		,0,(U8*)&gRFSW,sizeof(gRFSW)					,0,0,NULL},//0x00C2     //1�ֽڣ���Ƶ����?	
/*********Signal Generator Mode************/
	{ID_FCT_PARAM_WR,EE_SIG_ATT			,0,(U8*)&gSigAtt,sizeof(gSigAtt)				,0,0,execSigAtt},		//0x0160		//�ź�˥������
	{ID_FCT_PARAM_WR,EE_SIG_POW			,0,(U8*)&gSigPower,sizeof(gSigPower)			,0,0,execSigPower},		//0x0162		//�����������
	{ID_FCT_PARAM_WR,EE_SIG_POW_OFFSET	,0,(U8*)&gSigPowerOffset,sizeof(gSigPowerOffset),0,0,execSigCalibrate},	//0x0164		//����������ʼ�����ֵ	
	{ID_FCT_PARAM_WR,EE_SIG_FREQ_STEP	,0,(U8*)&gSigFreqStep,sizeof(gSigFreqStep)		,0,0,NULL},				//0x0168		//���ö���Ƶ�ʲ���	
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
/**
  * @brief  :��������
  * @param  :
  * @retval :
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
static void QuickSort(U16 *a, int left, int right)
{
    int i = left;
    int j = right;
    int key = a[left];
	
    if(left >= right)/*�������������ڻ��ߵ����ұߵ������ʹ������һ����*/
    {
        return ;
    }
     
    while(i < j)/*������һ������Ѱ��һ��*/
    {
        while(i < j && key <= a[j])
        /*��Ѱ�ҽ������������ǣ�
		1���ҵ�һ��С�ڻ��ߴ���key���������ڻ�С��ȡ�������������ǽ���?
		2��û�з�������1�ģ�����i��j�Ĵ�Сû�з�ת*/ 
        {
            j--;/*��ǰѰ��*/
        }
         
        a[i] = a[j];
        /*�ҵ�һ������������Ͱ�������ǰ��ı����ߵ�i��ֵ�������һ��ѭ����Key��a[left],��ô���Ǹ�key��*/
         
        while(i < j && key >= a[i])
        /*����i�ڵ�������ǰѰ�ң�ͬ�ϣ�����ע����key�Ĵ�С��ϵֹͣѭ���������෴����Ϊ����˼���ǰ����������ӣ�
		�����������ߵ�����С��key�Ĺ�ϵ�෴*/
        {
            i++;
        }
         
        a[j] = a[i];
    }
     
    a[i] = key;/*���ڵ���������һ���Ժ�Ͱ��м���key�ع�*/
    QuickSort(a, left, i - 1);/*�����ͬ���ķ�ʽ�Էֳ�������ߵ�С�����ͬ�ϵ�����*/
    QuickSort(a, i + 1, right);/*��ͬ���ķ����Էֳ������ұߵ�С�����ͬ�ϵ�����*/
                       /*��Ȼ�����ܻ���ֺܶ�����ң�ֱ��ÿһ���i=jΪֹ*/
}

/**
  * @brief  :��ȡ��ָ��Ƶ�ʺ͹����µĶ���ֵ
  * @param  :freq ��λ1MHz, power ��λ 0.01dBm
  * @retval :
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL GetSigOffsetWithPower(U16 freq,S16 pwr100,U16 *offset,BOOL *norFlag)
{	
	/*�ź�Դ����������˼·��
	  1��ʹ��ָ������ֵ�б���о�ȷ���ꡣ �������� = �����б��Ա����*�������
	  2��ʹ�ù��ƹ���ֵ���н��ж��ꡣ	  �������� = �����б��Ա���� + ���ʱ߽綨�����
	*/	
	
	U8 result = FALSE;
	U16 net_offset = 0,of[2]={0},freqMHz,freq10MHz;
	S16 *ptr = (S16*)&gSigOffset,k;	
	
	*norFlag = TRUE;
	
	if(offset == NULL) return FALSE;
		
	freqMHz = freq;
	freq10MHz = freq/10;
	
//	if( (pwr100 < 0 && pwr100 > -10 ) || ( pwr100 < -1000 && pwr100 > -1010 ))
//	{
//		*offset = 0;
//		*norFlag = FALSE;
//		return TRUE;
//	}
	
	if( pwr100 == SIGNAL_POWER_LOW )
	{
		result = ReadE2prom(EEPROM_SIG1_START + freqMHz*sizeof(U16),(U8*)of,sizeof(U16));
		if((of[0]&(U16)(1<<15))!=0)*norFlag = FALSE;
		of[0] &=~(1<<15);
		*offset = of[0];
		return result;
	}	
	
	for(net_offset = 0;net_offset<GET_SIG_TABLE_MEMCNT();net_offset++)
	{
		if(((net_offset+1)< GET_SIG_TABLE_MEMCNT()) && (ptr[net_offset] >= pwr100) && (ptr[net_offset+1] < pwr100))
		{
			break;
		}		
	}	

	if(net_offset == GET_SIG_TABLE_MEMCNT())net_offset-=1;
	
	result = ReadE2prom(EEPROM_SIG2_START + freq10MHz*sizeof(ST_SIG_OFFSET) + net_offset*sizeof(S16),(U8*)of,sizeof(of));
	
	if( pwr100 == -2000 )
	{
		if((of[0]&(U16)(1<<15))!=0)*norFlag = FALSE;
		of[0] &=~(1<<15);
		*offset = of[0];
	}
	else
	{	
		if(of[0] == 0xFFFF || of[1] == 0xFFFF)
		{
			of[0] = 0;
			of[1] = 0;
			*norFlag = FALSE;
		}
			
		if((of[0]&(U16)(1<<15))!=0)*norFlag = FALSE;
		if((of[1]&(U16)(1<<15))!=0)*norFlag = FALSE;
		
		of[0] &=~(1<<15);
		of[1] &=~(1<<15);		
		
		if(ptr[net_offset] == ptr[net_offset+1])return FALSE;
		k = (100*((S16)of[0]-(S16)of[1]))/(ptr[net_offset] - ptr[net_offset+1]);
		*offset = k * (pwr100 - ptr[net_offset])/100 + of[0];
	}	
	
	return result;
}
/**
  * @brief  :������ָ��Ƶ�ʺ͹����µĶ���ֵ
  * @param  :freq ��λ1MHz, power ��λ 0.01dBm
  * @retval :
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL SetSigOffsetWithPower(U16 freq,S16 pwr100,U16 offset,BOOL norFlag)
{
	U8 result = FALSE;
	U16 net_offset = 0;
	S16 *ptr = (S16*)&gSigOffset;
	
	offset &= 0x0FFF;
	
	if(norFlag == FALSE)offset |= 1<<15;
	
	if(pwr100 == SIGNAL_POWER_LOW)
	{		
		result = WriteE2prom(EEPROM_SIG1_START + freq*sizeof(offset),(U8*)&offset,sizeof(offset));				
	}
	else
	{
		freq /= 10;
		
		for(net_offset = 0;net_offset<GET_SIG_TABLE_MEMCNT();net_offset++)
		{
			if(ptr[net_offset] == pwr100)
			{
				result = WriteE2prom(EEPROM_SIG2_START + freq*sizeof(ST_SIG_OFFSET) + net_offset*sizeof(S16),		
				(U8*)&offset,sizeof(offset));			
				break;
			}
		}
	}
	
	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
int GetTableMebCnt(void)
{	
	return sizeof(tabInfo)/sizeof(JC_COMMAND);
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
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
	WriteSpiOneWord(SPI_VCO,&spiType,R5_INIT);

	counterTemp = divSel << 20;
	counterTemp|= R4_INIT|(enable << 5)|(power<<3);
	//R4
	WriteSpiOneWord(SPI_VCO,&spiType,counterTemp);
	//R3
	WriteSpiOneWord(SPI_VCO,&spiType,R3_INIT);
	//R2
	WriteSpiOneWord(SPI_VCO,&spiType,R2_INIT);
						  						
	counterTemp = (freqRef/freqStep)<<3;
	counterTemp|= R1_INIT;
	//R1
	WriteSpiOneWord(SPI_VCO,&spiType,counterTemp);
		
	counterTemp  = ((counterN%freqRef)/freqStep)<<3;
	counterN  	 = (counterN/freqRef)<<15;											
	counterTemp |= counterN;	
	//R0
	WriteSpiOneWord(SPI_VCO,&spiType,counterTemp);
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
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
	
	WriteSpiOneWord(SPI_DAC,&spiType,value);
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
U16 ReadPowerADC(void)
{
	U8 i;
	U16 adcValue,refValue = 0;
	U32 sumAdc=0,sumRef=0;
	static U16 aBuf[10]={0},rBuf[10]={0};
		
	for(i = 0;i<10;i++)
	{
		PWRDTADJ_RO(aBuf[i]);
		PWRDTREF_RO(rBuf[i]);	
	}
	
	QuickSort(aBuf,0,9);
	QuickSort(rBuf,0,9);
	
	for(i = 1;i<9;i++)
	{
		sumAdc+=aBuf[i];
		sumRef+=rBuf[i];
	}
	
	adcValue = sumAdc/8;
	refValue = sumRef/8;
		
	if(IS_OVER_3V())
	{		
		PWRDTREF_RO(refValue);
		adcValue += refValue;
	}
	
	//��һ��
	adcValue = 4096 * adcValue / ( 4096 + refValue );
		
	return adcValue/4;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL SetSigPowerAtt(S16 pwr100)
{
	U8 att1,att2;
	BOOL result = FALSE;
	
	if(pwr100 <= 1000 && pwr100 >= 0)
	{
		if(gCenFreq < 1800000)
		{
			gAtt1 = 40;att1=0;att2=0;
		}
		else
		{
			gAtt1 = 10;att1=0;att2=0;
		}
	}
	else if(pwr100 <= -10 && pwr100 >= -1000)
	{
		if(gCenFreq < 1800000)
		{
			gAtt1 = 40;att1=20;att2=0;
		}
		else
		{
			gAtt1 = 10;att1=10;att2=0;
		}
	}
	else if(pwr100 <= -1010 && pwr100 >= -2000)
	{
		if(gCenFreq < 1800000)
		{
			gAtt1 = 40;att1=40;att2=0;
		}
		else
		{
			gAtt1 = 10;att1=20;att2=0;
		}
	}
	else
	{
		if(gCenFreq < 700000)
		{
			gAtt1 = 0;att1=20;att2=20;			
		}
		else
		{
			gAtt1 = 0;att1=30;att2=30;
		}
		
		WritePLL(gCenFreq,gRefFreq,gFreqStep,0,FALSE);
	}	
		
	result|= WritePe4302(SPI_ATT1,att1);
	result|= WritePe4302(SPI_ATT2,att2);
	result|= WritePe4302(SPI_ATT3,gAtt1);
	
	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL SetSigPowerAttLow(u16 att)
{
	//att (0~126)
	BOOL result = FALSE;
	
	if(att > 126)att = 126;
		
	if((att%2) == 0)
	{
		result|= WritePe4302(SPI_ATT1,(u8)att/2);
		result|= WritePe4302(SPI_ATT2,(u8)att/2);
	}
	else
	{
		result|= WritePe4302(SPI_ATT1,(u8)att/2);
		result|= WritePe4302(SPI_ATT2,(u8)(att/2)+1);	
	}
	
	result|= WritePe4302(SPI_ATT3,0);	
	return result;	
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
static BOOL execSigAtt(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	U8 result = FALSE;
	
	if(flag == TRUE)
	{
		result = WritePe4302(SPI_ATT2,gSigAtt);
	}
	
	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
static BOOL execSigPower(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	BOOL result = TRUE;
	BOOL norFlag;
	S16 k;
	S16 pwr100 = *(S16*)&buf[13]; //����ֵ*100��������
	U16 offset,offsetH,offsetL,temp16;
	U32 freqMHz,freqTemp;
	
	if( pwr100 > 1000 || (pwr100 < -2000 && pwr100 > SIGNAL_POWER_LOW) || pwr100 < SIGNAL_POWER_LOW)
	{
		k = CALIB_OUT_RANGEL;
		memcpy(buf+13,(U8*)&k,sizeof(k));
		return TRUE;
	}
	
	freqMHz = gCenFreq/1000;
	
	if (pwr100 < 0 && pwr100 > -10)
	{
		pwr100 = 0;				
	}
	else if (pwr100 < -1000 && pwr100 > -1010)
	{
		pwr100 = -1000;				
	}	
		
	if(flag == TRUE)
	{		
		SetSigPowerAtt(pwr100);	
		
		if(pwr100 == SIGNAL_POWER_LOW)
		{
			result = GetSigOffsetWithPower(freqMHz,pwr100,&offset,&norFlag);
			if(norFlag == FALSE)*(S16*)&buf[13] = CALIB_ERR_LOW;
			SetSigPowerAttLow(offset);
			offset = 4000;
			//memcpy(buf+13,(U8*)&offset,2);
		}
		else
		{						
			if(gSigFreqStep == 0xFF)gSigFreqStep = 1;
			
			temp16 = gSigFreqStep*10;
			freqTemp = freqMHz/temp16*temp16;
			
			result = GetSigOffsetWithPower(freqTemp,pwr100,&offsetL,&norFlag);
			if(norFlag == FALSE)*(S16*)&buf[13] = CALIB_ERR_VLOW;	
			result = GetSigOffsetWithPower(freqTemp+temp16,pwr100,&offsetH,&norFlag);		
			if(norFlag == FALSE)*(S16*)&buf[13] = CALIB_ERR_VHIGH;					
			
			k = 100*((S16)offsetH - (S16)offsetL)/100;//б��*100Ȼ���100=10MHz/100Khz			
			
			if(k>=0)
				offset = offsetL + (abs(k)*(gCenFreq/100 - freqMHz/10*100)/100);
			else
				offset = offsetL - (abs(k)*(gCenFreq/100 - freqMHz/10*100)/100);
			
			//memcpy(buf+13,(U8*)&offset,2);
			//k = k*(gCenFreq/100 - freqMHz/10*100);
			//offset = (S16)(-50*(gCenFreq/100 - freqMHz/10*100)/100 + 100);
			//memcpy(buf+13,(U8*)&offset,2);
		}
		
		if(offset<4096)setALCRef(offset);
	}
	
	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
static BOOL execSigCalibrate(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	BOOL result = TRUE;
	BOOL norFlag;
	
	S16 pwr100 = *(S16*)(buf+13); //����ֵ*100��������
	U16 offset = *(U16*)(buf+15);
	
	if( pwr100 > 1000 || (pwr100 < -2000 && pwr100 > SIGNAL_POWER_LOW) || pwr100 < SIGNAL_POWER_LOW)
	{
		pwr100 = CALIB_OUT_RANGEL;
		memcpy(buf+13,(U8*)&pwr100,sizeof(pwr100));
		return TRUE;
	}	
	
	if(flag == TRUE)
	{		
		result = SetSigPowerAtt(pwr100);
				
		result = SetSigOffsetWithPower(gCenFreq/1000,pwr100,offset&0xfff,((offset&(1<<15))==0?TRUE:FALSE));
				
		if(pwr100 == SIGNAL_POWER_LOW)
		{
			SetSigPowerAttLow(offset);offset = 4000;
		}
		//memcpy(buf+13,(U8*)&result,2);
		
		if(offset<4096)setALCRef(offset);
	}
	else if(flag == 2)//flag = 2 ��ʾ����ָ��������ѯ,������flag = 0
	{
		result = GetSigOffsetWithPower(gCenFreq/1000,pwr100,&offset,&norFlag);
		//if(norFlag == FALSE)*(S16*)&buf[15] = -10001;			
		memcpy(buf+15,(U8*)&offset,sizeof(offset));		
	}		
	
	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL execAtt1Set(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{	
	if(flag == TRUE)
	{
		//д˥����
		//setAtt(gAtt1);	
		if( gAtt1 < 64 )
		{
			WritePe4302(SPI_ATT3,gAtt1-0);
		}
		else if( gAtt1 >= 64 && gAtt1 < 128)
		{
			WritePe4302(SPI_ATT1,gAtt1-64);
		}
		else if( gAtt1 >= 128 && gAtt1 < 192)
		{
			WritePe4302(SPI_ATT2,gAtt1-128);
		}		
	}
	return TRUE;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
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
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL execSwitchReference(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	if(flag == TRUE)
	{	

	}
	return TRUE;	
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL execALC(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		//ALC�����޷�
		//setALCRef(gPALim * 3.3 / 1023.0 * 4095 / 5.0 );	
		setALCRef(gPALim*4);				
	}
	return TRUE;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL execVCO(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
	if(flag == TRUE)
	{	
		//дVCO
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	}
	return TRUE;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL execRFSW(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{		
//	UserTimerDef Timer;
	
	if(flag == TRUE)
	{	
//		//�¶Ⱦ���	�������� פ������
//		if(IS_ALARM_TEMPERATURE()||IS_ALARM_CURRENT()||IS_ALARM_VSWR())
//		{
//			PA_RESET(TRUE);
//			
//			UserTimerReset(TIM2,&Timer);
//			while(FALSE == UserTimerOver(TIM2,&Timer,USER_TIMER_10MS(5)));
//			
//			PA_RESET(FALSE);
//		}

		VCO_CE(gRFSW);		
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSW);
		
		PA_POWER_SWITCH(gRFSW);
	}
	return TRUE;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
BOOL execVCOLim(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{	
	U8  tbuf[20];
	U16 len;	
	
	if(flag == TRUE)
	{	
		gCenFreq = *(U32*)&gFreqLim[0];
		WriteE2prom(EE_CenFreq,(U8*)&gCenFreq,sizeof(gCenFreq));
					
		if( buf[12] == 6 )
		{
			gPALim = *(U16*)&gFreqLim[4];	
			//ALC�����޷�
			setALCRef(gPALim*4);			
		}			
		//дVCO
		WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
		//����Ƶ�ʺ͹��ʵ�������ֵ	
		*(S16*)(tbuf+13) = gSigPower;
		execSigPower(TRUE,tbuf,sizeof(tbuf),&len);				
	}
	return TRUE;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */
void InitTaskControl(void)
{		
	gRFModify = FALSE;
	gPlusSwitchState = CLOSE;
	gPASW = CLOSE;
	gRFSW = CLOSE;
	gRFSrcSel = SRC_INTERNAL;	
	
	//ǰ���ʼ���л�����Ƶ ����ģʽ
	PWR_DET_SELECT(0);
	
	SOURCE_SWITCH(gRFSrcSel);		
	
	PA_POWER_SWITCH(FALSE);
	
	//WritePe4302(SPI_ATT1, 0);
	//WritePe4302(SPI_ATT2, 0);
	//WritePe4302(SPI_ATT3,63);
	
	//setAtt(gAtt1);
	setALCRef(0);
	//WritePe4302(SPI_ATT2,40);
	
	//VCO_CE(TRUE);
	//WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
	VCO_CE(FALSE);
	
	//gRFSrcSel = SRC_EXTERNAL;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author :mashuai
  * @version:
  * @date	:2015.11.9
  */		
int TaskControl(int*argv[],int argc)
{		
	//�޷�״̬
	gLimState = IS_ALC_LOCK();
	//VCO����״̬
	gPLLLock  = IS_VCO_LOCK();
//	//����ֵԤ��	
//	gPACurrent = 500;
//	//��ǰ����
//	gPwrOut = ReadPowerADC(); 
//	gPwrRef	= gPwrOut/30;
	//��ȡ��ǰ�¶�
	gCurRfTemp = ReadInternalTemperatureSensor();	
//	//�¶Ⱦ���	
//	if(IS_ALARM_TEMPERATURE())
//	{
//		gCurRfTemp = 80*2;
//		
//		gPASW = FALSE;
//		gRFSW = FALSE;
//		PA_POWER_SWITCH(gRFSW);
//	}
//	//��������
//	if(IS_ALARM_CURRENT())
//	{
//		gPACurrent = 10000;
//	}
//	//פ������
//	if(IS_ALARM_VSWR())
//	{
//		gPwrRef	= gPwrOut-10;
//	}
//	//�����仯���޸�
//	if(TRUE == gRFModify)
//	{		
//		gRFModify = FALSE;
//		//PA_POWER_SWITCH(gPASW);		
//		//д˥����
//		//setAtt(gAtt1);
//		//�л���Դ
//		//SOURCE_SWITCH(gRFSrcSel);
//		//������Դ
//		//VCO_CE(gRFSrcSel == SRC_INTERNAL && gRFSW == TRUE);		
//		//ALC�����޷�
//		//setALCRef(gPALim*4);		
//		//WritePLL(gCenFreq,gRefFreq,gFreqStep,AD4350_PWR_LIM,gRFSrcSel == SRC_INTERNAL?TRUE:FALSE);
//	}
	
//	GPIO_WriteBit(GPIOB,GPIO_Pin_12,(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == Bit_RESET) ? Bit_SET : Bit_RESET );
		
	return 1;
}




