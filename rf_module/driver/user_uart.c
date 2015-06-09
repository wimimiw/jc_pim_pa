/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:user_uart.c
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __USER_UART_C__
#define __USER_UART_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct _UART_Q_ {            /* UART GUEUE 结构体    */
    U8 *buf;                         /* 数据缓冲区指针       */
    U16 bufInPtr;                    /* 输入数据指针         */
    U16 bufOutPtr;                   /* 输出数据指针         */
    U16 bufDataLen;                  /* 数据长度             */
} UART_Q_S;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
		U8		uartBuf			[UART_MAX][UART_CPU_BUF_LEN];
static UART_Q_S uartBufQS		[UART_MAX];
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
	U8 i,out[] = "Debug STM32F100C8 USART!\n\r";
	UserTimerDef timer;

	InitTarget();

	USART_Cmd(USART1,DISABLE);
	
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);	

	USART_Cmd(USART1,ENABLE);

	i = 0;
	UserTimerReset(TIM2,&timer);
	for(;;)
	{
		if( UserTimerOver(TIM2,&timer,USER_TIMER_1S(1)))
		{
			UserTimerReset(TIM2,&timer);			
			while(1)
			{
				if (SET == USART_GetFlagStatus(USART1,USART_FLAG_TXE))
				{
					USART_ClearFlag(USART1,USART_FLAG_TXE);
					if ( '\0' != out[i])
					{
						USART_SendData(USART1,out[i++]);
					}
					else
					{
						i = 0;break;			
					}
				}
			}
		}
	}
}
#endif
/**
  * @brief  :写数据到UART缓冲区
  * @param  :buf 缓冲区指针
  * @param	:bus 选择通道
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitUartBuf(U8 bus)
{
    uartBufQS[bus].buf        = uartBuf[bus];
    uartBufQS[bus].bufDataLen = 0;
    uartBufQS[bus].bufInPtr   = 0;
    uartBufQS[bus].bufOutPtr  = 0;
}
/**
  * @brief  :复位UART Buf数据
  * @param  :bus 选择通道
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void ResetUartBuf(U8 bus)
{
    uartBufQS[bus].bufDataLen = 0;
    uartBufQS[bus].bufInPtr   = 0;
    uartBufQS[bus].bufOutPtr  = 0;
}
/**
  * @brief  :写数据到UART缓冲区
  * @param  :data 入队的数据
  * @param	:bus 选择通道
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void UartBufDataIn(U8 bus, U8 data)
{
    U16	i;
    i = uartBufQS[bus].bufInPtr;
    uartBufQS[bus].buf[i]		= data;                                 		   /* 数据入队             */
    uartBufQS[bus].bufInPtr 	= (uartBufQS[bus].bufInPtr + 1) % UART_CPU_BUF_LEN;/* 调整指针             */
    uartBufQS[bus].bufDataLen	= (uartBufQS[bus].bufDataLen+1) % UART_CPU_BUF_LEN;
}
/**
  * @brief  :UART缓冲区数据出队
  * @param  :len 出队的数据个数
  * @param	:bus 选择通道
  * @retval :U8* 输出数据地址
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static U8* UartBufDataOut(U8 bus)
{
    U8 i;
	if ( uartBufQS[bus].bufDataLen )                                                   /* 判断缓冲区数据长度   */
    {
        i = uartBufQS[bus].bufOutPtr;
		uartBufQS[bus].bufOutPtr  = ++uartBufQS[bus].bufOutPtr % UART_CPU_BUF_LEN; /* 调整指针             */
        uartBufQS[bus].bufDataLen --;                                                  /* 需要检查是否单指令   */
		return &uartBufQS[bus].buf[i];
    }
    return NULL;
}
/**
  * @brief  :UART口读数据
  * @param  :bus 选择通道
  * @param	:**ppbuf	缓冲区首地址
  * @param	:len		缓冲区数据长度
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void GetUartBufInfo(U8 bus, U8**ppbuf, U16 *plen)
{
	if (NULL != ppbuf)*ppbuf = uartBufQS[bus].buf;
	if (NULL != plen )*plen  = uartBufQS[bus].bufDataLen;
}
/**
  * @brief  :设置Uart缓冲区信息
  * @param	:**ppbuf	缓冲区首地址
  * @param	:len		缓冲区数据长度
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void SetUartBufInfo(U8 bus, U8*pbuf, U16 len)
{
	//if( NULL != pbuf)uartBufQS[bus].buf = pbuf;
	uartBufQS[bus].bufDataLen = len;
}
/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{		
	static UserTimerDef rxTimer;
	volatile U8 data;
	volatile U8	*pt;

	if ( SET == USART_GetITStatus(USART1,USART_IT_TC) )
	{//transmitter interrupt
		DRIVER_DIRECT_ENABLE();
		USART_ClearITPendingBit(USART1,USART_IT_TC);						
		pt = UartBufDataOut(UART1);
		if ( NULL != pt )
		{
			USART_SendData(USART1,*pt);		
			*pt = 0;//发送完成后将对应缓存清零
		}
		else
		{
			DRIVER_DIRECT_DISABLE();
		}
	}
	else if ( SET == USART_GetITStatus(USART1,USART_IT_RXNE) )
	{//receive interrupt
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);		
		//process the timeout
		if ( TRUE == UserTimerOver(TIM2,&rxTimer,USER_TIMER_1MS(5)) )
		{
			ResetUartBuf(UART1);
		}
		UserTimerReset(TIM2,&rxTimer);							
		data = USART_ReceiveData(USART1);
		UartBufDataIn(UART1,data);	
	}
	else
	{//exception 
		InitUart(UART1);
		ResetUartBuf(UART1);
	} 
	return;
}
/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{		
	static UserTimerDef rxTimer;
	volatile U8 data;
	volatile U8	*pt;

	if ( SET == USART_GetITStatus(USART2,USART_IT_TC) )
	{//transmitter interrupt
		DRIVER_DIRECT_ENABLE();
		USART_ClearITPendingBit(USART2,USART_IT_TC);						
		pt = UartBufDataOut(UART2);
		if ( NULL != pt )
		{
			USART_SendData(USART2,*pt);		
			*pt = 0;//发送完成后将对应缓存清零
		}
		else
		{
			DRIVER_DIRECT_DISABLE();
		}
	}
	else if ( SET == USART_GetITStatus(USART2,USART_IT_RXNE) )
	{//receive interrupt
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);		
		//process the timeout
		if ( TRUE == UserTimerOver(TIM2,&rxTimer,USER_TIMER_1MS(5)) )
		{
			ResetUartBuf(UART2);
		}
		UserTimerReset(TIM2,&rxTimer);							
		data = USART_ReceiveData(USART2);
		UartBufDataIn(UART2,data);	
	}
	else
	{//exception 
		InitUart(UART2);
		ResetUartBuf(UART2);
	} 
	return;
}

//None
void UartSendStr(USART_TypeDef* USARTx,u8*str,u8 len)
{
	u8 i;
	
	for(i=0;i<len;i++)
	{
		USART_SendData(USARTx,(u16)*(str++));
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)!=SET){};
	}
}

/**
  * @brief  :串口通信初始化
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitUart(U8 bus)
{
	USART_TypeDef*pUart;
	USART_InitTypeDef USART_InitStructure;

	if ( bus == UART1 )
	{
		pUart = USART1;
	}
	else if ( bus == UART2 )
	{
		pUart = USART2;
	}
	else
	{
		return;
	}	
	
	USART_InitStructure.USART_BaudRate = COM_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	/* Configure USART1 */
	USART_Init(pUart, &USART_InitStructure);
	//clear the it flag
	USART_ClearITPendingBit(pUart,USART_IT_RXNE);
	USART_ClearITPendingBit(pUart,USART_IT_TC);
	/* Enable USARTz Receive and Transmit interrupts */
	USART_ITConfig(pUart, USART_IT_RXNE, ENABLE);		
	USART_ITConfig(pUart, USART_IT_TC, ENABLE);
	/* Enable the USARTy */
	USART_Cmd(pUart, ENABLE);	
}

