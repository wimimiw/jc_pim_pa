/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:user_adc.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __USER_ADC_C__
#define	__USER_ADC_EXT__ 	extern
#else
#define	__USER_ADC_EXT__ 
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
#define ADC_VALUE_REF	  (1<<12)		//12bit
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
__USER_ADC_EXT__ void InitAdc(void);
__USER_ADC_EXT__ BOOL ReadAdcValue(U8 channel , U16*adcVal);
__USER_ADC_EXT__ S16 ReadAdcTemperature(void);
__USER_ADC_EXT__ U16 ADC_ConvertedValue[16];
/********************************END OF FILE***********************************/
