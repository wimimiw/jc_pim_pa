
/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: packet.c
**创   建   人: 程莺红 
**版        本: V1.0
**日　      期: 2006年9月30日
**最 新 版  本: V1.6
**描        述: 		对包的解释，并处理,并操作射频开关，PLL等
**
********************************************************************************************************/

#include <reg938.h>
#include <stdio.h>
#include <string.h>	  
#include <I2C.h>
#include <SPI.h>
#include <cpu.h>
#include <serial.h>
#include <task.h>
#include <packet.h>
#include <bro20W_RFparam.h>
#include <bro20W_RFcontrol.h>
#include <e2prom.h>
#include <readad.h>
#include <timer.h>
//#include "flash_config.h"
//#include "compiler.h"
//#include "flash_api.h"

/*软件下载*/
static unsigned int xdata nextSoftPktSn;				//请求软件下载包的序号
static unsigned int xdata preSoftChecksum = 0xFFFF;		//前次下载软件的校验和
static unsigned int xdata softChecksum;					//整个软件的checksum值
static unsigned int xdata softLen = 0;					//软件总长度
//static unsigned char idata pktDataBuf[32];
unsigned char code factry[8]={0xF0,0xD8,0x80,0xA8,0x5F,0xE0,0x87,0xC9}; 
#if debug == 1
	unsigned char code debugstr[17]={0x01,0x0F,0xA0 ,0xFF ,0x0D ,0xDA ,0x0A ,0x01 ,0x00 ,0x00 ,0x40 ,0x00 ,0x04 ,0xE8 ,0xAF ,0x0D ,0x00};
#endif 

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
void execute_cmd() 
{
	unsigned char resultCode = NO_ERR;		
	unsigned char downresult = NO_ERR;
	switch(RxBuf[5])
	{
		case 0xDA:										//查询和设置工厂参数
		{
			execFctParamSet();	
			break;
	    }
		case 0xF0:										//RF模 块射频（工厂）参数查询 	
		{
			execRFParamQ();
			break;
		}
		case 0xFD:										//软件下载确认或查询模块信息	
		{
			if(RxBuf[7]==0x01)
			{
				if(RxBuf[8]==0)							//下载取消
				{
					Data_len = 10;
					softDownLoad = UNDO;
				}
				else									//下载确认
				{
					if(softDownLoad == DOING)
					{
						downresult = endDwnldSoft();
			 			RxBuf[6] |= downresult;				//错误代码标志
					}
					else
					{
						RxBuf[6] |= OTHER_ERR;				//错误代码标志
					}
					Data_len = 10;
				}
			}
			break;
		}
		case 0xFE:										//软件下载			
		{
		    DwnldSoft1m_time = getTime();
			resultCode = execDwnldSoft();
			RxBuf[1] = 9;								//数据包长度	 
			RxBuf[4] = 7;
 			RxBuf[6] |= resultCode;						//错误代码标志
			RxBuf[9] = (unsigned char)nextSoftPktSn;
			RxBuf[10] = (unsigned char)(nextSoftPktSn>>8);
			Data_len = 11;
			break;
		}
		case 0xFB:
		{
			if(RxBuf[7] == 0x01)
			{
				AUXR1 = 8;                                //软件复位   20080618
			}
			break;
		}
	
		default:										//命令编号错
		{
			RxBuf[6] |= OCID_ERR;						//监控对象标号无法识别	
			break;
		}
	}	
}

