
本例程演示如何使用LCD测试WF-ESP8266 WiFi模块

通过LCD配置模块，并与外界进行数据传输

引脚连接：
UTXD  --- A3
URXD  --- A2
VCC   --- 3V3
GND   --- GND
CH_PD --- A0
RST   --- A1



bsp_illi9341_lcd.h:

#define COLUMM_WINDOW 320

#define PAGE          112 //240-32*4=112


