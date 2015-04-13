/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
****��   ��   ��: packet.c
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
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
*                         ��Ƶ������ʼ��
*
* Description: ��ʼ��RFģ��ĸ�����Ƶ�����Լ���ϵͳ��ص������������
*
* Arguments  : ��
*
* Returns    : ��
*********************************************************************************************************
*/
void InitRF()
{
	unsigned char temp[10];

	rfPramModified = FALSE;							//��Ƶ�����޸ı�־	
	Task5S_time = getTime() + DELAY5S;				//��ʼ�����������ʼʱ��
	Task30S_time= getTime() + 100;					//��һ��д�¶Ȳ�����������1S��
	Task100MS_time = getTime();

	SrcSwitch = 1;
	LD = 1;
	LD2 = 0;
  SM2 = 0;                                        //
	softDownLoad = UNDO;							//��ʼ��Ϊ������״̬
	ack_flag = NO_ERR;								//��ʼ��Ӧ���־
	gtestflag = 0;		 							//��ʼ������״̬
	gtestRfTemp = 50;								//��ʼ�������¶�ֵ
  gpreRfTemp = 50;					   			//��ʼ���¶�״̬
	gcurRfTemp = 50;
	gAtttempval = 0;

	re_Pwr = 0;
	PA_current = 0;
	OutputPwr = 0;
//	rfmute = 1;                      //	   ʹ��ADF4350,�ߵ�ƽ��Ч
//	readE2promStr(1,EE_RFSW,&AutoSwitch);				//��Ƶ����
	AutoSwitch = CLOSE;                     //��λ������
	gRFSW = CLOSE;
	
	switchRF(gRFSW);
	
	delay(1);
	watchdog();
	
	//Source select
	readE2promStr(1,EE_SOURCE_SELECT,temp);
	RFSrcSelect = (temp[0] == 0xFF)? SRC_INTERNAL:temp[0];
	
	readE2promStr(4,EE_CenFreq,temp);				//��ȡ����Ƶ��
	swapBytes(temp,(unsigned char *)&gCenFreq,4);

	readE2promStr(1,EE_RF_No,&gRF_No);				//��ȡ��Ƶģ���ַ

	gPALimCompensate();
	writeAD5314(gPALim,'A');

	watchdog();				
	writeLM75(0x90,0x00,2);							 //ѡ��ָ��Ϊ���üĴ���

	readE2promStr(1,EE_PAProtectLim,&gPAProtecttLim);//д���ű����¶�����
	writeLM75(0x03,gPAProtecttLim,3);	
	
	readE2promStr(1,EE_PAResetLim,&gPAResetLim);	 //д���������¶�����
	writeLM75(0x02,gPAResetLim,3);

	readE2promStr(1,EE_BaseTemp_VALUE,&gBaseTemp); 			//��ȡ�¶Ȳ����¶Ȼ�׼ֵ
	watchdog();	 
  readE2promStr(1,EE_Att1,&gGain);
	writeAtt1(gGain);
	writePLL();											
	delay(1);									
	readTemperatur();
	execCheckSumQ();							   	 //��ʼ������У���		
}


/*дLM75�еĹ��ű����ͻָ�����*/
void writeLM75(char point,char value,unsigned char num)
{
	char temp[5];
	if(num==2)
	{
		temp[0] = 0x01;
		temp[1] = 0x18;								 //���ֽ���ǰ
	}
	else if(num==3)
	{
		temp[0] = point;
		temp[1] = value;								//���ֽ���ǰ
		temp[2] = 0x00;									//���ֽ��ں�
	}
	watchdog();								   //�����ƺ�û��Ҫι��
	ISendStrExt(0x90,temp,num);
}


/*������Ƶ*/
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
������:		writeDA5741
����:		����DA��A\B\C\Dͨ����DA���
����˵��:	
			value:����5741������	
			function:����ѡ��	W:Ϊ����DA		A/B/C/D:��A/B/C/Dͨ������
����˵��:	Immediately following CS high-to-low transition, the data is shifted synchronously 
			and latched into the input register on the falling edge of the serial clock input(SCLK)