/*
*********************************************************************************************************
*                         输出功率采样						
*
* Description: 
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void OutputPwrsampling()
{
	unsigned char temp[4];
	unsigned char gPReadADNumerN;
	unsigned char i;
	int calVal;

	OutputPwr = 0;

	for(i=0;i<8;i++)
	{
		//OutputPwr += readAD(CHANNEL_0);
		OutputPwr += readAD(CHANNEL_4);
	}
	
	OutputPwr >>= 3;

	readE2promStr(1,EE_gPReadADNumer,temp);			//读取 功率读取补偿分子					
	swapBytes(temp,(unsigned char *)&gPReadADNumer,1);

	readE2promStr(1,EE_gPequDenomi,temp);			//读取 功率补偿分母					
	swapBytes(temp,(unsigned char *)&gPequDenomi,1);

	//readE2promStr(4,EE_CenFreq,temp);				//读取 中心频率
	//swapBytes(temp,(unsigned char *)&gCenFreq,4);

    readE2promStr(4,EE_gPBmFreq,temp);				//读取 基准频率
	swapBytes(temp,(unsigned char *)&gPBmFreq,4);

	gPReadADNumerN = ~gPReadADNumer + 1;

	calVal = OutputPwr;
	if(gCenFreq < gPBmFreq)
	{
		if(gPReadADNumer < 0)
		{
			calVal += (gPBmFreq - gCenFreq) * gPReadADNumerN / (gPequDenomi * 1000);
		}
		else
		{
			calVal -= (gPBmFreq - gCenFreq) * gPReadADNumer / (gPequDenomi * 1000);
		}
	}								  
	else if(gPBmFreq < gCenFreq)
	{
		if(gPReadADNumer < 0)
		{
			calVal -= (gCenFreq - gPBmFreq) * gPReadADNumerN / (gPequDenomi * 1000);
		}
		else
		{
			calVal += (gCenFreq - gPBmFreq) * gPReadADNumer / (gPequDenomi * 1000);
		}
	}

	//判断范围，防止溢出
	if(calVal < 0)
	{
		OutputPwr = 0;
	}
	else if(calVal > 1023)
	{
		OutputPwr = 1023;
	}
	else
	{
		OutputPwr = calVal;
	}

}
/*
*********************************************************************************************************
*                         定时查询告警信息						
*
* Description: 
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void updateAlmInfo() small
{
	unsigned char i;
	static unsigned char preRFSW;
//	static unsigned char gRFSWtemp = 0;
//	unsigned char tempNum;

	//preRFSW = gRFSW;												//放在操作射频开关之后

	PA_current = 0;
	re_Pwr =0;
	OutputPwr = 0;	

	if(gRFSW == CLOSE)
	{
		Lim_state = 0;								//射频开关关闭时限幅状态为正常
	}
	else
	{ 
		Lim_state = (LimYes == 0 ? FALSE : TRUE );	//检测限幅状态
	}
    
	PLL_state = (LockYes == 1 ? FALSE : TRUE);		//检测失锁状态
	 
	watchdog();
			
	if(AutoSwitch == CLOSE)								//频率没锁定或者手动强制关闭则关闭功放
	{
		gRFSW = CLOSE;
		TempOverAlm = 0;
	}
	else
	{
		if(gcurRfTemp > 2*gPAProtecttLim)
		{
			gRFSW = CLOSE;
			TempOverAlm = 1;
		}
		else 
		if(gcurRfTemp < 2*gPAResetLim)
		{
			gRFSW = OPEN;
			TempOverAlm = 0;             
		}
	}
	
  switchRF(gRFSW);
    
	if(preRFSW != gRFSW)						   	//开关变化则重写锁相环
	{
		writePLL();
	    if(gRFSW==OPEN)
		 {
		   delay(2);	 
		 }
	}
	preRFSW = gRFSW;

	/*for(i=0;i<20;i++)                              //先读100次并且丢弃，加快采样速度
	{
		readAD(CHANNEL_0);
		readAD(CHANNEL_1);							//读反射功率AD值
		readAD(CHANNEL_5);
	} */

	for(i=0;i<8;i++)
	{
// 		re_Pwr += readAD(CHANNEL_1);				//读反射功率AD值
// 		PA_current += readAD(CHANNEL_5);			//读出功放电流AD值
		re_Pwr += readAD(CHANNEL_5);				//读反射功率AD值
		PA_current += readAD(CHANNEL_1);			//读出功放电流AD值
	}

	re_Pwr >>= 3;
	PA_current >>= 3;
	OutputPwrsampling();							//正向功率的读取
  AD0CON = 0x01;              //关闭转换，以防止出现管脚相互影响现象。add by dw   20090623
    
#if debug == 1
	memcpy(RxBuf,debugstr,17);
	pack_valid = 1;
