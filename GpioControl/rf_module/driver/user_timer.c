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
		timer->cnt &=~0x0000ffff;

		if ( timer->cflag && 0xffff > tmp )
		{
			if ( 0xfffe == tmp )timer->cflag = FALSE;
			timer->cnt |= tmp;
		}
		else if ( !timer->cflag && 0xffff == tmp )
		{
			timer->cflag = TRUE;
			timer->cnt += 0x10000;
		}

		if ( timer->cnt >= limit )
		{
			timer->cnt = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	
}
/********************************END OF FILE***********************************/
