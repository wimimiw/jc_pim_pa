/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: i2c.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述: i2c 头文件				
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

sbit WP = P1^4;

#define	Generate_stop		0x54
#define Release_bus_ack		0x44
#define Release_bus_noack	0x40
#define Release_bus_sta		0x64


void GetBus();
void SendByte(unsigned char c);
unsigned char ISendStr(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);
unsigned char IRcvStr(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);
unsigned char ISendStrExt(unsigned char sla,unsigned char *s, unsigned char no);
//unsigned char IRcvStr(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);
//unsigned char ISendStrExt2(unsigned char sla,unsigned int suba,unsigned char *s, unsigned char no);
//unsigned char IRcvStr2(unsigned char sla,unsigned int suba,unsigned char *s,unsigned char no);
	