# 第四十七章 LCD-液晶显示实验

## 1. 硬件设计

本小节讲解如何使用FSMC外设控制实验板配套的3.2寸ILI9341液晶屏，该液晶屏的分辨率为320x240，支持RGB565格式。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD022.jpg)

<img title="" src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD023.jpeg" alt="" width="750">

屏幕的PCB底板引出的信号线会通过PCB底板上的FPC接口与液晶面板连接，这些信包括液晶控制相关的CS、RS等信号及DB0-DB15数据线， 其中RS引脚以高电平表示传输数据，低电平表示传输命令；另外还有引出LCD_BK引脚用于控制屏幕的背光供电，可以通过该引脚控制背光的强度， 该引脚为低电平时打开背光。图中的X+/X-/Y+/Y-引脚是液晶面板上触摸屏引出的信号线，它们会被连接到PCB底板的电阻触摸屏控制器，用于检测触摸信号， 其原理图见图 [屏幕PCB底板的触摸部分原理图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id42) 。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD024.jpeg" title="" alt="" width="775">

触摸检测的主体是型号为XPT2046的芯片，它接收触摸屏的X+/X-/Y+/Y-信号进行处理，把触摸信息使用SPI接口输出到STM32等控制器， 在触摸屏章节将会详细讲解其检测原理。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD025.jpg)

图 [液晶屏接口](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id43) 表示的是PCB底板引出的排针线序， 屏幕整体通过这些引出的排针与开发板或其它控制器连接。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD026.jpg" title="" alt="" width="757">

图 [开发板与屏幕的连接的信号说明](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id44) 是指南者开发板上的液晶排母接口原理图， 它说明了配套的3.2寸屏幕接入到开发板上时的信号连接关系。 其中请着重关注图中液晶屏LCD_CS及LCD_RS(即DC引脚)与FSMC存储区选择引脚FSMC_NE及地址信号FSMC_A的编号， 它们会决定STM32要使用什么内存地址来控制与液晶屏的通讯。

## 2. 软件设计

### 2.1 编程目标

1. 初始化通讯使用的目标引脚及端口时钟；

2. 使能FSMC外设的时钟；

3. 配置FSMC 为异步NOR FLASH模式以仿真8080时序；

4. 建立机制使用FSMC向液晶屏发送命令及数据；

5. 发送控制命令初始化液晶屏；

6. 编写液晶屏的绘制像素点函数；

7. 利用描点函数制作各种不同的液晶显示应用。

### 2.2 代码分析

- 硬件LCD硬件相关宏定义

```c
//由片选引脚决定的NOR/SRAM块
#define      FSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM1

/*********** ILI9341 显示屏8080通讯引脚定义 **********/
/******控制信号线******/
//片选，选择NOR/SRAM块
#define      ILI9341_CS_CLK                RCC_APB2Periph_GPIOD
#define      ILI9341_CS_PORT               GPIOD
#define      ILI9341_CS_PIN                GPIO_Pin_7

//DC引脚，使用FSMC的地址信号控制，本引脚决定了访问LCD时使用的地址
//PE2为FSMC_A23
#define      ILI9341_DC_CLK                RCC_APB2Periph_GPIOD
#define      ILI9341_DC_PORT               GPIOD
#define      ILI9341_DC_PIN                GPIO_Pin_11

// 写使能
#define      ILI9341_WR_CLK                RCC_APB2Periph_GPIOD
#define      ILI9341_WR_PORT               GPIOD
#define      ILI9341_WR_PIN                GPIO_Pin_5

// 读使能
#define      ILI9341_RD_CLK                RCC_APB2Periph_GPIOD
#define      ILI9341_RD_PORT               GPIOD
#define      ILI9341_RD_PIN                GPIO_Pin_4

// 复位引脚
#define      ILI9341_RST_CLK               RCC_APB2Periph_GPIOE
#define      ILI9341_RST_PORT              GPIOE
#define      ILI9341_RST_PIN               GPIO_Pin_1

// 背光引脚
#define      ILI9341_BK_CLK                RCC_APB2Periph_GPIOD
#define      ILI9341_BK_PORT               GPIOD
#define      ILI9341_BK_PIN                GPIO_Pin_12

/********数据信号线***************/
#define      ILI9341_D0_CLK                RCC_APB2Periph_GPIOD
#define      ILI9341_D0_PORT               GPIOD
#define      ILI9341_D0_PIN                GPIO_Pin_14
/*******此处省略其它数据线********/
```

