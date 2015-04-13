/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: e2prom.c
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��3��31��
**�� �� ��  ��: V1.2
**��        ��:��LPC938�ϵ�EEPROM���ж�д����
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/

#include <reg938.h>
#include <e2prom.h>

#define MSK_DEECON_FINISH 0x80

unsigned char readE2prom(unsigned int address)  
{
	register unsigned char eeprom_value;
	bit ea_save;	
	unsigned int temp;

	temp = address;							
	temp >>= 8;
	temp &= 0x0001;		   					//ȡ�õڰ�λ��ַ����

	ea_save = EA;
	EA=0;
	DEECON = (unsigned char)temp;		  	//��DEECON��ֵ������ECTL1��0��ECTL0��0�����λλ�ڰ�Ϊ��ַ����

	temp = address;							
	temp &= 0x00FF;
	DEEADR = (unsigned char)temp;	  		//ǰ��λ��ַ����д��DEEADR

	while((DEECON & MSK_DEECON_FINISH) == 0){}    //�ȴ��Ƿ�����
	
	DEECON &= 0x7F;
	eeprom_value = DEEDAT ;  
	EA=ea_save;								//�����ж�״̬

	return eeprom_value;
}


void writeE2prom(unsigned int address, unsigned char value) 
{
    bit ea_save;
	unsigned int temp;

	temp = address;
	temp >>= 8;
	temp &= 0x0001;							//ȡ�õڰ�λ��ַ����

	ea_save = EA;
	EA=0;
	DEECON = (unsigned char)temp;			//��DEECON��ֵ������ECTL1��0��ECTL0��0�����λλ�ڰ�Ϊ��ַ����
	
	DEEDAT = value;							//Ҫд�������		  			
	
	temp = address;
	temp &= 0x00FF;
	DEEADR = (unsigned char)temp;	  		//ǰ��λ��ַ����д��DEEADR	
	while((DEECON & MSK_DEECON_FINISH) == 0)	//�ȴ��Ƿ�д���
	{
	}	
	DEECON &= 0x7F;

	EA=ea_save;     
} 

void writeE2promStr(unsigned char len_car, unsigned int adr_eeprom, unsigned char *src) 
{
	unsigned char i;

	for (i=0; i<len_car; i++)
	{
	   writeE2prom(adr_eeprom++, src[i]);
	 }	
}

void readE2promStr(unsigned char len_car, unsigned int adr_eeprom, unsigned char *dest) 
{
	unsigned char i;
	
	for (i=0; i<len_car; i++)
	{
		*dest++ = readE2prom(adr_eeprom++);
	}
}