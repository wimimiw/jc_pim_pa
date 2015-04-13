/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: rfparam.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��6��1��
**�� �� ��  ��: V1.5
**��        ��: rfparam ͷ�ļ�				
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.4
** �ա���: 2006��4��24��
** �衡��: hex�ļ��汾�Ÿ�Ϊ1.1
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.5
** �ա���: 2006��6��1��
** �衡��: hex�ļ��汾�Ÿ�Ϊ1.2
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

#define debug 0
#define NEW_PLAD

#ifdef NEW_PLAD
	#define DAPOWER_LIM_CHAN 'A'
#else
	#define DAPOWER_LIM_CHAN 'B'
#endif	

sbit LimYes = P2^1;
sbit RFSrcSelect = P1^6;
sbit RFswitch = P1^7;
//sbit PAswitch = P0^3;
sbit SrcSwitch = P0^3;
sbit LD = P0^6;
sbit LD2 = P2^2;
sbit LockYes = P2^3;
sbit V33C = P2^6;   //add by dw  
//sbit rfmute = P2^7;
sbit  LE1=P2^7;						  
/*������ر�־*/
#define	UNDO		0x00		
#define	DOING		0x01
#define	DONE		0x10

#define FALSE 		0
#define TRUE 		1

#define CLOSE 		0
#define OPEN 		1

#define SRC_INTERNAL	0
#define SRC_EXTERNAL 	1

#define	 Ver_High  0x05
#define  Ver_Low  0x04

//#define	WAKE_UP	0x3C04				//ADоƬMAX5741����	

//������ѡ��ʹ�õ�ADCͨ��
#define CHANNEL_0	0x01			//ADת��ͨ��
#define CHANNEL_1	0x02
#define CHANNEL_2	0x04
#define CHANNEL_3	0x08
#define CHANNEL_4	0x10
#define CHANNEL_5	0x20
#define CHANNEL_6	0x40
#define CHANNEL_7	0x80

#define TEMP_LOW  				__temp_que[0]
#define TEMP_LOW_VALUE  	__temp_que[1]
#define TEMP_NORMAL  			__temp_que[2]
#define TEMP_NORMAL_VALUE __temp_que[3]
#define TEMP_HIGH  				__temp_que[4]
#define TEMP_HIGH_VALUE  	__temp_que[5]

APP_EXT	int gcurRfTemp;				//��ǰ�¶�ֵ
APP_EXT	int gpreRfTemp;				//��һ���¶�ֵ
APP_EXT	int gainCo;
/****debug****/
APP_EXT	int gtestRfTemp;				//�����¶�ֵ
APP_EXT	unsigned char gtestflag;		//���������־��0Ϊ����״̬��1Ϊ����״̬
APP_EXT	unsigned char xdata gTempgain;	//1�ֽڣ��²���׼ֵ�趨
APP_EXT	unsigned char xdata	gBaseTemp;	//�¶Ȼ�׼ֵ
/****gain*****/
APP_EXT	unsigned char xdata gAtteVal;		//0x0001      	//1�ֽڣ������趨ֵ
APP_EXT	unsigned char xdata gGainOffset;	//0x0002		//1�ֽڣ��������ֵ
APP_EXT	unsigned char xdata gAtt1;						//0x0003		//1�ֽڣ�1#˥������1U5

/****PA******/
APP_EXT	unsigned int idata gDAoutA;			//0x0034		//2�ֽڣ���������޷�ֵ
APP_EXT	unsigned int idata gDAoutB;			//0x0036		//2�ֽڣ���������޷�ֵ
APP_EXT	unsigned int idata gDAoutC;			//0x0038		//2�ֽڣ���������޷�ֵ
APP_EXT	unsigned int idata gDAoutD;			//0x003A		//2�ֽڣ���������޷�ֵ

APP_EXT	unsigned int idata gPALim;			//0x0020		//2�ֽڣ���������޷�ֵ
APP_EXT int xdata gOutPwrVal;               //0x0022		//2�ֽڣ�������ʶ���ֵ
APP_EXT	unsigned int xdata gOutPwrSlopeVal;	//0x0024		//2�ֽڣ���������趨ֵ����
APP_EXT	int xdata gRefPwrVal;		        //0x0026		//2�ֽڣ����书�ʶ���
APP_EXT	unsigned int xdata gRefPwrSlopeVal; //0x0028		//2�ֽڣ����书��б��ֵ
APP_EXT	unsigned char xdata gOutPwrShutVal;		//0x002e		//1�ֽڣ�������ʹر�����
APP_EXT char xdata gPALimNumer;		        //0x0075		//1�ֽڣ���������޷���������	    
APP_EXT char xdata gPReadADNumer;	        //0x0076		//1�ֽڣ�������ʶ�ȡ��������	    
APP_EXT char xdata gPequDenomi;	            //0x0077		//1�ֽڣ����ʲ�����ĸ	  

