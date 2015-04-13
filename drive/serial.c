/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: serial.c
**��   ��   ��: ��С��
**��   ��   ��: 
**����޸�����: 2008��9��1��
**�� �� ��  ��: V1.3
**��        ��: ȥ���Զ���ַʶ����
/**-------------��ʷ�汾��Ϣ--------------------------------------------------------------------------------
**��   ��   ��: serial.c
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��5��15��
**�� �� ��  ��: V1.3
**��        ��: ���ڳ�ʼ�������պͷ����жϴ������
				
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
** �衡��: 1�������˴��ڳ�ʱ������
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
*                         ���ڳ�ʼ��
*
* Description: ���ڳ�ʼ���Լ��봮�ڽ��ա�������ز���
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void InitSerial()		
{
	// ��ʼ������	
					//485������Ч�����ͽ�ֹ
	Rx_state = STA_ADDRESS1;	//��ʼ������״̬,��һ���ֽ�Ϊ
	pack_valid = 0;
//    RDE = 0;            //ûʹ��

	//9λ����ģʽ
	SCON  =	0xF0;				//����ģʽ3��ʹ�ö��ͨ�ţ�����ʹ��
	BRGCON= 0x00; 		  		//�����ʷ�����ʹ��
//	BRGR1 = 0x00;				//������115200bit/s������11.0592�ľ��� 
//	BRGR0 = 0x50;				//�ڸ߲����ʵ�����£�MCU��RXD�ŵĵ�����ñ��
	BRGR1 = 0x04;				//������9600bit/s�������ⲿ11.0592�ľ��� 
	BRGR0 = 0x70;

	SSTAT = 0xE0; 				//ѡ�������RX/TX�ж�
	BRGCON= 0x03;				//����UART�Ĳ����ʷ����� 

	//8λ����ģʽ
/*	SCON  =	0x50;				//����ģʽ3��ʹ�ö��ͨ�ţ�����ʹ��
	BRGCON= 0x00; 		  		//�����ʷ�����ʹ��
	//BRGR1 = 0x02;				//������9600bit/s������cpu�ڲ����� 
	//BRGR0 = 0xF0;
	

	BRGR1 = 0x04;				//������9600bit/s�������ⲿ11.0592�ľ��� 
	BRGR0 = 0x70;

	SSTAT = 0xA0; 				//ѡ�������RX/TX�ж�
	BRGCON= 0x03;				//����UART�Ĳ����ʷ����� 
*/	
	ESR = 1;					//����RIʹ��
	EST = 1;					//����TIʹ��

}

/*
*********************************************************************************************************
*                         ���ڷ����жϷ������
*
* Description: ʵ�ִ��ڵķ��ͣ��ж����Ĳ����ʷ�����
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void SerialSend_ISR() small interrupt 13 
{    
	if(TI)
	{
		TI = 0;
		/*commet test for uart*/
		TB8 = 0x00;					//��ʼ���������ֽڣ�TB8����Ϊ0
	 	Tx_ptr++;
		if(Tx_ptr<(TxBuf[4]+6))		//485ͨ��ʱ�����һ���ֽ�00��232ͨ��ʱ����Ҫȥ��00����ֹ��
		{
			SBUF = TxBuf[Tx_ptr];
		}
		else
		{
//			RDE =  0;    //232ͨ��ʱ��ûʹ��
		} 
	}		
}

