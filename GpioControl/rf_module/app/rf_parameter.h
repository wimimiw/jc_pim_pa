/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2015, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_parameter.h
 * @author	:mashuai
 * @version	:v2.0 
 * @date	:2011.10.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __RF_PARAMETER_H__
#define __RF_PARAMETER_H__

#ifndef __RF_PARAMETER_C__
#define	__RF_PARAMETER_EXT__ 	 extern
#else
#define	__RF_PARAMETER_EXT__
#endif
/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/****功率校准宏定义******************************************/    //by  2010.8.29
/* 参数类型定义         */
#define PARAM_RTE	        0x00	 //读临时变量或EEPROM
#define PARAM_WTE			0x01	 //写临时变量和EEPROM
#define PARAM_RT			0x02	 //读临时变量
#define PARAM_WT			0x03	 //写临时变量

/* 软件升级状态         */
#define SOFT_NORMAL_WORK	0x11
#define SOFT_UPGRADE_WAIT	0x12
#define SOFT_UPGRADE_ACK	0x13

#define EEPROM_ADDR_UPGRADE	0x00FF
#define EEPROM_ADDR_REBOOT	0x00FE

#define VERSION_GPIO_CONTROL  "3.X"
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
__RF_PARAMETER_EXT__ U8 modelAddr;
__RF_PARAMETER_EXT__ U16 modelFwCrc; 
__RF_PARAMETER_EXT__ BOOL modelReset; 
__RF_PARAMETER_EXT__ BOOL modelUpgrade; 
/* Public function prototypes ------------------------------------------------*/
__RF_PARAMETER_EXT__ int TaskParameter(int*argv[],int argc);
__RF_PARAMETER_EXT__ void InitParam(void);
__RF_PARAMETER_EXT__ void DownFlagSet(void);
__RF_PARAMETER_EXT__ BOOL DownFlagCheck(void);
__RF_PARAMETER_EXT__ BOOL RebootFlagSet(void);
__RF_PARAMETER_EXT__ BOOL RebootFlagCheck(void);
__RF_PARAMETER_EXT__ U16 CalcCheckSum(void);
__RF_PARAMETER_EXT__ U16 CalcUpgradeCheckSum(void);
__RF_PARAMETER_EXT__ U16 CalcFirmwareAllCRC16(void);
__RF_PARAMETER_EXT__ U16 CalcFirmwareBufCRC16(void);
__RF_PARAMETER_EXT__ BOOL FirmwareVerify(void);
__RF_PARAMETER_EXT__ BOOL FirmwareIsExist(void);

#endif/*__RF_PARAMETER_H__*/
/********************************END OF FILE***********************************/
