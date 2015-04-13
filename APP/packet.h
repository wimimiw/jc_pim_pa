/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: packet.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年5月15日
**最 新 版  本: V1.3
**描        述: packet 头文件				
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.3
** 日　期: 2006年5月15日
** 描　述: recv_timer由char改为int
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

#define		MAX_DATA_LEN		32			//256 + 2，命令体数据 + CRC
//有参数返回函数	
//unsigned char execDwnldSoft();					//下载软件
void i2cReadBytes(unsigned char dvcAddr, unsigned int offset, unsigned char count) ;
//unsigned char endDwnldSoft();			//下载成功标志或者查询模块信息

//无参数返回函数
void execute_cmd();
void updateAlmInfo();			//更新告警信息
void execFctParamSet();	 		//设置工厂参数
void execRFParamQ();			//查询RF模块参数
void calDefcksum(unsigned char *ptr, unsigned int len, unsigned char *checksum);  //计算异或校验和 
void sendPkt() ;				//发送数据包
void swapBytes(unsigned char *src, unsigned char *result,unsigned char num);
void delay(unsigned char time);
void watchdog(); 

//全局变量
APP_EXT unsigned char ack_flag;						//应答标志
APP_EXT unsigned char Data_len;						//数据包长度

APP_EXT unsigned char LNA_state;					//低噪放故障状态
APP_EXT unsigned char Lim_state;					//限幅状态
APP_EXT unsigned char PLL_state;					//中频失锁状态
APP_EXT unsigned int  PA_current;					//功放电流
APP_EXT unsigned int  OutputPwr;					//输出功率
APP_EXT unsigned int  re_Pwr;						//反射功率
APP_EXT unsigned int  InputPwr;						//输入功率
APP_EXT unsigned char PaSwitch;						//功放开关
APP_EXT unsigned char RfSwitch;						//射频开关
APP_EXT unsigned char TempOverAlm;

APP_EXT unsigned char xdata RcvDat[32];
APP_EXT unsigned char xdata XmtDat[34];
APP_EXT unsigned char xdata ByteCnt;
APP_EXT unsigned char xdata NoAck;