#endif

	watchdog();		   			
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
void execFctParamSet()
{

	unsigned char tempLen;//,bufPoint;
	unsigned short param_addr;//,j;//,j;							        //参数对应的EEPROM地址
	unsigned short tmpInt=0;//j;
//	unsigned long tmpLong=0;
	unsigned char temp[4];
	unsigned short tPALim;
	
	param_addr = RxBuf[10];
	param_addr += (RxBuf[11]<<8);						       //参数对应的EEPROM地址

	tempLen = RxBuf[12];
	
	tPALim = gPALim;

	if(RxBuf[7]==0)										//工厂参数查询
	{	
		readE2promStr(tempLen,param_addr,&RxBuf[13]);            //一般参数在E2PROM中读取
		  	
		switch(param_addr)                                          //个别参数则需要实时查询
		{
			case 0x0040:
				if ( RFSrcSelect )
				{
					RxBuf[13] = 0,RxBuf[14] = 0,RxBuf[15] = 0,RxBuf[16] = 0;
				}
				break;
			case EE_DA_CHANNEL_B:								
				RxBuf[13] = gDAoutB*gRFSW;
				RxBuf[14] = (gDAoutB*gRFSW)>>8;
				break;
			case EE_DA_CHANNEL_C:
				RxBuf[13] = gDAoutC*gRFSW;
				RxBuf[14] = (gDAoutC*gRFSW)>>8;
				break;
			case 0x00A4:
				IRcvStr(0x90,0x03,temp,2);         //0x90 由1 ，0 ，0 ，1 ,  A2  ，A1， A0，R/W 组成，高四位固定，低四位是地址位、读写位
				RxBuf[13] = temp[0];                  //0x03表示要读取的IIC器件内部寄存器地址
				break;
			case 0x00A5:
				IRcvStr(0x90,0x02,temp,2);
				RxBuf[13] = temp[0];
				break;        

			case 0x00C1:
			case 0x00C2:	  							    
				RxBuf[13] = gRFSW;
				break;
/*				case 0x00C4:
				RxBuf[13] = PLL_state;
				break;

			case 0x00C5:								    //2字节，程序校验和
				RxBuf[13] = Lim_state;
				break;
			case 0x00C6:	  							    //2字节，程序版本号
				RxBuf[13] = (unsigned char)PA_current;
				RxBuf[14] = (unsigned char)(PA_current >> 8);
				break;
			case 0x00C7:								    //2字节，程序校验和
				RxBuf[13] = (unsigned char)OutputPwr;
				RxBuf[14] = (unsigned char)(OutputPwr >> 8);
				break;
			case 0x00C8:	  							    //2字节，程序版本号
				RxBuf[13] = (unsigned char)re_Pwr;
				RxBuf[14] = (unsigned char)(re_Pwr >> 8);
				break;*/
			case 0x00CA:	
										
				RxBuf[13] = (unsigned char)gcurRfTemp;
				RxBuf[14] = (unsigned char)(gcurRfTemp >> 8);
				break;

			case 0x00E0:	  							    //2字节，程序版本号
				RxBuf[13] = Ver_Low;
				RxBuf[14] = Ver_High;
				break;
			case 0x00E2:								    //2字节，程序校验和
				RxBuf[13] = (unsigned char)all_checksum;
				RxBuf[14] = (unsigned char)(all_checksum >> 8);
				break;
  			default:
			    break;						
		}
	}
	else                                                    //工厂参数设定
	{
		
		if (tempLen==2)
		{
			tmpInt = RxBuf[13];
			tmpInt += (RxBuf[14]<<8);		
		}
	
		//switch(param_addr):主要用于对接收的数据进行检验，是否为正常值的范围内，若不是，则不存入EEPROM
		switch(param_addr)
		{
			//2字节，功放限幅
			case EE_DA_CHANNEL_B:
			case EE_DA_CHANNEL_C:
				if ( tmpInt > 1023 )
					RxBuf[6] |= DATRANGE_ERR;	
				break;
			case 0x0020: 									//2字节,功放偏压值			
				if( (tmpInt<0)||(tmpInt>1023) )
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break; 									//1字节，增益温补系数分子
			case 0x00A2: 									//1字节，增益温补系数分母
			case 0x00A3:
				if((RxBuf[13]<0)||(RxBuf[13]>255))
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;
			case 0x00A5: 									//1字节，功放重起温度下限   
				if((RxBuf[13]<0)||(RxBuf[13]>125))
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;
			case 0x00D2:									//1字节，调试状态  
				if((RxBuf[13]<0)||(RxBuf[13]>2))	   	
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;	  
			case 0x00D3: 									//1字节，温补基准值	 
				if((RxBuf[13]<0)||(RxBuf[13]>200))
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;
			default:
				break;		
		}
		if(RxBuf[6]==0x0A)									//接收的数据为正常值，则开始将数据存EEPROM
		{
			if(param_addr != 0x0140)
			{
				if ( param_addr == 0x0040 && tempLen == 6 )                     //2012-6-19 HAN增
				{
					writeE2promStr(4,0x0040,&RxBuf[13]);
					writeE2promStr(2,0x0020,&RxBuf[17]);								
				}
				else                                                            //han,长、命令是40，长度是6，做上述事。
				{
					writeE2promStr(tempLen,param_addr,&RxBuf[13]);			
				}
			}
			rfPramModified = TRUE;							//射频参数修改标志有效	
			switch(param_addr)
			{
				case EE_TEMPER_BUCHANG:
					memcpy(__temp_que,RxBuf+13,sizeof(__temp_que));
				break;
				case EE_PLUS_TRIGER://触发单次脉冲信号
					EA = 0;
					__PlusSwitchState = OPEN;
					__PlusReqAddr			= RxBuf[13];
					__PlusReqPower    = RxBuf[14];
					__PlusReqFreq			= RxBuf[15]+(RxBuf[16]<<8);
					EA = 1;
				break;
				case EE_DA_CHANNEL_B:
					gDAoutB = tmpInt;					
					writeAD5314(gDAoutB,'B');
				break;
				case EE_DA_CHANNEL_C:
					gDAoutC = tmpInt;
					writeAD5314(gDAoutC,'C');					
				break;
				case EE_SOURCE_SELECT:
					RFSrcSelect = (RxBuf[13] != SRC_INTERNAL && RxBuf[13] != SRC_EXTERNAL)?SRC_INTERNAL:RxBuf[13];					
					writePLL();
				break;
				//--------------------------增益-------------------------------
					//---------上行增益------------
				case 0x0003:
				   gGain=RxBuf[13];
				   writeAtt1(gGain);
				   updateAlmInfo();
				case 0x0020: 									//2字节，输出功率限幅值									
					//2012.10.26
					gPALimCompensate();
					gPALim = (gPALim + tPALim)/2;					
					writeAD5314(gPALim,DAPOWER_LIM_CHAN);
					updateAlmInfo();					
					
					delay(1);
					
					gPALimCompensate();
					writeAD5314(gPALim,DAPOWER_LIM_CHAN);
					updateAlmInfo();
					//Task5S_time = getTime() + 100;
					break;				
				case 0x0040: 								//4字节，中心频率
				case 0x0140:
					//gCenFreq = tmpLong;
					swapBytes(&RxBuf[13],(unsigned char *)&gCenFreq,4);
					writePLL();
					delay(1);
					//gPALimCompensate();
                			//writeAD5314(gPALim,'B');			   //改变中心频率需要同时进行限幅补偿

				   	/*2012.6.19 HAN add*/
					//2012.10.26
					if ( tempLen == 6 )
					{
						gPALimCompensate();
						gPALim = (gPALim + tPALim)/2;					
						writeAD5314(gPALim,DAPOWER_LIM_CHAN);
						updateAlmInfo();					
						
						delay(1);
						
						gPALimCompensate();
						writeAD5314(gPALim,DAPOWER_LIM_CHAN);
						updateAlmInfo();						
					}  

					break;

				case 0x004A: 								//2字节，频率步进值
				case 0x0054: 								//4字节，参考频率
					writePLL();
					break;

				case 0x0075:                      //功率限幅补偿分子
//					case 0x0076:
				case 0x0077:                      //功率补偿分母
				case 0x0078:                     //基准频率
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

				case 0x00A4: 									//2字节，功放保护温度下限
					gPAProtecttLim = RxBuf[13];
					writeLM75(0x03,gPAProtecttLim,3);	
					break;

				case 0x00A5: 									//2字节，功放重起温度下限
					gPAResetLim = RxBuf[13];
					writeLM75(0x02,gPAResetLim,3);	
					break;

				case 0x00C0: 									//1字节，射频模块编号
					gRF_No = RxBuf[13];	
					break;

				case 0x00C2: 									//1字节，射频开关
					if(AutoSwitch != RxBuf[13]);
					{
					  AutoSwitch = RxBuf[13];

						updateAlmInfo();
						writeAtt1(gGain);
						writePLL();                      //射频开关动作了，每次要从新操作锁相												�
/*						if(AutoSwitch == CLOSE)
						{
						    gRFSW = CLOSE;
							switchRF(gRFSW);
						}
						else if(gcurRfTemp < 2*gPAProtecttLim)
						{*/
						    //gRFSW = OPEN;
							//switchRF(gRFSW);
							//Task5S_time = getTime();
//						}
					}
					watchdog();			
					break;	
				default:
					break;							  
			}
		}
	}
	Data_len = RxBuf[1] + 2;
}

