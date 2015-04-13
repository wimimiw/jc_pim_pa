/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_protocol.c
 * @author	:mashuai
 * @version	:v1.0
 * @date	:2012.4.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __RF_PROTOCOL_C__
#define __RF_PROTOCOL_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	U8 flag;
	U8 mode;
	U8 addr;
	U8 cmd;
	U8 ack;
	U8 para[1];
}SLAYER2;
/* Private define ------------------------------------------------------------*/
#define MODE_Nocheck					0x00
#define MODE_UseChecksum				0x06
#define MODE_UseCRC						0x09

#define ACK_Success                		0x00
#define ACK_IllegalCheckAlgorithm		0x01
#define ACK_CheckError            		0x02
#define ACK_IllegalSubunit        		0x03
#define ACK_IllegalCommand        		0x04
#define ACK_IllegalACKSymbol     		0x05
#define ACK_IllegalDataLength    		0x06
#define ACK_IllegalParameter      		0x07
#define ACK_Fault                  		0x08
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//static U16 loadFwCnt;
/*�������*/
static U16  nextSoftPktSn;				//����������ذ������
static U16  preSoftChecksum = 0xFFFF;	//ǰ�����������У���
static U16  __softChecksum;				//���������checksumֵ
static U16  softLen = 0;				//����ܳ���
/* Private function prototypes -----------------------------------------------*/
//static BOOL ProcSetLoadStart(U8*buf,U16 len);
//static BOOL ProcSetLoading(U8*buf,U16 len);
//static BOOL ProcSetLoadingPrev(U8*buf,U16 len);
//static BOOL ProcSetLoadEnd(U8*buf,U16 len);
//static BOOL ProcSetLoadEndPrev(U8*buf,U16 len);
/**
  * @brief  :���ֽڲ��֮CRC16
  			 CRC16-CCITT
			 polynomial:x16+x12+x5+1  1021
			 references:ISO HDLC, ITU X.25, V.34/V.41/V.42, PPP-FCS
  * @param  :
  * @retval :
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
U16	CalcCRC16Value(U8 *ptr,U16 len)
{
	U16	 const crc16Table[16]={
		0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
		0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	};
	U16  crc;
	U8   da;

	for(crc = 0;len--!=0;ptr++)  {
		da = crc >> 12;
		crc <<=4;
		crc^=crc16Table[da^(*ptr>>4)];
		da = crc >> 12;
		crc <<=4;
		crc^= crc16Table[da^(*ptr&0x0f)];
	}

	return(crc);
}
///* Private functions ---------------------------------------------------------*/
///**
//  * @brief  :
//  			references 3GPP 25462-651 A.5 Option 15.1
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v1.0
//  * @date	:2012.4.20
//  */
//static U16 HdlcConvertData(U8 type,U8 ptr[],U16 len)
//{
//	U8 buf[512];
//	U16 i,j;

//	for(i=0;i<len;i++)buf[i]=ptr[i];

//	if ( type == 0x00 )
//	{
//		for (i=0,j=0;j<len;i++,j++)
//		{
//			if ( buf[i] == 0x7E && i!=0 && j!=len-1 )
//			{
//				ptr[j] = 0x7D;
//				j++,len++;
//				ptr[j] = 0x5E;
//			}
//			else if ( buf[i] == 0x7D)
//			{
//				ptr[j] = 0x7D;
//				j++,len++;
//				ptr[j] = 0x5D;
//			}
//			else
//			{
//				ptr[j] = buf[i];
//			}
//		}
//	}
//	else if ( type == 0x01 )
//	{
//		for (i=0,j=0;j<len;i++,j++)
//		{
//			if ( buf[i] == 0x7D && buf[i+1] == 0x5E )
//			{
//				ptr[j] = 0x7E;i++,len--;
//			}
//			else if ( buf[i] == 0x7D && buf[i+1] == 0x5D )
//			{
//				ptr[j] = 0x7D;i++,len--;
//			}
//			else
//			{
//				ptr[j] = buf[i];
//			}
//		}
//	}

//	return len;
//}
///**
//  * @brief  :Return the upgrade flag
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v2.0
//  * @date	:2011.10.20
//  */
//static void ReturnUpgradeFlag(U8 port)
//{
//	U8 *buf;
//	U16 len,crc16;

//	/*��Ӧ�ñ������Ժ�MCU����������������ȷ����Ϣ*/
// 	GetUartBufInfo(port,(U8**)&buf,NULL);