/*
*********************************************************************************************************
*                         ���ڽ����жϷ������
*
* Description: ʵ�ִ��ڵĽ��գ��ж����Ĳ����ʷ�����
*
* Arguments  : ��
*
* Returns    : ��
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

	EA = 0;												//���ж�

	if(RI)
	{		
		RI = 0;		
   		Rx_byte = SBUF;
		switch(Rx_state)
		{
			case STA_ADDRESS1:
				//if((Rx_byte==gRF_No)||(Rx_byte==0xFE))//���յ��ĵ�ַΪ������ַ��㲥��ַ
				//{
//				 	Rx_state = STA_INIT1;				
//					SM2 = 0;	
				//}
				//else									//�Ǳ�ģ�����ݣ��򲻽���
				//{
				//	Rx_state = STA_ADDRESS1;
				//}
//				break;
			case STA_INIT1:				  				//����ʼ��һ���ֽ�
				if(Rx_byte==SYNC1)
				{
					Rx_state = STA_INIT2;	
				}
			   	else									//�Ǳ�ģ�����ݣ��򲻽���
				{
					Rx_state = STA_ADDRESS1;
				}  
				break;
			case STA_INIT2:								//����ʼ�ڶ����ֽ�
				if(Rx_byte==SYNC2)
				{
					Rx_state = STA_ADDRESS2;	
				}
				else									//�Ǳ�ģ�����ݣ��򲻽���
				{
					Rx_state = STA_ADDRESS1;
				} 
				break; 
			case STA_ADDRESS2:			  				//ģ���ַ���߹㲥��ַ
//				if((Rx_byte==gRF_No)||(Rx_byte==0xFE))
//				{
					Rx_state = STA_PACKSIZE;
					Rx_chksum = Rx_byte;				//У���Ϊ���У��
					RxBuf[0] = Rx_byte;	
//				}
//				else									//�Ǳ�ģ�����ݣ��򲻽���
//				{
//					Rx_state = STA_ADDRESS1;
//				}
				break;
			case STA_PACKSIZE:				  			//���ݵ�Ԫ����

				 Rx_chksum ^= Rx_byte;
				 RxBuf[1] = Rx_byte;
				 if(Rx_byte)
				 {
					Rx_state = STA_CMD;	
				 }	
				 else 									//�����ݳ���Ϊ�գ���ص���ʼ��״̬��������
				 {
				 	 Rx_state = STA_ADDRESS1;	
				 }
				 break;
			case STA_CMD:				  				//���ݵ�Ԫ�е������ʶ���ڲ�����̶�Ϊ0xA0
				if(Rx_byte==FCT_CMD)
				{
					Rx_state = STA_ACK;
					RxBuf[2] = Rx_byte;
					Rx_chksum ^= Rx_byte;
				}
				else									//�������ڲ�����0xA0����Ӧ
				{
					Rx_state = STA_ADDRESS1;
				}
				break;
			case STA_ACK:  								//���ݵ�Ԫ�е�Ӧ���־
				if(Rx_byte==ACK)
				{
					Rx_state = STA_OL;
					Rx_chksum ^= Rx_byte;
					RxBuf[3] = 0x00;		   			//�ظ�����У�������Ϊ0
					ack_flag = 0;
				}
				else
				{
					Rx_state = STA_ADDRESS1;
				}
				break;
			case   STA_OL:								//��ض��󳤶�							
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
			case  STA_OID1:								//��ض�����
				 Rx_chksum ^= Rx_byte;
				 RxBuf[5] = Rx_byte;
				 Rx_state = STA_OID2;	
				 break;
			case  STA_OID2:								//��ض�����
				 Rx_chksum ^= Rx_byte;
				 RxBuf[6] = Rx_byte;
				 Rx_ptr = 7;
				 Rx_state = STA_OC;	
				 break;

			case STA_OC:								//��ض�������
				 Rx_chksum ^= Rx_byte;
				 RxBuf[Rx_ptr] = Rx_byte;
				 Rx_ptr++;
				 if((Rx_ptr-4)== RxBuf[4])				//��ض������ݽ������
				 {
				 	Rx_state = STA_CHKSUM;		
				 }
				 if(Rx_ptr>(RxBuf[1]+2))
				 {
				 	Rx_state = STA_ADDRESS1;
					pack_valid = 1;
					//RDE = 1;
					ack_flag = LEN_ERR;					//���ȴ�	
				   	//SM2 = 1;
				   	PktHandle2S_time = 0;               //���ݴ���ʱ������
				 }
				 break;
			case STA_CHKSUM:							//���յ����ݵ�У���
				if(Rx_byte!=Rx_chksum)
				{
					ack_flag = DEFSUM_ERR;		
				}
				Rx_state = STA_ADDRESS1;				
				pack_valid = 1;
				PktHandle2S_time = 0;               //���ݴ���ʱ������
				//RDE = 1;
				//SM2 = 1;
				break;
			default:		   							//���¿�ʼ������һ����
				Rx_state = STA_ADDRESS1;
				break;	
		} 
	}	
														//485������Ч�����ս�ֹ
	EA = 1;	  	   										//���ж�
}