void sendInitpacket()
{
	readE2promStr(1,EE_DOWN_FLAG,&gDownFlag);

	if(gDownFlag == 0xFF)
	{	
		gDownFlag = 0x00;
		writeE2promStr(1,EE_DOWN_FLAG,&gDownFlag);	   

		RxBuf[0] = 0x00;	//主控板地址
		RxBuf[1] = 8;	   	//长度
		RxBuf[2] = 0xA0; 	//命令标识
		RxBuf[3] = 0x00;	//应答标志
		RxBuf[4] = 6;		//监控对象长度
		RxBuf[5] = 0xFD;
		RxBuf[6] = 0x0A;
		RxBuf[7] = 0x01;
		RxBuf[8] = 0x01;
		RxBuf[9] = 0x01;

		Data_len = 10;

		sendPkt();
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
void execRFParamQ()
{
	unsigned char i;	
	unsigned int CKS = 0;
//	unsigned int selCurr;
	static unsigned int tmpCurr=0; 

	tmpCurr = (tmpCurr + PA_current + PA_current)/3;					 //电流值与前值平均

//	if (  PA_current > 400 )
//		selCurr = tmpCurr;
//	else
//		selCurr = PA_current;

	if(RxBuf[7]==0)										//定时数据查询
	{
		RxBuf[1] = 18; 									//数据包长度
		RxBuf[4] = 16;									//监控对象的长度
		
		RxBuf[8] = Lim_state;
		RxBuf[9] = gRFSW;
		RxBuf[10] = gRFSW;
		RxBuf[11] = (unsigned char)tmpCurr; 			//低位在前
		RxBuf[12] = (unsigned char)(tmpCurr>>8);		//高位在后
		RxBuf[13] = (unsigned char)OutputPwr;
		RxBuf[14] = (unsigned char)(OutputPwr>>8);
  		RxBuf[15] = (unsigned char)re_Pwr;
		RxBuf[16] = (unsigned char)(re_Pwr>>8);
		RxBuf[17] = (char)gcurRfTemp;
		RxBuf[18] = (char)(gcurRfTemp>>8);	
		RxBuf[19] = PLL_state;				   			//080220 增加
		Data_len = 20;
	}
	else if(RxBuf[7]==1)	  							//工厂参数查询数据包
	{		
		RxBuf[1] = 94;									//数据包长度						
		RxBuf[4] = 92;									//监控对象的长度
								
		readE2promStr(5,EE_AtteVal,&RxBuf[10]);			//增益参数		
		readE2promStr(4,EE_Att1slopeval,&RxBuf[15]);
		readE2promStr(11,EE_gPALim,&RxBuf[19]);			//功放参数
		readE2promStr(3,EE_gPALimNumer,&RxBuf[30]);		
		readE2promStr(20,EE_BandWidth,&RxBuf[37]);		//频率参数
		readE2promStr(4,EE_gPBmFreq,&RxBuf[57]);
		readE2promStr(14,EE_CurCo,&RxBuf[61]);			//电流参数		
		readE2promStr(4,EE_GainNumer,&RxBuf[75]);		//温度参数		
		readE2promStr(3,EE_RF_No,&RxBuf[79]);			//模块参数
		
		//test  palim
		//RxBuf[19] = (unsigned char)gPALim;
		//RxBuf[20] = (unsigned char)(gPALim >> 8);
		//test palim
		
		swapBytes((unsigned char *)&gCenFreq,&RxBuf[33],4);		
		//RxBuf[12] = 0;
		//RxBuf[13] = 0;
		RxBuf[80] = gRFSW;
		RxBuf[81] = gRFSW;
		RxBuf[82] = Ver_Low;
		RxBuf[83] = Ver_High;
		RxBuf[84] = (unsigned char)all_checksum;
		RxBuf[85] =	(unsigned char)(all_checksum >> 8);
		readE2promStr(10,EE_MODULE_No,&RxBuf[86]);			//模块参数		
	   	watchdog();
		for(i=0;i<86;i++)
		{
			CKS += RxBuf[10+i];
		}	
		watchdog();	
		RxBuf[8] = (unsigned char)CKS;
		RxBuf[9] = (unsigned char)(CKS>>8);
		
		Data_len = 96; 
	}
	else
	{
		ack_flag = OC_ERR;	 						
	}		
}
/*
*********************************************************************************************************
*                         数据包编码						
*
* Description: 对包数据进行编码,ack包置于readXPktBuf中
*			   本函数必须为可重入的
*
* Arguments  : X_PACKET指针，指向需要处理的包指针，未进行CRC校验
*
* Returns    : 无
*********************************************************************************************************
*/
void sendPkt()
{
	unsigned char *tmpPtr;
	unsigned char calPktChecksum;
	unsigned char x;
	tmpPtr = TxBuf;	
	*tmpPtr++= 0x00;									//先发主机地址，固定为0x00						   				
	*tmpPtr++= 0x55; 									//包起始标志
	*tmpPtr++= 0xAA;
	memcpy(tmpPtr,RxBuf,Data_len);						//整个包的数据长度，包括起始标志
   	calDefcksum(tmpPtr,Data_len,&calPktChecksum); 		//计算异或校验和
	tmpPtr  += Data_len;
	*tmpPtr++= calPktChecksum;
//	*tmpPtr++= 0x00;	                                //无用字节
	TB8 = 0x00;											//第一个字节为地址字节，TB8必须为1
	Tx_ptr = 0;
//	RDE = 1;             //用在485通信使能，紫光这里没有使用RDE
	for(x=0;x<0x80;x++)
	{
	}
	SBUF = 	TxBuf[Tx_ptr];
	pack_valid = 0;				

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
unsigned char endDwnldSoft()
{	
	unsigned int checksum = 0;
	unsigned int count;
	unsigned int i,j;
	unsigned char leaveCnt;
	unsigned char err;
	unsigned int tmpInt;
	unsigned int y;	 //x,
	
	count = 0x3700 - softLen;
	leaveCnt =count%32;								//不足32字节的数目
	count = count >> 5; 
	
	if( leaveCnt != 0)									//不满32字节
	{
		XmtDat[1] =  (unsigned char)softLen ;
		XmtDat[0] =  (unsigned char)(softLen>>8) ;
		for(i=0;i<leaveCnt;i++)
		{
	   		XmtDat[2+i] = 0xFF;
		}
		watchdog();
		err = ISendStrExt(0xA0,XmtDat,leaveCnt+2);
	   	if( err == FALSE )
		{
			return OTHER_ERR;							//E2PROM错
		}
	}	 
	for(j=0;j<count;j++)
	{
		err = 0;		
/*		for(x=0;x<0x05;x++)
		{
			for(y=0;y<0xFFFF;y++);
		}*/
		for(y=0;y<20000;y++);	 		 
		tmpInt = softLen + leaveCnt + j * 32;
	   	XmtDat[1] = (unsigned char)tmpInt;
		XmtDat[0] = (unsigned char)(tmpInt>>8);

		for(i=0;i<32;i++)
		{
	   		XmtDat[2+i] = 0xFF;
		}
		watchdog();
		err = ISendStrExt(0xA0,XmtDat,34);			
		if( err == FALSE )
		{
			return OTHER_ERR;							//E2PROM错
		}		
	}  
	gDownFlag = 0xFF;
	writeE2promStr(1,EE_DOWN_FLAG,&gDownFlag);			//下载时添加下载标志的设置 20090415 by zxl

	WP = 1;												//打开写保护，芯片只可读
	EA = 0;

	FMCON = 0x08;
	FMDATA = 0x96;	 			//WE=1	 打开flash写使能
	while((FMCON&0x0F)!=0);
	
	FMCON = 0x67;
	FMDATA = 0x96;				//CWP=0
	while((FMCON&0x0F)!=0);		 

	FMADRL = 0x02;
	FMCON = 0x6C;
	FMDATA = 0x37;				//启动向量0x37	
	while((FMCON&0x0F)!=0);
	
	FMADRL = 0x03;	 			//状态字节
	FMCON = 0x6C;				//访问由FMADRL 寻址的用户配置信息
	FMDATA = 0x61;				//CWP=1 AWP=1  BSB=1	
	while((FMCON&0x0F)!=0);
	
	while(1);
	//AUXR1 = 8;		

   	return TRUE;		
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
unsigned char execDwnldSoft()
{
	unsigned char xdata *tmpPtr = RxBuf;					//指向源数据的指针
	unsigned int idata curSoftPktSn;						//当前软件包序号，16位数的临时变量
   	unsigned char idata err;  	
	unsigned int idata total_len;
	unsigned char idata i;
	unsigned int idata datadrs;
	
	tmpPtr += 7;											//取到数据的开头，包序号
	swapBytes(tmpPtr, (unsigned char *)&curSoftPktSn,2);	//取到包序号置于curSoftPktSn中	
	tmpPtr += 2;											//指针指向数据长度
	swapBytes(tmpPtr, (unsigned char *)&total_len,2);		//取到数据长度	
	tmpPtr += 2;											//指向数据部分
	if(curSoftPktSn == 0)									//第一个包
	{
		if( total_len != 0x1C )								//取得数据长度并指向起始地址
		{																					 
			return OTHER_ERR;								//其他错误
		}
		tmpPtr += 24;//0x18;										//指针指向文件CHECKSUM字节
		swapBytes(tmpPtr, (unsigned char *)&softChecksum,2);//取得chechsum，保存于softChecksum
		nextSoftPktSn = 1;
		softDownLoad = DOING;
		WP = 0;												//关闭写保护，芯片可读可写
		softLen = 0;
	}
	else			  										//非第一包
	{	
		//dataLen = *tmpPtr++;
		if(curSoftPktSn != nextSoftPktSn)					//校验包序号		
		{
			return OTHER_ERR;								//包序号错
		}
		if(total_len>32)
		{
			return OTHER_ERR;								//超出一页写数据范围,一页写32个字节	
		} 
		datadrs = curSoftPktSn - 1;
		datadrs = datadrs * 32;
		XmtDat[1] =  (unsigned char)datadrs;				//低8位外部EEPROM地址
		XmtDat[0] =  (unsigned char)(datadrs>>8);			//高8位外部EEPROM地址		
		for(i=0;i<total_len;i++)
		{
	   		XmtDat[2+i] = *(tmpPtr+i);
		}
		watchdog();
		err = ISendStrExt(0xA0,XmtDat,total_len+2);			//存一包数据到外部EEPROM
		watchdog();
		if( err == FALSE )
		{
			return OTHER_ERR;								//E2PROM错
		}		
		softLen += total_len;								//计算软件总长
		
		if(curSoftPktSn==3)									//第三包数据为检验数据
		{
			for( i = 0; i < 8; i++ )
			{
				if(RxBuf[11+i]!= factry[i])
				{
					WP = 1;
					return OTHER_ERR;						//E2PROM错	
				}
			} 
		}  
		nextSoftPktSn++;
	}	
	return NO_ERR;											//其他错误
}


/*计算异或校验和*/
void calDefcksum(unsigned char *ptr, unsigned int len, unsigned char *checksum) small
{
	unsigned char xdata tmpChecksum = 0;

	while(len--)
	{
		tmpChecksum ^= *ptr++;
	}
	*checksum = tmpChecksum;  
}
/*
*********************************************************************************************************
*                         字节顺序交换
*
* Description: 字节流顺序交换，由于协议中大于2字节的数据低位在前，高位在后，所以需要字节交换
*
* Arguments  : 
*			   result	指针，指向处理目标
*			   src      指针，指向需要处理的字符串
*
* Returns    : 无
*********************************************************************************************************
*/
void swapBytes(unsigned char *src, unsigned char *result,unsigned char num)
{
	unsigned char i;
	src += (num-1);
	for(i=0;i<num;i++)
	{
		*result++ = *src--; 
	}
}