//	buf[0] = 0x7E;
//	buf[1] = MODE_UseCRC;
//	buf[2] = modelAddr;
//	buf[3] = ID_LOAD_END;
//	buf[4] = ACK_Success;
//	buf[5] = 6;
//	buf[6] = (U8)modelFwCrc;
//	buf[7] = modelFwCrc>>8;
//	buf[8] = 0;
//	buf[9] = 0;
//	buf[10] = 0;
//	buf[11] = 0;

//	crc16 = CalcCRC16Value(&buf[1],11);

//	buf[12] = crc16;
//	buf[13] = crc16/256;
//	buf[14] = 0x7E;

//	len = HdlcConvertData(0x00,buf,15);
//	UartTxOpen(port,len);
//}
///**
//  * @brief  :Return the upgrade flag
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v2.0
//  * @date	:2011.10.20
//  */
//static void ReturnResetFlag(U8 port)
//{
//	U8 *buf;
//	U16 len,crc16;

// 	GetUartBufInfo(port,(U8**)&buf,NULL);

//	buf[0] = 0x7E;
//	buf[1] = MODE_UseCRC;
//	buf[2] = modelAddr;
//	buf[3] = ID_RESET;
//	buf[4] = ACK_Success;
//	buf[5] = 6;
//	buf[6] = 0;
//	buf[7] = 0;
//	buf[8] = 0;
//	buf[9] = 0;
//	buf[10] = 0;
//	buf[11] = 0;

//	crc16 = CalcCRC16Value(&buf[1],11);

//	buf[12] = crc16;
//	buf[13] = crc16/256;
//	buf[14] = 0x7E;