以上代码根据硬件的连接，把与FSMC与液晶屏通讯使用的引脚号、引脚源以及复用功能映射都以宏封装起来。 其中请着重关注代码中液晶屏LCD_CS及LCD_RS(DC引脚)与FSMC存储区选择引脚FSMC_NE及地址信号FSMC_A的编号， 它们会决定STM32要使用什么内存地址来控制与液晶屏的通讯。

- 初始化FSMC的GPIO

```c
static void ILI9341_GPIO_Config ( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能FSMC对应相应管脚时钟
    RCC_APB2PeriphClockCmd(
    // 控制信号
    ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
    ILI9341_RD_CLK  |ILI9341_BK_CLK|ILI9341_RST_CLK|
    // 数据信号
    ILI9341_D0_CLK|ILI9341_D1_CLK, ENABLE );
    /*此处省略部分信号线*/

    // 配置FSMC相对应的数据线,FSMC-D0~D15
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
    GPIO_Init(ILI9341_D0_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
    GPIO_Init(ILI9341_D1_PORT, &GPIO_InitStructure);

    /*此处省略部分数据信号线*/

    /* 配置FSMC相对应的控制线
    * FSMC_NOE   :LCD-RD
    * FSMC_NWE   :LCD-WR
    * FSMC_NE1   :LCD-CS
    * FSMC_A16   :LCD-DC
    */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN;
    GPIO_Init(ILI9341_RD_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN;
    GPIO_Init(ILI9341_WR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
    GPIO_Init(ILI9341_CS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
    GPIO_Init(ILI9341_DC_PORT, &GPIO_InitStructure);

    // 配置LCD复位RST控制管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
    GPIO_Init(ILI9341_RST_PORT, &GPIO_InitStructure);

    // 配置LCD背光控制管脚BK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN;
    GPIO_Init(ILI9341_BK_PORT, &GPIO_InitStructure);
}
```

与控制SRAM中的GPIO初始化类似，对于FSMC引脚，全部直接初始化为复用推挽输出模式即可，而背光BK引脚及液晶复信RST信号则被初始化成普通的推挽输出模式， 这两个液晶控制信号直接输出普通的电平控制即可。

- 配置FSMC的模式

```c
static void ILI9341_FSMC_Config ( void )
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

    // 使能FSMC时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    // 地址建立时间（ADDSET）为1个HCLK 2/72M=28ns
    readWriteTiming.FSMC_AddressSetupTime = 0x01; // 地址建立时间
    // 数据保持时间（DATAST）+ 1个HCLK = 5/72M=70ns
    readWriteTiming.FSMC_DataSetupTime = 0x04;   // 数据建立时间
    // 选择控制的模式
    // 模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;

    // 以下配置与模式B无关
    // 地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_AddressHoldTime = 0x00; // 地址保持时间
    // 设置总线转换周期，仅用于复用模式的NOR操作
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    // 设置时钟分频，仅用于同步类型的存储器
    readWriteTiming.FSMC_CLKDivision           = 0x00;
    // 数据保持时间，仅用于同步型的NOR
    readWriteTiming.FSMC_DataLatency           = 0x00;

    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      =
    FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    // 使能 FSMC_Bank1_NORSRAM4 
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);
}
```

这个函数的主体是把FSMC配置成异步NOR FLASH使用的模式B，使用该方式模拟8080时序控制液晶屏，执行流程如下：

(1) 初始化FSMC时钟

> 函数开头使用库函数RCC_AHBPeriphClockCmd使能FSMC外设的时钟。

(2) 时序结构体赋值

> 接下来对时序结构体FSMC_NORSRAMTimingInitTypeDef赋值。在这个时序结构体配置中， 由于我们要使用异步NOR FLASH的方式模拟8080时序， 所以选择FSMC为模式B，在该模式下配置FSMC的控制时序结构体中， 实际上只有地址建立时间FSMC_AddressSetupTime（即ADDSET的值）以及数据建立时间FSMC_DataSetupTime（即DATAST的值）成员的配置值是有效的， 其它异步NOR FLASH没使用到的成员值全配置为0即可。而且，这些成员值使用的单位为：1个HCLK的时钟周期， 而HCLK的时钟频率为72MHz，对应每个时钟周期为1/72微秒。

