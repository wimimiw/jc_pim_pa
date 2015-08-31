/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:target.c
 * @author	:mashuai
 * @version	:v1.0 
 * @date	:2012.4.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __TARGET_C__
#define __TARGET_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct _GPIO_FUNC_ {
    void*group;
    U16	pin;
    U32 remap;
    GPIOMode_TypeDef mode; 
} GPIO_FUNC;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const SPI_BUS attBus[] = 
{
	{{GPIO_PortSourceGPIOB,GPIO_Pin_13},{GPIO_PortSourceGPIOB,GPIO_Pin_10},{GPIO_PortSourceGPIOB,GPIO_Pin_11}},
//	{{GPIO_PortSourceGPIOB,GPIO_Pin_12},{GPIO_PortSourceGPIOB,GPIO_Pin_10},{GPIO_PortSourceGPIOB,GPIO_Pin_11}},
	{{GPIO_PortSourceGPIOB,GPIO_Pin_2 },{GPIO_PortSourceGPIOB,GPIO_Pin_10},{GPIO_PortSourceGPIOB,GPIO_Pin_11}},
//	{{GPIO_PortSourceGPIOB,GPIO_Pin_15},{GPIO_PortSourceGPIOB,GPIO_Pin_10},{GPIO_PortSourceGPIOB,GPIO_Pin_11}},
};

static const GPIO_FUNC gpioTable[] = {
    /* GPROUP0       */
    {GPIOA, GPIO_Pin_0 , 0, GPIO_Mode_Out_PP	 }, /*RDE	         */
    {GPIOA, GPIO_Pin_1 , 0, GPIO_Mode_Out_PP	 }, /*12VDetect      */
    {GPIOA, GPIO_Pin_2 , 0, GPIO_Mode_AF_PP  	 }, /*TXD2	         */
    {GPIOA, GPIO_Pin_3 , 0, GPIO_Mode_IN_FLOATING}, /*RXD2           */
    {GPIOA, GPIO_Pin_4 , 0, GPIO_Mode_Out_PP	 }, /*9VDetect       */
    {GPIOA, GPIO_Pin_5 , 0, GPIO_Mode_Out_PP 	 }, /*5VDetect       */
    {GPIOA, GPIO_Pin_6 , 0, GPIO_Mode_Out_PP	 }, /*1_5VIDT        */
    {GPIOA, GPIO_Pin_7 , 0, GPIO_Mode_Out_PP	 }, /*1_9VIDT        */   
    {GPIOA, GPIO_Pin_8 , 0, GPIO_Mode_Out_PP	 }, /*5SW1           */
    {GPIOA, GPIO_Pin_9 , 0, GPIO_Mode_AF_PP 	 }, /*TXD1           */ 
    {GPIOA, GPIO_Pin_10, 0, GPIO_Mode_IN_FLOATING}, /*RXD1           */
    {GPIOA, GPIO_Pin_11, 0, GPIO_Mode_Out_PP	 }, /*5VSW2          */
    {GPIOA, GPIO_Pin_12, 0, GPIO_Mode_Out_PP	 }, /*4SW1           */
    {GPIOA, GPIO_Pin_13, 0, GPIO_Mode_Out_PP	 }, /*SWDIO          */ //The configuration is forbided with SWD.
    {GPIOA, GPIO_Pin_14, 0, GPIO_Mode_Out_PP	 }, /*SWCLK          */
    {GPIOA, GPIO_Pin_15, 0, GPIO_Mode_Out_PP	 }, /*JTDI /9VSW2    */
                                            	                     
    {GPIOB, GPIO_Pin_0 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_1 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_2 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_3 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_4 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_5 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_6 , 0, GPIO_Mode_AF_OD		 }, //SCL1(EEPROM)   
    {GPIOB, GPIO_Pin_7 , 0, GPIO_Mode_AF_OD		 }, //SDA1(EEPROM)  
    {GPIOB, GPIO_Pin_8 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_9 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_10, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOB, GPIO_Pin_11, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOB, GPIO_Pin_12, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_13, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_14, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOB, GPIO_Pin_15, 0, GPIO_Mode_Out_PP	 }, 

    {GPIOC, GPIO_Pin_0 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_1 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_2 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_3 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_4 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_5 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_6 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_7 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_8 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_9 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_10, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOC, GPIO_Pin_11, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOC, GPIO_Pin_12, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_13, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_14, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOC, GPIO_Pin_15, 0, GPIO_Mode_Out_PP	 }, 

    {GPIOD, GPIO_Pin_0 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_1 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_2 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_3 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_4 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_5 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_6 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_7 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_8 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_9 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_10, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOD, GPIO_Pin_11, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOD, GPIO_Pin_12, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_13, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_14, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOD, GPIO_Pin_15, 0, GPIO_Mode_Out_PP	 }, 

    {GPIOE, GPIO_Pin_0 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_1 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_2 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_3 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_4 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_5 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_6 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_7 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_8 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_9 , 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_10, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOE, GPIO_Pin_11, 0, GPIO_Mode_Out_PP 	 }, 
    {GPIOE, GPIO_Pin_12, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_13, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_14, 0, GPIO_Mode_Out_PP	 }, 
    {GPIOE, GPIO_Pin_15, 0, GPIO_Mode_Out_PP	 }, 
};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :Initialize the system clock
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void InitRcc(void)
{	
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     	    
	/* System Clocks Configuration */
	/* PCLK1 = HCLK/1 */
	RCC_PCLK1Config(RCC_HCLK_Div1);	
  	/* Enable peripheral clocks ------------------------------------------------*/
  	/* Enable DMA1 clock */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
	/* APB1Periph clock enable */
  	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2
						   	|RCC_APB1Periph_I2C1
						   	//|RCC_APB1Periph_I2C2
						   	|RCC_APB1Periph_USART2
								,ENABLE);
	/* APB2Periph clock enable */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO
									|RCC_APB2Periph_GPIOA
									|RCC_APB2Periph_GPIOB
									|RCC_APB2Periph_GPIOC
									|RCC_APB2Periph_GPIOD
									|RCC_APB2Periph_GPIOE
									|RCC_APB2Periph_USART1							
									,ENABLE);	
}
/**
  * @brief  :Initialize the pin`s function
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void InitGpio(void)
{
    U32 i;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* JTAG-DP Disabled and SW-DP Enable */	
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    for (i = 0; i < sizeof(gpioTable) / sizeof(GPIO_FUNC); i++)
    {
		GPIO_InitStructure.GPIO_Pin = gpioTable[i].pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = gpioTable[i].mode;
		GPIO_Init(gpioTable[i].group, &GPIO_InitStructure);
		
		if ( gpioTable[i].remap != 0 )
		{
			GPIO_PinRemapConfig(gpioTable[i].remap,ENABLE);
		}		
    }							
}
/**
  * @brief  :Configure the nested vectored interrupt controller.
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void InitNvic(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);			
}
/**
  * @brief  :Initialize the target board
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitTarget(void)
{
	InitRcc();
	InitNvic();
	InitGpio();	
	InitUart(UART1,COM_BAUDRATE);
	InitUart(UART2,COM_BAUDRATE);
	InitUartBuf(UART1);
	InitUartBuf(UART2);
	InitI2c();
	InitTimer();
}
/********************************END OF FILE***********************************/