//	len = HdlcConvertData(0x00,buf,15);
//	UartTxOpen(port,len);
//}
/*
*********************************************************************************************************
*                         RFģ����Ƶ������ѯ
*
* Description: ��ŷ���Ϊ0x0AF0���Ը���������������һ���Բ�ѯ
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void sendInitpacket(U8 port)
{
	U8 	xor,i,
		*buf,
		*bufPtr;
	
	readE2promStr(sizeof(gDownFlag),EE_DOWN_FLAG,&gDownFlag);
	
	if(gDownFlag == 0xFF)
	{
		gDownFlag = 0x00;		
		writeE2promStr(1,EE_DOWN_FLAG,&gDownFlag);
			
		GetUartBufInfo(port,(U8**)&buf,NULL);
		
		bufPtr = buf;
		
		*(bufPtr++) = 0x00;
		*(bufPtr++) = 0x55;
		*(bufPtr++) = 0xAA;		
				
		*(bufPtr++) = 0x00;	//���ذ��ַ
		*(bufPtr++) = 0x08;	//����
		*(bufPtr++) = 0xA0; //�����ʶ
		*(bufPtr++) = 0x00;	//Ӧ���־
		*(bufPtr++) = 0x06;	//��ض��󳤶�
		*(bufPtr++) = 0xFD;
		*(bufPtr++) = 0x0A;
		*(bufPtr++) = 0x01;
		*(bufPtr++) = 0x01;
		*(bufPtr++) = 0x01;		
		
		for( i = 3 , xor = 0; i < 13 ; i++)
		{
			xor ^= buf[i];
		}
		
		*(bufPtr++) = xor;
		
		UartTxOpen(port,14);
	}
}
/*
*********************************************************************************************************
*                         ����������������
*
* Description: ��ŷ���Ϊ0x0ADA���Ը������������������ã���ѯ����һ������
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void execFctParamSet(U8 *buf,U16 rxLen,U16 *txLen)
{
	U8 	tempLen,
		temp[4],
		*fReqFlag,
		*fRcvLen,
		*fRcvGet,
		*fRcvID,
		*fRcvDataLen,
		*fRcvData;

	U16 param_addr,		
		tmpInt=0,
		tPALim;

	fReqFlag 	= buf+6;
	fRcvLen		= buf+1;
	fRcvGet		= buf+7;
	fRcvID		= buf+10;
	fRcvDataLen	= buf+12;
	fRcvData	= buf+13;

	//������Ӧ��EEPROM��ַ
	param_addr 	= *(U16*)fRcvID;
	tempLen 	= *fRcvDataLen;
	tPALim 		= gPALim;

	//����������ѯ
	if(*fRcvGet==0)
	{
		//һ�������E2PROM�ж�ȡ
		readE2promStr(tempLen,param_addr,fRcvData);
		//�����������Ҫʵʱ��ѯ
		switch(param_addr)
		{
			case EE_CenFreq:
				if ( gRFSrcSelect == 0 )
				{
					memset(fRcvData,0,4);
				}
				break;
			case EE_DA_CHANNEL_B:
				*(fRcvData+0) = gDAoutB*gRFSW;
				*(fRcvData+1) = (gDAoutB*gRFSW)>>8;
				break;
			case EE_DA_CHANNEL_C:
				*(fRcvData+0) = gDAoutC*gRFSW;
				*(fRcvData+1) = (gDAoutC*gRFSW)>>8;
				break;
			case EE_PAProtectLim:
				//IRcvStr(0x90,0x03,temp,2);         //0x90 ��1 ��0 ��0 ��1 ,  A2  ��A1�� A0��R/W ��ɣ�����λ�̶�������λ�ǵ�ַλ����дλ
				*(fRcvData+0) = temp[0];             //0x03��ʾҪ��ȡ��IIC�����ڲ��Ĵ�����ַ
				break;
			case EE_PAResetLim:
				//IRcvStr(0x90,0x02,temp,2);
				*(fRcvData+0) = temp[0];
				break;
			case EE_PASW:
			case EE_RFSW:
				*(fRcvData+0) = gRFSW;
				break;
				/*
			case 0x00C4:
				*(fRcvData+0) = PLL_state;
				break;

			case 0x00C5:								    //2�ֽڣ�����У���
				*(fRcvData+0) = Lim_state;
				break;
			case 0x00C6:	  							    //2�ֽڣ�����汾��
				*(fRcvData+0) = (U8)PA_current;
				*(fRcvData+1) = (U8)(PA_current >> 8);
				break;
			case 0x00C7:								    //2�ֽڣ�����У���
				*(fRcvData+0) = (U8)OutputPwr;
				*(fRcvData+1) = (U8)(OutputPwr >> 8);
				break;
			case 0x00C8:	  							    //2�ֽڣ�����汾��
				*(fRcvData+0) = (U8)re_Pwr;
				*(fRcvData+1) = (U8)(re_Pwr >> 8);
				break;
				*/
			case EE_CURRENT_TEMP:
				*(fRcvData+0) = (U8)gcurRfTemp;
				*(fRcvData+1) = (U8)(gcurRfTemp >> 8);
				break;
			case EE_VERSION:	  							//2�ֽڣ�����汾��
				*(fRcvData+0) = Ver_Low;
				*(fRcvData+1) = Ver_High;
				break;
			case EE_ALL_CHECKSUM:							//2�ֽڣ�����У���
				*(fRcvData+0) = (U8)all_checksum;
				*(fRcvData+1) = (U8)(all_checksum >> 8);
				break;
  			default:
			    break;
		}
	}
	//���������趨
	else
	{
		if (tempLen==2)
		{
			tmpInt = *(U16*)fRcvData;
		}

		//��Ҫ���ڶԽ��յ����ݽ��м��飬�Ƿ�Ϊ����ֵ�ķ�Χ�ڣ������ǣ��򲻴���EEPROM
		switch(param_addr)
		{
			//2�ֽڣ������޷�
			case EE_DA_CHANNEL_B:
			case EE_DA_CHANNEL_C:
				if ( tmpInt > 1023 )
					*fReqFlag |= DATRANGE_ERR;
				break;
			//2�ֽ�,����ƫѹֵ
			case EE_gPALim:
				if( ((S16)tmpInt<0)||(tmpInt>1023) )
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1�ֽڣ������²�ϵ������
			case EE_GainNumer:
			//1�ֽڣ������²�ϵ����ĸ
			case EE_GainDenomi:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>255))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1�ֽڣ����������¶�����
			case EE_PAResetLim:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>125))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1�ֽڣ�����״̬
			case EE_TEST_MARK:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>2))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1�ֽڣ��²���׼ֵ
			case EE_TEMP_VALUE:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>200))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			default:
				break;
		}

		//���յ�����Ϊ����ֵ����ʼ�����ݴ�EEPROM
		if(*fReqFlag==0x0A)
		{
			if(param_addr != EE_FREQ_ADD_POWEWR)
			{
				if ( param_addr == EE_FREQ_ADD_POWEWR && tempLen == 6 )
				{
					writeE2promStr(4,EE_CenFreq,fRcvData);
					writeE2promStr(2,EE_gPALim,fRcvData+4);
				}
				else
				{
					writeE2promStr(tempLen,param_addr,&*(fRcvData+0));
				}
			}

			rfPramModified = TRUE;//��Ƶ�����޸ı�־��Ч

			switch(param_addr)
			{
				case EE_TEMPER_BUCHANG:
					memcpy((U8*)__temp_que,fRcvData,sizeof(__temp_que));
				break;
				//�������������ź�
				case EE_PLUS_TRIGER:
					//EA = 0;
					__PlusSwitchState 	= OPEN;
					__PlusReqAddr		= *(fRcvData+0);
					__PlusReqPower    	= *(fRcvData+1);
					__PlusReqFreq		= *(fRcvData+2)+(*(fRcvData+3)<<8);
					//EA = 1;
				break;
				case EE_DA_CHANNEL_B:
					gDAoutB = tmpInt;
					writeAD5314(gDAoutB,'B');
				break;
				case EE_DA_CHANNEL_C:
					gDAoutC = tmpInt;
					writeAD5314(gDAoutC,'C');
				break;
				//��Դѡ��
				case EE_SOURCE_SELECT:
					//RFSrcSelect = (*(fRcvData+0) != SRC_INTERNAL && *(fRcvData+0) != SRC_EXTERNAL)?SRC_INTERNAL:*(fRcvData+0);
					writePLL();
				break;
				//˥��������
				case EE_Att1:
				   gGain=*(fRcvData+0);
				   writeAtt1(gGain);
				   updateAlmInfo();
				//2�ֽڣ���������޷�ֵ
				case EE_gPALim:
					//2012.10.26
					gPALimCompensate();
					gPALim = (gPALim + tPALim)/2;
					//writeAD5314(gPALim,DAPOWER_LIM_CHAN);
					updateAlmInfo();

					delay(1);

					gPALimCompensate();
					//writeAD5314(gPALim,DAPOWER_LIM_CHAN);
					updateAlmInfo();
					//Task5S_time = getTime() + 100;
					break;
				//4�ֽڣ�����Ƶ��
				case EE_CenFreq:
				//6�ֽڣ�����Ƶ��+�޷�ֵ
				case EE_FREQ_ADD_POWEWR:
					//swapBytes(&*(fRcvData+0),(U8 *)&gCenFreq,4);
					writePLL();
					delay(1);		//�ı�����Ƶ����Ҫͬʱ�����޷�����
					//gPALimCompensate();
                	//writeAD5314(gPALim,'B');
					//2012.10.26
					if ( tempLen == 6 )
					{
						gPALimCompensate();
						gPALim = (gPALim + tPALim)/2;
						//writeAD5314(gPALim,DAPOWER_LIM_CHAN);
						updateAlmInfo();

						//;delay(1);

						gPALimCompensate();
						//writeAD5314(gPALim,DAPOWER_LIM_CHAN);
						updateAlmInfo();
					}
					break;
				//2�ֽڣ�Ƶ�ʲ���ֵ
				case EE_FreqStep:
				//4�ֽڣ��ο�Ƶ��
				case EE_RefFreq:
					writePLL();
					break;
				//�����޷���������
				case 0x0075:
				//���ʲ�����ĸ
				case 0x0077:
				//��׼Ƶ��
				case EE_gPBmFreq:
					//2012.10.26
					gPALimCompensate();
					gPALim = (gPALim + tPALim)/2;
					writeAD5314(gPALim,DAPOWER_LIM_CHAN);
					updateAlmInfo();

					delay(1);

					gPALimCompensate();
					writeAD5314(gPALim,DAPOWER_LIM_CHAN);
					updateAlmInfo();
					break;
				//2�ֽڣ����ű����¶�����
				case EE_PAProtectLim:
					gPAProtecttLim = *(fRcvData+0);
					writeLM75(0x03,gPAProtecttLim,3);
					break;
				//2�ֽڣ����������¶�����
				case EE_PAResetLim:
					gPAResetLim = *(fRcvData+0);
					writeLM75(0x02,gPAResetLim,3);
					break;
				//1�ֽڣ���Ƶģ����
				case EE_RF_No:
					gRF_No = *(fRcvData+0);
					break;
				//1�ֽڣ���Ƶ����
				case EE_RFSW:
					if(AutoSwitch != *(fRcvData+0));
					{
						AutoSwitch = *(fRcvData+0);
						updateAlmInfo();
						writeAtt1(gGain);
						writePLL();//��Ƶ���ض����ˣ�ÿ��Ҫ���²�������
					}
					//watchdog();
					break;
				default:
					break;
			}
		}
	}

	*txLen = *fRcvLen + 2;
}
/*
*********************************************************************************************************
*                         RFģ����Ƶ������ѯ
*
* Description: ��ŷ���Ϊ0x0AF0���Ը���������������һ���Բ�ѯ
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void execRFParamQ(U8 *buf,U16 rxLen,U16*txLen)
{
	U8 i;
	U16 CKS = 0;
	static U16 tmpCurr=0;

	//tmpCurr = (tmpCurr + PA_current + PA_current)/3;					 //����ֵ��ǰֵƽ��

	if(buf[7]==0)										
	{//��ʱ���ݲ�ѯ
		buf[1] = 18; //���ݰ�����									
		buf[4] = 16; //��ض���ĳ���									
		buf[8] = Lim_state;
		buf[9] = gRFSW;
		buf[10] = gRFSW;
		buf[11] = (U8)tmpCurr; 		//��λ��ǰ	
		buf[12] = (U8)(tmpCurr>>8);	//��λ�ں�	
		buf[13] = (U8)OutputPwr;
		buf[14] = (U8)(OutputPwr>>8);
  		buf[15] = (U8)re_Pwr;
		buf[16] = (U8)(re_Pwr>>8);
		buf[17] = (S8)gcurRfTemp;
		buf[18] = (S8)(gcurRfTemp>>8);
		buf[19] = PLL_state;			//080220 ����	
		
		*txLen = 20;
	}
	else if(buf[7]==1)	  							
	{//����������ѯ���ݰ�
		buf[1] = 94;					//���ݰ�����				
		buf[4] = 92;					//��ض���ĳ���				
		
		readE2promStr(5,EE_AtteVal,&buf[10]);			//�������
		readE2promStr(4,EE_Att1slopeval,&buf[15]);
		readE2promStr(11,EE_gPALim,&buf[19]);			//���Ų���
		readE2promStr(3,EE_gPALimNumer,&buf[30]);
		readE2promStr(20,EE_BandWidth,&buf[37]);		//Ƶ�ʲ���
		readE2promStr(4,EE_gPBmFreq,&buf[57]);
		readE2promStr(14,EE_CurCo,&buf[61]);			//��������
		readE2promStr(4,EE_GainNumer,&buf[75]);			//�¶Ȳ���
		readE2promStr(3,EE_RF_No,&buf[79]);				//ģ�����
				
		memcpy((U8*)&gCenFreq,buf+33,sizeof(gCenFreq));
		
		buf[80] = gRFSW;
		buf[81] = gRFSW;
		buf[82] = Ver_Low;
		buf[83] = Ver_High;
		buf[84] = (U8)all_checksum;
		buf[85] = (U8)(all_checksum >> 8);
		
		readE2promStr(10,EE_MODULE_No,&buf[86]);		//ģ�����	
		
	   	//watchdog();
		
		for(i=0;i<86;i++)
		{
			CKS += buf[10+i];
		}
		
		buf[8] = (U8)(CKS);
		buf[9] = (U8)(CKS>>8);
		
		*txLen = 96;
	}
	else
	{
		//ack_flag = OC_ERR;
	}
}
/*
*********************************************************************************************************
*                         RFģ���������						
*
* Description: ��ŷ���Ϊ0x0AFE������Ƶģ��ĳ��������ϵͳ����
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
U8 execDwnldSoft(U8 *buf)
{
	U8 *bufPtr;
	U16 dataLen;
	U16 curPktIdx;

	bufPtr = buf;

	bufPtr += 7;//ȡ�����ݵĿ�ͷ�������
	curPktIdx 	= *(U16*)bufPtr;//ȡ�����������curSoftPktSn��
	bufPtr += 2;//ָ��ָ�����ݳ���
	dataLen 	= *(U16*)bufPtr;//ȡ�����ݳ���	
	bufPtr += 2;//ָ�����ݲ���										
	
	if( curPktIdx == 0 )									//��һ����
	{
		if( dataLen != 0x1C )								//ȡ�����ݳ��Ȳ�ָ����ʼ��ַ
		{																					 
			return OTHER_ERR;								//��������
		}
		
		bufPtr += 0x18;										//ָ��ָ���ļ�CHECKSUM�ֽ�		
		__softChecksum = *(U16*)bufPtr;
		InitCode2Flash();
	}
	else			  										
	{
		Code2Flash(bufPtr,dataLen);		
	}	
	
	return NO_ERR;											//��������
}
/*
*********************************************************************************************************
*                         ������سɹ����ѯģ����Ϣ
*
* Description: ��ŷ���Ϊ0x0AFD������������Ҫ�������EEPROM�ж�����flash�У�������ת�Ƶ�0x0000ִ��
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
U8 endDwnldSoft()
{	
	EndCode2Flash();
	gDownFlag = 0xFF;	
	writeE2promStr(1,EE_DOWN_FLAG,&gDownFlag);			//����ʱ������ر�־������ 20090415 by zxl
   	return TRUE;
}

/*
*********************************************************************************************************
*                         ���ݰ�����
*
* Description: ��readXPktBuf�е����ݽ��н��룬�������ֲ�֣�������������ݣ�ȡ��CRCֵ
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void execute_cmd(U8 *buf,U16 rxLen,U16 *tlen)
{
	U8 	resultCode = NO_ERR,
		downresult = NO_ERR,
		*fMode,
		*fRcvGet,
		*fRcvUpgrade,
		*fRcvAck;
	
	fMode = buf+5;
	fRcvAck = buf+6;
	fRcvGet = buf+7;
	fRcvUpgrade = buf+8;
	
	switch(*fMode)
	{
		//��ѯ�����ù�������
		case ID_FCT_PARAM_WR:										
		{
			execFctParamSet(buf,rxLen,tlen);
			break;
	    }
		//RFģ�������ѯ
		case ID_FCT_PARAM_SAMPLE:										
		{
			execRFParamQ(buf,rxLen,tlen);
			break;
		}
		//�������ȷ�ϻ��ѯģ����Ϣ
		case ID_FIRMWARE_UPGRAD_ENTER:										
		{
			if(*fRcvGet==0x01)
			{
				if(*fRcvUpgrade==0)							
				{//����ȡ��
					*tlen = 10;
					//softDownLoad = UNDO;
				}
				else									
				{//����ȷ��
					//if(softDownLoad == DOING)
					{//��������־
						downresult = endDwnldSoft();
			 			*fRcvAck |= downresult;				
					}
					//else
					{//��������־
						*fRcvAck |= OTHER_ERR;				
					}
					
					*tlen = 10;
				}
			}
			break;
		}
		//�������
		case ID_FIRMWARE_UPGRAD_LOAD:										
		{
		  //DwnldSoft1m_time = getTime();
			//resultCode = execDwnldSoft();
			
			buf[1] = 9;//���ݰ�����								
			buf[4] = 7;
 			buf[6] |= resultCode;	//��������־					
			buf[9] = (U8)nextSoftPktSn;
			buf[10] = (U8)(nextSoftPktSn>>8);
			
			*tlen = 11;
			
			break;
		}
		//�����λ 
		case ID_FIRMWARE_UPGRAD_REBOOT:
		{
			if(*fRcvGet == 0x01)
			{
				//AUXR1 = 8;                                
			}
			break;
		}
		//�����Ŵ�,��ض������޷�ʶ��
		default:										
		{
			*fRcvAck |= OCID_ERR;						
			break;
		}
	}
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
static void TMAPktHandle(U8 port)
{
	U8 *buf,		
		xor;
		
	U16 len,
		tLen,
		i,j;	

	GetUartBufInfo(port,(U8**)&buf,&len);

	if ( len > 8  )
	{
		//֡��ʽ: 00 55 AA 00 06 A0 FF 04 F0 0A 00 A7 
		//�ҵ�֡��ʼ
		for( i = 0 ; i < 10 ; i++)
			if( buf[i] == SYNC1 )
				break;
		//��������ʼ
		if( i > 10 )
			return;
		//ȷ��֡��ʼ
		if( buf[i+1] != SYNC2 )		
			return;		
		//ȷ�Ͻ������һ֡����
		if( len > (i+5) && buf[i+3] <= (len-i-5) )
		{
			//����У���
			for( j = i+2,xor = 0; j < len - 1; j++)
				xor ^= buf[j];	
				
			if( xor != buf[len-1] )
			{
				ResetUartBuf(port);
				return;
			}			
			//���ݵ�Ԫ�е�Ӧ���־
			buf[i+5] = 0;
			//ִ������
			execute_cmd(buf+i+2,len-i-3,&tLen);
			//����У���
			for( j = 0,xor = 0; j < tLen ; j++) 
			{
				xor ^= buf[i+j+2];
			}			
			buf[i+2+tLen] = xor;
			//��������
			UartTxOpen(port,i+2+tLen);
		}			
	}		
}
///**
//  * @brief  :
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v1.0
//  * @date	:2012.4.20
//  */
//static BOOL ProcSetLoadStart(U8*buf,U16 len)
//{
//	loadFwCnt = 0;
//	return InitCode2Flash();
//}
///**
//  * @brief  :
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v1.0
//  * @date	:2012.4.20
//  */
//static BOOL ProcSetLoading(U8*buf,U16 len)
//{
//	BOOL result;