由图 [ILI9341时序参数说明图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id51) 及图 [ILI9341的时序参数要求](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id52) 中的ILI9341时序参数说明及要求可大致得知ILI9341的写周期为最小twc = 66ns， 而读周期最小为trdl+trod=45+20=65ns。 （对于读周期表中有参数要一个要求为trcfm和trc分别为450ns及160ns， 但经过测试并不需要遵照它们的指标要求）

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD027.jpg" title="" alt="" width="788">

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD028.jpeg" title="" alt="" width="786">

在FSMC代码中使用结构体中的FSMC_AddressSetupTime（即ADDSET的值）及FSMC_DataSetupTime（即DATAST的值）成员控制FSMC的读写周期

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD029.jpeg" title="" alt="" width="788">

结合ILI9341的时序要求和FSMC的配置图，代码中按照读写时序周期均要求至少66ns来计算，配置结果为ADDSET = 1及DATST = 4， 把时间单位1/72微秒(即1000/72纳秒)代入，因此读写周期的时间被配置为：

读周期：trc =((ADDSET+1)+(DATST+1)+2) *1000/72 = ((1+1)+(4+1)+2)*1000/72 = 125ns

写周期：twc =((ADDSET+1)+(DATST+1)) *1000/72 = ((1+1)+(4+1))*1000/72 = 97ns

所以把这两个参数值写入到FSMC后，它控制的读写周期都比ILI9341的最低要求值大。（经测试，这两个参数值也可以适当减小，您可以亲自试一下）

把计算得的参数赋值到时序结构体中的FSMC_AddressSetupTime（即ADDSET的值）及FSMC_DataSetupTime（即DATAST的值）中， 然后再把时序结构体作为指针赋值到下面的FSMC初始化结构体中，作为读写的时序参数，最后再调用FSMC_NORSRAMInit函数即可把参数写入到相应的寄存器中。

(3) 配置FSMC初始化结构体

> 函数接下来对FSMC SRAM的初始化结构体赋值。主要包括存储映射区域、存储器类型以及数据线宽度等，这些是根据外部电路设置的。

- 设置存储区域FSMC_Bank

> FSMC_Bank成员设置FSMC的NOR FLASH存储区域映射选择为宏 FSMC_Bank1_NORSRAMx (即FSMC_Bank1_NORSRAM4)， 这是由于我们的SRAM硬件连接到FSMC_NE4和NOR/PSRAM相关引脚， 所以对应到存储区域Bank1SRAM4，对应的基地址为0X6C00 0000；

- 存储器类型FSMC_MemoryType

> 由于使用异步NOR FLASH模式模拟8080时序，所以FSMC_MemoryType成员要选择相应的FSMC_MemoryType_NOR；

- 数据线宽度FSMC_MemoryDataWidth

> 根据硬件的数据线连接，数据线宽度被配置为16位宽FSMC_MemoryDataWidth_16b；

- 写使能FSMC_WriteOperation

> FSMC_WriteOperation用于设置写使能，只有使能了才能正常使用FSMC向外部存储器写入数据；

- 扩展模式以及读写时序

> 在FSMC_ExtendedMode成员中可以配置是否使用扩展模式，当设置扩展模式时，读时序使用FSMC_ReadWriteTimingStruct中的配置，写时序使用FSMC_WriteTimingStruct中的配置，两种配置互相独立，可以赋值为不同的读写时序结构体。在本实例中不使用扩展模式，即 读写时序使用相同的配置，都是赋值为前面的readWriteTiming结构体；

- 其它

> 配置FSMC还涉及到其它的结构体成员，但这些结构体成员与异步NOR FLASH控制不相关，都被设置为Disable了；

赋值完成后调用库函数FSMC_NORSRAMInit把初始化结构体配置的各种参数写入到FSMC_BCR控制寄存器及FSMC_BTR时序寄存器中。最后调用FSMC_NORSRAMCmd函数使能要控制的存储区域FSMC_Bank1_NORSRAM4。

- 计算控制液晶显屏时使用的地址

初始化完FSMC后，即可使用类似扩展外部SRAM中的读取方式：通过访问某个地址，由FSMC产生时序与外部存储器通讯，进行读写。

