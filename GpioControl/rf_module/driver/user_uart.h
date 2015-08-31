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
#define	__USER_UART_EXT__ 	 extern
#else
#define	__USER_UART_EXT__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef void(*UART_CALLBACK)(U8*,U16) ;
/* Public define -------------------------------------------------------------*/
#define UART_CPU_BUF_LEN            512
#define UART1                       0
#define UART2                       1
#define UART_MAX                    2
//RS485 direct pin
#define DRIVER_DIRECT_ENABLE()		GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET)
#define DRIVER_DIRECT_DISABLE()		GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET)
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
__USER_UART_EXT__ UART_CALLBACK __Uart1_Callback;
__USER_UART_EXT__ UART_CALLBACK __Uart2_Callback;
/* Public function prototypes ------------------------------------------------*/
__USER_UART_EXT__ void InitUart(U8 bus,U32 baud);
__USER_UART_EXT__ void InitUartBuf(U8 bus);
__USER_UART_EXT__ void ResetUartBuf(U8 bus);
__USER_UART_EXT__ void SetUartMode(U8 bus, U8 chkBit);
__USER_UART_EXT__ void GetUartBufInfo(U8 bus, U8**ppbuf, U16 *plen);
__USER_UART_EXT__ void SetUartBufInfo(U8 bus, U8*pbuf, U16 len);
__USER_UART_EXT__ void UartTxOpen(U8 bus,U16 len);
/********************************END OF FILE***********************************/
