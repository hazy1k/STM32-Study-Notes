【*】程序简介
-工程名称：ChipID
-实验平台: 野火STM32 指南者 开发板 
-MDK版本：5.16
-ST固件库版本：3.50
【 ！】功能简介：
读取STM32芯片的独立ID，每一块STM32芯片都有独立的ID，可以用于区分芯片、加密等应用
【 ！】实验操作：
电脑端使用串口调试助手，选择电脑与STM32相连的COM口，设置为115200-N-8-1，
下载程序并复位开发板，即可接收STM32串口发送给电脑的数据。