同样地，当访问特定地址时，FSMC会产生相应的模拟8080时序，控制地址线输出要访问的内存地址，使用数据信号线接收或发送数据， 其它片选信号NE、读使能信号NOE、写使能信号NWE辅助产生完整的时序，而由于控制液晶屏的硬件连接中， 使用如图 [FSMC与8080端口连接简图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id54) 中的连接来模拟8080时序， 所以FSMC产生的这些信号会被ILI9341接收，并且使用其中一根FSMC_Ax地址控制命令/数据选择引脚RS(即D/CX)，因此， 需要重点理解下当STM32访问什么地址时，对应的FSMC_Ax引脚会输出高电平表示传输的是数据，访问什么地址时， 对应的FSMC_Ax引脚会输出低电平表示传输的是命令。若理解了计算过程，以后就可以根据自己制作的硬件电路来计算访问地址了。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD030.png)

计算地址的过程如下：

(1) 本工程中使用的是FSMC_NE1作为8080_CS片选信号，所以首先可以确认地址范围， 当访问0X6000 0000 ~ 0X63FF FFFF地址时，FSMC均会对外产生片选有效的访问时序；

(2) 本工程中使用FSMC_A16地址线作为命令/数据选择线RS信号，所以在以上地址范围内， 再选择出使得FSMC_A16输出高电平的地址，即可控制表示数据，选择出使得FSMC_A16输出低电平的地址，即可控制表示命令。

- 要使FSMC_A16地址线为高电平，实质是输出地址信号的第16位为1即可，使用0X6000 0000~0X63FF FFFF内的任意地址，作如下运算：

> 设置地址的第16位为1： 0X6000 0000 |= (1<<16) = 0x6001 0000

- 要使FSMC_A16地址线为低电平，实质是输出地址信号的第16位为0即可，使用0X6000 0000~0X63FF FFFF内的任意地址，作如下运算：

> 设置地址的第16位为0： 0X6000 0000 &= ~ (1<<16) = 0x6000 0000

