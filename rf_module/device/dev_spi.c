/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:dev_spi.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __DEV_SPI_C__
#define __DEV_SPI_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI_INTERVAL 10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :operater the gpio
  * @param  :group 
  * @param  :pin
  * @param 	:level
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void WriteGpioPin(U8 group, U16 pin, BOOL level)
{		
	GPIO_TypeDef*gpio;
	switch (group)
	{
		case GPIO_PortSourceGPIOA:gpio = GPIOA;break;
		case GPIO_PortSourceGPIOB:gpio = GPIOB;break;
		case GPIO_PortSourceGPIOC:gpio = GPIOC;break;
		case GPIO_PortSourceGPIOD:gpio = GPIOD;break;
		default:return;
	}
	GPIO_WriteBit(gpio,pin,(HIGH == level)?Bit_SET:Bit_RESET);
}
/**
  * @brief  :用于检验掩码位对应的值
  * @param  :None
  * @param	:data   需要获取掩码的数据
  * @param	:mask   需要发送首位数据的掩码
  * @param 	:order  数据发送的顺序
  * @param 	:num    需要发送的位  
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static U8 ChkMaskBit(U32 data, U32 mask, U8 num, U8 order)
{

    if (MSB_FIRST == order)
    {
        mask >>= num;
    }
    else
    {
        mask <<= num;
    }

    return ((data & mask) == 0 ? LOW : HIGH);

}
/**
  * @brief  :结束PSI总线
  * @param  :spiType SPI接口的类型
  *	@param	:spiBus  SPI总线的通道
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void EndSpiBus(const SPI_BUS * spiBus,SPI_TYPE * spiType)
{
    switch ( spiType->type )
    {
        case SPI_LEVEL_LOW:
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉高使能脚                   */
            break;

        case SPI_LEVEL_HIGH:
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉低使能脚                   */
            break;

        case SPI_PULSE_LOW:
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉高使能脚                   */
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉高使能脚                   */
            break;

        case SPI_PULSE_HIGH:
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉低使能脚                   */
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉低使能脚                   */
            break;

        default:
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉低使能脚                   */
            break;
    }
}
/**
  * @brief  :启动PSI总线
  * @param  :spiType SPI接口的类型
  * @param	:spiBus  SPI总线的通道
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void StartSpiBus(const SPI_BUS* spiBus,SPI_TYPE* spiType)
{
    switch ( spiType->type )
    {
        case SPI_LEVEL_LOW:
            WriteGpioPin(spiBus->clk[0],spiBus->clk[1], LOW);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉高使能脚                   */
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉低使能脚                   */
            break;

        case SPI_LEVEL_HIGH:
            WriteGpioPin(spiBus->clk[0],spiBus->clk[1], LOW);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉低使能脚                   */
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉高使能脚                   */
            break;

        case SPI_PULSE_LOW:
            WriteGpioPin(spiBus->clk[0],spiBus->clk[1], LOW);
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉高使能脚                   */
            break;

        case SPI_PULSE_HIGH:
            WriteGpioPin(spiBus->clk[0],spiBus->clk[1], LOW);
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉低使能脚                   */
            break;

        default:
            WriteGpioPin(spiBus->clk[0],spiBus->clk[1], LOW);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  HIGH);          /* 拉高使能脚                   */
            usdelay(SPI_INTERVAL);
            WriteGpioPin(spiBus->cs[0],spiBus->cs[1],  LOW);           /* 拉低使能脚                   */
            break;
    }
}
/**
  * @brief  :从PSI接口写一个字
  * @param  :spiType SPI接口的类型
  * @param	:spiBus  SPI总线的通道
  * @param	:spiWord SPI需要发送的数据
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL WriteSpiOneWord(const SPI_BUS* spiBus, SPI_TYPE* spiType, U32 spiWord)
{
    U8 i;
    U8 data;

    /* 启动SPI总线  */
    StartSpiBus(spiBus,spiType);

    /* 发送数据位   */
    for (i = 0; i < spiType->len; i++)                                              /* SPI数据发送                  */
    {

        data = ChkMaskBit(spiWord, spiType->mask, i, spiType->order);               /* 设置数据脚电平               */

        usdelay(SPI_INTERVAL);
        WriteGpioPin(spiBus->data[0],spiBus->data[1], data);
        usdelay(SPI_INTERVAL);
        WriteGpioPin(spiBus->clk[0],spiBus->clk[1],  HIGH);            /* 产生时钟脉冲                 */
        usdelay(SPI_INTERVAL);
        WriteGpioPin(spiBus->clk[0],spiBus->clk[1],   LOW);
    }

    /* 结束SPI总线  */
    usdelay(SPI_INTERVAL);
    EndSpiBus( spiBus,spiType );

    return TRUE;
}
/********************************END OF FILE***********************************/
