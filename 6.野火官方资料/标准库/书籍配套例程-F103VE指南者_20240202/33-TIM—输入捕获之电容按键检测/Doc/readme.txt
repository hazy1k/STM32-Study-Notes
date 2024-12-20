/*********************************************************************************************/
【*】程序简介

-工程名称：TIM—输入捕获（电容按键）
-实验平台: 野火STM32-F103 指南者 开发板 
-ST固件库版本：3.5.0

【 ！】功能简介：
使用定时器实现 电容按键 功能

学习目的：学会定时器输入捕获功能，了解电容按键的原理

【 ！！】注意事项：
使用电容按键需要把电容按键的TPAD(J19)<--->PA1（J18）、BEEP(J21)<--->PA8（J20)跳线帽连接起来。(出厂默认已连接)
	PA1即通用定时器TIM5 的通道 2
 
【 ！】实验操作：

      1.把编译好的程序下载到开发板上
      2.打开【多功能调试助手】，并配置好相关的参数      配置模式为 115200 8-N-1
      3.打开串口
      4.按下板子的RESET按键
	  
	  触摸电容按键，蜂鸣器会响
	  
/*********************************************************************************************/

【*】 引脚分配

电容按键：
电容按键的金属板引出与定时器的通道相连
	TPAD(J19)<--->PA1（J18）
	
串口（TTL-USB TO UART）：
CH340的收发引脚与STM32的发收引脚相连。
	CH340  RXD  <----->  USART1  TX  (PA9)
	CH340  TXD  <----->  USART1  RX  (PA10)	

BEEP蜂鸣器：
	BEEP(J21)	<--->PA8（J20)  高电平响
	
【*】程序描述
	
	<bsp_tpad.c>
		
		1.输入捕获通道GPIO初始化
		2.时基结构体初始化、输入捕获结构体初始化
		3.复位电容按键
		4.获取定时器捕获值
		5.初始化触摸按键，获得空载的时候触摸按键的充电时间
		6.读取若干次定时器捕获值，并返回最大值
		7.按键扫描函数
		
/*********************************************************************************************/

【*】 联系我们

-野火官网  :https://embedfire.com
-野火论坛  :http://www.firebbs.cn
-野火商城  :https://yehuosm.tmall.com/
-野火资料下载中心 :http://doc.embedfire.com/products/link

/*********************************************************************************************/


