/**-------------文件信息--------------------------------------------------------------------------------
****文   件   名: packet.c
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
************************************************************************************/

#include <reg938.h>
#include <string.h>
#include <intrins.h>
#include <i2c.h>
#include <math.h>
#include <bro20W_RFcontrol.h>
#include <bro20W_RFparam.h>
#include <SPI.h>
#include <packet.h>
#include <task.h>
#include <serial.h>
#include <timer.h>
#include <e2prom.h>
#include <readad.h>

/*
*********************************************************************************************************
*                         射频参数初始化
*
* Description: 初始化RF模块的各类射频参数以及与系统相关的其余参数变量
*
* Arguments  : 无
*
* Returns    : 无
*********************************************************************************************************
*/
void InitRF()
{
	unsigned char temp[10];

	rfPramModified = FALSE;							//射频参数修改标志	
	Task5S_time = getTime() + DELAY5S;				//初始化例行任务初始时间
	Task30S_time= getTime() + 100;					//第一次写温度补偿在启动后1S后
	Task100MS_time = getTime();

	SrcSwitch = 1;
	LD = 1;
	LD2 = 0;
  SM2 = 0;                                        //
	softDownLoad = UNDO;							//初始化为非下载状态
	ack_flag = NO_ERR;								//初始化应答标志
	gtestflag = 0;		 							//初始化测试状态
	gtestRfTemp = 50;								//初始化测试温度值
  gpreRfTemp = 50;					   			//初始化温度状态
	gcurRfTemp = 50;
	gAtttempval = 0;

	re_Pwr = 0;
	PA_current = 0;
	OutputPwr = 0;
//	rfmute = 1;                      //	   使能ADF4350,高电平有效
//	readE2promStr(1,EE_RFSW,&AutoSwitch);				//射频开关
	AutoSwitch = CLOSE;                     //上位机开关
	gRFSW = CLOSE;
	
	switchRF(gRFSW);
	
	delay(1);
	watchdog();
	
	//Source select
	readE2promStr(1,EE_SOURCE_SELECT,temp);
	RFSrcSelect = (temp[0] == 0xFF)? SRC_INTERNAL:temp[0];
	
	readE2promStr(4,EE_CenFreq,temp);				//读取中心频率
	swapBytes(temp,(unsigned char *)&gCenFreq,4);

	readE2promStr(1,EE_RF_No,&gRF_No);				//读取射频模块地址

	gPALimCompensate();
	writeAD5314(gPALim,'A');

	watchdog();				
	writeLM75(0x90,0x00,2);							 //选择指针为配置寄存器

	readE2promStr(1,EE_PAProtectLim,&gPAProtecttLim);//写功放保护温度门限
	writeLM75(0x03,gPAProtecttLim,3);	
	
	readE2promStr(1,EE_PAResetLim,&gPAResetLim);	 //写功放重启温度门限
	writeLM75(0x02,gPAResetLim,3);

	readE2promStr(1,EE_BaseTemp_VALUE,&gBaseTemp); 			//读取温度补偿温度基准值
	watchdog();	 
  readE2promStr(1,EE_Att1,&gGain);
	writeAtt1(gGain);
	writePLL();											
	delay(1);									
	readTemperatur();
	execCheckSumQ();							   	 //初始化计算校验和		
}


/*写LM75中的功放保护和恢复门限*/
void writeLM75(char point,char value,unsigned char num)
{
	char temp[5];
	if(num==2)
	{
		temp[0] = 0x01;
		temp[1] = 0x18;								 //高字节在前
	}
	else if(num==3)
	{
		temp[0] = point;
		temp[1] = value;								//高字节在前
		temp[2] = 0x00;									//低字节在后
	}
	watchdog();								   //这里似乎没必要喂狗
	ISendStrExt(0x90,temp,num);
}


