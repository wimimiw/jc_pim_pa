/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: rfparam.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年6月1日
**最 新 版  本: V1.5
**描        述: rfparam 头文件				
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.4
** 日　期: 2006年4月24日
** 描　述: hex文件版本号改为1.1
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.5
** 日　期: 2006年6月1日
** 描　述: hex文件版本号改为1.2
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
/*软件下载标志*/
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

//#define	WAKE_UP	0x3C04				//AD芯片MAX5741唤醒	

//在这里选择使用的ADC通道
#define CHANNEL_0	0x01			//AD转换通道
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

APP_EXT	int gcurRfTemp;				//当前温度值
APP_EXT	int gpreRfTemp;				//上一次温度值
APP_EXT	int gainCo;
/****debug****/
APP_EXT	int gtestRfTemp;				//测试温度值
APP_EXT	unsigned char gtestflag;		//调试命令标志，0为正常状态，1为调试状态
APP_EXT	unsigned char xdata gTempgain;	//1字节，温补基准值设定
APP_EXT	unsigned char xdata	gBaseTemp;	//温度基准值
/****gain*****/
APP_EXT	unsigned char xdata gAtteVal;		//0x0001      	//1字节，增益设定值
APP_EXT	unsigned char xdata gGainOffset;	//0x0002		//1字节，增益调整值
APP_EXT	unsigned char xdata gAtt1;						//0x0003		//1字节，1#衰减器，1U5

/****PA******/
APP_EXT	unsigned int idata gDAoutA;			//0x0034		//2字节，输出功率限幅值
APP_EXT	unsigned int idata gDAoutB;			//0x0036		//2字节，输出功率限幅值
APP_EXT	unsigned int idata gDAoutC;			//0x0038		//2字节，输出功率限幅值
APP_EXT	unsigned int idata gDAoutD;			//0x003A		//2字节，输出功率限幅值

APP_EXT	unsigned int idata gPALim;			//0x0020		//2字节，输出功率限幅值
APP_EXT int xdata gOutPwrVal;               //0x0022		//2字节，输出功率定标值
APP_EXT	unsigned int xdata gOutPwrSlopeVal;	//0x0024		//2字节，输出功率设定值定标
APP_EXT	int xdata gRefPwrVal;		        //0x0026		//2字节，反射功率定标
APP_EXT	unsigned int xdata gRefPwrSlopeVal; //0x0028		//2字节，反射功率斜率值
APP_EXT	unsigned char xdata gOutPwrShutVal;		//0x002e		//1字节，输出功率关闭门限
APP_EXT char xdata gPALimNumer;		        //0x0075		//1字节，输出功率限幅补偿分子	    
APP_EXT char xdata gPReadADNumer;	        //0x0076		//1字节，输出功率读取补偿分子	    
APP_EXT char xdata gPequDenomi;	            //0x0077		//1字节，功率补偿分母	  

/***frequency**	*/
APP_EXT	unsigned long idata gPBmFreq;		//0x0078		//4字节，基准频率
/***frequency***/
APP_EXT	unsigned long gCenFreq;		//0x0040		//4字节，中心频率 KHz为单位
//APP_EXT	unsigned long xdata gBandWidth;		//0x0044		//4字节，带宽
//APP_EXT	int  xdata gIFOffset;				//0x0048		//2字节，中频调整值
//APP_EXT	unsigned int  xdata gFreqStep;		//0x004A		//2字节，频率步进值
//APP_EXT	unsigned long xdata gIFFreq;		//0x004C		//4字节，中频频率值
//APP_EXT	unsigned long  xdata gFreq;			//0x0050		//4字节，本振频率
//APP_EXT	unsigned long xdata gRefFreq;		//0x0054		//4字节，参考频率

/*****current*****/
APP_EXT	unsigned int  xdata gCurCo;			//0x0080		//2字节，电流斜率
APP_EXT	int  xdata gCurOffset;				//0x0082		//2字节，电流检测调整值
APP_EXT	unsigned int xdata gCurHlim;		//0x0084		//2字节，功放电流上限,整数在前,小数在后
APP_EXT	unsigned int xdata gCurLlim;		//0x0086		//2字节，功放电流下限,整数在前,小数在后

/*****temperature********/
APP_EXT	unsigned char xdata gGainNumer;		//0x00A2		//1字节，增益温补系数分子
APP_EXT	unsigned char xdata gGainDenomi;	//0x00A3		//1字节，增益温补系数分母
APP_EXT	char xdata gPAProtecttLim;			//0x00A4		//1字节，功放保护温度门限
APP_EXT	char xdata gPAResetLim;				//0x00A5		//1字节，功放回复温度
APP_EXT unsigned char xdata gGain;
/*****module*******/
APP_EXT	unsigned char xdata gRF_No;			//0x00C0		//1字节，射频模块编号
//APP_EXT	unsigned char xdata gPASW;			//0x00C1      	//1字节，功放开关
APP_EXT	unsigned char xdata gRFSW;			//0x00C2      	//1字节，射频开关
APP_EXT	unsigned char xdata AutoSwitch;				  //1个字节

/****************other parameter********************/
APP_EXT	unsigned char xdata gAtttempval;					//温度补偿值		
	
APP_EXT bit rfPramModified;	   				//射频参数修改标志(0:无效，1：有效)//4次
APP_EXT unsigned int xdata all_checksum;					//程序校验和

APP_EXT unsigned char xdata gDownFlag;		//0x01FF		//1字节，升级标志

/*********apparatus parameter************/
APP_EXT	unsigned char xdata PaAdjEquModulus;         //0x00F0  	    //1字节, 功放的校准方程的补偿系数
APP_EXT	unsigned int xdata PaLastAdjDate_year;	     //0x00F1	    //2字节，放的最终校准日期(年)
APP_EXT	unsigned int xdata PaLastAdjDate_month;	     //0x00F3	    //1字节，放的最终校准日期(月)
APP_EXT	unsigned int xdata PaLastAdjDate_day;	     //0x00F1	    //1字节，放的最终校准日期(年)
APP_EXT	unsigned int xdata PaUpLimit;		         //0x00F5		//2字节，功放上限
APP_EXT	unsigned char xdata AppModel;	             //0x00F7	    //1字节，仪表型号

/****************单次脉冲触发变量********************/
APP_EXT	volatile unsigned char 	xdata __PlusSwitchState;     //脉冲开关状态变量
APP_EXT	volatile unsigned char 	xdata __PlusReqAddr;			 	 //请求地址
APP_EXT	volatile unsigned char 	xdata __PlusReqPower;			 	 //请求功率值 单位dB
APP_EXT	volatile unsigned short xdata __PlusReqFreq;				 //请求频率值 单位MHz

APP_EXT	signed char	xdata __temp_que[6];						 //温补操作队列

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







