/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: bro20w_rfparam.h
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述: bro20w_rfparam 头文件				
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
********************************************************************************************************/

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif

/*存于LPC938 EEPROM 中的直放站参数地址*/
//EEPROM area define(512bytes=0x200) 
/****gain*****/
#define EE_AtteVal			0x0001		//1字节，增益设定值
#define EE_GainOffset		0x0002		//1字节，增益调整值
#define EE_Att1				0x0003		//1字节，1#衰减器
#define	EE_Att2				0x0004		//1字节，2#衰减器
#define EE_GainMAX    		0x0005		//1字节，最大增益

#define EE_Att1slopeval    	0x0008		//2字节，1＃衰减器斜率
#define EE_Att1val    		0x000A		//2字节，1＃衰减器定标值


/****PA******/
#define EE_gPALim			0x0020		//2字节，输出功率限幅值
#define EE_gOutPwrVal		0x0022		//2字节，输出功率定标值
#define EE_gOutPwrSlopeVal	0x0024		//2字节，输出功率设定值定标
#define EE_gRefPwrVal	    0x0026		//2字节，反射功率定标
#define EE_gRefPwrSlopeVal	0x0028		//2字节，反射功率斜率值
#define EE_gInPwrVal	    0x002e		//1字节，输出功率关闭门限
#define EE_gPALimNumer	    0x0075		//1字节，输出功率限幅补偿分子
#define EE_gPReadADNumer	0x0076		//1字节，输出功率读取补偿分子
#define EE_gPequDenomi	    0x0077		//1字节，功率补偿分母
#define EE_InPwrShutHValue 	0x0130		//2字节, 输入功率关闭上门限值

/**DAchannel**/
#define EE_DA_CHANNEL_A		0x0034	
#define EE_DA_CHANNEL_B		0x0036	
#define EE_DA_CHANNEL_C		0x0038	
#define EE_DA_CHANNEL_D		0x003A
/**Source Select**/
#define EE_SOURCE_SELECT	0x003C

/***frequency***/
#define	EE_CenFreq 			0x0040		//4字节，中心频率,长整型数据
#define EE_BandWidth		0x0044		//4字节，带宽
#define	EE_IFOffset			0x0048		//2字节，中频调整值
#define	EE_FreqStep			0x004A		//2字节，频率步进值
#define EE_IFFreq			0x004C		//4字节，中频频率值
#define	EE_Freq				0x0050		//4字节，本振频率,长整型数据
#define	EE_RefFreq			0x0054		//4字节，参考频率

#define EE_gPBmFreq		    0x0078		//4字节，基准频率

#define	EE_PLL2316R			0x0058		//4字节，PLLR寄存器
#define	EE_PLL2316N			0x005C		//4字节，PLLN寄存器
#define	EE_PLL2316F			0x0060		//4字节，PLLF寄存器

/*****current*****/
#define	EE_CurCo			0x0080		//2字节，电流斜率
#define	EE_CurOffset		0x0082		//2字节，电流检测调整值
#define EE_CurHlim			0x0084		//2字节，功放电流上限
#define EE_CurLlim			0x0086		//2字节，功放电流下限
#define EE_PAIq				0x0088		//2字节，功放偏压值
#define EE_PAIqCo			0x008A		//2字节，偏压温补系数
#define EE_PAIqval			0x008C		//2字节，偏压温补调整值

/*****temperature********/
#define EE_SAWFNumer		0x00A0		//1字节，声表温补系数分子
#define EE_SAWFDenomi		0x00A1		//1字节，声表温补系数分母
#define	EE_GainNumer		0x00A2		//1字节，增益温补系数分子
#define EE_GainDenomi		0x00A3		//1字节，增益温补系数分母
#define EE_PAProtectLim		0x00A4		//1字节，功放保护温度门限
#define EE_PAResetLim		0x00A5		//1字节，功放恢复温度门限

/*****module*******/
#define	EE_RF_No			0x00C0		//1字节，射频模块编号
#define EE_PASW				0x00C1      //1字节，功放开关
#define EE_RFSW				0x00C2      //1字节，射频开关

/******debug maker********/
#define	EE_TEST_TEMP		0x00D0		//2字节，设置温度
#define	EE_TEST_MARK		0x00D2		//1字节，设置调试状态
#define	EE_TEMP_VALUE   	0x00D3		//1字节，设置温度补偿的基准值
#define	EE_BaseTemp_VALUE 	0x00D4		//1字节，上行设置温度补偿的温度基准值

/******version**********/
#define	EE_VERSION			0x00E0		//2字节，程序版本号
#define	EE_ALL_CHECKSUM		0x00E2		//2字节，程序校验和
#define	EE_MODULE_No		0x00E4		//10字节，模块序列号

#define EE_PLUS_TRIGER 			0x0100		//触发单次脉冲信号 
#define EE_TEMPER_BUCHANG		0x0110		//温补命令

#define EE_DOWN_FLAG        0x01FF		//1字节，软件下载标志

