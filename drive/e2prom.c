/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: e2prom.c
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述:对LPC938上的EEPROM进行读写操作
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
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
	temp &= 0x0001;		   					//取得第八位地址数据

	ea_save = EA;
	EA=0;
	DEECON = (unsigned char)temp;		  	//给DEECON赋值，其中ECTL1＝0，ECTL0＝0，最低位位第八为地址数据

	temp = address;							
	temp &= 0x00FF;
	DEEADR = (unsigned char)temp;	  		//前七位地址数据写入DEEADR

	while((DEECON & MSK_DEECON_FINISH) == 0){}    //等待是否读完成
	
	DEECON &= 0x7F;
	eeprom_value = DEEDAT ;  
	EA=ea_save;								//保存中断状态

	return eeprom_value;
}


void writeE2prom(unsigned int address, unsigned char value) 
{
    bit ea_save;
	unsigned int temp;

	temp = address;
	temp >>= 8;
	temp &= 0x0001;							//取得第八位地址数据

	ea_save = EA;
	EA=0;
	DEECON = (unsigned char)temp;			//给DEECON赋值，其中ECTL1＝0，ECTL0＝0，最低位位第八为地址数据
	
	DEEDAT = value;							//要写入的数据		  			
	
	temp = address;
	temp &= 0x00FF;
	DEEADR = (unsigned char)temp;	  		//前七位地址数据写入DEEADR	
	while((DEECON & MSK_DEECON_FINISH) == 0)	//等待是否写完成
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