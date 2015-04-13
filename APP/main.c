
/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: main.c
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��5��15��
**�� �� ��  ��: V1.3
**��        ��: ������
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.3
** �ա���: 2006��5��15��
** �衡��:1�� �����˴��ڳ�ʱ�жϣ�
**
*******************************************************************************************************

**-------------�汾�Ķ���Ϣ----------------------------------------------------------------------------
**
** 
** �ա���: 2010��5��20��
** �衡��:1�� ������˥����������ز���
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
*                         ������
*
* Description: ��ʼ��ϵͳ�󣬽�����ѭ������˳��ִ�и�������
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void main(void) small
{
	InitCpu();									// �ͱ�Ӧ��ϵͳ��صĳ�ʼ��
	InitSerial();								//���ڳ�ʼ��
  InitSPI();									//��ʼ��SPI����
	InitRF();									//��ʼ��ģ�����
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
 		TaskRoutine();       //��ʱִ��
		TaskHandlePkt();    //����ͨѶ����
	}
}
/*
*********************************************************************************************************
*                         �°���������
*
* Description: ����������Ϣ����Ӧ����
*
* Arguments  : 
*			   parameter	ָ�룬ָ���ṩ���������ز���
*
* Returns    : ��
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
			execute_cmd();						//û�д�����ִ����Ӧ������
		}
		if(ack_flag!=0)
		//else
		{										//��֯�����
			RxBuf[3] = ack_flag;				//��Ӧ���־	
			Data_len = RxBuf[1] + 2;			//���еĳ��ȣ���ַ��1�ֽڣ��������ֽڱ���1�ֽڣ�
		}
		
		watchdog();
		while( OPEN == __PlusSwitchState ){bPulse = TRUE;watchdog();};
		
		if(PktHandle2S_time < PktHandleTimeOut || TRUE == bPulse)
		{						//û�г�ʱ�򷢰�
			sendPkt();
		}
		else
		{						//�����������ݰ���Ч
			pack_valid = 0;
		} 												
	}
}

/*
*********************************************************************************************************
*                         ��ʱִ������
*
* Description: ����Ҫʱ��ѯ����ϵͳ���������쳣ʱ����澯��Ϣ
*
* Arguments  : 
*			   parameter	ָ�룬ָ���ṩ���������ز���
*
* Returns    : ��
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
*                         ��ʱִ�ж�ADF4350������ʱ�ӵļ���л� 
*
* Description: ��TaskRoutine�е��ã�ÿ100MS 
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void Switch_clock()                  //add by  dw	20090603 
{
    unsigned int data vcc_in = 0;
	unsigned char data i;
	if( softDownLoad != DOING )
		{
			//��ADת����ĵ�ѹ�����б𣬲���P2.6���һ����ƽ		 
			//watchdog();
			for( i = 0; i < 8; i++ )
			{
				//vcc_in += readAD(CHANNEL_2);
				vcc_in += readAD(CHANNEL_6);
			}
			//ADCON0 = 0x01;              //�ر�ת�����Է�ֹ���ֵĹܽ��໥Ӱ������add by dw   20090623
			
			vcc_in >>= 3;
			
			if( vcc_in < 62  )              //62 = (0.2  / 3.3) * 2 >>10
				V33C = 0;
			else
				V33C = 1;
		}
}
/*
*********************************************************************************************************
*                         ��ʱִ�и澯��Ϣ����
*
* Description: ��TaskRoutine�е��ã�ÿ5��һ��
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void TaskRunPer5s()
{
	if( softDownLoad != DOING )											//�������ʱ
	{
		updateAlmInfo();												//���¸澯��Ϣ
	}
}
/*
*********************************************************************************************************
*                         ��ʱִ���¶Ȳ���	  ��ȡ�¶�ֵ
*
* Description: ��TaskRoutine�е��ã�ÿ30��һ��
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void TaskRunPer30s() 
{
	if( softDownLoad != DOING )							//�������ʱ�������澯
	{
								                //����������Ƶ���أ�����Ҫ��˥���������໷���½��в���
		readTemperatur();								//�����¶ȼ�¼
		
		//���Ŵ򿪺����ڽ����¶Ȳ�������
		if(gRFSW == OPEN)
		{
			gPALimCompensate();
			writeAD5314(gPALim,DAPOWER_LIM_CHAN);
		}
		//	if((rfPramModified == TRUE) || (gpreRfTemp != gcurRfTemp) )	//��Ƶ�������Ļ��¶��б仯
	//	{
			//tempCompensate();								//�¶Ȳ���
	//	}
	watchdog();
	}  
}
