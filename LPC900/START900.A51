/**-------------文件信息--------------------------------------------------------------------------------
**文   件   名: STAR900.A51
**创   建   人: 程莺红
**修   改   人: 
**最后修改日期: 2006年3月31日
**最 新 版  本: V1.2
**描        述: 启动代码
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 程莺红 
** 版  本: V1.2
** 日　期: 2006年3月31日
** 描　述: 原始版本
**
********************************************************************************************************/
                NAME    ?C_STARTUP

?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

                RSEG    ?STACK
                DS      1
									 
                EXTRN CODE (?C_START)
                PUBLIC  ?C_STARTUP

                CSEG    AT      0
?C_STARTUP:     JMP     STARTUP1
				CSEG	AT 0040H
MYTABLE:		DB	0xF0
				DB 	0xD8
				DB	0x80
				DB	0xA8
				DB	0x5F
				DB	0xE0
				DB	0x87
				DB	0xC9
				DB	0x01
				DB	0x0F

                RSEG    ?C_C51STARTUP

STARTUP1:

                MOV     SP,#0xB0
                JMP     ?C_START

                END
