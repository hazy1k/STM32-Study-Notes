【*】程序简介
-工程名称：PVD电源监控
-实验平台: 野火STM32 指南者 开发板 
-MDK版本：5.16
-ST固件库版本：3.50
【 ！】功能简介：
使用PVD电源监控功能
【 ！！】注意事项：
使用可调电源给实验板供电，其它电源线都拔掉(包括下载器、USB线)
由于直接接排针供电没有电路保护，调节电源时小心不要使供电电压远高于5V,电压太高会烧坏实验板！！
【 ！】实验操作：
 1.使用外部可调电源，调节成5V输出，连接到实验板引出的 5V<--->GND排针给板子供电；
 2.复位实验板，电压正常时板子上的LED彩灯应为绿色
 3.向下调节可调电源的电压，大约当降至4.2V的时候，LED彩灯会转为红色。
   （程序中控制PVD监控电压约为2.6V,当5V电源输入降至4.2V的时候，连接STM32的VDD电源(3.3V电源线)会低于2.6V，产生PVD事件，在中断中控制亮红灯）