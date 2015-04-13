/*******************************Copyright (c)***********************************
 *
 *              Copyright (C), 1999-2011, Jointcom . Co., Ltd.
 *
 *------------------------------------------------------------------------------
 * @file	:rf_parameter.c
 * @author	:mashuai
 * @version	:v1.0 
 * @date	:2012.4.20
 * @brief	:
 *------------------------------------------------------------------------------
 * @attention
 ******************************************************************************/
#ifndef __RF_PARAMETER_C__
#define __RF_PARAMETER_C__
#endif
/* Includes ------------------------------------------------------------------*/
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  :Read the code from the irom,and calculate the chksum of the firmware
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL FirmwareVerify(void)
{
	BOOL result1 = TRUE,result2 = TRUE;
	U32 i,j,*src,*des;	
	
	for( i = BANK_CTRL_START_ADDR,j = BANK_CTRL_BAK_START_ADDR;
		 i < BANK_CTRL_END_ADDR; 
		 i+=4,j+=4 )
	{
		des = (U32*)i;
		src = (U32*)j;
		if ( *des != *src )
		{//rollback is failed
			result1 = FALSE;	
		}
	}
			
	for( i = BANK_CTRL_START_ADDR,j = BANK_CTRL_BUF_START_ADDR;
		 i < BANK_CTRL_END_ADDR; 
		 i+=4,j+=4 )
	{
		des = (U32*)i;
		src = (U32*)j;
		if ( *des != *src )
		{//upgrade is failed
			result2 = FALSE;	
		}
	}

	if ( result1 == TRUE || result2 == TRUE )
		return TRUE;
	else
		return FALSE;
}
/**
  * @brief  :watchdog init
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL FirmwareIsExist(void)
{
	BOOL result;
	U32 *addr,i;
		
	for ( i = BANK_CTRL_BAK_START_ADDR ; i < BANK_CTRL_BAK_END_ADDR ; i += 4 )
	{//the buffer is empty
		addr = (U32*)i;
		if ( *addr != (U32)-1 )break;			
	}

	if ( i == BANK_CTRL_BAK_END_ADDR )
	{//first running
		Section2Bak();
		NVIC_SystemReset();
	}
	else if ( modelUpgrade == SOFT_UPGRADE_WAIT )
	{//after the upgrade reset
		if ( (result = FirmwareVerify()) == FALSE )
		{
			Bak2Section();	//rollback
			NVIC_SystemReset();	
		}
		return result;
	}	

	return TRUE;	
}
/**
  * @brief  :Read the code from the irom,and calculate the chksum of the firmware
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
U16 CalcCheckSum(void)
{
	U8  *pFlash;	
	U16 checksum= 0;

	for (pFlash = (U8*)BANK_WORK_START_ADDR; pFlash < (U8*)(BANK_WORK_START_ADDR + 0x8000); pFlash++)
	{
		checksum += *pFlash;
	}
	
	return (checksum - 0x8000);
}
/**
  * @brief  :Read the code from the irom,and calculate the chksum of the firmware
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
U16 CalcUpgradeCheckSum(void)
{
	U8  *pFlash;	
	U16 checksum= 0;

	for (pFlash = (U8*)BANK_BUF_START_ADDR; pFlash < (U8*)BANK_BUF_END_ADDR; pFlash++)
	{
		checksum += *pFlash;
	}

	for (pFlash = (U8*)BANK_WORK_START_ADDR+SIZEOF_BUFFER; pFlash < (U8*)(BANK_WORK_START_ADDR + 0x8000); pFlash++)
	{
		checksum += *pFlash;
	}
	
	return (checksum - 0x8000);
}
/**
  * @brief  :Read the code from the irom,and calculate the chksum of the firmware
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
U16 CalcFirmwareAllCRC16(void)
{
	return CalcCRC16Value((U8*)BANK_WORK_START_ADDR,SIZEOF_BUFFER);	
}
/**
  * @brief  :Read the code from the irom,and calculate the chksum of the firmware
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
U16 CalcFirmwareBufCRC16(void)
{
	return CalcCRC16Value((U8*)BANK_BUF_START_ADDR,SIZEOF_BUFFER);	
}
/**
  * @brief  :
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void DownFlagSet(void)
{
	modelUpgrade = SOFT_UPGRADE_WAIT;
	WriteE2prom(EEPROM_ADDR_UPGRADE,&modelUpgrade,sizeof(modelUpgrade));	
}
/**
  * @brief  :Check the firmware whether that the firmware has been upgrade. 
  * @param  :None
  * @retval :BOOL
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL DownFlagCheck(void)
{
	BOOL result = FALSE;

	if ( modelUpgrade == SOFT_UPGRADE_WAIT )
	{
		result = TRUE;
		modelUpgrade = SOFT_NORMAL_WORK;
		WriteE2prom(EEPROM_ADDR_UPGRADE,(U8*)&modelUpgrade,sizeof(modelUpgrade));
	}			

	return result;
}
/**
  * @brief  :Check the firmware whether that the firmware has been upgrade. 
  * @param  :None
  * @retval :BOOL
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL RebootFlagCheck(void)
{
	BOOL result = modelReset,tmp;

	if ( modelReset == TRUE )
	{
		tmp = FALSE;
		WriteE2prom(EEPROM_ADDR_REBOOT ,(U8*)&tmp  ,sizeof(tmp));		
	}

	return result;
}
/**
  * @brief  :Check the firmware whether that the firmware has been upgrade. 
  * @param  :None
  * @retval :BOOL
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
BOOL RebootFlagSet(void)
{
	modelReset = TRUE;	
	return WriteE2prom(EEPROM_ADDR_REBOOT ,(U8*)&modelReset  ,sizeof(modelReset));
}
/**
  * @brief  :Initialize the parameters of the rf module.
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
void InitParam(void)
{
	U16 i;
	STABLE_COMMAND *sc = GetTable();
	
	ReadE2prom(EEPROM_ADDR_UPGRADE,(U8*)&modelUpgrade,sizeof(modelUpgrade));
	ReadE2prom(EEPROM_ADDR_REBOOT ,(U8*)&modelReset  ,sizeof(modelReset));
	
	for(i = 0; i < GetTableMebCnt();i++)
	{
		ReadE2prom(sc->sub,(U8*)&sc->var,sc->varLen);		
	}
}
/**
  * @brief  :parameter aask routine
  * @param  :None
  * @retval :None
  * @author	:mashuai
  * @version:v2.0
  * @date	:2011.10.20
  */
int TaskParameter(int*argv[],int argc)
{	
	return 0;
}
/********************************END OF FILE***********************************/
