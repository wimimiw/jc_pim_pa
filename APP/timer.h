/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: timer.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��3��31��
**�� �� ��  ��: V1.2
**��        ��: timer ͷ�ļ�				
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
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
#define PktHandleTimeOut 200            //���ݴ���ʱ2S

APP_EXT unsigned short idata PktHandle2S_time; //���ݰ�����ʱ
APP_EXT unsigned short idata Task100MS_time;
APP_EXT unsigned short data Task5S_time;
APP_EXT unsigned short data Task30S_time;
APP_EXT unsigned short DwnldSoft1m_time;

unsigned short getTime();
void InitTimer1();
		
#endif
