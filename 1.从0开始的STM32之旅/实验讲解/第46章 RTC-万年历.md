# 第四十六章 RTC-万年历

## 1. 硬件设计

开发板中与RTC相关的硬件设计见图 [RTC硬件电路](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/RTC.html#id12) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/RTC004.jpg)

原理图中的右上角是备份域的供电电路，在本开发板中提供了一个钮扣电池插槽，可以接入型号为CR1220的钮扣电池，该型号的钮扣电池电压为3.2V， 图中的BAT54C双向二极管可切换输入到STM32备份域电源引脚VBAT的供电，当主电源正常供电时，由稳压器输出的3.3V供电，当主电源掉电时，由钮扣电池供电 。

原理图下方的是本开发板采用的LSE晶振电路，此处使用的晶振频率为32.768KHz，RTC外设可以使用LSE作为时钟，把它进行分频得到1Hz的RTC计时时钟。

本实验默认使用LSI内部时钟，使用内部时钟时，即使安装了钮扣电池，主电源掉电后时间是不会继续走的，只会保留上次断电的时间。 若要持续运行，需要修改bsp_rtc.h文件，使用RTC_CLOCK_SOURCE_LSE宏，切换成使用LSE外部时钟。

## 2. 软件设计

### 2.1 编程目标

1. 初始化RTC外设；

2. 设置时间以及添加配置标志；

3. 获取当前时间；

### 2.2 代码分析


