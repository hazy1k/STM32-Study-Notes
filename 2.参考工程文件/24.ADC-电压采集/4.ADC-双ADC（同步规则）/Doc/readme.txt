【*】程序简介	
-工程名称：ADC-双ADC（同步规则）
-实验平台: 野火STM32-F103 指南者 开发板 
-ST固件库版本：3.5.0

【 ！】功能简介：
使用双ADC多通道DMA模式测量电压

【 ！！】注意事项：

 跳线帽：使用跳线帽连接 板子右侧的VR<--->PC1排针，调节电位器可以改变电压

 外接电压范围为0~3.3V，超出范围可能导致烧坏
 
【 ！】实验操作：
电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
复位开发板，即可接收STM32串口发送给电脑的数据。

调节电位器可以改变PC1的电压
	  
【*】 引脚分配

ADC：
	VR<--->PC1(通过跳线帽连接)
	PC4 悬空，可用杜邦线接3V3或者GND来实验
	
串口（TTL-USB TO UART）：
CH340的收发引脚与STM32的发收引脚相连。
	CH340  RXD  <----->  USART1  TX  (PA9)
	CH340  TXD  <----->  USART1  RX  (PA10)

【&】程序描述：
	  
       <bsp_adc.c>:
		
		1.初始化ADC GPIO； 
		2. 初始化ADC工作参数；
		3. 配置DMA工作参数； 
		4. 读取ADC采集的数据；
		
	ADCx_Mode_Config() 与独立模式多通道配置基本一样，只是有几点需要注意： ADC工作模式要设置为同步规则模式；
	两个ADC的通道的采样时间需要一致；ADC1设置为软件触发；ADC2 设置为外部触发。其他的基本一样，看代码注释理解即可。