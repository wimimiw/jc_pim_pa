/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: readad.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��3��31��
**�� �� ��  ��: V1.2
**��        ��: readad ͷ�ļ�				
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/

#ifndef __readAD_H__
#define __readAD_H__

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif



//��չ�Ĵ���
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