************************************************************************************************/
/*
void writeDA5741(unsigned int value,unsigned char function)
{
	value <<=2;
	value &=0xFFFC;				//ȡDA����(bit2~bit11)

	switch(function)
	{
		case 	'W':			//wake up	A��B��C��Dͨ��
			value |= 0x0000;	
			break;

		case	'A':
			value |= 0x0000;
			break;
		
		case	'B':
			value |= 0x1000;
			break;

		case	'C':
			value |= 0x2000;	//��4λΪģʽ����λShift data from input register to DAC register, OUTC updated
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


void writeAD5314(unsigned short value,unsigned char channel)            //DAת����
{
	value <<=2;
	value &=0xFFFC;	
	if(channel=='A')
	{
		value |= 0x2000;		 		//��4λΪģʽ����λ
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
//����Att1--1U5(ǰ��˥����)
/**********��˥��ֵ��������Ӧ�Ĵ��� 2--10,    4-20		   6-30*/
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
//�����ܵ�˥��ֵ
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
*                         ��������޷�ֵ����
*
* Description: �����޷�ֵ 
*
* Arguments  : ��
*
* Returns    : ��
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

	readE2promStr(1,EE_gPALimNumer,temp);			//��ȡ �����޷���������					
	swapBytes(temp,(unsigned char *)&gPALimNumer,1);

	readE2promStr(1,EE_gPequDenomi,temp);			//��ȡ ���ʲ�����ĸ					
	swapBytes(temp,(unsigned char *)&gPequDenomi,1);

  readE2promStr(4,EE_gPBmFreq,temp);				//��ȡ ��׼Ƶ��
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

	//��鷶Χ����ֹ���
	if(calVal < 0)
	{
		gPALim = 0;
	}
	else if(calVal > 1023)								   //��ֹ����޷�ֵ��������µ�����
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
*                         ��ȡ�¶�
*
* Description: ��ȡ�¶ȣ���ʱִ�У������¶�ֵ 
*
* Arguments  : ��
*
* Returns    : ��
*****************************************************************************************************************
*/
void readTemperatur()
{
	signed short value = 0;

	gpreRfTemp =  gcurRfTemp;
	
	IRcvStr(0x92,0x00,(unsigned char*)&value,2);			//��ȡ�¶ȣ�����ָ��Ĵ���Ϊ00
	value >>= 7;
	gcurRfTemp = value;

/*	if(gtestflag == 0)										//���ڵ���״̬�£�ʹ��ʵ���¶�
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
*                         �¶Ȳ���
*
* Description: ���ݶ�ȡ���¶�ֵ����˥��ֵ�����໷�ֱ�����²����� 
*
* Arguments  : ��
*
* Returns    : ��
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

//	readE2promStr(1,EE_GainNumer,&numerator);	 				//�����²�ϵ������
//	readE2promStr(1,EE_GainDenomi,&denominator);	 			//�����²�ϵ����ĸ
//	readE2promStr(1,EE_TEMP_VALUE,&gTempgain); 				//��ȡ�¶Ȼ�׼ֵ
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

	rfPramModified = FALSE;							//��Ƶ�����޸ı�־	

}*/

/*д���໷4350*/
void progPLL(unsigned long value)
{	
	unsigned char num_bit,i;
	unsigned long masque_bit;
	unsigned char x;

	num_bit = 32;
	masque_bit = 0x80000000;	
	
	CLK = 0;					//��ʼ״̬
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
*                         д���໷ADF4350
*
* Description: �����趨������Ƶ��, ��ƵƵ��, Ƶ��ƫ��, Ƶ�ʲ���, Ƶ���¶Ȳ���ϵ����������Ĵ���ֵ, �ٵ���progPLL
*
* Arguments  : ��
*
* Returns    : ��
*****************************************************************************************************************
*/
void writePLL()
{
		#define R0_INIT		0x00000000
	//
	//DB31             ��                  DB4 DB3 DB2 DB1 DB0
	//0    0    0    0    0    0    0          0
	//0                ��                  0   0   0   0   0

	#define R1_INIT		0x08008001
	//			  PRESCALER
	//DB31��DB28  DB27       DB26 DB25 DB24 DB23  �� DB16 DB15 DB14 DB13 DB12 DB11 �� DB4 DB3 DB2 DB1 DB0
	//0			  8			 	   			0     0		  8					  0    0      1
	//0   ��0     1			 0    0    0    0    ��   0   1    0    0    0    0    �� 0	  0	  0   0   1 

	#define R2_INIT		0x18004EC2
	//     NOISE-MODE[1:0] MUXOUT[2:0]	   REF-DOU RDIV2    					 DOUB-BUFF	CHARGE-PUMP[3:0]  LDF LDP PD-POLA PD  CP-THR  COUNT-RESET
	//DB31 DB30 DB29       DB28 DB27 DB26  DB25    DB24   DB23 �� DB16 DB15 DB14 DB13       DB12 DB11 B10 DB9 DB8 DB7 DB6     DB5 DB4     DB3         DB2 DB1 DB0
	//1							8						  0	   0	   4						 E				  C						  2
	//0    0    0          1    1    0     0       0      0    �� 0    0    1    0			0	 1	  1	  1	  0	  1	  1	      0	  0		  0           0   1   0


	#define R3_INIT		0x000004B3
	//					CSR		  CLK-DIV[1:0]
	//DB31 �� DB20 DB19 DB18 DB17 DB16 DB15    DB14 DB13 DB12 DB11 DB10 DB9 DB8 DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	//0  0  0	   0                   0                      4  				B				3
	//0    �� 0    0    0    0    0    0       0    0    0    0	   1    0   0   1   0   1   1   0   0   1   1 
	
	#define R4_INIT		0x0085041C				//0x00850414
	//				 FEEDBACK DIV-SEL[2:0]	     BAND-SELECT-CLOCK-DIVIDER-VALUE[7:0]        VCO-POW  MTLD AUXOUT-SEL AUXOUT-EN	AUXOUT-POW[1:0]	RFOUT-EN OUTPOW[1:0]
	//DB31 �� DB24   DB23     DB22 DB21 DB20     DB19 DB18 DB17 DB16 DB15 DB14 DB13 DB12     DB11     B10  DB9        DB8       DB7 DB6         DB5      DB4 DB3     DB2 DB1 DB0
	//0    0		 8							 5					 0						 4									1							 C
	//0    �� 0      1        0    0    0        0    1    0    1    0    0    0    0		 0		  1	   0		  0			0	0			0		 1	 1		 1	 0	 0

	#define R5_INIT		0x00580005
	//				 LD-PIN[1:0]											 
	//DB31 �� DB24   DB23 DB22   DB21 DB20 DB19 DB18 DB17 DB16 DB15 �� DB4 DB3 DB2 DB1 DB0
	//0    0		 5					   8                   0 0 0       5
	//0    �� 0      0    1    	 0	  1	   1    0    0    0    0    �� 0   0   1   0   1
		  
		  
	unsigned long counterTemp,counterN,divSel,refFreq;
	unsigned short freqStep;
	unsigned char temp[4];
 

	readE2promStr(4,EE_RefFreq,temp);									//��ȡ�ο�Ƶ�ʣ�4���ֽ�
	swapBytes(temp, (unsigned char *)&refFreq,4);

	readE2promStr(2,EE_FreqStep,temp);									//��ȡ��Ƶ����ֵ
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
	//VCOֻ�����2.2GHz-4.4GHz��Ƶ�ʣ�������Ҫ�����Ƶ��ȷ��VCO����ķ�Ƶֵ
   
	divSel = 4400000 / counterN;
	divSel = checkMBit((unsigned char)divSel);
	counterN <<= divSel;							//�Ŵ�ΪVCO�����Ƶ�ʺͲ���ֵ
	freqStep <<= divSel;
	progPLL(R5_INIT);								//R5

	counterTemp = divSel;
	counterTemp <<= 20;
	counterTemp = counterTemp | R4_INIT | (gRFSW << 5);//��Ƶ���عر���ʹ�����
	progPLL(counterTemp);							//R4

	progPLL(R3_INIT);								//R3

	progPLL(R2_INIT);								//R2

						  							//ת���ɶ�Ӧ�ķ�Ƶֵ1��2��4��8��16
	//divSel = 1 << divSel;							//����С����Ƶ��ģ
	counterTemp = refFreq;
	counterTemp /= (unsigned long)freqStep;
	counterTemp <<= 3;
	counterTemp |= R1_INIT;
	progPLL(counterTemp);							//R1
													//����С����Ƶ����
	
	counterTemp = counterN % refFreq;
	counterTemp /= (unsigned long)freqStep;
	counterTemp <<= 3;
	counterN /= refFreq;							//����������Ƶ
	counterN <<= 15;
	counterTemp |= counterN;
	progPLL(counterTemp);							//R0
	watchdog();		
}
/*
*********************************************************************************************************
*                         A/Dת��
*
* Description: ���ݸ�����A/Dת��ͨ����ȡ����Ӧ��A/Dת��ֵ
*
* Arguments  : channel  ������ͨ����
*
* Returns    : value    ת�����
*********************************************************************************************************
*/
unsigned short readAD(unsigned char channel)
{

	unsigned short value = 0; 
	unsigned short temp = 0;
	
	AD0INS  = channel;	  					//ѡ��ת��ͨ�� //
	AD0MODA |= 0x10;						//����ת��
	AD0MODB |= 0x20;						// ADCʱ��=CCLK/2,��2��Ƶ
	AD0CON = 0x05;							//��������
	while(!AD0CON&0x08);					//�ȴ�ת�����
    //while(!AD0CON&0x08);					//�ȴ�ת�����
   
	switch(channel)
	{
		#ifdef CHANNEL_0
		case CHANNEL_0:
			value |= AD0DAT0R;					//�ұ��ֽڣ���λ��ȡλ7.6.5.4.3.2.1.0. ��λ
			temp  |= AD0DAT0L;	 				//����ֽڣ���λ��λ9.8.7.6.5.4.3.2Ϊһ�ֽڣ�ȡ9.8.��λ
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
    //AD0CON = 0x01;              //�ر�ת�����Է�ֹ���ֵĹܽ��໥Ӱ������add by dw   20090623
	
    temp <<= 2;
	temp &= 0x0300;
	value &= 0x00FF;
	temp |= value;
	return temp;
}
/*
*********************************************************************************************************
*                         �������У���
*
* Description: ��ȡflash�еĴ��룬����У���
*
* Arguments  : ��
*
* Returns    : ������������ȫ�ֱ���all_checksum��
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
//ι������
void watchdog(void) 
{
	EA = 0; 			//�ر��ж� 
	WFEED1 = 0xA5; 		//ִ�������һ���� 
	WFEED2 = 0x5A; 		//ִ������ڶ����� 
	EA = 1; 			//���ж�				
}

void delay(unsigned char time)
{
	unsigned char x;
	unsigned short y;
	for(x=0;x<time;x++)
	{
		watchdog();
		for(y=0;y<0x8FFF;y++);								//han
		//for(y=0;y<0x0700;y++); // 1����	  
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