(3) 但是，以上方法计算的地址还不完全正确，根据《STM32参考手册》对FSMC访问NOR FLASH的说明， 见图 [HADDR与FSMC地址线的说明](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#haddrfsmc) ，STM32内部访问地址时使用的是内部HADDR总线， 它是需要转换到外部存储器的内部AHB地址线，它是字节地址(8位)， 而存储器访问不都是按字节访问，因此接到存储器的地址线依存储器的数据宽度有所不同。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD031.png)

在本工程中使用的是16位的数据访问方式，所以HADDR与FSMC_A的地址线连接关系会左移一位，如HADDR1与FSMC_A0对应、HADDR2与FSMC_A1对应。因此，当FSMC_A0地址线为1时，实际上内部地址的第1位为1，FSMC_A1地址线为1时，实际上内部地址的第2位为1。同样地，当希望 FSMC_A16地址输出高电平或低电平时，需要重新调整计算公式：

- 要使FSMC_A16地址线为高电平，实质是访问内部HADDR地址的第(16+1)位为1即可， 使用0X6000 0000~0X63FF FFFF内的任意地址，作如下运算：

> 使FSMC_A16地址线为高电平：0X6000 0000 |= (1<<(16+1)) = 0x6002 0000

- 要使FSMC_A16地址线为低电平，实质是访问内部HADDR地址的第(16+1)位为0即可， 使用0X6000 0000~0X63FF FFFF内的任意地址，作如下运算：

> 使FSMC_A16地址线为低电平： 0X6000 0000 &= ~ (1<<(16+1)) = 0x6000 0000

根据最终的计算结果，总结如下：当STM32访问内部的0x6002 0000地址时，FSMC自动输出时序， 且使得与液晶屏的数据/命令选择线RS(即D/CX)相连的FSMC_A16输出高电平，使得液晶屏会把传输过程理解为数据传输； 类似地，当STM32访问内部的0X60000000地址时，FSMC自动输出时序，且使得与液晶屏的数据/命令选择线RS(即D/CX)相连的FSMC_A16输出低电平， 使得液晶屏会把传输过程理解为命令传输。

在工程代码中，把以上计算结果封装成了宏:

```c
/****************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM1 连接 TFT，地址范围为0X6000 0000 ~ 0X63FF FFFF
FSMC_A16 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X6C00 0000
RAM基地址 = 0X6002 0000 = 0X6000 0000+2^16*2 = 0X6000 0000 + 0x2 0000 = 0X6002 0000
当选择不同的地址线时，地址要重新计算
********************************************************/

/*************** ILI9341 显示屏的 FSMC 参数定义 ****************/
//FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define      FSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x60020000 )

//FSMC_Bank1_NORSRAM用于LCD数据操作的地址
#define      FSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x60000000 )
```

利用这样的宏，再使用指针的形式访问其地址，即可控制FSMC产生相应的时序，工程代码中把发送命令及发送数据的操作封装成了内联函数， 方便后面调用

```c
/**
* @brief  向ILI9341写入命令
* @param  usCmd :要写入的命令（表寄存器地址）
* @retval 无
*/
__inline void ILI9341_Write_Cmd (uint16_t usCmd )
{
    * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = usCmd;

}


/**
* @brief  向ILI9341写入数据
* @param  usData :要写入的数据
* @retval 无
*/
__inline void ILI9341_Write_Data ( uint16_t usData )
{
    * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = usData;

}
```

需要写操作时，只要把要发送的命令代码或数据作为参数输入到函数然后调用即可，对于液晶屏的读操作，把向指针赋值的过程改为读取指针内容即可。

- 向液晶屏写入初始化配置

利用上面的发送命令及数据操作，可以向液晶屏写入一些初始化配置

```c
// 初始化ILI9341寄存器
static void ILI9341_REG_Config ( void )
{
    //  Power control B (CFh)
    DEBUG_DELAY();
    ILI9341_Write_Cmd(0xCF);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data (0x81);
    ILI9341_Write_Data(0x30);

    //  Power on sequence control (EDh) 
    DEBUG_DELAY();
    ILI9341_Write_Cmd(0xED);
    ILI9341_Write_Data(0x64);
    ILI9341_Write_Data(0x03);
    ILI9341_Write_Data(0x12);
    ILI9341_Write_Data(0x81);

    /*以下省略大量配置内容*/

}
```

以上列出的代码中省略了大量的配置内容，本质上它们都是使用ILI9341_Write_Cmd发送代码，然后使用ILI9341_Write_Data函数发送命令对应的参数对液晶屏进行配置。

这个初始化过程中发送的代码及参数主要是配置了液晶屏的上电过程、显示屏的伽玛参数、分辨率、像素格式等内容， 这些配置主要由液晶屏生产厂家提供，本教程后面只针对常用命令进行讲解，此处不作详细说明，关于命令及参数可以查询《ILI9341数据手册》获知， 在该文档中搜索命令代码即可方便定位到相应的说明。例如，要查找代码中的0xCF命令说明，在文档中搜索“CFh”即可，见图 [CFh命令的部分说明](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#cfh) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD032.png)

- 设置液晶显示窗口

根据液晶屏的要求，在发送显示数据前，需要先设置显示窗口确定后面发送的像素数据的显示区域

```c
/********** ILI934 命令 ********************************/
#define      CMD_SetCoordinateX           0x2A       //设置X坐标
#define      CMD_SetCoordinateY           0x2B       //设置Y坐标

/**
* @brief  在ILI9341显示器上开辟一个窗口
* @param  usX ：在特定扫描方向下窗口的起点X坐标
* @param  usY ：在特定扫描方向下窗口的起点Y坐标
* @param  usWidth ：窗口的宽度
* @param  usHeight ：窗口的高度
* @retval 无
*/
void ILI9341_OpenWindow ( uint16_t usX, uint16_t usY,
uint16_t usWidth, uint16_t usHeight )
{
    ILI9341_Write_Cmd ( CMD_SetCoordinateX );          /* 设置X坐标 */
    ILI9341_Write_Data ( usX >> 8  );  /* 先高8位，然后低8位 */
    ILI9341_Write_Data ( usX & 0xff  );  /* 设置起始点和结束点*/
    ILI9341_Write_Data ( ( usX + usWidth - 1 ) >> 8  );
    ILI9341_Write_Data ( ( usX + usWidth - 1 ) & 0xff  );

    ILI9341_Write_Cmd ( CMD_SetCoordinateY );            /* 设置Y坐标*/
    ILI9341_Write_Data ( usY >> 8  );
    ILI9341_Write_Data ( usY & 0xff  );
    ILI9341_Write_Data ( ( usY + usHeight - 1 ) >> 8 );
    ILI9341_Write_Data ( ( usY + usHeight - 1) & 0xff );
}
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD033.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD034.png)

代码中定义的ILI9341_OpenWindow函数实现了图 [设置显示窗口的X坐标](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#x) 及图 [设置液晶显示窗口的Y坐标](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#y) 的0x2A和0x2B命令， 它们分别用于设置显示窗口的起始及结束的X坐标和Y坐标，每个命令后包含4个8位的参数，这些参数组合后成起始坐标和结束坐标各1个用16位表示的值。

ILI9341_OpenWindow把它的四个函数输入参数X、Y起始坐标，宽度、高度转化成命令参数的格式，写入到液晶屏中，从而设置出一个显示窗口。

- 发送像素数据

调用上面的ILI9341_OpenWindow函数设置显示窗口后，再向液晶屏发送像素数据时，这些数据就会直接显示在它设定的窗口位置中。

```c
#define      CMD_SetPixel                 0x2C       //填充像素

/**
* @brief  在ILI9341显示器上以某一颜色填充像素点
* @param  ulAmout_Point ：要填充颜色的像素点的总数目
* @param  usColor ：颜色
* @retval 无
*/
static __inline void ILI9341_FillColor ( uint32_t ulAmout_Point,
uint16_t usColor )
{
    uint32_t i = 0;

    /* memory write */
    ILI9341_Write_Cmd ( CMD_SetPixel );

    for ( i = 0; i < ulAmout_Point; i ++ )
        ILI9341_Write_Data ( usColor );
}
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD035.png)

发送像素数据的命令非常简单，首先发送命令代码0x2C，然后后面紧跟着要传输的像素数据即可。按照本液晶屏的配置， 像素点的格式为RGB565，所以像素数据就是要显示的RGB565格式的颜色值。

本ILI9341_FillColor函数包含两个输入参数，分别用于设置要发送的像素数据个数ulAmout_Point及像素点的颜色值usColor， 在代码实现中它调用ILI9341_Write_Cmd发送一次命令代码，接着使用for循环调用ILI9341_Write_Data写入ulAmout_Po int个同样的颜色值。

这些颜色值会按顺序填充到前面使用ILI9341_OpenWindow函数设置的显示窗口中，例如，若设置了一个usX=10，usY=30，usWidth=50， usHeight=20的窗口，然后再连续填充50*20个颜色值为0XFFFF的像素数据，即可在(10,30)的起始坐标处显示一个宽50像素高20像素的白色矩形。

- 绘制矩形

类似地，使用ILI9341_OpenWindow和ILI9341_FillColor制作的绘制矩形操作见

```c
/**
* @brief  在 ILI9341 显示器上画一个矩形
* @param  usX_Start ：在特定扫描方向下矩形的起始点X坐标
* @param  usY_Start ：在特定扫描方向下矩形的起始点Y坐标
* @param  usWidth：矩形的宽度（单位：像素）
* @param  usHeight：矩形的高度（单位：像素）
* @param  ucFilled ：选择是否填充该矩形
*   该参数为以下值之一：
*     @arg 0 :空心矩形
*     @arg 1 :实心矩形
* @note 可使用LCD_SetBackColor、LCD_SetTextColor、LCD_SetColors函数设置颜色
* @retval 无
*/
void ILI9341_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start,
                uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled )
{
    if ( ucFilled ) {
        ILI9341_OpenWindow ( usX_Start, usY_Start, usWidth, usHeight );
        ILI9341_FillColor ( usWidth * usHeight ,CurrentTextColor);
    } else {
        ILI9341_DrawLine ( usX_Start, usY_Start,
                        usX_Start + usWidth - 1, usY_Start );
        ILI9341_DrawLine ( usX_Start, usY_Start + usHeight - 1,
                    usX_Start + usWidth - 1, usY_Start + usHeight - 1 );
        ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start,
                            usY_Start + usHeight - 1 );
        ILI9341_DrawLine ( usX_Start + usWidth - 1, usY_Start,
                    usX_Start + usWidth - 1, usY_Start + usHeight - 1 );
    }
}
```

ILI9341_DrawRectangle函数分成两部分，它根据输入参数ucFilled是否为真值决定绘制的是实心矩形还是只有边框的矩形。 绘制实心矩形时，直接使用ILI9341_OpenWindow函数根据输入参数设置显示矩形窗口，然后根据实心矩形的像素点个数调用ILI9341_FillColor即可完成绘制； 而绘制空心矩形时，实质上是绘制四条边框线，它调用ILI9341_DrawLine函数绘制，ILI9341_DrawLine函数的输入参数是用于表示直接的两个坐标点(x1,y1)与(x2,y2)， 该函数内部根据数据关系，使用这两个点确定一条直线，最后调用ILI9341_SetPointPixel函数一点一点地绘制成完整的直线。

关于ILI9341_DrawLine画线函数、ILI9341_DrawCircle画圆函数等代码不再讲解，它们都是根据数学关系在特点的位置显示坐标点而已。 另外关于工程中的显示字符串的原理将在下一个章节详细说明。

- 设置液晶的扫描方向

控制液晶屏时，还有一个非常重要的参数，就是设置液晶屏的扫描方向

```c
#define       ILI9341_LESS_PIXEL        240     //液晶屏较短方向的像素宽度
#define       ILI9341_MORE_PIXEL      320     //液晶屏较长方向的像素宽度

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
uint16_t LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
//参数可选值为0-7
//调用ILI9341_GramScan函数设置方向时会自动更改
//LCD刚初始化完成时会使用本默认值
uint8_t LCD_SCAN_MODE = 6;

/**
* @brief  设置ILI9341的GRAM的扫描方向
* @param  ucOption ：选择GRAM的扫描方向
*     @arg 0-7 :参数可选值为0-7这八个方向
*
*  ！！！其中0、3、5、6 模式适合从左至右显示文字，
*        不推荐使用其它模式显示文字 其它模式显示文字会有镜像效果
*
*  其中0、2、4、6 模式的X方向像素为240，Y方向像素为320
*  其中1、3、5、7 模式下X方向像素为320，Y方向像素为240
*
*  其中 6 模式为大部分液晶例程的默认显示方向
*  其中 3 模式为摄像头例程使用的方向
*  其中 0 模式为BMP图片显示例程使用的方向
*
* @retval 无
* @note  坐标图例：A表示向上，V表示向下，<表示向左，>表示向右
        X表示X轴，Y表示Y轴

------------------------------------------------------------
模式0：        .   模式1：    . 模式2：      . 模式3：
        A   .         A   .   A         .   A
        |   .         |   .   |         .   |
        Y   .         X   .   Y         .   X
        0   .         1   .   2         .   3
<--- X0 o   . <----Y1 o   .   o 2X--->  .   o 3Y--->
------------------------------------------------------------
模式4：        . 模式5：      . 模式6：      . 模式7：
<--- X4 o   . <--- Y5 o   .   o 6X--->  .   o 7Y--->
        4   .         5   .   6         .   7
        Y   .         X   .   Y         .   X
        |   .         |   .   |         .   |
        V   .         V   .   V         .   V
---------------------------------------------------------
                    LCD屏示例
                |-----------------|
                |     野火Logo    |
                |                 |
                |                 |
                |                 |
                |                 |
                |                 |
                |                 |
                |                 |
                |                 |
                |-----------------|
                屏幕正面（宽240，高320）

*******************************************************/
void ILI9341_GramScan ( uint8_t ucOption )
{
    //参数检查，只可输入0-7
    if (ucOption >7 )
        return;

    //根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
    LCD_SCAN_MODE = ucOption;

    //根据模式更新XY方向的像素宽度
    if (ucOption%2 == 0) {
        //0 2 4 6模式下X方向像素宽度为240，Y方向为320
        LCD_X_LENGTH = ILI9341_LESS_PIXEL;
        LCD_Y_LENGTH =  ILI9341_MORE_PIXEL;
    } else {
        //1 3 5 7模式下X方向像素宽度为320，Y方向为240
        LCD_X_LENGTH = ILI9341_MORE_PIXEL;
        LCD_Y_LENGTH =  ILI9341_LESS_PIXEL;
    }

    //0x36命令参数的高3位可用于设置GRAM扫描方向
    ILI9341_Write_Cmd ( 0x36 );
    ILI9341_Write_Data ( 0x08 |(ucOption<<5));//根据ucOption的值设置LCD参数，共0-7种模式
    ILI9341_Write_Cmd ( CMD_SetCoordinateX );
    ILI9341_Write_Data ( 0x00 );    /* x 起始坐标高8位 */
    ILI9341_Write_Data ( 0x00 );    /* x 起始坐标低8位 */
    ILI9341_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF );/* x 结束坐标高8位 */
    ILI9341_Write_Data ( (LCD_X_LENGTH-1)&0xFF );    /* x 结束坐标低8位 */

    ILI9341_Write_Cmd ( CMD_SetCoordinateY );
    ILI9341_Write_Data ( 0x00 );    /* y 起始坐标高8位 */
    ILI9341_Write_Data ( 0x00 );    /* y 起始坐标低8位 */
    ILI9341_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );/*y 结束坐标高8位 */
    ILI9341_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );    /*y 结束坐标低8位 */

    /* write gram start */
    ILI9341_Write_Cmd ( CMD_SetPixel );
}
```

当设置了液晶显示窗口，再连续向液晶屏写入像素点时，它会一个点一个点地往液晶屏的X方向填充， 填充完一行X方向的像素点后，向Y方向下移一行，X坐标回到起始位置，再往X方向一个点一个点地填充，如此循环直至填充完整个显示窗口。

而屏幕的坐标原点和XY方向都可以根据实际需要使用0X36命令来配置的， 该命令的说明见 图 [液晶扫描模式命令](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id64) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD036.jpg)

