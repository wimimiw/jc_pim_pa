/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: e2prom.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��3��31��
**�� �� ��  ��: V1.2
**��        ��: e2prom ͷ�ļ�				
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

unsigned char readE2prom(unsigned int address) ;
void writeE2prom(unsigned int address, unsigned char value) ;
void writeE2promStr(unsigned char len_car, unsigned int adr_eeprom, unsigned char *src) ;
void readE2promStr(unsigned char len_car, unsigned int adr_eeprom, unsigned char *dest) ;