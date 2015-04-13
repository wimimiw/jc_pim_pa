SET C51INC=C:\Program Files\Keil\C51\Inc;C:\Program Files\Keil\C51\Inc\Philips
SET C51LIB=C:\Program Files\Keil\C51\Lib
SET CPU_TYPE=P89LPC936
SET CPU_VENDOR=NXP (founded by Philips)
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x00B71B00
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\main.__i
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\timer.__i
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\BRO20W~1.__I
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\BRO20W~3.__I
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\BRO20W~2.__I
"C:\Program Files\Keil\C51\BIN\A51.EXE" @.\OUTPUT\START900._ia
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\I2C.__i
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\SPI.__i
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\e2prom.__i
"C:\Program Files\Keil\C51\BIN\C51.EXE" @.\OUTPUT\serial.__i
"C:\Program Files\Keil\C51\BIN\BL51.EXE" @.\OUTPUT\MY_PRO~1.LNP
"C:\Program Files\Keil\C51\BIN\OH51.EXE" ".\OUTPUT\my_project" 
.\output\conv_v2.exe .\output\my_project.hex -32 -R .\output\ZGRF_V30
