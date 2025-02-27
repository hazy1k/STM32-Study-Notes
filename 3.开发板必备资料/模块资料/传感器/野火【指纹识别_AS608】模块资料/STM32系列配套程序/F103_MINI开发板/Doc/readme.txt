/*********************************************************************************************/

【*】程序简介
-工程名称：指纹模块实验
-实验平台：野火STM32 F103 MINI 开发板
-MDK版本： 5.23
-ST固件库版本：1.5.1

【 ！】功能简介：

指纹模块采用杭州晟元公司AS608高性能指纹识别模块，STM32与AS608 SOC采用USART串口通信，通信遵守AS608通信协议格式。

本实验可以完成录指纹，指纹识别，删除制定用户指纹以及清空指纹库的功能。

使用串口调试助手向STM32发送命令，STM32通过解析调试助手发送的命令，向指纹模块发送控制指令，而后等待指纹模块向STM32反馈的处理信息，STM32接收到反馈信息后，通过指令解析向串行助手显示指纹模块的处理结果。

该实验使用了2个中断，中断1使用串口接受收中断，采用环形缓冲方式缓存来自指纹模块反馈的信息。中断2通过EXTI中断方式，通过指纹模块的wak线电压变化检测有无手指按下，发现有手指按下指示灯亮，反之则灭。


【 ！！】注意事项：

本实验STM32的指令输入调用了scanf函数，由于scanf函数调用的特性，须在串口调试助手输完指令数字后按回车键后再发送指令。

/*********************************************************************************************/

【*】 引脚分配

USART2：

   指纹模块Tx<--->PA3(Rx)
   指纹模块Rx<--->PA2(Tx)    
	  
LED：
RGB彩灯的三个阴极接到STM32三个引脚上，LED低电平亮。

	R<--->PC2
	G<--->PC3
	

wak线：
感应信号输出，默认高电平有效。  
    
  wak<--->PA8

/*********************************************************************************************/

【*】 时钟

A.晶振：
-外部高速晶振：25MHz
-RTC晶振：32.768KHz

B.各总线运行时钟：
-系统时钟 = SYCCLK = AHB1 = 180MHz
-APB2 = 90MHz 
-APB1 = 45MHz

C.浮点运算单元：
  不使用

/*********************************************************************************************/

【*】 版本

-程序版本：1.0
-发布日期：2013-xx

-版本更新说明：首次发布

/*********************************************************************************************/

【*】 联系我们

-野火论坛    :http://www.firebbs.cn
-淘宝店铺    :https://fire-stm32.taobao.com

/*********************************************************************************************/