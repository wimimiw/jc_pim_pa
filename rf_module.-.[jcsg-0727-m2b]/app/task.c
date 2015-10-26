/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_task.c
 * @author	:mashuai
 * @version	:v1.0 
 * @date	:2012.4.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __TASK_C__
#define __TASK_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* task information */
typedef struct _TASK_TABLE_ {    
	UserTimerDef timer;
    S32 delayTime;
	__callback Procf task;
} TaskTableType;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static U8 taskNum = 0;
static TaskTableType taskTable[TASK_MAX_NUM];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :add the timer of task 
  * @param  :interval    the interval time of the task
  * @param  :handler     the function of task
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void AddTaskTimer( Procf handler, S32 interval )
{
	if ( taskNum < TASK_MAX_NUM && NULL == taskTable[taskNum].task )
    {
    	taskTable[taskNum].delayTime = interval;
    	taskTable[taskNum].task      = handler;
		UserTimerReset( TASK_TIM_HANDLE, &taskTable[taskNum++].timer );
    }
}
/**
  * @brief  :delete the timer of the task.
  * @param  :void (*handler)()
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void DelTaskTimer( Procf handler )
{
    U8 i;

	if ( TASK_MAX_NUM >= taskNum && 0 < taskNum )
	{
		for ( i = 0 ; i < taskNum ; i++ )
		{
			if ( taskTable[i].task == handler )
		    {//move the last task to this index
		        taskTable[i].delayTime = taskTable[taskNum-1].delayTime;
		        taskTable[i].timer     = taskTable[taskNum-1].timer;
		        taskTable[i].task      = taskTable[taskNum-1].task;				
				taskNum--;				
		    }
		}
	}
}
/**
  * @brief  :Initialize the timer of the task.
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitTaskTimer( void )
{
    U8 i;
    for ( i = 0; i < taskNum; i++ )
    {
		UserTimerReset( TASK_TIM_HANDLE, &taskTable[i].timer);
    }
}
/**
  * @brief  :task running kernel
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void TaskTimerRoutine( void )
{
    U8 i;

	for(;;)
	{
	    for ( i = 0; i < taskNum; i++ )
	    {
	        if ( taskTable[i].task != NULL )             /* task is valid        */
	        {
	            if ( UserTimerOver( TASK_TIM_HANDLE, &taskTable[i].timer, taskTable[i].delayTime ) )/* 定时时间到           */
	            {
	                UserTimerReset( TASK_TIM_HANDLE, &taskTable[i].timer );
	                taskTable[i].task(NULL,0);                 /* run the task         */
	            }
	        }
	    }
	}
}
/********************************END OF FILE***********************************/
