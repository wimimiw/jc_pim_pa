/*H**************************************************************************
* NAME:         flash_api.h         
*----------------------------------------------------------------------------
* Copyright (c) 2002 Atmel.
*----------------------------------------------------------------------------
* RELEASE:      rd2-fa-uart-1_0_0      
* REVISION:     1.2     
*----------------------------------------------------------------------------
* PURPOSE: Header file for C flash drivers
*****************************************************************************/
#ifndef _FLASH_API_H_
#define _FLASH_API_H_

/*_____ C O N F I G U R A T I O N ____________________________________________*/

//#define  __API_FCT_SET_1 
#define  __API_FCT_SET_2 
//#define  __API_FCT_SET_3

//#define  __API_WR_CODE_BYTE
//#define  __API_WR_CODE_PAGE


//#define __API_START_BOOTLOADER

typedef enum {	
		NO_SECURITY = 0x10,
		WR_SECURITY = 0x00,
		RD_WR_SECURITY = 0x01
	} ssb_t;

typedef enum {
		BLOCK_0 = 0x00,
    BLOCK_1 = 0x2000,
    BLOCK_2 = 0x4000
 } block_t;

typedef enum {	
		EEPROM_NOT_BUSY, 
		EEPROM_BUSY     }eeprom_t;



#ifdef __API_FCT_SET_1
	Uchar __api_fct_set_1 (Uchar _R1, Uint16 _DPTR)small;

  #define __api_rd_manufacturer()	__api_fct_set_1(0, 0)
	#define __api_rd_device_id1()		__api_fct_set_1(0, 1)
	#define __api_rd_device_id2()		__api_fct_set_1(0, 2)
	#define __api_rd_device_id3()		__api_fct_set_1(0, 3)
	#define __api_erase_SBV()				__api_fct_set_1(4, 0)

	#define __api_erase_block(BLOCK)	__api_fct_set_1(1, BLOCK)

	#define __api_wr_SSB(SSB)	__api_fct_set_1(5, SSB)

	#define __api_wr_SSB_RD_WR_SECURITY() 	__api_wr_SSB(RD_WR_SECURITY)
	#define __api_wr_SSB_WR_SECURITY()			__api_wr_SSB(WR_SECURITY)
	#define __api_wr_SSB_NO_SECURITY()			__api_wr_SSB(NO_SECURITY)

	#define __api_rd_SSB()		__api_fct_set_1(7, 0)
	#define __api_rd_BSB()		__api_fct_set_1(7, 1)
	#define __api_rd_SBV()		__api_fct_set_1(7, 2)
	#define __api_rd_EB()		  __api_fct_set_1(7, 6)
	#define __api_rd_HSB()		__api_fct_set_1(0x0B, 0)
	#define __api_rd_bootloader_version()	__api_fct_set_1(0x0F, 0)

#endif


#ifdef __API_FCT_SET_2
	Uchar __api_fct_set_2 (Uchar _ACC, Uchar _DPL)small;
	
	#define __api_wr_BSB(BSB)				__api_fct_set_2(BSB, 0)
	#define __api_wr_SBV(SBV)				__api_fct_set_2(SBV, 1)
	#define __api_wr_EB(EB)					__api_fct_set_2(EB, 6)
						
#endif

#ifdef __API_FCT_SET_3
	Uchar __api_fct_set_3 (Uchar _ACC, Uchar _DPL)small;
	
	#define __api_set_X2()				__api_fct_set_3(0,8)
	#define __api_clr_X2()				__api_fct_set_3(1,8)
	#define __api_set_BLJB()			__api_fct_set_3(0,4)
	#define __api_clr_BLJB()			__api_fct_set_3(1,4)
						
#endif


#define __api_rd_code_byte(address)  (*((Uchar code*) (address)))

#ifdef  __API_WR_CODE_BYTE
	Uchar	__api_wr_code_byte	(Uint16 , Uchar)small;
#endif

#ifdef  __API_WR_CODE_PAGE
	Uchar	__api_wr_code_page	(Uint16 , Uint16, Uchar)small;
#endif


#ifdef 	__API_START_BOOTLOADER
 void __api_start_bootloader();
#endif

#endif /* _FLASH_API_H_ */

