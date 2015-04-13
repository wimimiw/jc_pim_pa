/*C**************************************************************************
* NAME:         flash_api.c
*----------------------------------------------------------------------------
* Copyright (c) 2002 Atmel.
*----------------------------------------------------------------------------
* RELEASE:      rd2-fa-uart-1_0_0      
* REVISION:     1.2     
*----------------------------------------------------------------------------
* PURPOSE: 
* Read/Write flash
*****************************************************************************/

/*_____ I N C L U D E - F I L E S ____________________________________________*/
#include "flash_config.h"
#include <reg931.h>

/*_____ G L O B A L S ________________________________________________________*/


/*_____ P R I V A T E - F U N C T I O N S - D E C L A R A T I O N ____________*/


/*_____ L O C A L S __________________________________________________________*/

#define MSK_AUXR1_ENBOOT	0x20
#define MSK_AUXR_M0				0x20
#define MAP_BOOT 			AUXR1 |= MSK_AUXR1_ENBOOT;
#define UNMAP_BOOT		AUXR1 &= ~MSK_AUXR1_ENBOOT;


/*_____ EXTERNAL - F U N C T I O N S - D E C L A R A T I O N ____________*/

extern void ASM_MOV_R1_A(void);
extern void __API_FLASH_ENTRY_POINT(void);
#define __API_JMP_BOOTLOADER (*((const void(code*)(void)) 0xF800 ))

/*F**************************************************************************
* NAME: __api_wr_code_byte 
*----------------------------------------------------------------------------
* PARAMS:  
* 		Uint16 address : address to program   
* 		Uchar value : data to write
* 		Uchar return : 
*             return  = 0x00 -> pass                            
*             return != 0x00 -> fail
*----------------------------------------------------------------------------
* PURPOSE: 
*  Program data byte in Flash memory 
*****************************************************************************/
#ifdef __API_WR_CODE_BYTE
Uchar __api_wr_code_byte (Uint16 address, Uchar value) small
{
  bit ea_save;

  ea_save = EA;
  EA = 0;
  DPTR = address;    
  ACC = 0x02;
  ASM_MOV_R1_A();
  ACC = value;  
	MAP_BOOT;
  __API_FLASH_ENTRY_POINT();
	UNMAP_BOOT;
  EA = ea_save;    	// restore interrupt state
  return (ACC); 

}
#endif

/*F**************************************************************************
* NAME: __api_wr_code_page 
*----------------------------------------------------------------------------
* PARAMS:  
* 		Uint16 add_flash : address of the first byte to program in the Flash
* 		Uint16 add_xram  : address in XRAM of the first data to program
* 		Uchar nb_data : number of bytes to program
*			Uchar return : 
*           return = 0x00 -> pass                           
*           return != 0x00 -> fail 
*----------------------------------------------------------------------------
* PURPOSE: 
*  Program until 128 Datas in Flash memory.
* Number of bytes to program is limited such as the Flash write remains in a
* single 128 bytes page.               
*****************************************************************************/
#ifdef __API_WR_CODE_PAGE
Uchar __api_wr_code_page (Uint16 add_flash, Uint16 add_xram, Uchar nb_data) small
{
	Uchar save_auxr1;

  bit ea_save;

  ea_save = EA;
  EA = 0;
	save_auxr1 = AUXR1;
	
  AUXR1 &= ~0x01;        	// Set DPTR	=	DPTR0
  DPTR = add_flash;
  AUXR1++;        				// DPTR = DPTR1
  DPTR = add_xram;
  ACC = 0x09;
  ASM_MOV_R1_A();
  ACC = nb_data;
	AUXR1 &= ~0x01;        	// Set DPTR = DPTR0
	MAP_BOOT
  __API_FLASH_ENTRY_POINT();
	UNMAP_BOOT;
  AUXR1 = save_auxr1;        		
  EA = ea_save;    	// restore interrupt state
return (ACC);

}
#endif


/*F**************************************************************************
* NAME: __api_fct_set_1 
*----------------------------------------------------------------------------
* PARAMS:  
* 		Uchar _R1 : 
* 		Uint16 _DPTR : 
*			Uchar return : 
*----------------------------------------------------------------------------
* PURPOSE: 
* Common function for API access in the bootloader
*****************************************************************************/
#ifdef __API_FCT_SET_1
Uchar __api_fct_set_1 (Uchar _R1, Uint16 _DPTR) small
{
  bit ea_save;

  ea_save = EA;
  EA = 0;
  DPTR 	= _DPTR;
  ACC 	= _R1;
  ASM_MOV_R1_A();
	MAP_BOOT;
  __API_FLASH_ENTRY_POINT();
	UNMAP_BOOT;
  EA = ea_save;    	// restore interrupt state
  return (ACC);

}
#endif


/*F**************************************************************************
* NAME: __api_fct_set_2
*----------------------------------------------------------------------------
* PARAMS:  
* 		Uchar _ACC : 
* 		Uchar _DPL : 
*			Uchar return : 
*----------------------------------------------------------------------------
* PURPOSE: 
* Common function for API access in the bootloader
*****************************************************************************/
#ifdef __API_FCT_SET_2
Uchar __api_fct_set_2 (Uchar _ACC, Uchar _DPL) small
{
  bit ea_save;

  ea_save = EA;
  EA = 0;
  DPH = 0x00;
  DPL = _DPL;
  ACC = 0x06;
  ASM_MOV_R1_A();
  ACC = _ACC;
	MAP_BOOT;
  __API_FLASH_ENTRY_POINT();
	UNMAP_BOOT;
  EA = ea_save;    	// restore interrupt state
  return 1;
}
#endif

/*F**************************************************************************
* NAME: __api_fct_set_3
*----------------------------------------------------------------------------
* PARAMS:  
* 		Uchar _ACC : 
* 		Uchar _DPL : 
*			Uchar return : 
*----------------------------------------------------------------------------
* PURPOSE: 
* Common function for API access in the bootloader
*****************************************************************************/
#ifdef __API_FCT_SET_3
Uchar __api_fct_set_3 (Uchar _ACC, Uchar _DPL) small
{
  bit ea_save;

  ea_save = EA;
  EA = 0;
  DPH = 0x00;
  DPL = _DPL;
  ACC = 0x0A;
  ASM_MOV_R1_A();
  ACC = _ACC;
	MAP_BOOT;
  __API_FLASH_ENTRY_POINT();
	UNMAP_BOOT;
  EA = ea_save;    	// restore interrupt state
  return 1;
}
#endif


/*F**************************************************************************
* NAME: api_start_bootloader                                            
*----------------------------------------------------------------------------
* PARAMS:  
* return: 
*----------------------------------------------------------------------------
* PURPOSE: 
*****************************************************************************
* NOTE: 
* To use this function the constante __API_START_BOOTLOADER must be define in
* C header file.
*****************************************************************************/
#ifdef __API_START_BOOTLOADER
void __api_start_bootloader (void)
{
	__api_wr_BSB (0xFF);
	__api_wr_SBV (0xFC);
	// stop application peripherals 
	T2CON=0x00;
	TL2=0x00;
	TH2=0x00;
	BDRCON=0x00;
	TCON=0x00;
	SCON = 0x00;
	TMOD = 0x00;
	PCON = 0x00;
	TCON = 0x00;
	EA = 0;
	MAP_BOOT;
	__API_JMP_BOOTLOADER();
}
#endif























































