/*开关射频*/
void switchRF(unsigned char value)
{
	unsigned char temp[4];
	
	readE2promStr(2,EE_DA_CHANNEL_B,temp);
	swapBytes(temp,(unsigned char *)&gDAoutB,2);
	if ( gDAoutB > 1023 )gDAoutB = 0;
	
	readE2promStr(2,EE_DA_CHANNEL_C,temp);
	swapBytes(temp,(unsigned char *)&gDAoutC,2);
	if ( gDAoutC > 1023 )gDAoutC = 0;
	
	if(value == CLOSE)
	{
		writeAD5314(0,'B');
		writeAD5314(0,'C');	
		RFswitch = 1;
	}
	else
	{
		writeAD5314(gDAoutB,'B');
		writeAD5314(gDAoutC,'C');			
		RFswitch = 0;
	}
}
/***********************************************************************************************
函数名:		writeDA5741
功能:		唤醒DA及A\B\C\D通道的DA输出
参数说明:	
			value:送入5741的数据	
			function:功能选择	W:为唤醒DA		A/B/C/D:对A/B/C/D通道操作
操作说明:	Immediately following CS high-to-low transition, the data is shifted synchronously 
			and latched into the input register on the falling edge of the serial clock input(SCLK)
************************************************************************************************/
/*
void writeDA5741(unsigned int value,unsigned char function)
{
	value <<=2;
	value &=0xFFFC;				//取DA数据(bit2~bit11)

	switch(function)
	{
		case 	'W':			//wake up	A、B、C、D通道
			value |= 0x0000;	
			break;

		case	'A':
			value |= 0x0000;
			break;
		
		case	'B':
			value |= 0x1000;
			break;

		case	'C':
			value |= 0x2000;	//高4位为模式控制位Shift data from input register to DAC register, OUTC updated
			break;

		case	'D':
			value |= 0x3000;
			break;

		default:
			break;	
	}
	LD = 1;
	LD = 0;
	MSend_bit(value,16);
	LD = 1;	
} */


void writeAD5314(unsigned short value,unsigned char channel)            //DA转换器
{
	value <<=2;
	value &=0xFFFC;	
	if(channel=='A')
	{
		value |= 0x2000;		 		//高4位为模式控制位
	}
	else if(channel=='B')	
	{
		value |= 0x6000;
	}
	else if(channel=='C')	
	{
		value |= 0xA000;
	}
	else 	
	{
		value |= 0xE000;
	}
	LD = 1;
	LD = 0;
	MSend_bit(value,16);
	LD = 1;
}
//减器Att1--1U5(前部衰减器)
/**********对衰减值的做了相应的处理 2--10,    4-20		   6-30*/
void writeAtt1(unsigned char att1)
{
 	unsigned short value	;
	LE1 = 0;
	value =(unsigned int)att1/2;
	/*	switch(value)
	{
	case 2: value=20;break;
	case 4 : value=40;break;
	case 6:value=60;break;

	default: break;
	}
*/
	value<<=10;
	MSend_bit(value,6);	
	LE1 = 1;
	LE1 = 0;
}
  /*
//设置总的衰减值
unsigned char writeAttenuator(unsigned char attVal)
{
	gAtt1 = attVal <<= 1;
	gAtt1 += gAtttempval;
	gAtt1 += gGainOffset;

	if(gAtt1 > 0x3F)
	{
		gAtt1 = 0x3F;
	}

	writeAtt1(gAtt1);	
	return TRUE;		
}  */
/*
***************************************************************************************************************
*                         输出功率限幅值补偿
*
* Description: 补偿限幅值 
*
* Arguments  : 无
*
* Returns    : 无
***************************************************************************************************************
*/
void gPALimCompensate()
{		
    unsigned char temp[4];
    unsigned char gPALimNumerN;
	signed short calVal;

    readE2promStr(2, EE_gPALim, temp);											
	swapBytes(temp, (unsigned char *)&gPALim, 2);

#if debug == 1
	gPALim = 600;
#endif

	readE2promStr(1,EE_gPALimNumer,temp);			//读取 功率限幅补偿分子					
	swapBytes(temp,(unsigned char *)&gPALimNumer,1);

	readE2promStr(1,EE_gPequDenomi,temp);			//读取 功率补偿分母					
	swapBytes(temp,(unsigned char *)&gPequDenomi,1);

  readE2promStr(4,EE_gPBmFreq,temp);				//读取 基准频率
	swapBytes(temp,(unsigned char *)&gPBmFreq,4);

	gPALimNumerN = ~gPALimNumer + 1;

	calVal = gPALim;
	
	if(gCenFreq < gPBmFreq)
    {
		if(gPALimNumer < 0)
		{
			calVal += (gPBmFreq - gCenFreq) * gPALimNumerN / (gPequDenomi * 1000);
		}
		else
		{
			calVal -= (gPBmFreq - gCenFreq) * gPALimNumer / (gPequDenomi * 1000);
		}
	}
	else if(gPBmFreq < gCenFreq)
	{
		if(gPALimNumer < 0)
		{
			calVal -= (gCenFreq - gPBmFreq) * gPALimNumerN / (gPequDenomi * 1000);
		}
		else
		{
			calVal += (gCenFreq - gPBmFreq) * gPALimNumer / (gPequDenomi * 1000);
		}
	}

	//检查范围，防止溢出
	if(calVal < 0)
	{
		gPALim = 0;
	}
	else if(calVal > 1023)								   //防止输出限幅值溢出，导致掉功率
	{
		gPALim = 1023;
	}
	else
	{
		gPALim = calVal;
	}


}
/*
****************************************************************************************************************
*                         读取温度
*
* Description: 读取温度，定时执行，更新温度值 
*
* Arguments  : 无
*
* Returns    : 无
*****************************************************************************************************************
*/
void readTemperatur()
{
	signed short value = 0;

	gpreRfTemp =  gcurRfTemp;
	
	IRcvStr(0x92,0x00,(unsigned char*)&value,2);			//读取温度，其中指针寄存器为00
	value >>= 7;
	gcurRfTemp = value;

/*	if(gtestflag == 0)										//不在调试状态下，使用实际温度
	{
		gainCo = gcurRfTemp - gBaseTemp*2;							 
	}
	else if(gtestflag == 2)
	{
		gainCo = gtestRfTemp - gBaseTemp*2;							
	} */
}
/*
****************************************************************************************************************
*                         温度补偿
*
* Description: 根据读取的温度值，对衰减值和锁相环分别进行温补操作 
*
* Arguments  : 无
*
* Returns    : 无
*****************************************************************************************************************
*/
/*
void tempCompensate() 
{
//	unsigned char numerator, denominator;
	int gainCotemp;

	if( (softDownLoad == DOING)||(gtestflag==1) )
	{
		return;
	}

//	readE2promStr(1,EE_GainNumer,&numerator);	 				//增益温补系数分子
//	readE2promStr(1,EE_GainDenomi,&denominator);	 			//增益温补系数分母
//	readE2promStr(1,EE_TEMP_VALUE,&gTempgain); 				//读取温度基准值
	if( gTempgain > 16 ) 
	{		
		gTempgain = 9;
	}	
	 
	gainCotemp = gainCo*gGainNumer; //gGainNumer
	if(gGainDenomi == 0)
	{
		gGainDenomi = 100;
	}
	gainCotemp = gainCotemp/gGainDenomi;
	  
	gainCotemp = gTempgain - gainCotemp;
	if(gainCotemp < 0)
	{
		gAtttempval = 0;
	}
	else if(gainCotemp > 0x3F)
	{
		gAtttempval = 0x3F;
	}
	else
	{
		gAtttempval = (unsigned char)gainCotemp;
	}

	writeAttenuator(gAtteVal);

	rfPramModified = FALSE;							//射频参数修改标志	

}*/