//	result = Code2Flash(buf,len);

//	if ( result == TRUE )
//	{
//		loadFwCnt += len;
//		buf[0] = (U8)loadFwCnt;
//		buf[1] = loadFwCnt/256;
//		buf[2] = 0;
//		buf[3] = 0;
//		buf[4] = 0;
//		buf[5] = 0;
//	}

//	return result;
//}
///**
//  * @brief  :
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v1.0
//  * @date	:2012.4.20
//  */
//static BOOL ProcSetLoadingPrev(U8*buf,U16 len)
//{
//	BOOL result;

//	if ( (len + loadFwCnt) > SIZEOF_BUFFER )
//	{
//		len = SIZEOF_BUFFER - loadFwCnt;
//	}

//	if ( len == 0 )return FALSE;

//	result = Code2Flash(buf,len);

//	if ( result == TRUE )
//	{
//		loadFwCnt += len;
//		buf[0] = (U8)loadFwCnt;
//		buf[1] = loadFwCnt/256;
//		buf[2] = 0;
//		buf[3] = 0;
//		buf[4] = 0;
//		buf[5] = 0;
//	}

//	return result;
//}
///**
//  * @brief  :
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v1.0
//  * @date	:2012.4.20
//  */
//static BOOL ProcSetLoadEndPrev(U8*buf,U16 len)
//{
//	U16 crc16;

