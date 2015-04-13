/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: serial.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年5月15日
**最 新 版  本: V1.3
**描        述: serial 头文件				
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
** 描　述: 1、增加了5个错误标志
**
********************************************************************************************************/

#ifdef 	APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

//sbit RDE = P0^7;

void InitSerial() small;			//串口初始化
void putChar(unsigned char ch) small;		//显示字符
void putCh(unsigned char ch) small;//内部使用，不建议用户使用。
void putStr(unsigned char *str) small;//显示字符串
unsigned char bin_to_ascii (unsigned char to_convert) small;
void putCharAsAscii(unsigned char byte_to_tx) small;
void putStrLen(unsigned char *str, unsigned char len) small;
void putStrBuf(unsigned char *front, unsigned char *rear) small;


//应答标志常量列表
#define		NO_ERR				0x00
#define		CMDID_ERR			0x02
#define		LEN_ERR				0x03
#define		DEFSUM_ERR			0x04
#define		DATRANGE_ERR		0x20
#define		OTHER_ERR			0x90

#define		OCID_ERR			0x10
#define		NOMOD_ERR			0xC0
#define		RS485_ERR			0xC1
#define		EEPROM_ERR			0xC2
#define		OC_ERR				0xC3


//接收到数据状态常量
#define STA_ADDRESS1	0				//模块地址（头部从机地址）	
#define STA_INIT1		1				//包起始	
#define STA_INIT2		2
#define STA_ADDRESS2	3		   		//模块地址
#define STA_PACKSIZE	4 				//数据单元长度（包括命令标识、应答标志、监控对象的长度）
#define STA_CMD			5				//数据单元中的命令标识，内部命令固定为0xA0
#define STA_ACK			6		   		//数据单元中的应答标志
#define STA_OL			7				//监控对象长度
#define STA_OID1		8				//监控对象标号
#define STA_OID2		9				//监控对象标号
#define STA_OC			10				//监控对象内容
#define STA_CHKSUM		11				//接收到数据的校验和


//常量
#define SYNC1			0x55		 	//同步字节1
#define SYNC2			0xAA		 	//同步字节2
#define FCT_CMD			0xA0		 	//内部命令标识
#define ACK				0xFF		 	//接收到的应答标志

//收发缓冲区长度
#define LenTxBuf        128    
#define LenRxBuf        128

//外部变量
APP_EXT unsigned char xdata TxBuf[LenTxBuf];
APP_EXT unsigned char xdata RxBuf[LenRxBuf];

APP_EXT unsigned char pack_valid;


APP_EXT unsigned char idata Rx_state;	//当前状态
APP_EXT unsigned char idata Rx_chksum;	//接收到字节的校验和
APP_EXT unsigned char idata Rx_ptr;		//接收到数据缓冲区指针
APP_EXT unsigned char idata Rx_size;	//接收到数据长度

APP_EXT unsigned char Tx_ptr;			//发送数据缓冲区指针
APP_EXT unsigned char Tx_size;			//发送数据长度
APP_EXT unsigned char Tx_chksum;		//发送数据校验和


