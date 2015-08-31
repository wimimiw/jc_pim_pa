/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:dev_att.c
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:写衰减器函数
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __DEV_ATT_C__
#define __DEV_ATT_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PE4302_DATA_LEN         6
#define PE4302_MASK_BIT         0x20
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :写衰减器PE4302
  * @param  :attVal 需要设置的衰减值
  * @param  :bus spi总线
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL WritePe4302(const SPI_BUS * bus,U8 attVal)
{
    SPI_TYPE spiType;

    spiType.len   = PE4302_DATA_LEN;
    spiType.order = MSB_FIRST;
    spiType.type  = SPI_PULSE_HIGH;
    spiType.mask  = PE4302_MASK_BIT;

	/* adjust 7dB~12dB + 0.5dB */
	if ( &attBus[2] == bus || &attBus[3] == bus )
	{
		if ( attVal >= 7*2 && attVal <= 12*2)
		{
			attVal ++;
		}
	}

    return WriteSpiOneWord( bus, &spiType, attVal );
}
/********************************END OF FILE***********************************/
