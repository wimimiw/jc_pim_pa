/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: readad.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述: readad 头文件				
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
********************************************************************************************************/

#ifndef __readAD_H__
#define __readAD_H__

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif



//扩展寄存器
/* Analog Digital Converter 0*/
#define ADC0HBND (*((unsigned char volatile xdata*)0xFFEF))
#define ADC0LBND (*((unsigned char volatile xdata*)0xFFEE))
#define AD0DAT0R (*((unsigned char volatile xdata*)0xFFFE))
#define AD0DAT0L (*((unsigned char volatile xdata*)0xFFFF))
#define AD0DAT1R (*((unsigned char volatile xdata*)0xFFFC))
#define AD0DAT1L (*((unsigned char volatile xdata*)0xFFFD))
#define AD0DAT2R (*((unsigned char volatile xdata*)0xFFFA))
#define AD0DAT2L (*((unsigned char volatile xdata*)0xFFFB))
#define AD0DAT3R (*((unsigned char volatile xdata*)0xFFF8))
#define AD0DAT3L (*((unsigned char volatile xdata*)0xFFF9))
#define AD0DAT4R (*((unsigned char volatile xdata*)0xFFF6))
#define AD0DAT4L (*((unsigned char volatile xdata*)0xFFF7))
#define AD0DAT5R (*((unsigned char volatile xdata*)0xFFF4))
#define AD0DAT5L (*((unsigned char volatile xdata*)0xFFF5))
#define AD0DAT6R (*((unsigned char volatile xdata*)0xFFF2))
#define AD0DAT6L (*((unsigned char volatile xdata*)0xFFF3))
#define AD0DAT7R (*((unsigned char volatile xdata*)0xFFF0))
#define AD0DAT7L (*((unsigned char volatile xdata*)0xFFF1))
#define BNDSTA0  (*((unsigned char volatile xdata*)0xFFED))

#endif