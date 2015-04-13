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
/*软件下载*/
static U16  nextSoftPktSn;				//请求软件下载包的序号
static U16  preSoftChecksum = 0xFFFF;	//前次下载软件的校验和
static U16  __softChecksum;				//整个软件的checksum值
static U16  softLen = 0;				//软件总长度
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

//	/*在应用编程完成以后，MCU重启后向主机发送确认消息*/
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
*                         工厂参数设置命令
*
* Description: 标号分配为0x0ADA，对各个工厂参数进行设置，查询在另一命令中
*
* Arguments  : 无
*
* Returns    : 无
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

	//参数对应的EEPROM地址
	param_addr 	= *(U16*)fRcvID;
	tempLen 	= *fRcvDataLen;
	tPALim 		= gPALim;

	//工厂参数查询
	if(*fRcvGet==0)
	{
		//一般参数在E2PROM中读取
		readE2promStr(tempLen,param_addr,fRcvData);
		//个别参数则需要实时查询
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
				//IRcvStr(0x90,0x03,temp,2);         //0x90 由1 ，0 ，0 ，1 ,  A2  ，A1， A0，R/W 组成，高四位固定，低四位是地址位、读写位
				*(fRcvData+0) = temp[0];             //0x03表示要读取的IIC器件内部寄存器地址
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

			case 0x00C5:								    //2字节，程序校验和
				*(fRcvData+0) = Lim_state;
				break;
			case 0x00C6:	  							    //2字节，程序版本号
				*(fRcvData+0) = (U8)PA_current;
				*(fRcvData+1) = (U8)(PA_current >> 8);
				break;
			case 0x00C7:								    //2字节，程序校验和
				*(fRcvData+0) = (U8)OutputPwr;
				*(fRcvData+1) = (U8)(OutputPwr >> 8);
				break;
			case 0x00C8:	  							    //2字节，程序版本号
				*(fRcvData+0) = (U8)re_Pwr;
				*(fRcvData+1) = (U8)(re_Pwr >> 8);
				break;
				*/
			case EE_CURRENT_TEMP:
				*(fRcvData+0) = (U8)gcurRfTemp;
				*(fRcvData+1) = (U8)(gcurRfTemp >> 8);
				break;
			case EE_VERSION:	  							//2字节，程序版本号
				*(fRcvData+0) = Ver_Low;
				*(fRcvData+1) = Ver_High;
				break;
			case EE_ALL_CHECKSUM:							//2字节，程序校验和
				*(fRcvData+0) = (U8)all_checksum;
				*(fRcvData+1) = (U8)(all_checksum >> 8);
				break;
  			default:
			    break;
		}
	}
	//工厂参数设定
	else
	{
		if (tempLen==2)
		{
			tmpInt = *(U16*)fRcvData;
		}

		//主要用于对接收的数据进行检验，是否为正常值的范围内，若不是，则不存入EEPROM
		switch(param_addr)
		{
			//2字节，功放限幅
			case EE_DA_CHANNEL_B:
			case EE_DA_CHANNEL_C:
				if ( tmpInt > 1023 )
					*fReqFlag |= DATRANGE_ERR;
				break;
			//2字节,功放偏压值
			case EE_gPALim:
				if( ((S16)tmpInt<0)||(tmpInt>1023) )
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1字节，增益温补系数分子
			case EE_GainNumer:
			//1字节，增益温补系数分母
			case EE_GainDenomi:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>255))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1字节，功放重起温度下限
			case EE_PAResetLim:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>125))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1字节，调试状态
			case EE_TEST_MARK:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>2))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			//1字节，温补基准值
			case EE_TEMP_VALUE:
				if(((S8)*(fRcvData+0)<0)||(*(fRcvData+0)>200))
				{
					*fReqFlag |= DATRANGE_ERR;
				}
				break;
			default:
				break;
		}

		//接收的数据为正常值，则开始将数据存EEPROM
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

			rfPramModified = TRUE;//射频参数修改标志有效

			switch(param_addr)
			{
				case EE_TEMPER_BUCHANG:
					memcpy((U8*)__temp_que,fRcvData,sizeof(__temp_que));
				break;
				//触发单次脉冲信号
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
				//信源选择
				case EE_SOURCE_SELECT:
					//RFSrcSelect = (*(fRcvData+0) != SRC_INTERNAL && *(fRcvData+0) != SRC_EXTERNAL)?SRC_INTERNAL:*(fRcvData+0);
					writePLL();
				break;
				//衰减器设置
				case EE_Att1:
				   gGain=*(fRcvData+0);
				   writeAtt1(gGain);
				   updateAlmInfo();
				//2字节，输出功率限幅值
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
				//4字节，中心频率
				case EE_CenFreq:
				//6字节，中心频率+限幅值
				case EE_FREQ_ADD_POWEWR:
					//swapBytes(&*(fRcvData+0),(U8 *)&gCenFreq,4);
					writePLL();
					delay(1);		//改变中心频率需要同时进行限幅补偿
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
				//2字节，频率步进值
				case EE_FreqStep:
				//4字节，参考频率
				case EE_RefFreq:
					writePLL();
					break;
				//功率限幅补偿分子
				case 0x0075:
				//功率补偿分母
				case 0x0077:
				//基准频率
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
				//2字节，功放保护温度下限
				case EE_PAProtectLim:
					gPAProtecttLim = *(fRcvData+0);
					writeLM75(0x03,gPAProtecttLim,3);
					break;
				//2字节，功放重起温度下限
				case EE_PAResetLim:
					gPAResetLim = *(fRcvData+0);
					writeLM75(0x02,gPAResetLim,3);
					break;
				//1字节，射频模块编号
				case EE_RF_No:
					gRF_No = *(fRcvData+0);
					break;
				//1字节，射频开关
				case EE_RFSW:
					if(AutoSwitch != *(fRcvData+0));
					{
						AutoSwitch = *(fRcvData+0);
						updateAlmInfo();
						writeAtt1(gGain);
						writePLL();//射频开关动作了，每次要从新操作锁相
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
*                         RF模块射频参数查询
*
* Description: 标号分配为0x0AF0，对各个工厂参数进行一次性查询
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void execRFParamQ(U8 *buf,U16 rxLen,U16*txLen)
{
	U8 i;
	U16 CKS = 0;
	static U16 tmpCurr=0;

	//tmpCurr = (tmpCurr + PA_current + PA_current)/3;					 //电流值与前值平均

	if(buf[7]==0)										
	{//定时数据查询
		buf[1] = 18; //数据包长度									
		buf[4] = 16; //监控对象的长度									
		buf[8] = Lim_state;
		buf[9] = gRFSW;
		buf[10] = gRFSW;
		buf[11] = (U8)tmpCurr; 		//低位在前	
		buf[12] = (U8)(tmpCurr>>8);	//高位在后	
		buf[13] = (U8)OutputPwr;
		buf[14] = (U8)(OutputPwr>>8);
  		buf[15] = (U8)re_Pwr;
		buf[16] = (U8)(re_Pwr>>8);
		buf[17] = (S8)gcurRfTemp;
		buf[18] = (S8)(gcurRfTemp>>8);
		buf[19] = PLL_state;			//080220 增加	
		
		*txLen = 20;
	}
	else if(buf[7]==1)	  							
	{//工厂参数查询数据包
		buf[1] = 94;					//数据包长度				
		buf[4] = 92;					//监控对象的长度				
		
		readE2promStr(5,EE_AtteVal,&buf[10]);			//增益参数
		readE2promStr(4,EE_Att1slopeval,&buf[15]);
		readE2promStr(11,EE_gPALim,&buf[19]);			//功放参数
		readE2promStr(3,EE_gPALimNumer,&buf[30]);
		readE2promStr(20,EE_BandWidth,&buf[37]);		//频率参数
		readE2promStr(4,EE_gPBmFreq,&buf[57]);
		readE2promStr(14,EE_CurCo,&buf[61]);			//电流参数
		readE2promStr(4,EE_GainNumer,&buf[75]);			//温度参数
		readE2promStr(3,EE_RF_No,&buf[79]);				//模块参数
				
		memcpy((U8*)&gCenFreq,buf+33,sizeof(gCenFreq));
		
		buf[80] = gRFSW;
		buf[81] = gRFSW;
		buf[82] = Ver_Low;
		buf[83] = Ver_High;
		buf[84] = (U8)all_checksum;
		buf[85] = (U8)(all_checksum >> 8);
		
		readE2promStr(10,EE_MODULE_No,&buf[86]);		//模块参数	
		
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
*                         RF模块软件下载						
*
* Description: 标号分配为0x0AFE，对射频模块的程序进行在系统升级
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
U8 execDwnldSoft(U8 *buf)
{
	U8 *bufPtr;
	U16 dataLen;
	U16 curPktIdx;

	bufPtr = buf;

	bufPtr += 7;//取到数据的开头，包序号
	curPktIdx 	= *(U16*)bufPtr;//取到包序号置于curSoftPktSn中
	bufPtr += 2;//指针指向数据长度
	dataLen 	= *(U16*)bufPtr;//取到数据长度	
	bufPtr += 2;//指向数据部分										
	
	if( curPktIdx == 0 )									//第一个包
	{
		if( dataLen != 0x1C )								//取得数据长度并指向起始地址
		{																					 
			return OTHER_ERR;								//其他错误
		}
		
		bufPtr += 0x18;										//指针指向文件CHECKSUM字节		
		__softChecksum = *(U16*)bufPtr;
		InitCode2Flash();
	}
	else			  										
	{
		Code2Flash(bufPtr,dataLen);		
	}	
	
	return NO_ERR;											//其他错误
}
/*
*********************************************************************************************************
*                         软件下载成功或查询模块信息
*
* Description: 标号分配为0x0AFD，软件下载完成要将程序从EEPROM中读出到flash中，并程序转移到0x0000执行
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
U8 endDwnldSoft()
{	
	EndCode2Flash();
	gDownFlag = 0xFF;	
	writeE2promStr(1,EE_DOWN_FLAG,&gDownFlag);			//下载时添加下载标志的设置 20090415 by zxl
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
		//查询和设置工厂参数
		case ID_FCT_PARAM_WR:										
		{
			execFctParamSet(buf,rxLen,tlen);
			break;
	    }
		//RF模块采样查询
		case ID_FCT_PARAM_SAMPLE:										
		{
			execRFParamQ(buf,rxLen,tlen);
			break;
		}
		//软件下载确认或查询模块信息
		case ID_FIRMWARE_UPGRAD_ENTER:										
		{
			if(*fRcvGet==0x01)
			{
				if(*fRcvUpgrade==0)							
				{//下载取消
					*tlen = 10;
					//softDownLoad = UNDO;
				}
				else									
				{//下载确认
					//if(softDownLoad == DOING)
					{//错误代码标志
						downresult = endDwnldSoft();
			 			*fRcvAck |= downresult;				
					}
					//else
					{//错误代码标志
						*fRcvAck |= OTHER_ERR;				
					}
					
					*tlen = 10;
				}
			}
			break;
		}
		//软件下载
		case ID_FIRMWARE_UPGRAD_LOAD:										
		{
		  //DwnldSoft1m_time = getTime();
			//resultCode = execDwnldSoft();
			
			buf[1] = 9;//数据包长度								
			buf[4] = 7;
 			buf[6] |= resultCode;	//错误代码标志					
			buf[9] = (U8)nextSoftPktSn;
			buf[10] = (U8)(nextSoftPktSn>>8);
			
			*tlen = 11;
			
			break;
		}
		//软件复位 
		case ID_FIRMWARE_UPGRAD_REBOOT:
		{
			if(*fRcvGet == 0x01)
			{
				//AUXR1 = 8;                                
			}
			break;
		}
		//命令编号错,监控对象标号无法识别
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
		//帧格式: 00 55 AA 00 06 A0 FF 04 F0 0A 00 A7 
		//找到帧起始
		for( i = 0 ; i < 10 ; i++)
			if( buf[i] == SYNC1 )
				break;
		//不存在起始
		if( i > 10 )
			return;
		//确认帧起始
		if( buf[i+1] != SYNC2 )		
			return;		
		//确认接收完成一帧数据
		if( len > (i+5) && buf[i+3] <= (len-i-5) )
		{
			//计数校验和
			for( j = i+2,xor = 0; j < len - 1; j++)
				xor ^= buf[j];	
				
			if( xor != buf[len-1] )
			{
				ResetUartBuf(port);
				return;
			}			
			//数据单元中的应答标志
			buf[i+5] = 0;
			//执行命令
			execute_cmd(buf+i+2,len-i-3,&tLen);
			//计数校验和
			for( j = 0,xor = 0; j < tLen ; j++) 
			{
				xor ^= buf[i+j+2];
			}			
			buf[i+2+tLen] = xor;
			//发送数据
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
