
/**-------------ÎÄ¼şĞÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼ş   Ãû: packet.c
**´´   ½¨   ÈË: ³Ìİººì 
**°æ        ±¾: V1.0
**ÈÕ¡¡      ÆÚ: 2006Äê9ÔÂ30ÈÕ
**×î ĞÂ °æ  ±¾: V1.6
**Ãè        Êö: 		¶Ô°üµÄ½âÊÍ£¬²¢´¦Àí,²¢²Ù×÷ÉäÆµ¿ª¹Ø£¬PLLµÈ
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

/*Èí¼şÏÂÔØ*/
static unsigned int xdata nextSoftPktSn;				//ÇëÇóÈí¼şÏÂÔØ°üµÄĞòºÅ
static unsigned int xdata preSoftChecksum = 0xFFFF;		//Ç°´ÎÏÂÔØÈí¼şµÄĞ£ÑéºÍ
static unsigned int xdata softChecksum;					//Õû¸öÈí¼şµÄchecksumÖµ
static unsigned int xdata softLen = 0;					//Èí¼ş×Ü³¤¶È
//static unsigned char idata pktDataBuf[32];
unsigned char code factry[8]={0xF0,0xD8,0x80,0xA8,0x5F,0xE0,0x87,0xC9}; 
#if debug == 1
	unsigned char code debugstr[17]={0x01,0x0F,0xA0 ,0xFF ,0x0D ,0xDA ,0x0A ,0x01 ,0x00 ,0x00 ,0x40 ,0x00 ,0x04 ,0xE8 ,0xAF ,0x0D ,0x00};
#endif 

/*
*********************************************************************************************************
*                         Êı¾İ°ü½âÂë						
*
* Description: ¶ÔreadXPktBufÖĞµÄÊı¾İ½øĞĞ½âÂë£¬°üÀ¨¶ş×Ö²ğ·Ö£¬Çø·ÖÃüÁîºÍÊı¾İ£¬È¡µÃCRCÖµ
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
*********************************************************************************************************
*/
void execute_cmd() 
{
	unsigned char resultCode = NO_ERR;		
	unsigned char downresult = NO_ERR;
	switch(RxBuf[5])
	{
		case 0xDA:										//²éÑ¯ºÍÉèÖÃ¹¤³§²ÎÊı
		{
			execFctParamSet();	
			break;
	    }
		case 0xF0:										//RFÄ£ ¿éÉäÆµ£¨¹¤³§£©²ÎÊı²éÑ¯ 	
		{
			execRFParamQ();
			break;
		}
		case 0xFD:										//Èí¼şÏÂÔØÈ·ÈÏ»ò²éÑ¯Ä£¿éĞÅÏ¢	
		{
			if(RxBuf[7]==0x01)
			{
				if(RxBuf[8]==0)							//ÏÂÔØÈ¡Ïû
				{
					Data_len = 10;
					softDownLoad = UNDO;
				}
				else									//ÏÂÔØÈ·ÈÏ
				{
					if(softDownLoad == DOING)
					{
						downresult = endDwnldSoft();
			 			RxBuf[6] |= downresult;				//´íÎó´úÂë±êÖ¾
					}
					else
					{
						RxBuf[6] |= OTHER_ERR;				//´íÎó´úÂë±êÖ¾
					}
					Data_len = 10;
				}
			}
			break;
		}
		case 0xFE:										//Èí¼şÏÂÔØ			
		{
		    DwnldSoft1m_time = getTime();
			resultCode = execDwnldSoft();
			RxBuf[1] = 9;								//Êı¾İ°ü³¤¶È	 
			RxBuf[4] = 7;
 			RxBuf[6] |= resultCode;						//´íÎó´úÂë±êÖ¾
			RxBuf[9] = (unsigned char)nextSoftPktSn;
			RxBuf[10] = (unsigned char)(nextSoftPktSn>>8);
			Data_len = 11;
			break;
		}
		case 0xFB:
		{
			if(RxBuf[7] == 0x01)
			{
				AUXR1 = 8;                                //Èí¼ş¸´Î»   20080618
			}
			break;
		}
	
		default:										//ÃüÁî±àºÅ´í
		{
			RxBuf[6] |= OCID_ERR;						//¼à¿Ø¶ÔÏó±êºÅÎŞ·¨Ê¶±ğ	
			break;
		}
	}	
}

