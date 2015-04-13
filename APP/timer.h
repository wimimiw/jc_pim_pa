/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: timer.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述: timer 头文件				
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
********************************************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef APP_GLOBALS
#define APP_EXT
#else
#define APP_EXT extern
#endif 

#define DELAY100MS		10
#define DELAY5S		200
#define DELAY30S	6000
#define DwnldSoftTimeOut 12000
#define PktHandleTimeOut 200            //数据处理超时2S

APP_EXT unsigned short idata PktHandle2S_time; //数据包处理定时
APP_EXT unsigned short idata Task100MS_time;
APP_EXT unsigned short data Task5S_time;
APP_EXT unsigned short data Task30S_time;
APP_EXT unsigned short DwnldSoft1m_time;

unsigned short getTime();
void InitTimer1();
		
#endif