0X36命令参数中的MY、MX、MV这三个数据位用于配置扫描方向，因此一共有23 = 8种模式。 ILI9341_GramScan函数就是根据输入的模式设置这三个数据位，并且根据相应的模式更改XY方向的分辨率LCD_X_LENGTH和LCD_Y_LENGTH， 使得其它函数可以利用这两个全局变量获屏幕实际的XY方向分辨率信息；同时， 函数内还设置了全局变量LCD_SCAN_MODE的值用于记录当前的屏幕扫描模式，这在后面计算触摸屏坐标的时候会使用到。 设置完扫描方向后，代码中还调用设置液晶显示窗口的命令CMD_SetCoordinateX/Y（0X2A/0X2B命令）默认打开一个与屏幕大小一致的显示窗口，方便后续的显示操作。

调用ILI9341_GramScan函数设置0-7模式时，各个模式的坐标原点及XY方向如图 [液晶屏的8种扫描模式](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LCD.html#id65) 所示。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LCD037.jpg)

其中模式6最符合我们的阅读习惯，扫描方向与文字方向一致，都是从左到右，从上到下，所以本开发板中的大部分液晶程序都是默认使用模式6。

其实模式0、3、5、6的液晶扫描方向都与文字方向一致，比较适合显示文字，只要适当旋转屏幕即可，使得用屏幕四个边沿作为正面看去都有适合的文字显示模式。 而其它模式由于扫描方向与文字方向不一致，要想实现同样的效果非常麻烦，也没有实现的必要。

- 液晶屏全局初始化

利用前面介绍的各种函数，我们把它封装成ILI9341_Init函数

```c
//液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
//参数可选值为0-7
//调用ILI9341_GramScan函数设置方向时会自动更改
//LCD刚初始化完成时会使用本默认值
uint8_t LCD_SCAN_MODE = 6;

/**
* @brief  ILI9341初始化函数，如果要用到lcd，一定要调用这个函数
* @param  无
* @retval 无
*/
void ILI9341_Init ( void )
{
    ILI9341_GPIO_Config ();
    ILI9341_FSMC_Config ();

    ILI9341_BackLed_Control ( ENABLE );      //点亮LCD背光灯
    ILI9341_Rst ();//复位液晶屏
    ILI9341_REG_Config ();//写入寄存器配置

    //设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向
    ILI9341_GramScan(LCD_SCAN_MODE);
}
```

本函数初始化GPIO、FSMC外设，然后开启液晶屏的背光，复位液晶屏，并且写入基本的液晶屏配置， 最后调用ILI9341_GramScan函数设置默认的液晶扫描方向。在需要使用液晶屏的时候，直接调用本函数即可完成初始化。

- 基本液晶显示例程的Main函数

本章内容中配套了两个工程进行演示，它们的液晶驱动完全一样，仅是main函数里的应用层展示不同效果时稍有区别， 先讲解基本的《液晶显示》例程，其main函数内容见

```c
/**
* @brief  主函数
* @param  无
* @retval 无
*/
int main ( void )
{
    ILI9341_Init ();         //LCD 初始化

    USART_Config();

    printf("\r\n ********** 液晶屏英文显示程序*********** \r\n");
    printf("\r\n 本程序不支持中文，显示中文的程序请学习下一章 \r\n");

//其中0、3、5、6 模式适合从左至右显示文字，
//不推荐使用其它模式显示文字 其它模式显示文字会有镜像效果
//其中 6 模式为大部分液晶例程的默认显示方向
    ILI9341_GramScan ( 6 );
    while ( 1 ) {
        LCD_Test();
    }
}
```

程序中，调用了ILI9341_Init函数初始化液晶屏，然后再初始化串口。（在实际测试中，若先初始化串口再初始化液晶屏， 会导致错误，原因不明。所以在应用时，注意先初始化液晶屏再初始化串口）

初始化完成后，调用LCD_Test函数显示各种图形进行测试(如直线、矩形、圆形)

```c

```
