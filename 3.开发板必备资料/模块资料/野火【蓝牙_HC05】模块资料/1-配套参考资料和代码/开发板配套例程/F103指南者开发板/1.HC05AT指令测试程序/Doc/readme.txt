HC05  AT指令测试实验

注：
1. 取消注释 bsp_hc05.h 文件里面 ENABLE_LCD_DISPLAY 宏，可以切换到带LCD液晶显示的模式。
2. 将 bsp_hc05.h 文件里面 HC05_DEBUG_ON 宏定义设置为 1，可以让程序输出详细的调试信息，方便了解程序底层和调试。


连接好HC05蓝牙模块，如果板子上面有 EBF-Module 模块接口，可以直接插到上面去，注意下电源正负极模块不要插反了。

下载好程序后，打开串口调试助手，发送“AT”，测试 HC05 是否准备好，如果正常会收到回应“OK”。
使用例程，发送的AT指令都要大写，不需要回车换行。

更多AT指令请参考模块资料

更多信息请查阅模块用户手册或者模块例程说明文档



【*】 引脚分配
HC05与开发板的连接线，板子上没有 EBF-Module 模块接口的话使用杜邦线连接：

    蓝牙HC05        开发板
    HC05_TXD  <---> PA3     //串口2接收引脚
    HC05_RXD  <---> PA2     //串口2发送引脚
    HC05_KEY  <---> PB14    //普通GPIO、输出
    HC05_INT  <---> PB13    //普通GPIO、输入
    
    HC05_VCC  <---> 接5V或3.3V
    HC05_GND  <---> 接地线

调试串口(TTL-USB TO USART)：
CH340的收发引脚与STM32的发收引脚相连。

    RX<--->PA9
    TX<--->PA10



/*********************************************************************************************/

【*】 联系我们

-野火官网  :https://embedfire.com
-野火论坛  :http://www.firebbs.cn
-野火商城  :https://yehuosm.tmall.com/
-野火资料下载中心：http://doc.embedfire.com/products/link
-Github主页：https://github.com/Embedfire
-Gitee主页： https://gitee.com/Embedfire
-邮箱： embedfire@embedfire.com
-电话： 0769-33894118

/*********************************************************************************************/
