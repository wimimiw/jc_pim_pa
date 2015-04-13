/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: packet.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��5��15��
**�� �� ��  ��: V1.3
**��        ��: packet ͷ�ļ�				
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
** �衡��: recv_timer��char��Ϊint
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

#define		MAX_DATA_LEN		32			//256 + 2������������ + CRC
//�в������غ���	
//unsigned char execDwnldSoft();					//�������
void i2cReadBytes(unsigned char dvcAddr, unsigned int offset, unsigned char count) ;
//unsigned char endDwnldSoft();			//���سɹ���־���߲�ѯģ����Ϣ

//�޲������غ���
void execute_cmd();
void updateAlmInfo();			//���¸澯��Ϣ
void execFctParamSet();	 		//���ù�������
void execRFParamQ();			//��ѯRFģ�����
void calDefcksum(unsigned char *ptr, unsigned int len, unsigned char *checksum);  //�������У��� 
void sendPkt() ;				//�������ݰ�
void swapBytes(unsigned char *src, unsigned char *result,unsigned char num);
void delay(unsigned char time);
void watchdog(); 

//ȫ�ֱ���
APP_EXT unsigned char ack_flag;						//Ӧ���־
APP_EXT unsigned char Data_len;						//���ݰ�����

APP_EXT unsigned char LNA_state;					//����Ź���״̬
APP_EXT unsigned char Lim_state;					//�޷�״̬
APP_EXT unsigned char PLL_state;					//��Ƶʧ��״̬
APP_EXT unsigned int  PA_current;					//���ŵ���
APP_EXT unsigned int  OutputPwr;					//�������
APP_EXT unsigned int  re_Pwr;						//���书��
APP_EXT unsigned int  InputPwr;						//���빦��
APP_EXT unsigned char PaSwitch;						//���ſ���
APP_EXT unsigned char RfSwitch;						//��Ƶ����
APP_EXT unsigned char TempOverAlm;

APP_EXT unsigned char xdata RcvDat[32];
APP_EXT unsigned char xdata XmtDat[34];
APP_EXT unsigned char xdata ByteCnt;
APP_EXT unsigned char xdata NoAck;