/*
*********************************************************************************************************
*                         Êä³ö¹¦ÂÊ²ÉÑù						
*
* Description: 
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
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

	readE2promStr(1,EE_gPReadADNumer,temp);			//¶ÁÈ¡ ¹¦ÂÊ¶ÁÈ¡²¹³¥·Ö×Ó					
	swapBytes(temp,(unsigned char *)&gPReadADNumer,1);

	readE2promStr(1,EE_gPequDenomi,temp);			//¶ÁÈ¡ ¹¦ÂÊ²¹³¥·ÖÄ¸					
	swapBytes(temp,(unsigned char *)&gPequDenomi,1);

	//readE2promStr(4,EE_CenFreq,temp);				//¶ÁÈ¡ ÖĞĞÄÆµÂÊ
	//swapBytes(temp,(unsigned char *)&gCenFreq,4);

    readE2promStr(4,EE_gPBmFreq,temp);				//¶ÁÈ¡ »ù×¼ÆµÂÊ
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

	//ÅĞ¶Ï·¶Î§£¬·ÀÖ¹Òç³ö
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
*                         ¶¨Ê±²éÑ¯¸æ¾¯ĞÅÏ¢						
*
* Description: 
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
*********************************************************************************************************
*/
void updateAlmInfo() small
{
	unsigned char i;
	static unsigned char preRFSW;
//	static unsigned char gRFSWtemp = 0;
//	unsigned char tempNum;

	//preRFSW = gRFSW;												//·ÅÔÚ²Ù×÷ÉäÆµ¿ª¹ØÖ®ºó

	PA_current = 0;
	re_Pwr =0;
	OutputPwr = 0;	

	if(gRFSW == CLOSE)
	{
		Lim_state = 0;								//ÉäÆµ¿ª¹Ø¹Ø±ÕÊ±ÏŞ·ù×´Ì¬ÎªÕı³£
	}
	else
	{ 
		Lim_state = (LimYes == 0 ? FALSE : TRUE );	//¼ì²âÏŞ·ù×´Ì¬
	}
    
	PLL_state = (LockYes == 1 ? FALSE : TRUE);		//¼ì²âÊ§Ëø×´Ì¬
	 
	watchdog();
			
	if(AutoSwitch == CLOSE)								//ÆµÂÊÃ»Ëø¶¨»òÕßÊÖ¶¯Ç¿ÖÆ¹Ø±ÕÔò¹Ø±Õ¹¦·Å
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
    
	if(preRFSW != gRFSW)						   	//¿ª¹Ø±ä»¯ÔòÖØĞ´ËøÏà»·
	{
		writePLL();
	    if(gRFSW==OPEN)
		 {
		   delay(2);	 
		 }
	}
	preRFSW = gRFSW;

	/*for(i=0;i<20;i++)                              //ÏÈ¶Á100´Î²¢ÇÒ¶ªÆú£¬¼Ó¿ì²ÉÑùËÙ¶È
	{
		readAD(CHANNEL_0);
		readAD(CHANNEL_1);							//¶Á·´Éä¹¦ÂÊADÖµ
		readAD(CHANNEL_5);
	} */

	for(i=0;i<8;i++)
	{
// 		re_Pwr += readAD(CHANNEL_1);				//¶Á·´Éä¹¦ÂÊADÖµ
// 		PA_current += readAD(CHANNEL_5);			//¶Á³ö¹¦·ÅµçÁ÷ADÖµ
		re_Pwr += readAD(CHANNEL_5);				//¶Á·´Éä¹¦ÂÊADÖµ
		PA_current += readAD(CHANNEL_1);			//¶Á³ö¹¦·ÅµçÁ÷ADÖµ
	}

	re_Pwr >>= 3;
	PA_current >>= 3;
	OutputPwrsampling();							//ÕıÏò¹¦ÂÊµÄ¶ÁÈ¡
  AD0CON = 0x01;              //¹Ø±Õ×ª»»£¬ÒÔ·ÀÖ¹³öÏÖ¹Ü½ÅÏà»¥Ó°ÏìÏÖÏó¡£add by dw   20090623
    
#if debug == 1
	memcpy(RxBuf,debugstr,17);
	pack_valid = 1;
#endif

	watchdog();		   			
}
/*
*********************************************************************************************************
*                         ¹¤³§²ÎÊıÉèÖÃÃüÁî						
*
* Description: ±êºÅ·ÖÅäÎª0x0ADA£¬¶Ô¸÷¸ö¹¤³§²ÎÊı½øĞĞÉèÖÃ£¬²éÑ¯ÔÚÁíÒ»ÃüÁîÖĞ
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
*********************************************************************************************************
*/
void execFctParamSet()
{

	unsigned char tempLen;//,bufPoint;
	unsigned short param_addr;//,j;//,j;							        //²ÎÊı¶ÔÓ¦µÄEEPROMµØÖ·
	unsigned short tmpInt=0;//j;
//	unsigned long tmpLong=0;
	unsigned char temp[4];
	unsigned short tPALim;
	
	param_addr = RxBuf[10];
	param_addr += (RxBuf[11]<<8);						       //²ÎÊı¶ÔÓ¦µÄEEPROMµØÖ·

	tempLen = RxBuf[12];
	
	tPALim = gPALim;

	if(RxBuf[7]==0)										//¹¤³§²ÎÊı²éÑ¯
	{	
		readE2promStr(tempLen,param_addr,&RxBuf[13]);            //Ò»°ã²ÎÊıÔÚE2PROMÖĞ¶ÁÈ¡
		  	
		switch(param_addr)                                          //¸ö±ğ²ÎÊıÔòĞèÒªÊµÊ±²éÑ¯
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
				IRcvStr(0x90,0x03,temp,2);         //0x90 ÓÉ1 £¬0 £¬0 £¬1 ,  A2  £¬A1£¬ A0£¬R/W ×é³É£¬¸ßËÄÎ»¹Ì¶¨£¬µÍËÄÎ»ÊÇµØÖ·Î»¡¢¶ÁĞ´Î»
				RxBuf[13] = temp[0];                  //0x03±íÊ¾Òª¶ÁÈ¡µÄIICÆ÷¼şÄÚ²¿¼Ä´æÆ÷µØÖ·
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

			case 0x00C5:								    //2×Ö½Ú£¬³ÌĞòĞ£ÑéºÍ
				RxBuf[13] = Lim_state;
				break;
			case 0x00C6:	  							    //2×Ö½Ú£¬³ÌĞò°æ±¾ºÅ
				RxBuf[13] = (unsigned char)PA_current;
				RxBuf[14] = (unsigned char)(PA_current >> 8);
				break;
			case 0x00C7:								    //2×Ö½Ú£¬³ÌĞòĞ£ÑéºÍ
				RxBuf[13] = (unsigned char)OutputPwr;
				RxBuf[14] = (unsigned char)(OutputPwr >> 8);
				break;
			case 0x00C8:	  							    //2×Ö½Ú£¬³ÌĞò°æ±¾ºÅ
				RxBuf[13] = (unsigned char)re_Pwr;
				RxBuf[14] = (unsigned char)(re_Pwr >> 8);
				break;*/
			case 0x00CA:	
										
				RxBuf[13] = (unsigned char)gcurRfTemp;
				RxBuf[14] = (unsigned char)(gcurRfTemp >> 8);
				break;

			case 0x00E0:	  							    //2×Ö½Ú£¬³ÌĞò°æ±¾ºÅ
				RxBuf[13] = Ver_Low;
				RxBuf[14] = Ver_High;
				break;
			case 0x00E2:								    //2×Ö½Ú£¬³ÌĞòĞ£ÑéºÍ
				RxBuf[13] = (unsigned char)all_checksum;
				RxBuf[14] = (unsigned char)(all_checksum >> 8);
				break;
  			default:
			    break;						
		}
	}
	else                                                    //¹¤³§²ÎÊıÉè¶¨
	{
		
		if (tempLen==2)
		{
			tmpInt = RxBuf[13];
			tmpInt += (RxBuf[14]<<8);		
		}
	
		//switch(param_addr):Ö÷ÒªÓÃÓÚ¶Ô½ÓÊÕµÄÊı¾İ½øĞĞ¼ìÑé£¬ÊÇ·ñÎªÕı³£ÖµµÄ·¶Î§ÄÚ£¬Èô²»ÊÇ£¬Ôò²»´æÈëEEPROM
		switch(param_addr)
		{
			//2×Ö½Ú£¬¹¦·ÅÏŞ·ù
			case EE_DA_CHANNEL_B:
			case EE_DA_CHANNEL_C:
				if ( tmpInt > 1023 )
					RxBuf[6] |= DATRANGE_ERR;	
				break;
			case 0x0020: 									//2×Ö½Ú,¹¦·ÅÆ«Ñ¹Öµ			
				if( (tmpInt<0)||(tmpInt>1023) )
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break; 									//1×Ö½Ú£¬ÔöÒæÎÂ²¹ÏµÊı·Ö×Ó
			case 0x00A2: 									//1×Ö½Ú£¬ÔöÒæÎÂ²¹ÏµÊı·ÖÄ¸
			case 0x00A3:
				if((RxBuf[13]<0)||(RxBuf[13]>255))
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;
			case 0x00A5: 									//1×Ö½Ú£¬¹¦·ÅÖØÆğÎÂ¶ÈÏÂÏŞ   
				if((RxBuf[13]<0)||(RxBuf[13]>125))
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;
			case 0x00D2:									//1×Ö½Ú£¬µ÷ÊÔ×´Ì¬  
				if((RxBuf[13]<0)||(RxBuf[13]>2))	   	
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;	  
			case 0x00D3: 									//1×Ö½Ú£¬ÎÂ²¹»ù×¼Öµ	 
				if((RxBuf[13]<0)||(RxBuf[13]>200))
				{
					RxBuf[6] |= DATRANGE_ERR;	
				}
				break;
			default:
				break;		
		}
		if(RxBuf[6]==0x0A)									//½ÓÊÕµÄÊı¾İÎªÕı³£Öµ£¬Ôò¿ªÊ¼½«Êı¾İ´æEEPROM
		{
			if(param_addr != 0x0140)
			{
				if ( param_addr == 0x0040 && tempLen == 6 )                     //2012-6-19 HANÔö
				{
					writeE2promStr(4,0x0040,&RxBuf[13]);
					writeE2promStr(2,0x0020,&RxBuf[17]);								
				}
				else                                                            //han,³¤¡¢ÃüÁîÊÇ40£¬³¤¶ÈÊÇ6£¬×öÉÏÊöÊÂ¡£
				{
					writeE2promStr(tempLen,param_addr,&RxBuf[13]);			
				}
			}
			rfPramModified = TRUE;							//ÉäÆµ²ÎÊıĞŞ¸Ä±êÖ¾ÓĞĞ§	
			switch(param_addr)
			{
				case EE_TEMPER_BUCHANG:
					memcpy(__temp_que,RxBuf+13,sizeof(__temp_que));
				break;
				case EE_PLUS_TRIGER://´¥·¢µ¥´ÎÂö³åĞÅºÅ
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
				//--------------------------ÔöÒæ-------------------------------
					//---------ÉÏĞĞÔöÒæ------------
				case 0x0003:
				   gGain=RxBuf[13];
				   writeAtt1(gGain);
				   updateAlmInfo();
				case 0x0020: 									//2×Ö½Ú£¬Êä³ö¹¦ÂÊÏŞ·ùÖµ									
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
				case 0x0040: 								//4×Ö½Ú£¬ÖĞĞÄÆµÂÊ
				case 0x0140:
					//gCenFreq = tmpLong;
					swapBytes(&RxBuf[13],(unsigned char *)&gCenFreq,4);
					writePLL();
					delay(1);
					//gPALimCompensate();
                			//writeAD5314(gPALim,'B');			   //¸Ä±äÖĞĞÄÆµÂÊĞèÒªÍ¬Ê±½øĞĞÏŞ·ù²¹³¥

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

				case 0x004A: 								//2×Ö½Ú£¬ÆµÂÊ²½½øÖµ
				case 0x0054: 								//4×Ö½Ú£¬²Î¿¼ÆµÂÊ
					writePLL();
					break;

				case 0x0075:                      //¹¦ÂÊÏŞ·ù²¹³¥·Ö×Ó
//					case 0x0076:
				case 0x0077:                      //¹¦ÂÊ²¹³¥·ÖÄ¸
				case 0x0078:                     //»ù×¼ÆµÂÊ
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

				case 0x00A4: 									//2×Ö½Ú£¬¹¦·Å±£»¤ÎÂ¶ÈÏÂÏŞ
					gPAProtecttLim = RxBuf[13];
					writeLM75(0x03,gPAProtecttLim,3);	
					break;

				case 0x00A5: 									//2×Ö½Ú£¬¹¦·ÅÖØÆğÎÂ¶ÈÏÂÏŞ
					gPAResetLim = RxBuf[13];
					writeLM75(0x02,gPAResetLim,3);	
					break;

				case 0x00C0: 									//1×Ö½Ú£¬ÉäÆµÄ£¿é±àºÅ
					gRF_No = RxBuf[13];	
					break;

				case 0x00C2: 									//1×Ö½Ú£¬ÉäÆµ¿ª¹Ø
					if(AutoSwitch != RxBuf[13]);
					{
					  AutoSwitch = RxBuf[13];

						updateAlmInfo();
						writeAtt1(gGain);
						writePLL();                      //ÉäÆµ¿ª¹Ø¶¯×÷ÁË£¬Ã¿´ÎÒª´ÓĞÂ²Ù×÷ËøÏà												·
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

		RxBuf[0] = 0x00;	//Ö÷¿Ø°åµØÖ·
		RxBuf[1] = 8;	   	//³¤¶È
		RxBuf[2] = 0xA0; 	//ÃüÁî±êÊ¶
		RxBuf[3] = 0x00;	//Ó¦´ğ±êÖ¾
		RxBuf[4] = 6;		//¼à¿Ø¶ÔÏó³¤¶È
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
*                         RFÄ£¿éÉäÆµ²ÎÊı²éÑ¯						
*
* Description: ±êºÅ·ÖÅäÎª0x0AF0£¬¶Ô¸÷¸ö¹¤³§²ÎÊı½øĞĞÒ»´ÎĞÔ²éÑ¯
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
*********************************************************************************************************
*/
void execRFParamQ()
{
	unsigned char i;	
	unsigned int CKS = 0;
//	unsigned int selCurr;
	static unsigned int tmpCurr=0; 

	tmpCurr = (tmpCurr + PA_current + PA_current)/3;					 //µçÁ÷ÖµÓëÇ°ÖµÆ½¾ù

//	if (  PA_current > 400 )
//		selCurr = tmpCurr;
//	else
//		selCurr = PA_current;

	if(RxBuf[7]==0)										//¶¨Ê±Êı¾İ²éÑ¯
	{
		RxBuf[1] = 18; 									//Êı¾İ°ü³¤¶È
		RxBuf[4] = 16;									//¼à¿Ø¶ÔÏóµÄ³¤¶È
		
		RxBuf[8] = Lim_state;
		RxBuf[9] = gRFSW;
		RxBuf[10] = gRFSW;
		RxBuf[11] = (unsigned char)tmpCurr; 			//µÍÎ»ÔÚÇ°
		RxBuf[12] = (unsigned char)(tmpCurr>>8);		//¸ßÎ»ÔÚºó
		RxBuf[13] = (unsigned char)OutputPwr;
		RxBuf[14] = (unsigned char)(OutputPwr>>8);
  		RxBuf[15] = (unsigned char)re_Pwr;
		RxBuf[16] = (unsigned char)(re_Pwr>>8);
		RxBuf[17] = (char)gcurRfTemp;
		RxBuf[18] = (char)(gcurRfTemp>>8);	
		RxBuf[19] = PLL_state;				   			//080220 Ôö¼Ó
		Data_len = 20;
	}
	else if(RxBuf[7]==1)	  							//¹¤³§²ÎÊı²éÑ¯Êı¾İ°ü
	{		
		RxBuf[1] = 94;									//Êı¾İ°ü³¤¶È						
		RxBuf[4] = 92;									//¼à¿Ø¶ÔÏóµÄ³¤¶È
								
		readE2promStr(5,EE_AtteVal,&RxBuf[10]);			//ÔöÒæ²ÎÊı		
		readE2promStr(4,EE_Att1slopeval,&RxBuf[15]);
		readE2promStr(11,EE_gPALim,&RxBuf[19]);			//¹¦·Å²ÎÊı
		readE2promStr(3,EE_gPALimNumer,&RxBuf[30]);		
		readE2promStr(20,EE_BandWidth,&RxBuf[37]);		//ÆµÂÊ²ÎÊı
		readE2promStr(4,EE_gPBmFreq,&RxBuf[57]);
		readE2promStr(14,EE_CurCo,&RxBuf[61]);			//µçÁ÷²ÎÊı		
		readE2promStr(4,EE_GainNumer,&RxBuf[75]);		//ÎÂ¶È²ÎÊı		
		readE2promStr(3,EE_RF_No,&RxBuf[79]);			//Ä£¿é²ÎÊı
		
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
		readE2promStr(10,EE_MODULE_No,&RxBuf[86]);			//Ä£¿é²ÎÊı		
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
*                         Êı¾İ°ü±àÂë						
*
* Description: ¶Ô°üÊı¾İ½øĞĞ±àÂë,ack°üÖÃÓÚreadXPktBufÖĞ
*			   ±¾º¯Êı±ØĞëÎª¿ÉÖØÈëµÄ
*
* Arguments  : X_PACKETÖ¸Õë£¬Ö¸ÏòĞèÒª´¦ÀíµÄ°üÖ¸Õë£¬Î´½øĞĞCRCĞ£Ñé
*
* Returns    : ÎŞ
*********************************************************************************************************
*/
void sendPkt()
{
	unsigned char *tmpPtr;
	unsigned char calPktChecksum;
	unsigned char x;
	tmpPtr = TxBuf;	
	*tmpPtr++= 0x00;									//ÏÈ·¢Ö÷»úµØÖ·£¬¹Ì¶¨Îª0x00						   				
	*tmpPtr++= 0x55; 									//°üÆğÊ¼±êÖ¾
	*tmpPtr++= 0xAA;
	memcpy(tmpPtr,RxBuf,Data_len);						//Õû¸ö°üµÄÊı¾İ³¤¶È£¬°üÀ¨ÆğÊ¼±êÖ¾
   	calDefcksum(tmpPtr,Data_len,&calPktChecksum); 		//¼ÆËãÒì»òĞ£ÑéºÍ
	tmpPtr  += Data_len;
	*tmpPtr++= calPktChecksum;
//	*tmpPtr++= 0x00;	                                //ÎŞÓÃ×Ö½Ú
	TB8 = 0x00;											//µÚÒ»¸ö×Ö½ÚÎªµØÖ·×Ö½Ú£¬TB8±ØĞëÎª1
	Tx_ptr = 0;
//	RDE = 1;             //ÓÃÔÚ485Í¨ĞÅÊ¹ÄÜ£¬×Ï¹âÕâÀïÃ»ÓĞÊ¹ÓÃRDE
	for(x=0;x<0x80;x++)
	{
	}
	SBUF = 	TxBuf[Tx_ptr];
	pack_valid = 0;				

}

/*
*********************************************************************************************************
*                         Èí¼şÏÂÔØ³É¹¦»ò²éÑ¯Ä£¿éĞÅÏ¢						
*
* Description: ±êºÅ·ÖÅäÎª0x0AFD£¬Èí¼şÏÂÔØÍê³ÉÒª½«³ÌĞò´ÓEEPROMÖĞ¶Á³öµ½flashÖĞ£¬²¢³ÌĞò×ªÒÆµ½0x0000Ö´ĞĞ
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
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
	leaveCnt =count%32;								//²»×ã32×Ö½ÚµÄÊıÄ¿
	count = count >> 5; 
	
	if( leaveCnt != 0)									//²»Âú32×Ö½Ú
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
			return OTHER_ERR;							//E2PROM´í
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
			return OTHER_ERR;							//E2PROM´í
		}		
	}  
	gDownFlag = 0xFF;
	writeE2promStr(1,EE_DOWN_FLAG,&gDownFlag);			//ÏÂÔØÊ±Ìí¼ÓÏÂÔØ±êÖ¾µÄÉèÖÃ 20090415 by zxl

	WP = 1;												//´ò¿ªĞ´±£»¤£¬Ğ¾Æ¬Ö»¿É¶Á
	EA = 0;

	FMCON = 0x08;
	FMDATA = 0x96;	 			//WE=1	 ´ò¿ªflashĞ´Ê¹ÄÜ
	while((FMCON&0x0F)!=0);
	
	FMCON = 0x67;
	FMDATA = 0x96;				//CWP=0
	while((FMCON&0x0F)!=0);		 

	FMADRL = 0x02;
	FMCON = 0x6C;
	FMDATA = 0x37;				//Æô¶¯ÏòÁ¿0x37	
	while((FMCON&0x0F)!=0);
	
	FMADRL = 0x03;	 			//×´Ì¬×Ö½Ú
	FMCON = 0x6C;				//·ÃÎÊÓÉFMADRL Ñ°Ö·µÄÓÃ»§ÅäÖÃĞÅÏ¢
	FMDATA = 0x61;				//CWP=1 AWP=1  BSB=1	
	while((FMCON&0x0F)!=0);
	
	while(1);
	//AUXR1 = 8;		

   	return TRUE;		
}
/*
*********************************************************************************************************
*                         RFÄ£¿éÈí¼şÏÂÔØ						
*
* Description: ±êºÅ·ÖÅäÎª0x0AFE£¬¶ÔÉäÆµÄ£¿éµÄ³ÌĞò½øĞĞÔÚÏµÍ³Éı¼¶
*
* Arguments  : ÎŞ
*
* Returns    : ÎŞ
*********************************************************************************************************
*/
unsigned char execDwnldSoft()
{
	unsigned char xdata *tmpPtr = RxBuf;					//Ö¸ÏòÔ´Êı¾İµÄÖ¸Õë
	unsigned int idata curSoftPktSn;						//µ±Ç°Èí¼ş°üĞòºÅ£¬16Î»ÊıµÄÁÙÊ±±äÁ¿
   	unsigned char idata err;  	
	unsigned int idata total_len;
	unsigned char idata i;
	unsigned int idata datadrs;
	
	tmpPtr += 7;											//È¡µ½Êı¾İµÄ¿ªÍ·£¬°üĞòºÅ
	swapBytes(tmpPtr, (unsigned char *)&curSoftPktSn,2);	//È¡µ½°üĞòºÅÖÃÓÚcurSoftPktSnÖĞ	
	tmpPtr += 2;											//Ö¸ÕëÖ¸ÏòÊı¾İ³¤¶È
	swapBytes(tmpPtr, (unsigned char *)&total_len,2);		//È¡µ½Êı¾İ³¤¶È	
	tmpPtr += 2;											//Ö¸ÏòÊı¾İ²¿·Ö
	if(curSoftPktSn == 0)									//µÚÒ»¸ö°ü
	{
		if( total_len != 0x1C )								//È¡µÃÊı¾İ³¤¶È²¢Ö¸ÏòÆğÊ¼µØÖ·
		{																					 
			return OTHER_ERR;								//ÆäËû´íÎó
		}
		tmpPtr += 24;//0x18;										//Ö¸ÕëÖ¸ÏòÎÄ¼şCHECKSUM×Ö½Ú
		swapBytes(tmpPtr, (unsigned char *)&softChecksum,2);//È¡µÃchechsum£¬±£´æÓÚsoftChecksum
		nextSoftPktSn = 1;
		softDownLoad = DOING;
		WP = 0;												//¹Ø±ÕĞ´±£»¤£¬Ğ¾Æ¬¿É¶Á¿ÉĞ´
		softLen = 0;
	}
	else			  										//·ÇµÚÒ»°ü
	{	
		//dataLen = *tmpPtr++;
		if(curSoftPktSn != nextSoftPktSn)					//Ğ£Ñé°üĞòºÅ		
		{
			return OTHER_ERR;								//°üĞòºÅ´í
		}
		if(total_len>32)
		{
			return OTHER_ERR;								//³¬³öÒ»Ò³Ğ´Êı¾İ·¶Î§,Ò»Ò³Ğ´32¸ö×Ö½Ú	
		} 
		datadrs = curSoftPktSn - 1;
		datadrs = datadrs * 32;
		XmtDat[1] =  (unsigned char)datadrs;				//µÍ8Î»Íâ²¿EEPROMµØÖ·
		XmtDat[0] =  (unsigned char)(datadrs>>8);			//¸ß8Î»Íâ²¿EEPROMµØÖ·		
		for(i=0;i<total_len;i++)
		{
	   		XmtDat[2+i] = *(tmpPtr+i);
		}
		watchdog();
		err = ISendStrExt(0xA0,XmtDat,total_len+2);			//´æÒ»°üÊı¾İµ½Íâ²¿EEPROM
		watchdog();
		if( err == FALSE )
		{
			return OTHER_ERR;								//E2PROM´í
		}		
		softLen += total_len;								//¼ÆËãÈí¼ş×Ü³¤
		
		if(curSoftPktSn==3)									//µÚÈı°üÊı¾İÎª¼ìÑéÊı¾İ
		{
			for( i = 0; i < 8; i++ )
			{
				if(RxBuf[11+i]!= factry[i])
				{
					WP = 1;
					return OTHER_ERR;						//E2PROM´í	
				}
			} 
		}  
		nextSoftPktSn++;
	}	
	return NO_ERR;											//ÆäËû´íÎó
}


/*¼ÆËãÒì»òĞ£ÑéºÍ*/
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
*                         ×Ö½ÚË³Ğò½»»»
*
* Description: ×Ö½ÚÁ÷Ë³Ğò½»»»£¬ÓÉÓÚĞ­ÒéÖĞ´óÓÚ2×Ö½ÚµÄÊı¾İµÍÎ»ÔÚÇ°£¬¸ßÎ»ÔÚºó£¬ËùÒÔĞèÒª×Ö½Ú½»»»
*
* Arguments  : 
*			   result	Ö¸Õë£¬Ö¸Ïò´¦ÀíÄ¿±ê
*			   src      Ö¸Õë£¬Ö¸ÏòĞèÒª´¦ÀíµÄ×Ö·û´®
*
* Returns    : ÎŞ
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

