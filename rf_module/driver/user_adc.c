/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:user_adc.c
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:为了保持程序的可移植性，ADC处理采用轮询方式，不建议中断和DMA处理
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __USER_ADC_C__
#define __USER_ADC_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC_CONVERT_LIMIT	2000 
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
	InitTarget();
	return 1;
}
#endif
/**
  * @brief  :ADC初始化函数
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitAdc(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC1, &ADC_InitStructure);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_71Cycles5);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);	
	/* Enable ADC1 reset calibration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));	
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
}
/**
  * @brief  :读取CPU自带ADC采样值，读一次，由上一层进行滤波处理
  * @param  :channel 选择ADC采样通道
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL ReadAdcValue(U8 channel , U16*adcVal)
{   
	U16	i;
	BOOL result = FALSE;

	if ( NULL == adcVal ) return result;
	/* ADC1 regular channel6 configuration */ 
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_13Cycles5);
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	/* read adc value */
	for( i = 0; 
		 i < ADC_CONVERT_LIMIT && RESET == ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC);
		 i ++);
	//convert success
	if( ADC_CONVERT_LIMIT != i )
	{
		*adcVal = ADC_GetConversionValue(ADC1);
		result = TRUE;
	}
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    return result;
}
/**
  * @brief  :read the temperature form mcu internal
  * @param  :None
  * @retval :temperature
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
S16 ReadAdcTemperature(void)
{
	U16	timerLimit,Vsense;
	F32 temperature = 0;
	/* ADC1 regular channel6 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);
	//Enables the temperature sensor and Vrefint channel.
	ADC_TempSensorVrefintCmd(ENABLE);
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	/* read adc value */
	for( timerLimit = 0; 
		 timerLimit < ADC_CONVERT_LIMIT && RESET == ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC);
		 timerLimit ++);
	//convert success
	if( ADC_CONVERT_LIMIT != timerLimit )
	{
		Vsense = ADC_GetConversionValue(ADC1);
		temperature = (1.41 - 3.3*Vsense/(1.0*ADC_VALUE_REF))/4300.0 + 25;
	}
	else
	{
		temperature = 0;
	}
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	//disables the temperature sensor and Vrefint channel.
	ADC_TempSensorVrefintCmd(DISABLE);
    return (S16)temperature*2;	
}
/********************************END OF FILE***********************************/
