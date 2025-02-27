/*********************************************************************************************/
本文档使用 TAB = 4 对齐，使用keil5默认配置打开阅读比较方便。

【*】程序简介

-工程名称：fatfs文件系统常用功能演示
-实验平台: 野火STM32 F103-指南者 开发板 


【 ！】功能简介：
演示文件系统的各种应用

【 ！！】注意事项：


FLASH芯片出厂前已有烧录数据，具体见下文FLASH使用情况说明。

【 ！】实验操作：
电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
复位开发板，即可接收STM32串口发送给电脑的调试信息。

/***************************************************************************************************************/

【 ！】外部Flash使用情况说明（W25Q64）	

|-------------------------------------------------------------------------------------------------------------------|												
|序号	|文件名/工程					|功能										|起始地址		|长度				|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|1		|外部flash读写例程			|预留给裸机Flash测试							|0				|1024 (BYTE)		|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|2		|裸机触摸屏例程				|裸机触摸校准参数							|1024			|10 (BYTE)			|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|3		|裸机中文显示例程			|裸机中文字库（HZLIB.bin）					|4096			|53*4096 (212KB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|4		|app.c						|XBF字库文件（emWin使用,songti.xbf）			|60*4096		|317*4096 (1.23MB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|5		|外部flash读写例程			|文件系统中文支持字库(emWin使用,UNIGBK.BIN)	|465*4096		|43*4096 (172KB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|6		|Touch_CalibrationApp.c		|电阻屏触摸校准参数（emWin使用）				|510*4096		|34 (BYTE)			|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|
|7		|外部flash读写例程			|文件系统中文支持字库(emWin使用,UNIGBK.BIN)	|512*4096		|1536*4096 (6MB)	|
|-------|---------------------------|-------------------------------------------|---------------|-------------------|

 

*FLASH芯片从编号512-1536扇区是专门预留给文件系统的，其位置及容量为FLASH芯片的 后6MB。
 若用户有特殊需求，可修改文件系统的格式化接口，通过它设置文件系统的大小，
 fatfs_flash_spi.c的TM_FATFS_FLASH_SPI_disk_ioctl函数。
 
 但可能会导致FLASH芯片其它数据毁坏，做其它用到这些数据的实验时需要重新给FLASH写入这些数据。
  可使用配套例程中的“刷外部FLASH程序”给FLASH恢复出厂数据。
 
/*******************************************************************************************************************/
【*】 引脚分配

FLASH(W25Q64)：
FLASH芯片的SPI接口与STM32的SPI3相连。
		SCK	<--->PA5
		MISO<--->PA6
		MOSI<--->PA7
		CS	<--->PC0

串口(TTL-USB TO USART)：
CH340的收发引脚与STM32的发收引脚相连。
	RX<--->PA9
	TX<--->PA10		
													
/*********************************************************************************************/

【*】 程序描述

	<main.c>
		
			1.设备信息获取
			2.文件定位和格式化写入功能测试
			3.目录创建和重命名功能测试
			4.文件信息获取
			5.文件扫描测试
/*********************************************************************************************/

【*】 联系我们

-野火官网  :https://embedfire.com
-野火论坛  :http://www.firebbs.cn
-野火商城  :https://yehuosm.tmall.com/
-野火资料下载中心：http://doc.embedfire.com/products/link

/*********************************************************************************************/