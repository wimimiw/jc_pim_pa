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
#ifndef	__CONFIG_H__
#define __CONFIG_H__
/* Public typedef ------------------------------------------------------------*/
typedef unsigned char		U8;
typedef unsigned short  	U16;
typedef unsigned int		U32;
typedef unsigned long long	U64;

typedef   signed char		S8;
typedef   signed short  	S16;
typedef   signed int		S32;
typedef   signed long long	S64;

typedef unsigned char		INT8U;
typedef unsigned short  	INT16U;
typedef unsigned int		INT32U;
typedef unsigned long long	INT64U;

typedef   signed char		INT8S;
typedef   signed short  	INT16S;
typedef   signed int		INT32S;
typedef   signed long long	INT64S;

typedef unsigned char		BOOL;
typedef float				F32;
typedef double				F64;
/* Public define -------------------------------------------------------------*/
#define __callback

#define TRUE 	1
#define FALSE	0

#define HIGH	1
#define LOW		0

#define IO_PORT    0
#define IO_PIN     1

#define NULL 	(void*)(0)

#define NUM_NOP_IN_US	(SystemCoreClock/1000000)

#define	LOOK_UP_TABLE		1		//????1, ????0
#define	HMC6101				0			
#define	AD8362				1			
#define	ADL5501				2			
#define DETECT_DEV			HMC6101

#define ADDR_BYTE_BOOTLOADERV	(BANK_CTRL_START_ADDR+0xFF0)
#define ADDR_BYTE_SOFTWAREV		(BANK_CTRL_START_ADDR+0xFF4)
#define ADDR_BYTE_HARDWAREV		(BANK_CTRL_START_ADDR+0xFF8)
#define ADDE_BYTE_BOOTLOADVBAK 	(BANK_CTRL_BUF_START_ADDR+0xFF0)
#define ADDE_BYTE_SOFTWAREVBAK 	(BANK_CTRL_BUF_START_ADDR+0xFF4)
#define ADDE_BYTE_HARDWAREVBAK 	(BANK_CTRL_BUF_START_ADDR+0xFF8)

//Bootloader Version
#define BOOTLOADER_VERSION	  BootloaderV
//Firmware Version
//Byte 		bit7~bit4 				bit3~bit0
//		 UpgradeAll Version	   UpgradeSct Version
#define SOFTWARE_VERSION	  SoftwareV
//Hardware Version
#define HARDWARE_VERSION	  HardwareV

#define SOFTWARE_VERSION_BAK  (*(U8*)(ADDE_BYTE_SOFTWAREVBAK))
#define HARDWARE_VERSION_BAK  (*(U8*)(ADDE_BYTE_HARDWAREVBAK))
/* Public macro --------------------------------------------------------------*/
extern const U8 BootloaderV ;
extern const U8 SoftwareV 	;
extern const U8 HardwareV 	;
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "math.h"

#include "stm32f10x.h"

#include "user_adc.h"
#include "user_i2c.h"
#include "user_uart.h"
#include "user_timer.h"
#include "user_flash.h"

#include "dev_spi.h"
#include "dev_e2prom.h"
#include "dev_att.h"
#include "dev_temp.h"

#include "task.h"
#include "target.h"

#include "rf_protocol.h"
#include "rf_parameter.h"
#include "rf_control.h"

#endif /*__CONFIG_H*/
/********************************END OF FILE***********************************/
