# 第二十七章 LCD-液晶显示介绍

## 1. LED和OLED显示器简介

LED点阵彩色显示器的单个像素点内包含红绿蓝三色LED灯，显示原理类似我们实验板上的LED彩灯， 通过控制红绿蓝颜色的强度进行混色，实现全彩颜色输出，多个像素点构成一个屏幕。由于每个像素点都是LED灯自发光的， 所以在户外白天也显示得非常清晰，但由于LED灯体积较大，导致屏幕的像素密度低，所以它一般只适合用于广场上的巨型显示器。 相对来说，单色的LED点阵显示器应用得更广泛，如公交车上的信息展示牌、店招等，见图 [LED点阵彩屏有LED单色显示屏](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#ledled) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD004.jpg)

新一代的OLED显示器与LED点阵彩色显示器的原理类似， 但由于它采用的像素单元是“有机发光二极管”(Organic Light Emitting Diode)， 所以像素密度比普通LED点阵显示器高得多，见图 [OLED像素结构](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#oled) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD005.jpg)

OLED显示器不需要背光源、对比度高、轻薄、视角广及响应速度快等优点。待到生产工艺更加成熟时， 必将取代现在液晶显示器的地位，见图 [采用OLED屏幕的电视及智能手表](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id4) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD006.jpg)

## 2. 液晶控制原理

图 [适合STM32控制的显示屏实物图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#stm32) 是两种适合于STM32芯片使用的显示屏，我们以它为例讲解控制液晶屏的基本原理。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD007.jpeg)

这个完整的显示屏由液晶显示面板、电容触摸面板以及PCB底板构成。图中的触摸面板带有触摸控制芯片，该芯片处理触摸信号并通过引出的信号线与外部器件通讯， 触摸面板中间是透明的，它贴在液晶面板上面，一起构成屏幕的主体，触摸面板与液晶面板引出的排线连接到PCB底板上，根据实际需要， PCB底板上可能会带有“液晶控制器芯片”，图中右侧的液晶屏PCB上带有RA8875液晶控制器。因为控制液晶面板需要比较多的资源， 所以大部分低级微控制器都不能直接控制液晶面板，需要额外配套一个专用液晶控制器来处理显示过程，外部微控制器只要把它希望显示的数据直接交给液晶控制器即可。 而不带液晶控制器的PCB底板 ，只有小部分的电源管理电路，液晶面板的信号线与外部微控制器相连，直接控制。STM32F429系列的芯片不需要额外的液晶控制器， 也就是说它把专用液晶控制器的功能集成到STM32F429芯片内部了，可以理解为电脑的CPU集成显卡，它节约了额外的控制器成本。 而STM32F1系列的芯片由于没有集成液晶控制器到芯片内部，所以它只能驱动自带控制器的屏幕，可以理解为电脑的外置显卡。

总的来说，这两类屏幕的控制框图如图 [两类液晶屏控制框图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id7) 所示。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD008.jpg)

### 2.1 液晶面板的控制信号

本章我们主要讲解如何控制液晶面板，液晶面板的控制信号线即图 [适合STM32控制的显示屏实物图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#stm32) 中液晶面板引出的FPC排线， 其说明见表 [液晶面板的信号线](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id9) ， 液晶面板通过这些信号线与液晶控制器通讯，使用这种通讯信号的被称为RGB接口(RGB Interface)。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD01.png)

(1) RGB信号线

> RGB信号线各有8根，分别用于表示液晶屏一个像素点的红、绿、蓝颜色分量。使用红绿蓝颜色分量来表示颜色是一种通用的做法， 打开Windows系统自带的画板调色工具，可看到颜色的红绿蓝分量值，见图 [颜色表示法](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id10) 。 常见的颜色表示会在“RGB”后面附带各个颜色分量值的数据位数，如RGB565表示红绿蓝的数据线数分别为5、6、5根， 一共为16个数据位，可表示216种颜色；而这个液晶屏的种颜色分量的数据线都有8根， 所以它支持RGB888格式，一共24位数据线，可表示的颜色为224种。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD009.png)

(2) 同步时钟信号CLK

> 液晶屏与外部使用同步通讯方式，以CLK信号作为同步时钟，在同步时钟的驱动下，每个时钟传输一个像素点数据。

(3) 水平同步信号HSYNC

> 水平同步信号HSYNC(Horizontal Sync)用于表示液晶屏一行像素数据的传输结束， 每传输完成液晶屏的一行像素数据时，HSYNC会发生电平跳变， 如分辨率为800x480的显示屏(800列，480行)，传输一帧的图像HSYNC的电平会跳变480次。

(4) 垂直同步信号VSYNC

> 垂直同步信号VSYNC(Vertical Sync)用于表示液晶屏一帧像素数据的传输结束，每传输完成一帧像素数据时，VSYNC会发生电平跳变。 其中“帧”是图像的单位，一幅图像称为一帧，在液晶屏中，一帧指一个完整屏液晶像素点。人们常常用“帧/秒”来表示液晶屏的刷新特性， 即液晶屏每秒可以显示多少帧图像，如液晶屏以60帧/秒的速率运行时，VSYNC每秒钟电平会跳变60次。

(5) 数据使能信号DE

> 数据使能信号DE(Data Enable)用于表示数据的有效性，当DE信号线为高电平时，RGB信号线表示的数据有效。

### 2.2 液晶数据传输时序

通过上述信号线向液晶屏传输像素数据时，各信号线的时序见图 [液晶时序图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id12) 。图中表示的是向液晶屏传输一帧图像数据的时序，中间省略了多行及多个像素点。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD010.jpg)

液晶屏显示的图像可看作一个矩形，结合图 [液晶数据传输图解](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id13) 来理解。液晶屏有一个显示指针，它指向将要显示的像素。 显示指针的扫描方向方向从左到右、从上到下，一个像素点一个像素点地描绘图形。这些像素点的数据通过RGB数据线传输至液晶屏， 它们在同步时钟CLK的驱动下一个一个地传输到液晶屏中，交给显示指针，传输完成一行时，水平同步信号HSYNC电平跳变一次，而传输完一帧时VSYNC电平跳变一次。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD011.jpeg)

但是，液晶显示指针在行与行之间，帧与帧之间切换时需要延时，而且HSYNC及VSYNC信号本身也有宽度， 这些时间参数说明见表 [液晶通讯中的时间参数](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id14) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD02.png)

在这些时间参数控制的区域，数据使能信号线“DE”都为低电平，RGB数据线的信号无效，当“DE”为高电平时，表示的数据有效，传输的数据会直接影响液晶屏的显示区域。

## 3. 显存

液晶屏中的每个像素点都是数据，在实际应用中需要把每个像素点的数据缓存起来，再传输给液晶屏，一般会使用SRAM或SDRAM性质的存储器， 而这些专门用于存储显示数据的存储器，则被称为显存。显存一般至少要能存储液晶屏的一帧显示数据，如分辨率为800x480的液晶屏， 使用RGB888格式显示，它的一帧显示数据大小为：3x800x480=1152000字节；若使用RGB565格式显示，一帧显示数据大小为：2x800x480=768000字节。

一般来说，外置的液晶控制器会自带显存，而像STM32F429等集成液晶控制器的芯片可使用内部SRAM或外扩SDRAM用于显存空间。
