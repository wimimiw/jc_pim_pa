/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:dev_timer.c
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef	__USER_TIMER_C__
#define __USER_TIMER_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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
  * @brief  :初始化定时器
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitTimer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock/TIM_CNT_FREQ) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);	
}

/**
  * @brief  :Configure the nested vectored interrupt controller.
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitTIM1(void)
{	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;  
	
	/* Time base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = 200; 			//1kHz产生定时
	TIM_TimeBaseStructure.TIM_Prescaler = 40-1;//(uint16_t) (SystemCoreClock/2000) - 1;//500Hz 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	/* TIM1 channel1 configuration in PWM mode */
// 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
// 	TIM_OCInitStructure.TIM_Pulse = 0x7F; 
// 	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
// 	TIM_OC1Init(TIM1, &TIM_OCInitStructure);	
// 	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	//如果需要产生TIM1更新中断，请将TIM_OC1Init，TIM_OC1PreloadConfig函数屏幕掉
	//TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);	
	/* TIM1 main Output Enable */
	//TIM_CtrlPWMOutputs(TIM1,ENABLE);	
 	/* TIM1 counter enable */
	TIM_Cmd(TIM1,ENABLE);			
}
/**
  * @brief  :Reset the user timer
  * @param  :default
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void UserTimerReset(TIM_TypeDef* TIMx,UserTimerDef * timer)
{
	/* Check the parameters */
	assert_param(timer);
	timer->record = TIM_GetCounter(TIMx);
	timer->cnt = 0;
	timer->cntBak = 0;
}
/**
  * @brief  :get the timer is over
  * @param  :default
  * @retval :Bool
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL UserTimerOver(TIM_TypeDef* TIMx,UserTimerDef * timer,S32 limit)
{
	U16 tmp;

	/* Check the parameters */
	assert_param(timer);

	tmp = TIM_GetCounter(TIMx) - *(U16*)&timer->record;	//兼容32bit&16bit定时器
	if ( 0xffff >= limit )
	{//计数范围在16位内	
		return ( tmp >= limit )?TRUE:FALSE;
	}
	else
	{//计数范围超过16位
		*(U16*)&timer->record = TIM_GetCounter(TIMx);
		timer->cnt += tmp;
		return ( timer->cnt >= limit )?TRUE:FALSE;
	}	
}

void TIM1_UP_TIM16_IRQHandler(void)
{	
	if (TIM_GetITStatus(TIM1,TIM_IT_Update)!= RESET )
	{
		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
		if(Timer1CallBack!=(void*)0)
			Timer1CallBack();
	}
}
/********************************END OF FILE***********************************/