/**
  * @brief  :串口通信初始化
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitUartIAP(U8 bus)
{
	USART_TypeDef*pUart;
	USART_InitTypeDef USART_InitStructure;

	if ( bus == UART1 )
	{
		pUart = USART1;
	}
	else if ( bus == UART2 )
	{
		pUart = USART2;
	}
	else
	{
		return;
	}	
	
	USART_InitStructure.USART_BaudRate = COM_BAUDRATE_IAP;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	/* Configure USART1 */
	USART_Init(pUart, &USART_InitStructure);
	//clear the it flag
	USART_ClearITPendingBit(pUart,USART_IT_RXNE);
	USART_ClearITPendingBit(pUart,USART_IT_TC);
	/* Enable USARTz Receive and Transmit interrupts */
	USART_ITConfig(pUart, USART_IT_RXNE, ENABLE);		
	USART_ITConfig(pUart, USART_IT_TC, ENABLE);
	/* Enable the USARTy */
	USART_Cmd(pUart, ENABLE);	
}
/**
  * @brief  :Open the uart interrupt
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void UartTxOpen(U8 bus,U16 len)
{
	U8*pt;
	uartBufQS[bus].bufDataLen = len;
	uartBufQS[bus].bufOutPtr  = 0;
	pt = UartBufDataOut(bus);
	if ( NULL != pt )
	{
		(bus == UART1)?USART_SendData(USART1,*pt):USART_SendData(USART2,*pt);
	}
}
/********************************END OF FILE***********************************/
