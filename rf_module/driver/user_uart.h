/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:user_uart.h
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

#ifndef __USER_UART_C__
#define	__USER_UART_EXT__ 	 extern
#else
#define	__USER_UART_EXT__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
#define UART_CPU_BUF_LEN            512
#define UART1                       0
#define UART2                       1
#define UART_MAX                    2
//RS485 direct pin
#define DRIVER_DIRECT_ENABLE()
#define DRIVER_DIRECT_DISABLE()
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
__USER_UART_EXT__ void InitUart(U8 bus);
__USER_UART_EXT__ void InitUartBuf(U8 bus);
__USER_UART_EXT__ void ResetUartBuf(U8 bus);
__USER_UART_EXT__ void SetUartMode(U8 bus, U8 chkBit);
__USER_UART_EXT__ void GetUartBufInfo(U8 bus, U8**ppbuf, U16 *plen);
__USER_UART_EXT__ void SetUartBufInfo(U8 bus, U8*pbuf, U16 len);
__USER_UART_EXT__ void UartTxOpen(U8 bus,U16 len);
__USER_UART_EXT__ void UartSendStr(USART_TypeDef* USARTx,u8*str,u8 len);
/********************************END OF FILE***********************************/
