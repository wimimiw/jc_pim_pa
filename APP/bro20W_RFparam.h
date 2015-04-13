/**-------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: bro20w_rfparam.h
**��   ��   ��: ��ݺ��
**��   ��   ��: 
**����޸�����: 2006��3��31��
**�� �� ��  ��: V1.2
**��        ��: bro20w_rfparam ͷ�ļ�				
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ��ݺ�� 
** ��  ��: V1.2
** �ա���: 2006��3��31��
** �衡��: ԭʼ�汾
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

/*����LPC938 EEPROM �е�ֱ��վ������ַ*/
//EEPROM area define(512bytes=0x200) 
/****gain*****/
#define EE_AtteVal			0x0001		//1�ֽڣ������趨ֵ
#define EE_GainOffset		0x0002		//1�ֽڣ��������ֵ
#define EE_Att1				0x0003		//1�ֽڣ�1#˥����
#define	EE_Att2				0x0004		//1�ֽڣ�2#˥����
#define EE_GainMAX    		0x0005		//1�ֽڣ��������

#define EE_Att1slopeval    	0x0008		//2�ֽڣ�1��˥����б��
#define EE_Att1val    		0x000A		//2�ֽڣ�1��˥��������ֵ


/****PA******/
#define EE_gPALim			0x0020		//2�ֽڣ���������޷�ֵ
#define EE_gOutPwrVal		0x0022		//2�ֽڣ�������ʶ���ֵ
#define EE_gOutPwrSlopeVal	0x0024		//2�ֽڣ���������趨ֵ����
#define EE_gRefPwrVal	    0x0026		//2�ֽڣ����书�ʶ���
#define EE_gRefPwrSlopeVal	0x0028		//2�ֽڣ����书��б��ֵ
#define EE_gInPwrVal	    0x002e		//1�ֽڣ�������ʹر�����
#define EE_gPALimNumer	    0x0075		//1�ֽڣ���������޷���������
#define EE_gPReadADNumer	0x0076		//1�ֽڣ�������ʶ�ȡ��������
#define EE_gPequDenomi	    0x0077		//1�ֽڣ����ʲ�����ĸ
#define EE_InPwrShutHValue 	0x0130		//2�ֽ�, ���빦�ʹر�������ֵ

/**DAchannel**/
#define EE_DA_CHANNEL_A		0x0034	
#define EE_DA_CHANNEL_B		0x0036	
#define EE_DA_CHANNEL_C		0x0038	
#define EE_DA_CHANNEL_D		0x003A
/**Source Select**/
#define EE_SOURCE_SELECT	0x003C

/***frequency***/
#define	EE_CenFreq 			0x0040		//4�ֽڣ�����Ƶ��,����������
#define EE_BandWidth		0x0044		//4�ֽڣ�����
#define	EE_IFOffset			0x0048		//2�ֽڣ���Ƶ����ֵ
#define	EE_FreqStep			0x004A		//2�ֽڣ�Ƶ�ʲ���ֵ
#define EE_IFFreq			0x004C		//4�ֽڣ���ƵƵ��ֵ
#define	EE_Freq				0x0050		//4�ֽڣ�����Ƶ��,����������
#define	EE_RefFreq			0x0054		//4�ֽڣ��ο�Ƶ��

#define EE_gPBmFreq		    0x0078		//4�ֽڣ���׼Ƶ��

#define	EE_PLL2316R			0x0058		//4�ֽڣ�PLLR�Ĵ���
#define	EE_PLL2316N			0x005C		//4�ֽڣ�PLLN�Ĵ���
#define	EE_PLL2316F			0x0060		//4�ֽڣ�PLLF�Ĵ���

/*****current*****/
#define	EE_CurCo			0x0080		//2�ֽڣ�����б��
#define	EE_CurOffset		0x0082		//2�ֽڣ�����������ֵ
#define EE_CurHlim			0x0084		//2�ֽڣ����ŵ�������
#define EE_CurLlim			0x0086		//2�ֽڣ����ŵ�������
#define EE_PAIq				0x0088		//2�ֽڣ�����ƫѹֵ
#define EE_PAIqCo			0x008A		//2�ֽڣ�ƫѹ�²�ϵ��
#define EE_PAIqval			0x008C		//2�ֽڣ�ƫѹ�²�����ֵ

/*****temperature********/
#define EE_SAWFNumer		0x00A0		//1�ֽڣ������²�ϵ������
#define EE_SAWFDenomi		0x00A1		//1�ֽڣ������²�ϵ����ĸ
#define	EE_GainNumer		0x00A2		//1�ֽڣ������²�ϵ������
#define EE_GainDenomi		0x00A3		//1�ֽڣ������²�ϵ����ĸ
#define EE_PAProtectLim		0x00A4		//1�ֽڣ����ű����¶�����
#define EE_PAResetLim		0x00A5		//1�ֽڣ����Żָ��¶�����

/*****module*******/
#define	EE_RF_No			0x00C0		//1�ֽڣ���Ƶģ����
#define EE_PASW				0x00C1      //1�ֽڣ����ſ���
#define EE_RFSW				0x00C2      //1�ֽڣ���Ƶ����

/******debug maker********/
#define	EE_TEST_TEMP		0x00D0		//2�ֽڣ������¶�
#define	EE_TEST_MARK		0x00D2		//1�ֽڣ����õ���״̬
#define	EE_TEMP_VALUE   	0x00D3		//1�ֽڣ������¶Ȳ����Ļ�׼ֵ
#define	EE_BaseTemp_VALUE 	0x00D4		//1�ֽڣ����������¶Ȳ������¶Ȼ�׼ֵ

/******version**********/
#define	EE_VERSION			0x00E0		//2�ֽڣ�����汾��
#define	EE_ALL_CHECKSUM		0x00E2		//2�ֽڣ�����У���
#define	EE_MODULE_No		0x00E4		//10�ֽڣ�ģ�����к�

#define EE_PLUS_TRIGER 			0x0100		//�������������ź� 
#define EE_TEMPER_BUCHANG		0x0110		//�²�����

#define EE_DOWN_FLAG        0x01FF		//1�ֽڣ�������ر�־

