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
  * @brief  :半字节查表法之CRC16
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

	/*在应用编程完成以后，MCU重启后向主机发送确认消息*/
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
*                         RF模块射频参数查询
*
* Description: 标号分配为0x0AF0，对各个工厂参数进行一次性查询
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void sendInitpacket(U8 port)
{
	U8 	xor,i,
		*buf,
		*bufPtr;	
	
	if(gDownFlag == 0xFF)
	{
		gDownFlag = 0x00;	

		WriteE2prom(EE_DOWN_FLAG,(U8*)&gDownFlag,sizeof(gDownFlag));	
			
		GetUartBufInfo(port,(U8**)&buf,NULL);
		
		bufPtr = buf;
		
		*(bufPtr++) = 0x00;
		*(bufPtr++) = 0x55;
		*(bufPtr++) = 0xAA;		
				
		*(bufPtr++) = 0x00;	//主控板地址
		*(bufPtr++) = 0x08;	//长度
		*(bufPtr++) = 0xA0; //命令标识
		*(bufPtr++) = 0x00;	//应答标志
		*(bufPtr++) = 0x06;	//监控对象长度
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
*                         RF模块射频参数查询
*
* Description: 标号分配为0x0AF0，对各个工厂参数进行一次性查询
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
BOOL execRFParamQ(U8 flag,U8 *buf,U16 rxLen,U16*txLen)
{
	U8 i;
	U16 cks = 0;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;

	layer->ack = NO_ERR;
	
	if(layer->get==0)										
	{//定时数据查询
		layer->totLen = 18;								
		layer->objLen = 16;		
		
		*(U8 *)(buf+8 ) = !gLimState;
		*(U8 *)(buf+9 ) = gRFSW;
		*(U8 *)(buf+10) = gRFSW;
		*(U16*)(buf+11) = gPACurrent;
		*(U16*)(buf+13) = gPwrOut;
		*(U16*)(buf+15) = gPwrRef;
		*(U16*)(buf+17) = gCurRfTemp;
		*(U8 *)(buf+19) = !gPLLLock;
		
		*txLen = 20;
	}
	else if(layer->get==1)	  							
	{//工厂参数查询数据包
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
		
		*txLen = 96;
	}
	else
	{
		layer->ack |= OC_ERR;
	}
	
	return TRUE;
}
/*
*********************************************************************************************************
*                         数据包解码
*
* Description: 对readXPktBuf中的数据进行解码，包括二字拆分，区分命令和数据，取得CRC值
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
//软件下载确认或查询模块信息
BOOL execFWEnter(U8 flag,U8*buf,U16 rxLen,U16 *txLen)
{
	BOOL result = TRUE;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;
	
	result = result;
	layer->ack = NO_ERR;
	
	if(layer->get == 1)
	{
		if(layer->upgrade1 == 0)
		{//下载取消
			//
		}
		else
		{//下载确认	
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
	
	bufPtr = buf;

	curPktIdx 	= *(U16*)(bufPtr+7);//取到包序号置于curSoftPktSn中
	dataLen 	= *(U16*)(bufPtr+9);//取到数据长度	
	bufPtr += 18;					//指向数据部分										
	
	if( curPktIdx == 0 )			//第一个包
	{
		if( dataLen != 0x1C )		//取得数据长度并指向起始地址
		{																					 
			layer->ack |= LEN_ERR;	//其他错误
			return FALSE;
		}
		
		bufPtr += 0x18;				//指针指向文件CHECKSUM字节		
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
//软件复位 
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
	U16 cmd;
	S32 temp;
	BOOL recvflag = FALSE;
	JC_LAYER1 *layer = (JC_LAYER1*)buf;
	JC_COMMAND *sc = GetTable();
	
	for(i=0;i<GetTableMebCnt();i++)
	{
		if(sc[i].cmd == layer->mode)
		{
			recvflag = TRUE;
			cmd = layer->id1 + (layer->id2<<8);
			
			if(cmd == sc[i].sub)
			{
				if(layer->get == 0)
				{//查询 
					memcpy(layer->idBuf,sc[i].var,layer->idLen);
				}
				else if( layer->get == 1 )
				{//设置
					if(sc[i].max != 0 && sc[i].min != 0)
					{
						//
					}					
					
					memcpy(sc[i].var,layer->idBuf,layer->idLen);					
					WriteE2prom(sc[i].sub,sc[i].var,sc[i].varLen);

					gRFModify = TRUE;					
				}

				*tlen = layer->totLen + 2;		
			}	

			if(sc[i].proc != NULL)sc[i].proc(layer->get,buf,rxLen,tlen);
		}				
	}	
	
	//命令编号错,监控对象标号无法识别
	if(recvflag == FALSE)layer->ack |= OCID_ERR;	
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
	
	JC_LAYER1 *layer;

	GetUartBufInfo(port,(U8**)&buf,&len);

	if ( len > 5  )
	{
		//帧格式: 00 55 AA 00 06 A0 FF 04 F0 0A 00 A7 
		//找到帧起始
		for( i = 0 ; i < 5 ; i++)
			if( buf[i] == SYNC1 )
				break;
		//不存在起始
		if( i > 5 )
			return;
		//确认帧起始
		if( buf[i+1] != SYNC2 )				
			return;	

		layer = (JC_LAYER1*)&buf[i+2];
		
		//确认接收完成一帧数据
		if( len > (i+5) && layer->totLen == (len-i-5) )
		{
			ResetUartBuf(port);
			//计数校验和
			for( j = i+2,xor = 0; j < len - 1; j++)
				xor ^= buf[j];	
				
			if( xor != buf[len-1] )
			{	
				layer->ack |= DEFSUM_ERR;
				
				//计数校验和
				for( j = 0,xor = 0; j < layer->totLen ; j++) 
				{
					xor ^= buf[i+j+2];
				}		
				
				buf[i+2+layer->totLen] = xor;
				//发送数据
				UartTxOpen(port,i+3+layer->totLen);				
				return;
			}			
			//执行命令
			execAnaylize(buf+i+2,len-i-3,&tLen);
			//计数校验和
			for( j = 0,xor = 0; j < tLen ; j++) 
			{
				xor ^= buf[i+j+2];
			}			
			buf[i+2+tLen] = xor;
			//发送数据
			UartTxOpen(port,i+3+tLen);
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
	//TMAPktHandle(UART2);
	return 0;
}
/********************************END OF FILE***********************************/
