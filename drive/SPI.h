/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: spi.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述: SPI 头文件				
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

sbit CLK =  P2^5;
sbit DATA=  P2^4;

void InitSPI(void);
void MSend_bit(unsigned int send_data,unsigned char Bitcnt);