/***frequency**	*/
APP_EXT	unsigned long idata gPBmFreq;		//0x0078		//4�ֽڣ���׼Ƶ��
/***frequency***/
APP_EXT	unsigned long gCenFreq;		//0x0040		//4�ֽڣ�����Ƶ�� KHzΪ��λ
//APP_EXT	unsigned long xdata gBandWidth;		//0x0044		//4�ֽڣ�����
//APP_EXT	int  xdata gIFOffset;				//0x0048		//2�ֽڣ���Ƶ����ֵ
//APP_EXT	unsigned int  xdata gFreqStep;		//0x004A		//2�ֽڣ�Ƶ�ʲ���ֵ
//APP_EXT	unsigned long xdata gIFFreq;		//0x004C		//4�ֽڣ���ƵƵ��ֵ
//APP_EXT	unsigned long  xdata gFreq;			//0x0050		//4�ֽڣ�����Ƶ��
//APP_EXT	unsigned long xdata gRefFreq;		//0x0054		//4�ֽڣ��ο�Ƶ��

/*****current*****/
APP_EXT	unsigned int  xdata gCurCo;			//0x0080		//2�ֽڣ�����б��
APP_EXT	int  xdata gCurOffset;				//0x0082		//2�ֽڣ�����������ֵ
APP_EXT	unsigned int xdata gCurHlim;		//0x0084		//2�ֽڣ����ŵ�������,������ǰ,С���ں�
APP_EXT	unsigned int xdata gCurLlim;		//0x0086		//2�ֽڣ����ŵ�������,������ǰ,С���ں�

/*****temperature********/
APP_EXT	unsigned char xdata gGainNumer;		//0x00A2		//1�ֽڣ������²�ϵ������
APP_EXT	unsigned char xdata gGainDenomi;	//0x00A3		//1�ֽڣ������²�ϵ����ĸ
APP_EXT	char xdata gPAProtecttLim;			//0x00A4		//1�ֽڣ����ű����¶�����
APP_EXT	char xdata gPAResetLim;				//0x00A5		//1�ֽڣ����Żظ��¶�
APP_EXT unsigned char xdata gGain;
/*****module*******/
APP_EXT	unsigned char xdata gRF_No;			//0x00C0		//1�ֽڣ���Ƶģ����
//APP_EXT	unsigned char xdata gPASW;			//0x00C1      	//1�ֽڣ����ſ���
APP_EXT	unsigned char xdata gRFSW;			//0x00C2      	//1�ֽڣ���Ƶ����
APP_EXT	unsigned char xdata AutoSwitch;				  //1���ֽ�

/****************other parameter********************/
APP_EXT	unsigned char xdata gAtttempval;					//�¶Ȳ���ֵ		
	
APP_EXT bit rfPramModified;	   				//��Ƶ�����޸ı�־(0:��Ч��1����Ч)//4��
APP_EXT unsigned int xdata all_checksum;					//����У���

APP_EXT unsigned char xdata gDownFlag;		//0x01FF		//1�ֽڣ�������־

/*********apparatus parameter************/
APP_EXT	unsigned char xdata PaAdjEquModulus;         //0x00F0  	    //1�ֽ�, ���ŵ�У׼���̵Ĳ���ϵ��
APP_EXT	unsigned int xdata PaLastAdjDate_year;	     //0x00F1	    //2�ֽڣ��ŵ�����У׼����(��)
APP_EXT	unsigned int xdata PaLastAdjDate_month;	     //0x00F3	    //1�ֽڣ��ŵ�����У׼����(��)
APP_EXT	unsigned int xdata PaLastAdjDate_day;	     //0x00F1	    //1�ֽڣ��ŵ�����У׼����(��)
APP_EXT	unsigned int xdata PaUpLimit;		         //0x00F5		//2�ֽڣ���������
APP_EXT	unsigned char xdata AppModel;	             //0x00F7	    //1�ֽڣ��Ǳ��ͺ�

/****************�������崥������********************/
APP_EXT	volatile unsigned char 	xdata __PlusSwitchState;     //���忪��״̬����
APP_EXT	volatile unsigned char 	xdata __PlusReqAddr;			 	 //�����ַ
APP_EXT	volatile unsigned char 	xdata __PlusReqPower;			 	 //������ֵ ��λdB
APP_EXT	volatile unsigned short xdata __PlusReqFreq;				 //����Ƶ��ֵ ��λMHz

APP_EXT	signed char	xdata __temp_que[6];						 //�²���������

//void writeDA5741(unsigned int value,unsigned char function);
void writeAD5314(unsigned short value,unsigned char channel);
void readTemperatur();
void OutputPwrsampling();
unsigned char writeAttenuator(unsigned char attVal);
void writeAtt1(unsigned char att1);
void writeAtt2(unsigned char att2);

void writePLL();
void progPLL(unsigned long value);
void tempCompensate();
void InitRF();
void execCheckSumQ();
void sendInitpacket();
//void tempwritePAIq();
unsigned char endDwnldSoft();
unsigned char execDwnldSoft();
unsigned short readAD(unsigned char channel);

//void switchPA(unsigned char value);
void switchRF(unsigned char value);
void writeLM75(char point,char value,unsigned char num);
void gPALimCompensate(void);
unsigned char checkMBit(unsigned char value);







