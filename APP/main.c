
/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: main.c
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年5月15日
**最 新 版  本: V1.3
**描        述: 主函数
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.3
** 日　期: 2006年5月15日
** 描　述:1、 增加了串口超时判断；
**
*******************************************************************************************************

**-------------版本改动信息----------------------------------------------------------------------------
**
** 
** 日　期: 2010年5月20日
** 描　述:1、 增加了衰减器及其相关操作
**
********************************************************************************************************/

#define APP_GLOBALS

#include <reg936.h>
#include <stdio.h>
#include <intrins.h>
#include <string.h>	  
#include <SPI.h>
#include <cpu.h>
#include <serial.h>
#include <task.h>
#include <packet.h>
#include <bro20W_RFcontrol.h>
#include <bro20W_RFparam.h>
#include <timer.h>
#include <readad.h>
#include <e2prom.h>
//#include <math.h>

static void ZhouQiFaSong5ms(void) small
{
#define TRIG_PLUS_BAUD		20	//20hz
//const static unsigned char xdata txPaBuf[] = {0x00,0x40,0x55,0x02,0x00,0x40,0x66,0x02,0x00,0x40,0x77,0x02};	
	static unsigned char idx = 0,cycCnt = 0;
	static unsigned char xdata txPaBuf[] = {0x40,0x00,0x00,0x00,0x00,0x02};
	unsigned char state,check;

	if( ++cycCnt < 100/TRIG_PLUS_BAUD )	
		return;	
	else
		cycCnt = 0;

	if ( __PlusSwitchState == CLOSE )return;
	
	if( idx == 0 )
	{
		txPaBuf[1] = __PlusReqAddr;
		txPaBuf[2] = (unsigned char)((__PlusReqPower&0x0f) + ((__PlusReqFreq<<4)&0x00f0));
		txPaBuf[3] = (unsigned char)(__PlusReqFreq>>4);
		check = 0;
		check^=txPaBuf[1];
		check^=txPaBuf[2];
		check^=txPaBuf[3];
		txPaBuf[4] = check;
	}
	
	state = (unsigned char)(txPaBuf[idx/8] & (1<<(idx%8)));

	if(++idx/8>=sizeof(txPaBuf))
	{
		idx = 0;
		__PlusSwitchState = CLOSE;
	}
	
	if(state!=0)
	{
#ifdef NEW_PLAD		
		writeAD5314(gDAoutB,'B');
		writeAD5314(gDAoutC,'C');			
#endif 		
		RFswitch = 0;
	}
	else
	{
#ifdef NEW_PLAD
		writeAD5314(0,'B');
		writeAD5314(0,'C');	
#endif		
		RFswitch = 1;			
	}		
}

/*
*********************************************************************************************************
*                         主函数
*
* Description: 初始化系统后，进入主循环，按顺序执行各个任务
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void main(void) small
{
	InitCpu();									// 和本应用系统相关的初始化
	InitSerial();								//串口初始化
  InitSPI();									//初始化SPI总线
	InitRF();									//初始化模块参数
	InitTimer1();
	sendInitpacket();
	DwnldSoft1m_time = getTime();

	EA = 0;
	TimerCallback = (void*)ZhouQiFaSong5ms;
	EA = 1;
	
	while(1)
	{			
		if (getTime()- DwnldSoft1m_time >= DwnldSoftTimeOut)
		{
			softDownLoad = UNDO;
		}
 		TaskRoutine();       //定时执行
		TaskHandlePkt();    //串口通讯处理
	}
}
/*
*********************************************************************************************************
*                         新包处理任务
*
* Description: 根据邮箱信息做相应处理
*
* Arguments  : 
*			   parameter	指针，指向提供给任务的相关参数
*
* Returns    : 无
*********************************************************************************************************
*/
void TaskHandlePkt() 
{
	unsigned char bPulse = FALSE;
	
	if(pack_valid == 1)
	{	

//			RDE = 1;
//			SM2 = 0;

		if(ack_flag==0)
		{
			watchdog();
			execute_cmd();						//没有错误则执行相应的命令
		}
		if(ack_flag!=0)
		//else
		{										//组织错误包
			RxBuf[3] = ack_flag;				//置应答标志	
			Data_len = RxBuf[1] + 2;			//包中的长度＋地址（1字节）＋长度字节本身（1字节）
		}
		
		watchdog();
		while( OPEN == __PlusSwitchState ){bPulse = TRUE;watchdog();};
		
		if(PktHandle2S_time < PktHandleTimeOut || TRUE == bPulse)
		{						//没有超时则发包
			sendPkt();
		}
		else
		{						//否则设置数据包无效
			pack_valid = 0;
		} 												
	}
}

/*
*********************************************************************************************************
*                         定时执行任务
*
* Description: 在需要时查询各个系统参数，有异常时处理告警信息
*
* Arguments  : 
*			   parameter	指针，指向提供给任务的相关参数
*
* Returns    : 无
*********************************************************************************************************
*/
void TaskRoutine()
{
	watchdog();
	if( softDownLoad == UNDO || softDownLoad == DONE )
	{
	 	if(Task100MS_time < getTime())		 //add by  dw	20090603 
		{
			Task100MS_time += DELAY100MS;
			Switch_clock();
		}
		if(Task5S_time < getTime())
		{
			Task5S_time += DELAY5S;
			TaskRunPer5s();
		}
		if(Task30S_time < getTime())
		{
			Task30S_time += DELAY30S;
			TaskRunPer30s();
		}
	}
}

/*
*********************************************************************************************************
*                         定时执行对ADF4350的内外时钟的检测切换 
*
* Description: 在TaskRoutine中调用，每100MS 
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void Switch_clock()                  //add by  dw	20090603 
{
    unsigned int data vcc_in = 0;
	unsigned char data i;
	if( softDownLoad != DOING )
		{
			//对AD转换后的电压进行判别，并对P2.6输出一个电平		 
			//watchdog();
			for( i = 0; i < 8; i++ )
			{
				//vcc_in += readAD(CHANNEL_2);
				vcc_in += readAD(CHANNEL_6);
			}
			//ADCON0 = 0x01;              //关闭转换，以防止出现的管脚相互影响现象。add by dw   20090623
			
			vcc_in >>= 3;
			
			if( vcc_in < 62  )              //62 = (0.2  / 3.3) * 2 >>10
				V33C = 0;
			else
				V33C = 1;
		}
}
/*
*********************************************************************************************************
*                         定时执行告警信息更新
*
* Description: 在TaskRoutine中调用，每5秒一次
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void TaskRunPer5s()
{
	if( softDownLoad != DOING )											//软件下载时
	{
		updateAlmInfo();												//更新告警信息
	}
}
/*
*********************************************************************************************************
*                         定时执行温度补偿	  读取温度值
*
* Description: 在TaskRoutine中调用，每30秒一次
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void TaskRunPer30s() 
{
	if( softDownLoad != DOING )							//软件下载时，不发告警
	{
								                //若动作了射频开关，则需要对衰减器和锁相环重新进行操作
		readTemperatur();								//更新温度记录
		
		//功放打开后周期进行温度补偿操作
		if(gRFSW == OPEN)
		{
			gPALimCompensate();
			writeAD5314(gPALim,DAPOWER_LIM_CHAN);
		}
		//	if((rfPramModified == TRUE) || (gpreRfTemp != gcurRfTemp) )	//射频参数更改或温度有变化
	//	{
			//tempCompensate();								//温度补偿
	//	}
	watchdog();
	}  
}