//	crc16 = *(U16*)buf;

//	//hardware version is consistency
//	if ( HARDWARE_VERSION != HARDWARE_VERSION_BAK )return FALSE;
//	//UpgradeAll version is consistency
//	if ( (SOFTWARE_VERSION&0xF0) != (SOFTWARE_VERSION_BAK&0xF0) )return FALSE;

//	if ( modelFwCrc == crc16 ) return TRUE;

//	if( crc16 != CalcFirmwareBufCRC16())return FALSE;

//	return EndCode2FlashRrev();
//}
///**
//  * @brief  :
//  * @param  :None
//  * @retval :None
//  * @author	:mashuai
//  * @version:v1.0
//  * @date	:2012.4.20
//  */
//static BOOL ProcSetLoadEnd(U8*buf,U16 len)
//{
//	U16 crc16;

//	crc16 = *(U16*)buf;

//	//hardware version is consistency
//	if ( HARDWARE_VERSION != HARDWARE_VERSION_BAK )return FALSE;

//	if ( modelFwCrc == crc16 ) return TRUE;

//	if( crc16 != CalcFirmwareBufCRC16())return FALSE;

//	return EndCode2Flash();
//}
/**
  * @brief  :Init the protocol
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitProtocol(void)
{
	InitUartBuf(UART1);
	InitUartBuf(UART2);

	sendInitpacket(UART1);
	
//	if ( TRUE == RebootFlagCheck() )
//	{
//		ReturnResetFlag(UART1);
//		ReturnResetFlag(UART2);
//	}
//	if ( TRUE == DownFlagCheck() )
//	{
//		ReturnUpgradeFlag(UART1);
//		ReturnUpgradeFlag(UART2);
//	}
}
/**
  * @brief  :protocol task routine
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
int TaskProtocol(int*argv[],int argc)
{
	TMAPktHandle(UART1);
	TMAPktHandle(UART2);
	return 0;
}
/********************************END OF FILE***********************************/
