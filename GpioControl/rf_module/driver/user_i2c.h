/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:user_i2c.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __USER_I2C_C__
#define __USER_I2C_C__
#endif

#ifndef __USER_I2C_C__
#define	__USER_I2C_EXT__ 	 extern
#else
#define	__USER_I2C_EXT__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
__callback typedef void (*I2C_SLAVE_HANDLE)(U8*rxBuf,U16 rxlen, U8**txBuf,U16*txlen);
/* Public function prototypes ------------------------------------------------*/
__USER_I2C_EXT__ void InitI2c(void);
__USER_I2C_EXT__ void InitI2cProc(I2C_TypeDef* I2Cx, I2C_SLAVE_HANDLE handle);
__USER_I2C_EXT__ BOOL I2cWriteDevice(I2C_TypeDef* I2Cx, U8 addr, U8*iaddr, U8 iaddrLen, U8*pbuf, U8 plen);
__USER_I2C_EXT__ BOOL I2cReadDevice(I2C_TypeDef* I2Cx, U8 addr, U8*iaddr, U8 iaddrLen, U8*pbuf, U8 plen);
/********************************END OF FILE***********************************/
