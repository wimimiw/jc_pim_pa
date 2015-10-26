/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:dev_timer.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __USER_TIMER_H__
#define __USER_TIMER_H__
#endif

#ifndef __USER_TIMER_C__
#define	__USER_TIMER_EXT__ 	 extern
#else
#define	__USER_TIMER_EXT__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct{
	U32 cnt;
	U16 cntBak;
	U32 record;
}UserTimerDef;
/* Public define -------------------------------------------------------------*/
//#define TIM_CNT_FREQ		38400						  /*因为晶振为11.0592MHz
//														  	所以以38400为计数频率
//															易与得到整数定时*/
#define TIM_CNT_FREQ		40000						  /*因为晶振为12MHz
														  	所以以40000为计数频率
															易与得到整数定时*/

#define USER_TIMER_NO_DELAY	(0)							  //无延时

#define USER_TIMER_1S(t)	((t)*TIM_CNT_FREQ		)	  //精确
#define USER_TIMER_10MS(t)	((t)*TIM_CNT_FREQ/100	)	  //精确
#define USER_TIMER_1MS(t)	((t)*TIM_CNT_FREQ/1000	)	  //粗糙
#define USER_TIMER_100US(t)	((t)*TIM_CNT_FREQ/10000	)	  //粗糙
/* Public macro --------------------------------------------------------------*/
__inline void usdelay(U16 delayTimer)
{
	U16 i;
	for(i = 0;i < delayTimer*NUM_NOP_IN_US ;i ++);
}
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
__USER_TIMER_EXT__ void InitTimer(void);
__USER_TIMER_EXT__ void UserTimerReset(TIM_TypeDef* TIMx,UserTimerDef * timer);
__USER_TIMER_EXT__ BOOL UserTimerOver(TIM_TypeDef* TIMx,UserTimerDef * timer,S32 limit);
__USER_TIMER_EXT__ void (*Timer1CallBack)(void);
__USER_TIMER_EXT__ void InitTIM1(void);
/********************************END OF FILE***********************************/