/*写锁相环4350*/
void progPLL(unsigned long value)
{	
	unsigned char num_bit,i;
	unsigned long masque_bit;
	unsigned char x;

	num_bit = 32;
	masque_bit = 0x80000000;	
	
	CLK = 0;					//初始状态
	LD2 = 1;

	LD2 = 0;
	for(i=0;i<num_bit;i++)
	{
		if((value & masque_bit)==0)
		{
			DATA = 0;
		}			
		else
		{
			DATA = 1;	
		}
		for(x=0;x<2;x++)
		{
		}
		CLK = 1;
		value <<= 1;
		CLK = 0;
	}
	LD2 = 1;
}

/*
void writePLL()
{
 	progPLL(0x00580005);     //5
	progPLL(0x0095003c);
	progPLL(0x000004b3);
	progPLL(0x60004ec2);
	progPLL(0x08008191);
	progPLL(0x00b48000);     //0
}
/*
****************************************************************************************************************
*                         写锁相环ADF4350
*
* Description: 根据设定的中心频率, 中频频率, 频率偏移, 频率步进, 频率温度补偿系数计算出各寄存器值, 再调用progPLL
*
* Arguments  : 无
*
* Returns    : 无
*****************************************************************************************************************
*/
void writePLL()
{
		#define R0_INIT		0x00000000
	//
	//DB31             ～                  DB4 DB3 DB2 DB1 DB0
	//0    0    0    0    0    0    0          0
	//0                ～                  0   0   0   0   0

	#define R1_INIT		0x08008001
	//			  PRESCALER
	//DB31～DB28  DB27       DB26 DB25 DB24 DB23  ～ DB16 DB15 DB14 DB13 DB12 DB11 ～ DB4 DB3 DB2 DB1 DB0
	//0			  8			 	   			0     0		  8					  0    0      1
	//0   ～0     1			 0    0    0    0    ～   0   1    0    0    0    0    ～ 0	  0	  0   0   1 

	#define R2_INIT		0x18004EC2
	//     NOISE-MODE[1:0] MUXOUT[2:0]	   REF-DOU RDIV2    					 DOUB-BUFF	CHARGE-PUMP[3:0]  LDF LDP PD-POLA PD  CP-THR  COUNT-RESET
	//DB31 DB30 DB29       DB28 DB27 DB26  DB25    DB24   DB23 ～ DB16 DB15 DB14 DB13       DB12 DB11 B10 DB9 DB8 DB7 DB6     DB5 DB4     DB3         DB2 DB1 DB0
	//1							8						  0	   0	   4						 E				  C						  2
	//0    0    0          1    1    0     0       0      0    ～ 0    0    1    0			0	 1	  1	  1	  0	  1	  1	      0	  0		  0           0   1   0


	#define R3_INIT		0x000004B3
	//					CSR		  CLK-DIV[1:0]
	//DB31 ～ DB20 DB19 DB18 DB17 DB16 DB15    DB14 DB13 DB12 DB11 DB10 DB9 DB8 DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0  0  0	   0                   0                      4  				B				3
	//0    ～ 0    0    0    0    0    0       0    0    0    0	   1    0   0   1   0   1   1   0   0   1   1 
	
	#define R4_INIT		0x0085041C				//0x00850414
	//				 FEEDBACK DIV-SEL[2:0]	     BAND-SELECT-CLOCK-DIVIDER-VALUE[7:0]        VCO-POW  MTLD AUXOUT-SEL AUXOUT-EN	AUXOUT-POW[1:0]	RFOUT-EN OUTPOW[1:0]
	//DB31 ～ DB24   DB23     DB22 DB21 DB20     DB19 DB18 DB17 DB16 DB15 DB14 DB13 DB12     DB11     B10  DB9        DB8       DB7 DB6         DB5      DB4 DB3     DB2 DB1 DB0
	//0    0		 8							 5					 0						 4									1							 C
	//0    ～ 0      1        0    0    0        0    1    0    1    0    0    0    0		 0		  1	   0		  0			0	0			0		 1	 1		 1	 0	 0

	#define R5_INIT		0x00580005
	//				 LD-PIN[1:0]											 
	//DB31 ～ DB24   DB23 DB22   DB21 DB20 DB19 DB18 DB17 DB16 DB15 ～ DB4 DB3 DB2 DB1 DB0
	//0    0		 5					   8                   0 0 0       5
	//0    ～ 0      0    1    	 0	  1	   1    0    0    0    0    ～ 0   0   1   0   1
		  
		  
	unsigned long counterTemp,counterN,divSel,refFreq;
	unsigned short freqStep;
	unsigned char temp[4];
 

	readE2promStr(4,EE_RefFreq,temp);									//读取参考频率，4个字节
	swapBytes(temp, (unsigned char *)&refFreq,4);

	readE2promStr(2,EE_FreqStep,temp);									//读取中频步进值
	swapBytes(temp, (unsigned char *)&freqStep,2);

#if debug == 1
	gCenFreq = 1805000;	//930000
	freqStep = 100;
	refFreq = 10000;
	gRFSW = 1;
#endif

	if (freqStep == 0) 
	{
		freqStep = 25;
	}
 
//	counterN = gCenFreq;
	counterN = RFSrcSelect?0:gCenFreq;
	//VCO只能输出2.2GHz-4.4GHz的频率，根据需要输出的频率确定VCO输出的分频值
   
	divSel = 4400000 / counterN;
	divSel = checkMBit((unsigned char)divSel);
	counterN <<= divSel;							//放大为VCO的输出频率和步进值
	freqStep <<= divSel;
	progPLL(R5_INIT);								//R5

	counterTemp = divSel;
	counterTemp <<= 20;
	counterTemp = counterTemp | R4_INIT | (gRFSW << 5);//射频开关关闭则不使能输出
	progPLL(counterTemp);							//R4

	progPLL(R3_INIT);								//R3

	progPLL(R2_INIT);								//R2

						  							//转换成对应的分频值1、2、4、8、16
	//divSel = 1 << divSel;							//计算小数分频的模
	counterTemp = refFreq;
	counterTemp /= (unsigned long)freqStep;
	counterTemp <<= 3;
	counterTemp |= R1_INIT;
	progPLL(counterTemp);							//R1
													//计算小数分频分子
	
	counterTemp = counterN % refFreq;
	counterTemp /= (unsigned long)freqStep;
	counterTemp <<= 3;
	counterN /= refFreq;							//计算整数分频
	counterN <<= 15;
	counterTemp |= counterN;
	progPLL(counterTemp);							//R0
	watchdog();		
}
/*
*********************************************************************************************************
*                         A/D转换
*
* Description: 根据给定的A/D转换通道，取得相应的A/D转换值
*
* Arguments  : channel  给定的通道号
*
* Returns    : value    转换结果
*********************************************************************************************************
*/
unsigned short readAD(unsigned char channel)
{

	unsigned short value = 0; 
	unsigned short temp = 0;
	
	AD0INS  = channel;	  					//选择转换通道 //
	AD0MODA |= 0x10;						//单次转换
	AD0MODB |= 0x20;						// ADC时钟=CCLK/2,即2分频
	AD0CON = 0x05;							//立即启动
	while(!AD0CON&0x08);					//等待转换完毕
    //while(!AD0CON&0x08);					//等待转换完毕
   
	switch(channel)
	{
		#ifdef CHANNEL_0
		case CHANNEL_0:
			value |= AD0DAT0R;					//右边字节，低位，取位7.6.5.4.3.2.1.0. 八位
			temp  |= AD0DAT0L;	 				//左边字节，高位，位9.8.7.6.5.4.3.2为一字节，取9.8.两位
			break;
		#endif

		#ifdef CHANNEL_1
		case CHANNEL_1:
			value |= AD0DAT1R;
			temp  |= AD0DAT1L;
			break;
		#endif

		#ifdef CHANNEL_2
		case CHANNEL_2:
			value |= AD0DAT2R;
			temp  |= AD0DAT2L;
			break;
		#endif

		#ifdef CHANNEL_3
		case CHANNEL_3:
			value |= AD0DAT3R;
			temp  |= AD0DAT3L;
			break;
		#endif

		#ifdef CHANNEL_4
		case CHANNEL_4:
			value |= AD0DAT4R;
			temp  |= AD0DAT4L;
			break;
		#endif

		#ifdef CHANNEL_5
		case CHANNEL_5:
			value |= AD0DAT5R;
			temp  |= AD0DAT5L;
			break;
		#endif

		#ifdef CHANNEL_6
		case CHANNEL_6:
			value |= AD0DAT6R;
			temp  |= AD0DAT6L;
			break;
		#endif

		#ifdef CHANNEL_7
		case CHANNEL_7:
			value |= AD0DAT7R;
			temp  |= AD0DAT7L;
			break;
		#endif 
		default:
			break;
	}
    //AD0CON = 0x01;              //关闭转换，以防止出现的管脚相互影响现象。add by dw   20090623
	
    temp <<= 2;
	temp &= 0x0300;
	value &= 0x00FF;
	temp |= value;
	return temp;
}
/*
*********************************************************************************************************
*                         计算程序校验和
*
* Description: 读取flash中的代码，计算校验和
*
* Arguments  : 无
*
* Returns    : 计算结果保存在全局变量all_checksum中
*********************************************************************************************************
*/
void execCheckSumQ()
{
	unsigned char code *data pt_flash;	
	all_checksum = 0;
	for (pt_flash = 0x0000; pt_flash < 0x1D00; pt_flash++)
	{
		all_checksum += *pt_flash;
		watchdog();
	}
	for (pt_flash = 0x0000; pt_flash < 0xE300; pt_flash++)
	{
		all_checksum += 0xFF;
		watchdog();
	}
}
//喂狗程序
void watchdog(void) 
{
	EA = 0; 			//关闭中断 
	WFEED1 = 0xA5; 		//执行清零第一部分 
	WFEED2 = 0x5A; 		//执行清零第二部分 
	EA = 1; 			//开中断				
}

void delay(unsigned char time)
{
	unsigned char x;
	unsigned short y;
	for(x=0;x<time;x++)
	{
		watchdog();
		for(y=0;y<0x8FFF;y++);								//han
		//for(y=0;y<0x0700;y++); // 1毫秒	  
	}
}

unsigned char checkMBit(unsigned char value)
{
	#define MASQUE 0x80
	unsigned char i;

	for(i=7;i>0;i--)
	{
		if((value & MASQUE)==0x80)
		{
			break;
		}
		value <<= 1;
	}

	return i;
}
