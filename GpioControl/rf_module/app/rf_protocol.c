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
#include "stdio.h"
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

typedef struct 
{
	U8 rev1;
	U8 totLen;
	U8 pm1;
	U8 pm2;
	U8 objLen;
	U8 mode;
	U8 ack;
	U8 get;
	U8 upgrade1;	
	U8 upgrade2;
	U8 id1;
	U8 id2;
	U8 idLen;
	U8 idBuf[1];
}JC_LAYER1;
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
static int loadFwCnt;
/* Private function prototypes -----------------------------------------------*/
//static BOOL ProcSetLoadStart(U8*buf,U16 len);
//static BOOL ProcSetLoading(U8*buf,U16 len);
//static BOOL ProcSetLoadingPrev(U8*buf,U16 len);
//static BOOL ProcSetLoadEnd(U8*buf,U16 len);
//static BOOL ProcSetLoadEndPrev(U8*buf,U16 len);
/**
  * @brief  :���ֽڲ����֮CRC16
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
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :
  			references 3GPP 25462-651 A.5 Option 15.1
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
U16 HdlcConvertData(U8 type,U8 ptr[],U16 len)
{
	U8 buf[512];
	U16 i,j;

	for(i=0;i<len;i++)buf[i]=ptr[i];

	if ( type == 0x00 )
	{
		for (i=0,j=0;j<len;i++,j++)
		{
			if ( buf[i] == 0x7E && i!=0 && j!=len-1 )
			{
				ptr[j] = 0x7D;
				j++,len++;
				ptr[j] = 0x5E;
			}
			else if ( buf[i] == 0x7D)
			{
				ptr[j] = 0x7D;
				j++,len++;
				ptr[j] = 0x5D;
			}
			else
			{
				ptr[j] = buf[i];
			}
		}
	}
	else if ( type == 0x01 )
	{
		for (i=0,j=0;j<len;i++,j++)
		{
			if ( buf[i] == 0x7D && buf[i+1] == 0x5E )
			{
				ptr[j] = 0x7E;i++,len--;
			}
			else if ( buf[i] == 0x7D && buf[i+1] == 0x5D )
			{
				ptr[j] = 0x7D;i++,len--;
			}
			else
			{
				ptr[j] = buf[i];
			}
		}
	}

	return len;
}
/**
  * @brief  :Return the upgrade flag
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void ReturnUpgradeFlag(U8 port)
{
	U8 *buf;
	U16 len,crc16;

	/*��Ӧ�ñ������Ժ�MCU����������������ȷ����Ϣ*/
 	GetUartBufInfo(port,(U8**)&buf,NULL);

	buf[0] = 0x7E;
	buf[1] = MODE_UseCRC;
	buf[2] = modelAddr;
	buf[3] = ID_LOAD_END;
	buf[4] = ACK_Success;
	buf[5] = 6;
	buf[6] = (U8)modelFwCrc;
	buf[7] = modelFwCrc>>8;
	buf[8] = 0;
	buf[9] = 0;
	buf[10] = 0;
	buf[11] = 0;

	crc16 = CalcCRC16Value(&buf[1],11);

	buf[12] = crc16;
	buf[13] = crc16/256;
	buf[14] = 0x7E;

	len = HdlcConvertData(0x00,buf,15);
	UartTxOpen(port,len);
}
/**
  * @brief  :Return the upgrade flag
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void ReturnResetFlag(U8 port)
{
	U8 *buf;
	U16 len,crc16;

 	GetUartBufInfo(port,(U8**)&buf,NULL);

	buf[0] = 0x7E;
	buf[1] = MODE_UseCRC;
	buf[2] = modelAddr;
	buf[3] = ID_RESET;
	buf[4] = ACK_Success;
	buf[5] = 6;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;
	buf[9] = 0;
	buf[10] = 0;
	buf[11] = 0;

	crc16 = CalcCRC16Value(&buf[1],11);

	buf[12] = crc16;
	buf[13] = crc16/256;
	buf[14] = 0x7E;

	len = HdlcConvertData(0x00,buf,15);
	UartTxOpen(port,len);
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
void sendInitpacket(U8 port)
{
	U8 	chk,i,*buf,*bufPtr,flag;		
	UserTimerDef timer;
	
	chk = chk;
	i = i;	
	bufPtr = bufPtr;
	
	if(port == UART1)		
		ReadE2prom(EE_IAP_FALG1,&flag,sizeof(flag));
	else if(port == UART2)
		ReadE2prom(EE_IAP_FALG2,&flag,sizeof(flag));
			
	if(flag == 0xFF)
	{		
		flag = 0;
		
		if(port == UART1)		
			WriteE2prom(EE_IAP_FALG1,&flag,sizeof(flag));
		else if(port == UART2)
			WriteE2prom(EE_IAP_FALG2,&flag,sizeof(flag));
			
		GetUartBufInfo(port,(U8**)&buf,NULL);
					
		bufPtr = buf;
		
		buf[0] = 0;
		buf[1] = 'Y';
		
		InitUart(port,COM_BAUDRATE_IAP);
		
		UartTxOpen(port,2);
		
		UserTimerReset(TIM2,&timer);
		//�˴���ʱ����100ms			
		while(!UserTimerOver(TIM2,&timer,USER_TIMER_10MS(5)));	
		
		InitUart(port,COM_BAUDRATE);
		
//		*(bufPtr++) = 0x00;
//		*(bufPtr++) = 0x55;
//		*(bufPtr++) = 0xAA;		
//				
//		*(bufPtr++) = 0x00;	//���ذ��ַ
//		*(bufPtr++) = 0x08;	//����
//		*(bufPtr++) = 0xA0; //�����ʶ
//		*(bufPtr++) = 0x00;	//Ӧ���־
//		*(bufPtr++) = 0x06;	//��ض��󳤶�
//		*(bufPtr++) = 0xFD;
//		*(bufPtr++) = 0x0A;
//		*(bufPtr++) = 0x01;
//		*(bufPtr++) = 0x01;
//		*(bufPtr++) = 0x01;		
//		
//		for( i = 3 , chk = 0; i < 13 ; i++)
//		{
//			chk ^= buf[i];
//		}
//		
//		*(bufPtr++) = chk;
//		
//		UartTxOpen(port,14);
	}
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
BOOL execRFParamQ(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	U8 i;
	U16 cks = 0;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;

	//layer->ack = NO_ERR;
	layer->ack = 0x0A;
	
	if(layer->get==0)										
	{//��ʱ���ݲ�ѯ
		layer->totLen = 18;								
		layer->objLen = 16;		
		
		*(U8 *)(buf+8 ) = gLimState;
		*(U8 *)(buf+9 ) = gRFSW;
		*(U8 *)(buf+10) = gRFSW;
		*(U16*)(buf+11) = gPACurrent;
		*(U16*)(buf+13) = gPwrOut;
		*(U16*)(buf+15) = gPwrRef;
		*(U16*)(buf+17) = gCurRfTemp;
		*(U8 *)(buf+19) = !gPLLLock;
		
		*txLen = layer->totLen+2;
	}
	else if(layer->get==1)	  							
	{//����������ѯ���ݰ�
		layer->totLen = 94;								
		layer->objLen = 92;							
		
		ReadE2prom(EE_AtteVal		,buf+10,5);
		ReadE2prom(EE_Att1slopeval	,buf+15,4);
		ReadE2prom(EE_gPALim		,buf+19,11);
		ReadE2prom(EE_gPALimNumer	,buf+30,3);
		ReadE2prom(EE_CenFreq		,buf+33,4);
		ReadE2prom(EE_BandWidth		,buf+37,20);
		ReadE2prom(EE_gPBmFreq		,buf+57,4);
		ReadE2prom(EE_CurCo			,buf+61,14);
		ReadE2prom(EE_GainNumer		,buf+75,4);
		ReadE2prom(EE_RF_No			,buf+79,3);
		ReadE2prom(EE_MODULE_No		,buf+86,10);
						
		buf[80] = gRFSW;
		buf[81] = gRFSW;
		
		*(U16*)(buf+82) = (FW_VERSION_H<<8) + FW_VERSION_L;		
		*(U16*)(buf+84) = gFWCheck;
				
		for(i=0;i<86;i++)cks += buf[10+i];

		*(U16*)(buf+8) = cks;
		
		*txLen = layer->totLen+2;
	}
	else
	{
		layer->ack |= OC_ERR;
	}
	
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
//��������ȷ�ϻ��ѯģ����Ϣ
BOOL execFWEnter(U8 flag,U8*buf,U16 rxLen,U16 *txLen)
{
	BOOL result = TRUE;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;
	
	result = result;
	layer->ack = NO_ERR;
	
	if(layer->get == 1)
	{
		if(layer->upgrade1 == 0)
		{//����ȡ��
			//
		}
		else
		{//����ȷ��	
			//00 55 AA 00 08 A0 FF 06 FD 0A 01 01 00 A6 
			if(EndCode2Flash() == TRUE)
			{
				gDownFlag = 0xFF;
				if(FALSE == WriteE2prom(EE_DOWN_FLAG,(U8*)&gDownFlag,sizeof(gDownFlag)))
				{
					layer->ack |= EEPROM_ERR;
				}				
			}
			else
			{
				layer->ack |= EEPROM_ERR;
			}
		}
		
		*txLen = 10;
	}
	
	return TRUE;
}

BOOL execFWLoad(U8 flag,U8*buf,U16 rxLen,U16 *txLen)
{
	BOOL result = TRUE;
	U8 *bufPtr;
	U16 dataLen;
	U16 curPktIdx;	
	JC_LAYER1 *layer = (JC_LAYER1*)buf;	
	static U16 nextPktIdx = 0,checksum;
	
	layer->rev1 = 0;
	layer->pm2 = 0;
	//layer->mode = 0x0A;
	//layer->ack = NO_ERR;
	
	result = result;
	checksum = checksum;
	bufPtr = buf;

	curPktIdx 	= *(U16*)(bufPtr+7);//ȡ�����������curSoftPktSn��
	dataLen 	= *(U16*)(bufPtr+9);//ȡ�����ݳ���	
	bufPtr += 18;					//ָ�����ݲ���										
	
	if( curPktIdx == 0 )			//��һ����
	{
		if( dataLen != 0x1C )		//ȡ�����ݳ��Ȳ�ָ����ʼ��ַ
		{																					 
			layer->ack |= LEN_ERR;	//��������
			return FALSE;
		}
		
		bufPtr += 0x18;				//ָ��ָ���ļ�CHECKSUM�ֽ�		
		checksum = *(U16*)bufPtr;
		
		if(InitCode2Flash()==FALSE)
			layer->ack |= EEPROM_ERR;
	}
	else			  										
	{
		if( curPktIdx != nextPktIdx )
		{
			layer->ack |= OC_ERR;;
		}
		else if(Code2Flash(bufPtr,dataLen)==FALSE)
		{
			layer->ack |= OTHER_ERR;	
		}
	}	

	nextPktIdx = curPktIdx + 1;
	
	layer->totLen = 9;
	layer->objLen = 7;
	*(U16*)&layer->upgrade2 = nextPktIdx;

	*txLen = 11;
			
	return TRUE;
}
//������λ 
BOOL execFWReboot(U8 flag,U8*buf,U16 rxLen,U16 *txLen)
{
	BOOL result = TRUE;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;	
	
	result = result;
	layer->ack = NO_ERR;

	*txLen = 10;
	
	return TRUE;
}

void execAnaylize(U8 *buf,U16 rxLen,U16 *tlen)
{
	U8 i;
	U16 objCmd;
	S32 temp;
	BOOL recvflag = FALSE;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;
	JC_COMMAND *sc = GetTable();
	
	temp = temp;
	
	objCmd = layer->id1 + (layer->id2<<8);
	
	if( layer->get == 1 && layer->mode == ID_FCT_PARAM_WR )
	{								
		WriteE2prom(objCmd,layer->idBuf,layer->idLen);				
	}		
	
	*tlen = layer->totLen + 2;
	layer->pm2 = 0;
	
	for(i=0;i<GetTableMebCnt();i++)
	{
		recvflag = TRUE;
		
		if(sc[i].cmd == layer->mode && sc[i].sub == NULL)
		{						
			if(sc[i].proc != NULL)sc[i].proc(layer->get,buf,rxLen,tlen);
			break;
		}
		else if(sc[i].cmd == layer->mode && sc[i].sub == objCmd)
		{
			if(layer->get == 0 && sc[i].var != NULL )
			{//��ѯ 
				memcpy(layer->idBuf,sc[i].var,layer->idLen);
			}
			else if( layer->get == 1 && sc[i].var != NULL )
			{//����
				if(sc[i].max != 0 && sc[i].min != 0)
				{	
					
				}
				
				memcpy(sc[i].var,layer->idBuf,layer->idLen);
				
				gRFModify = TRUE;					
			}
										
			if(sc[i].proc != NULL)sc[i].proc(layer->get,buf,rxLen,tlen);			
			break;
		}
	}	
	
	//�����Ŵ�,��ض������޷�ʶ��
	if(recvflag == FALSE)layer->ack |= OCID_ERR;	
}

static void TaskIAP(U8 port)
{
	U8 *buf,result,chk,flag,addr = 0;
	U16 len=0,tLen,i,bufIdx;
	UserTimerDef timer;	
	static BOOL bFWLoad = FALSE;
	static UserTimerDef rollTimer;
	
	const char *atStart = "AT+STA";
	const char *atDown  = "AT+DOW";
	const char *atExec  = "AT+EXE";
	const char *atVers  = "AT+CHK?";
	
	GetUartBufInfo(port,(U8**)&buf,&len);	
	
	/*�������ػ�����,���ع�����5�����޶�����ָ�����Ĭ������*/
	if(UserTimerOver(TIM2,&rollTimer,USER_TIMER_10MS(500)) && bFWLoad == TRUE)
	{
		bFWLoad = FALSE;
		InitUart(port,COM_BAUDRATE);
	}	
	
	//���ߵ�ַʶ��	(0~64)
	//if(buf[0] != 0 && buf[0] != addr)return;
	
	if( buf[1] == 'F' && len >= 5 )
	{//��һ��Э��	
		if( buf[2] == 'S' && 
			buf[3] == 'T' &&
			buf[4] == 'A' )
		{//00 46 53 54 41
			ResetUartBuf(port);
			memset(buf+1,0,4);
			result = 'N';
			
			if(InitCode2Flash()==TRUE)
				result = 'Y';
			
			buf[0] = addr;
			buf[1] = result;			
			UartTxOpen(port,2);
			
			UserTimerReset(TIM2,&timer);
			//??????100ms			
			while(!UserTimerOver(TIM2,&timer,USER_TIMER_10MS(5)));	
			
			InitUart(port,COM_BAUDRATE_IAP);
			
			//?????????
			bFWLoad = TRUE;
			UserTimerReset(TIM2,&rollTimer);
		}		
		if( buf[2] == 'S' && 
			buf[3] == 'T' &&
			buf[4] == 'B' )
		{//00 46 53 54 41
			ResetUartBuf(port);
			memset(buf+1,0,4);
			result = 'N';
			
			if(InitCode2Flash()==TRUE)
				result = 'Y';
			
			buf[0] = addr;
			buf[1] = result;			
			UartTxOpen(port,2);
			
			UserTimerReset(TIM2,&timer);
			//??????100ms			
			while(!UserTimerOver(TIM2,&timer,USER_TIMER_10MS(5)));	
			
			//?????????
			bFWLoad = TRUE;
			UserTimerReset(TIM2,&rollTimer);
		}		
		else if( buf[2] == 'D' &&
				 buf[3] == 'O' &&
				 buf[4] == 'W' && len >= buf[5] + 7 )
		{//00 46 44 4F 57 00 00
			UserTimerReset(TIM2,&rollTimer);
			ResetUartBuf(port);
			memset(buf+1,0,4);
			result = 'N';
			chk = 0;
			
			tLen = buf[5];
						
			for(i = 0 ; i< tLen;i++)
			{
				chk ^= buf[i+6];
			}
			
			if(chk == buf[tLen+6] && Code2Flash(buf+6,tLen)== TRUE )
			{
				result = 'Y';
			}
			
			buf[0] = addr;
			buf[1] = result;
			UartTxOpen(port,2);				
		}
		else if( buf[2] == 'E' && 
				 buf[3] == 'X' && 
				 buf[4] == 'E' )
		{//00 46 45 58 45
			UserTimerReset(TIM2,&rollTimer);
			ResetUartBuf(port);
			memset(buf+1,0,4);
			result = 'N';
			
			if(port == UART1)
			{	
				flag = 0xFF;				
				if(TRUE == WriteE2prom(EE_IAP_FALG1,&flag,sizeof(flag)))
				{
					EndCode2Flash();
					result = 'Y';
				}
			}
			else if(port == UART2)
			{	
				flag = 0xFF;				
				if(TRUE == WriteE2prom(EE_IAP_FALG2,&flag,sizeof(flag)))
				{
					EndCode2Flash();
					result = 'Y';
				}
			}	

			buf[0] = addr;
			buf[1] = result;									
			UartTxOpen(port,2);			
		}
		else if( buf[2] == 'C' && 
				 buf[3] == 'H' && 
				 buf[4] == 'K' )
		{//00 46 45 58 45
			ResetUartBuf(port);
			memset(buf+1,0,8);
				
			buf[0] = addr;
			sprintf((char*)&buf[1],"%04X",gFWCheck);									
			UartTxOpen(port,5);			
		}			
	}	
	else if(len > 4 && buf[len] == '\n')
	{//�ڶ���Э��		
		if( 0==memcmp(&buf[1],atStart,strlen(atStart)) )
		{
			ResetUartBuf(port);
			memset(buf+1,0,strlen(atStart));
			result = 'N';
			
			if(InitCode2Flash()==TRUE)
				result = 'Y';
			
			buf[0] = addr;
			buf[1] = result;			
			UartTxOpen(port,2);
			
			UserTimerReset(TIM2,&timer);
			//??????100ms			
			while(!UserTimerOver(TIM2,&timer,USER_TIMER_10MS(5)));	
			
			//?????????
			bFWLoad = TRUE;
			UserTimerReset(TIM2,&rollTimer);			
		}
		else if(0==memcmp(&buf[1],atDown,strlen(atDown)))
		{
			UserTimerReset(TIM2,&rollTimer);
			ResetUartBuf(port);
			memset(buf+1,0,strlen(atDown));
			result = 'N';
			chk = 0;
			
			bufIdx = strlen(atDown)+2;
			tLen = buf[bufIdx-1];
						
			for(i = 0 ; i< tLen;i++)
			{
				chk ^= buf[i+bufIdx];
			}
			
			if(chk == buf[tLen+bufIdx] && Code2Flash(buf+bufIdx,tLen)== TRUE )
			{
				result = 'Y';
			}
			
			buf[0] = addr;
			buf[1] = result;
			UartTxOpen(port,2);				
		}
		else if(0==memcmp(&buf[1],atExec,strlen(atExec)))
		{
			UserTimerReset(TIM2,&rollTimer);
			ResetUartBuf(port);
			memset(buf+1,0,strlen(atExec));
			result = 'N';
			
			if(port == UART1)
			{	
				flag = 0xFF;				
				if(TRUE == WriteE2prom(EE_IAP_FALG1,&flag,sizeof(flag)))
				{
					EndCode2Flash();
					result = 'Y';
				}
			}
			else if(port == UART2)
			{	
				flag = 0xFF;				
				if(TRUE == WriteE2prom(EE_IAP_FALG2,&flag,sizeof(flag)))
				{
					EndCode2Flash();
					result = 'Y';
				}
			}	

			buf[0] = addr;
			buf[1] = result;									
			UartTxOpen(port,2);						
		}
		else if(0==memcmp(&buf[1],atVers,strlen(atVers)))	
		{
			ResetUartBuf(port);
			memset(buf+1,0,strlen(atVers));
				
			buf[0] = addr;
			sprintf((char*)&buf[1],"%04X",gFWCheck);									
			UartTxOpen(port,5);				
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
	U8 *buf,chk;		
	U16 len,tLen,i,j;		
	JC_LAYER1 *layer;

	GetUartBufInfo(port,(U8**)&buf,&len);
					
	if ( len > 5  )
	{
		//֡��ʽ: 00 55 AA 00 06 A0 FF 04 F0 0A 00 A7 
		//�ҵ�֡��ʼ
		for( i = 0 ; i < 5 ; i++)
			if( buf[i] == SYNC1 )
				break;
		//��������ʼ
		if( i >= 5 )
			return;
		//ȷ��֡��ʼ
		if( buf[i+1] != SYNC2 )				
			return;	

		layer = (JC_LAYER1*)&buf[i+2];
		
		//ȷ�Ͻ������һ֡����
		if( len > (i+5) && layer->totLen == (len-i-5) )
		{
			ResetUartBuf(port);
			//����У���
			for( j = i+2,chk = 0; j < len - 1; j++)
				chk ^= buf[j];	
				
			if( chk != buf[len-1] )
			{	
				layer->ack |= DEFSUM_ERR;
				
				//����У���
				for( j = 0,chk = 0; j < layer->totLen ; j++) 
				{
					chk ^= buf[i+j+2];
				}		
				
				buf[i+2+layer->totLen] = chk;
				//��������
				UartTxOpen(port,i+3+layer->totLen);				
				return;
			}			
			//ִ������
			execAnaylize(buf+i+2,len-i-3,&tLen);
			//����У���
			for( j = 0,chk = 0; j < tLen ; j++) 
			{
				chk ^= buf[i+j+2];
			}			
			buf[i+2+tLen] = chk;
			//��������
			if(i == 0)
			{
				for(j= i+4+tLen ;j > 0;j--)
				{
					buf[j] = buf[j-1];
				}
				buf[0] = 0;
				buf[1] = SYNC1;
				//��������
				UartTxOpen(port,i+4+tLen);
			}
			else
			{
				//��������
				UartTxOpen(port,i+3+tLen);
			}
		}			
	}		
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
BOOL ProcSetLoadStart(U8*buf,U16 len)
{
	loadFwCnt = 0;
	return InitCode2Flash();
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
BOOL ProcSetLoading(U8*buf,U16 len)
{
	BOOL result;

	result = Code2Flash(buf,len);

	if ( result == TRUE )
	{
		loadFwCnt += len;
		buf[0] = (U8)loadFwCnt;
		buf[1] = loadFwCnt/256;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 0;
	}

	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
BOOL ProcSetLoadingPrev(U8*buf,U16 len)
{
	BOOL result;

	if ( (len + loadFwCnt) > SIZEOF_BUFFER )
	{
		len = SIZEOF_BUFFER - loadFwCnt;
	}

	if ( len == 0 )return FALSE;

	result = Code2Flash(buf,len);

	if ( result == TRUE )
	{
		loadFwCnt += len;
		buf[0] = (U8)loadFwCnt;
		buf[1] = loadFwCnt/256;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 0;
	}

	return result;
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
BOOL ProcSetLoadEndPrev(U8*buf,U16 len)
{
	U16 crc16;

	crc16 = *(U16*)buf;

	//hardware version is consistency
	if ( HARDWARE_VERSION != HARDWARE_VERSION_BAK )return FALSE;
	//UpgradeAll version is consistency
	if ( (SOFTWARE_VERSION&0xF0) != (SOFTWARE_VERSION_BAK&0xF0) )return FALSE;

	if ( modelFwCrc == crc16 ) return TRUE;

	if( crc16 != CalcFirmwareBufCRC16())return FALSE;

	return EndCode2FlashRrev();
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v1.0
  * @date	:2012.4.20
  */
BOOL ProcSetLoadEnd(U8*buf,U16 len)
{
	U16 crc16;

	crc16 = *(U16*)buf;

	//hardware version is consistency
	if ( HARDWARE_VERSION != HARDWARE_VERSION_BAK )return FALSE;

	if ( modelFwCrc == crc16 ) return TRUE;

	if( crc16 != CalcFirmwareBufCRC16())return FALSE;

	return EndCode2Flash();
}
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
	GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);
	ReadE2prom(EE_ADDR,(U8*)&gAddr,sizeof(gAddr));
	
	InitUartBuf(UART1);
	InitUartBuf(UART2);

	sendInitpacket(UART1);
	sendInitpacket(UART2);
	
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
	TaskIAP(UART1);
	TaskIAP(UART2);
	return 0;
}
/********************************END OF FILE***********************************/