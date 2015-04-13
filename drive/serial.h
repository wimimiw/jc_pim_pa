/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: serial.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��5��15��
**�� �� ��  ��: V1.3
**��        ��: serial ͷ�ļ�				
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.3
** �ա���: 2006��5��15��
** �衡��: 1��������5�������־
**
********************************************************************************************************/

#ifdef 	APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

//sbit RDE = P0^7;

void InitSerial() small;			//���ڳ�ʼ��
void putChar(unsigned char ch) small;		//��ʾ�ַ�
void putCh(unsigned char ch) small;//�ڲ�ʹ�ã��������û�ʹ�á�
void putStr(unsigned char *str) small;//��ʾ�ַ���
unsigned char bin_to_ascii (unsigned char to_convert) small;
void putCharAsAscii(unsigned char byte_to_tx) small;
void putStrLen(unsigned char *str, unsigned char len) small;
void putStrBuf(unsigned char *front, unsigned char *rear) small;


//Ӧ���־�����б�
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


//���յ�����״̬����
#define STA_ADDRESS1	0				//ģ���ַ��ͷ���ӻ���ַ��	
#define STA_INIT1		1				//����ʼ	
#define STA_INIT2		2
#define STA_ADDRESS2	3		   		//ģ���ַ
#define STA_PACKSIZE	4 				//���ݵ�Ԫ���ȣ����������ʶ��Ӧ���־����ض���ĳ��ȣ�
#define STA_CMD			5				//���ݵ�Ԫ�е������ʶ���ڲ�����̶�Ϊ0xA0
#define STA_ACK			6		   		//���ݵ�Ԫ�е�Ӧ���־
#define STA_OL			7				//��ض��󳤶�
#define STA_OID1		8				//��ض�����
#define STA_OID2		9				//��ض�����
#define STA_OC			10				//��ض�������
#define STA_CHKSUM		11				//���յ����ݵ�У���


//����
#define SYNC1			0x55		 	//ͬ���ֽ�1
#define SYNC2			0xAA		 	//ͬ���ֽ�2
#define FCT_CMD			0xA0		 	//�ڲ������ʶ
#define ACK				0xFF		 	//���յ���Ӧ���־

//�շ�����������
#define LenTxBuf        128    
#define LenRxBuf        128

//�ⲿ����
APP_EXT unsigned char xdata TxBuf[LenTxBuf];
APP_EXT unsigned char xdata RxBuf[LenRxBuf];

APP_EXT unsigned char pack_valid;


APP_EXT unsigned char idata Rx_state;	//��ǰ״̬
APP_EXT unsigned char idata Rx_chksum;	//���յ��ֽڵ�У���
APP_EXT unsigned char idata Rx_ptr;		//���յ����ݻ�����ָ��
APP_EXT unsigned char idata Rx_size;	//���յ����ݳ���

APP_EXT unsigned char Tx_ptr;			//�������ݻ�����ָ��
APP_EXT unsigned char Tx_size;			//�������ݳ���
APP_EXT unsigned char Tx_chksum;		//��������У���


