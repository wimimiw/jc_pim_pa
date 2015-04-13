/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: serial.c
**创   建   人: 张小龙
**修   改   人: 
**最后修改日期: 2008年9月1日
**最 新 版  本: V1.3
**描        述: 去掉自动地址识别功能
/**-------------历史版本信息--------------------------------------------------------------------------------
**文   件   名: serial.c
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年5月15日
**最 新 版  本: V1.3
**描        述: 串口初始化、接收和发送中断处理程序
				
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
** 描　述: 1、增加了串口超时操作；
**
********************************************************************************************************/

#include <reg938.h>
#include <serial.h>
#include <string.h>
#include <packet.h>
#include <bro20W_RFcontrol.h>
#include "timer.h"
										

/*
*********************************************************************************************************
*                         串口初始化
*
* Description: 串口初始化以及与串口接收、发送相关参数
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void InitSerial()		
{
	// 初始化串口	
					//485接收有效，发送禁止
	Rx_state = STA_ADDRESS1;	//初始化接收状态,第一个字节为
	pack_valid = 0;
//    RDE = 0;            //没使用

	//9位数据模式
	SCON  =	0xF0;				//串口模式3，使用多机通信，接收使能
	BRGCON= 0x00; 		  		//波特率发生器使能
//	BRGR1 = 0x00;				//波特率115200bit/s，采用11.0592的晶振 
//	BRGR0 = 0x50;				//在高波特率的情况下，MCU端RXD脚的电容最好别加
	BRGR1 = 0x04;				//波特率9600bit/s，采用外部11.0592的晶振 
	BRGR0 = 0x70;

	SSTAT = 0xE0; 				//选择独立的RX/TX中断
	BRGCON= 0x03;				//启动UART的波特率发生器 

	//8位数据模式
/*	SCON  =	0x50;				//串口模式3，使用多机通信，接收使能
	BRGCON= 0x00; 		  		//波特率发生器使能
	//BRGR1 = 0x02;				//波特率9600bit/s，采用cpu内部晶振 
	//BRGR0 = 0xF0;
	

	BRGR1 = 0x04;				//波特率9600bit/s，采用外部11.0592的晶振 
	BRGR0 = 0x70;

	SSTAT = 0xA0; 				//选择独立的RX/TX中断
	BRGCON= 0x03;				//启动UART的波特率发生器 
*/	
	ESR = 1;					//串口RI使能
	EST = 1;					//串口TI使能

}

/*
*********************************************************************************************************
*                         串口发送中断服务程序
*
* Description: 实现串口的发送，有独立的波特率发生器
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void SerialSend_ISR() small interrupt 13 
{    
	if(TI)
	{
		TI = 0;
		/*commet test for uart*/
		TB8 = 0x00;					//开始发送数据字节，TB8必须为0
	 	Tx_ptr++;
		if(Tx_ptr<(TxBuf[4]+6))		//485通信时，最后发一个字节00，232通信时，需要去除00，防止误发
		{
			SBUF = TxBuf[Tx_ptr];
		}
		else
		{
//			RDE =  0;    //232通信时，没使用
		} 
	}		
}

