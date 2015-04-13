/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_target.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __TARGET_H__
#define __TARGET_H__

#ifndef __TARGET_C__
#define	__TARGET_EXT__ 	 extern
#else	  
#define	__TARGET_EXT__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
#define I2C_EEPROM_ADDR     		0xA0
#define I2C_LM75_ADDR1      		0x90
#define I2C_LM75_ADDR2      		0x90

#define BYPASS						0
#define PATH1						1
#define PATH2						2

#define AD_SAMPLE_TIME				16
#define OUT_AD_MAX         			4096
#define ATT_VALUE_MAX				63

#define COM_BAUDRATE				9600

#define ADC_CHANNEL_5V				5
#define ADC_CHANNEL_9V				4
#define ADC_CHANNEL_12V				1

#define ADC_CHANNEL_5V_1			6
#define ADC_CHANNEL_5V_2			8
#define ADC_CHANNEL_9V_1			7
#define ADC_CHANNEL_9V_2			9
 
//PC14,PC15
#define DIRMD1(x)					GPIO_WriteBit(GPIOC,GPIO_Pin_14,LOW == x ? Bit_RESET:Bit_SET)
#define DIRMD2(x)					GPIO_WriteBit(GPIOC,GPIO_Pin_15,LOW == x ? Bit_RESET:Bit_SET)
                   		
//operater the gpio
#define RFPOWER_5V_1(x)				GPIO_WriteBit(GPIOB,GPIO_Pin_9 ,FALSE == x ? Bit_RESET:Bit_SET)
#define RFPOWER_5V_2(x)		    	GPIO_WriteBit(GPIOA,GPIO_Pin_11,FALSE == x ? Bit_RESET:Bit_SET)
#define RFPOWER_9V_1(x)				GPIO_WriteBit(GPIOB,GPIO_Pin_8 ,FALSE == x ? Bit_RESET:Bit_SET)
#define RFPOWER_9V_2(x)				GPIO_WriteBit(GPIOA,GPIO_Pin_15,FALSE == x ? Bit_RESET:Bit_SET)

#define RFPOWER_5V_1_RD()			GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_9 )
#define RFPOWER_5V_2_RD()		    GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_11)
#define RFPOWER_9V_1_RD()			GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_8 )
#define RFPOWER_9V_2_RD()			GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_15)

#define RFSWITCH_PATH1(x)		    do{																	  \
										GPIO_WriteBit(GPIOB,GPIO_Pin_4 ,1 != x ? Bit_RESET:Bit_SET);     \
										GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,1 == x ? Bit_RESET:Bit_SET);     \
									}while(0)															  \

#define RFSWITCH_PATH2(x)		    do{																	  \
										GPIO_WriteBit(GPIOA,GPIO_Pin_12,1 == x ? Bit_RESET:Bit_SET);      \
										GPIO_WriteBit(GPIOC,GPIO_Pin_13,1 != x ? Bit_RESET:Bit_SET);      \
									}while(0)															  \

#define RFSWITCH_SW6(x)				GPIO_WriteBit(GPIOB,GPIO_Pin_14,FALSE != x ? Bit_RESET:Bit_SET)
#define RFSWITCH_BYPASS1(x)			GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,FALSE == x ? Bit_RESET:Bit_SET)
#define RFSWITCH_BYPASS2(x)			GPIO_WriteBit(GPIOA,GPIO_Pin_8 ,FALSE != x ? Bit_RESET:Bit_SET)
#define RFSWITCH_TEST(x) 			do{								  \
										GPIO_WriteBit(GPIOB,GPIO_Pin_15 ,1 == x ? Bit_RESET:Bit_SET);\
										GPIO_WriteBit(GPIOB,GPIO_Pin_12 ,1 != x ? Bit_RESET:Bit_SET);\
									}while(0)

#define MAX9947_CONFIG(baud)		do{								\
										if ( baud == 9600 )         \
										{                           \
											DIRMD1(LOW);            \
											DIRMD2(LOW);	        \
										}                           \
										else if ( baud == 38400 )   \
										{                           \
											DIRMD1(HIGH);           \
											DIRMD2(LOW);            \
										}                           \
										else if ( baud == 115200 )  \
										{                           \
											DIRMD1(LOW);            \
											DIRMD2(HIGH);	        \
										}                           \
									}while(0)
	
#define EEPROM_WP_UNPROTECT()   	//GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET  )
#define EEPROM_WP_PROTECT()     	//GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET)
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
extern  const SPI_BUS attBus[];
__TARGET_EXT__ __IO U16 ADCConvertedValue;
/* Public function prototypes ------------------------------------------------*/
void InitTarget(void);
									
#endif/*__TARGET_H*/									
/********************************END OF FILE***********************************/
