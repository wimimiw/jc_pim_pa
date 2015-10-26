/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __DEV_E2PROM_C__
#define __DEV_E2PROM_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EEPROM_DELAY        1000
#define EEPROM_PAGE_SIZE    64
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :debug driver
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
//#define __DEBUG_THIS
#ifdef __DEBUG_THIS 
int main(void)
{
	static U8 buf[64];

	InitTarget();
	ReadE2prom (0x0000,buf,64);
	WriteE2prom(0x0000,buf,64);
	ReadE2prom (0x0000,buf,64);	

	for(;;);
}
#endif
/**
  * @brief  :专门用于写EEPROM的函数，使之可以跨页(一页64字节,需根据情况设定)写
  * @param	:iaddr  EEPROM内部地址
  * @param	:pbuf   将要写入的数据的指针
  * @param	:plen   将要写入的数据的个数
  * @retval	:TRUE   操作成功
  * @retval	:FALSE  操作失败
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL WriteE2prom(U16 iaddr, U8 *pbuf, U16 plen)
{ 
    U8 	addr[2];
    U32 len, sentLen = 0;

    EEPROM_WP_UNPROTECT();                                                     
    while (sentLen < plen)
    {
        addr[0] = (iaddr + sentLen) >> 8;
        addr[1] = (iaddr + sentLen) & 0xFF;

        len = EEPROM_PAGE_SIZE - ((iaddr + sentLen) % EEPROM_PAGE_SIZE);        /* EEROM本页剩余空间    */

        if (plen - sentLen < len)                                                /* 未写的数据比较少     */
        {
            len = plen - sentLen;
        }

        if (FALSE == I2cWriteDevice(I2C1,I2C_EEPROM_ADDR, addr, sizeof(addr), pbuf+sentLen, len))
        {
            usdelay(EEPROM_DELAY);
            break;
        }

        usdelay(EEPROM_DELAY);
        sentLen += len;
    }
    EEPROM_WP_PROTECT();
    
    return (sentLen == plen);
}
/**
  * @brief  :专门用于读EEPROM的函数
  * @param	:iaddr  EEPROM内部地址
  * @param	:pbuf   将要读出的数据的指针
  * @param	:plen   将要读出的数据的个数
  * @retval	:TRUE   操作成功
  * @retval	:FALSE  操作失败
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL ReadE2prom(U16 iaddr, U8 *pbuf, U16 plen)
{
    U8 addr[2];

    addr[0] = iaddr >> 8;
    addr[1] = iaddr & 0xff;

    return I2cReadDevice(I2C1, I2C_EEPROM_ADDR, addr, sizeof(addr), pbuf, plen);
}
/********************************END OF FILE***********************************/