/*
*********************************************************************************************************
*                         串口接收中断服务程序
*
* Description: 实现串口的接收，有独立的波特率发生器
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void SerialRcv_ISR() small interrupt 4 
{    
	static unsigned short recv_timer;	       
	unsigned char Rx_byte;

	if ( getTime() - recv_timer > 10 )	  //50ms
	{
		pack_valid = 0;
		Rx_state = STA_ADDRESS1;			
	}
	
	recv_timer = getTime();	

	EA = 0;												//关中断

	if(RI)
	{		
		RI = 0;		
   		Rx_byte = SBUF;
		switch(Rx_state)
		{
			case STA_ADDRESS1:
				//if((Rx_byte==gRF_No)||(Rx_byte==0xFE))//接收到的地址为本机地址或广播地址
				//{
//				 	Rx_state = STA_INIT1;				
//					SM2 = 0;	
				//}
				//else									//非本模块数据，则不接收
				//{
				//	Rx_state = STA_ADDRESS1;
				//}
//				break;
			case STA_INIT1:				  				//包起始第一个字节
				if(Rx_byte==SYNC1)
				{
					Rx_state = STA_INIT2;	
				}
			   	else									//非本模块数据，则不接收
				{
					Rx_state = STA_ADDRESS1;
				}  
				break;
			case STA_INIT2:								//包起始第二个字节
				if(Rx_byte==SYNC2)
				{
					Rx_state = STA_ADDRESS2;	
				}
				else									//非本模块数据，则不接收
				{
					Rx_state = STA_ADDRESS1;
				} 
				break; 
			case STA_ADDRESS2:			  				//模块地址或者广播地址
//				if((Rx_byte==gRF_No)||(Rx_byte==0xFE))
//				{
					Rx_state = STA_PACKSIZE;
					Rx_chksum = Rx_byte;				//校验和为异或校验
					RxBuf[0] = Rx_byte;	
//				}
//				else									//非本模块数据，则不接收
//				{
//					Rx_state = STA_ADDRESS1;
//				}
				break;
			case STA_PACKSIZE:				  			//数据单元长度

				 Rx_chksum ^= Rx_byte;
				 RxBuf[1] = Rx_byte;
				 if(Rx_byte)
				 {
					Rx_state = STA_CMD;	
				 }	
				 else 									//若数据长度为空，则回到初始化状态，不接收
				 {
				 	 Rx_state = STA_ADDRESS1;	
				 }
				 break;
			case STA_CMD:				  				//数据单元中的命令标识，内部命令固定为0xA0
				if(Rx_byte==FCT_CMD)
				{
					Rx_state = STA_ACK;
					RxBuf[2] = Rx_byte;
					Rx_chksum ^= Rx_byte;
				}
				else									//若不是内部命令0xA0则不响应
				{
					Rx_state = STA_ADDRESS1;
				}
				break;
			case STA_ACK:  								//数据单元中的应答标志
				if(Rx_byte==ACK)
				{
					Rx_state = STA_OL;
					Rx_chksum ^= Rx_byte;
					RxBuf[3] = 0x00;		   			//回复包的校验和设置为0
					ack_flag = 0;
				}
				else
				{
					Rx_state = STA_ADDRESS1;
				}
				break;
			case   STA_OL:								//监控对象长度							
				 Rx_chksum ^= Rx_byte;
				 RxBuf[4] = Rx_byte;
				 if(Rx_byte)
				 {
				 	Rx_state = STA_OID1;	
				 }	
				 else 				   					
				 {
				 	Rx_state = STA_ADDRESS1;		
				 }
				 break;
			case  STA_OID1:								//监控对象标号
				 Rx_chksum ^= Rx_byte;
				 RxBuf[5] = Rx_byte;
				 Rx_state = STA_OID2;	
				 break;
			case  STA_OID2:								//监控对象标号
				 Rx_chksum ^= Rx_byte;
				 RxBuf[6] = Rx_byte;
				 Rx_ptr = 7;
				 Rx_state = STA_OC;	
				 break;

			case STA_OC:								//监控对象内容
				 Rx_chksum ^= Rx_byte;
				 RxBuf[Rx_ptr] = Rx_byte;
				 Rx_ptr++;
				 if((Rx_ptr-4)== RxBuf[4])				//监控对象内容接收完毕
				 {
				 	Rx_state = STA_CHKSUM;		
				 }
				 if(Rx_ptr>(RxBuf[1]+2))
				 {
				 	Rx_state = STA_ADDRESS1;
					pack_valid = 1;
					//RDE = 1;
					ack_flag = LEN_ERR;					//长度错	
				   	//SM2 = 1;
				   	PktHandle2S_time = 0;               //数据处理定时器清零
				 }
				 break;
			case STA_CHKSUM:							//接收到数据的校验和
				if(Rx_byte!=Rx_chksum)
				{
					ack_flag = DEFSUM_ERR;		
				}
				Rx_state = STA_ADDRESS1;				
				pack_valid = 1;
				PktHandle2S_time = 0;               //数据处理定时器清零
				//RDE = 1;
				//SM2 = 1;
				break;
			default:		   							//重新开始接受下一个包
				Rx_state = STA_ADDRESS1;
				break;	
		} 
	}	
														//485发送有效，接收禁止
	EA = 1;	  	   										//开中断
}