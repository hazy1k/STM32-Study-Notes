# 第二十一章 SPI读写串行FLASH实验

## 1. 硬件设计

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI008.png" title="" alt="" width="603">

实验板中的FLASH芯片(型号：W25Q64)是一种使用SPI通讯协议的NOR FLASH存储器， 它的CS/CLK/DIO/DO引脚分别连接到了STM32对应的SPI引脚NSS/SCK/MOSI/MISO上，其中STM32的NSS引脚是一个普通的GPIO， 不是SPI的专用NSS引脚，所以程序中我们要使用软件控制的方式。

FLASH芯片中还有WP和HOLD引脚。WP引脚可控制写保护功能，当该引脚为低电平时，禁止写入数据。 我们直接接电源，不使用写保护功能。HOLD引脚可用于暂停通讯，该引脚为低电平时，通讯暂停， 数据输出引脚输出高阻抗状态，时钟和数据输入引脚无效。我们直接接电源，不使用通讯暂停功能。

## 2. 软件设计

### 2.1 编程目的

1. 初始化通讯使用的目标引脚及端口时钟；

2. 使能SPI外设的时钟；

3. 配置SPI外设的模式、地址、速率等参数并使能SPI外设；

4. 编写基本SPI按字节收发的函数；

5. 编写对FLASH擦除及读写操作的的函数；

6. 编写测试程序，对读写数据进行校验。

### 2.2 代码分析

- SPI硬件相关宏定义

```c
// SPI接口定义
#define      FLASH_SPIx                       SPI1
#define      FLASH_SPI_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define      FLASH_SPI_CLK                    RCC_APB2Periph_SPI1

// CS(NSS)引脚 片选选普通GPIO即可
#define      FLASH_SPI_CS_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      FLASH_SPI_CS_CLK                 RCC_APB2Periph_GPIOC
#define      FLASH_SPI_CS_PORT                GPIOC
#define      FLASH_SPI_CS_PIN                 GPIO_Pin_0
// SCK引脚
#define      FLASH_SPI_SCK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      FLASH_SPI_SCK_CLK                RCC_APB2Periph_GPIOA
#define      FLASH_SPI_SCK_PORT               GPIOA
#define      FLASH_SPI_SCK_PIN                GPIO_Pin_5
// MISO引脚
#define      FLASH_SPI_MISO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MISO_CLK               RCC_APB2Periph_GPIOA
#define      FLASH_SPI_MISO_PORT              GPIOA
#define      FLASH_SPI_MISO_PIN               GPIO_Pin_6
// MOSI引脚
#define      FLASH_SPI_MOSI_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MOSI_CLK               RCC_APB2Periph_GPIOA
#define      FLASH_SPI_MOSI_PORT              GPIOA
#define      FLASH_SPI_MOSI_PIN               GPIO_Pin_7

#define  FLASH_SPI_CS_LOW()         GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define  FLASH_SPI_CS_HIGH()        GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
```

以上代码根据硬件连接，把与FLASH通讯使用的SPI号 、GPIO等都以宏封装起来， 并且定义了控制CS(NSS)引脚输出电平的宏，以便配置产生起始和停止信号时使用。

- 利用上面的宏，编写SPI的初始化函数

```c
// SPI_FLASH初始化
void SPI_FLASH_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1.使能SPI时钟
    FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK, ENABLE );
    // 2.使能SPI引脚相关的时钟
    FLASH_SPI_CS_APBxClock_FUN(FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MISO_PIN|FLASH_SPI_MOSI_PIN, ENABLE);
    // 3.配置SPI的 CS引脚，普通IO即可
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);

    // 4.配置SPI的 SCK引脚
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure);

    // 5.配置SPI的 MISO引脚
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
    GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);

    // 6.配置SPI的 MOSI引脚
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
    GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);

    // 停止信号 FLASH: CS引脚高电平
    FLASH_SPI_CS_HIGH();
    // 为方便讲解，以下省略 SPI 模式初始化部分
```

与所有使用到GPIO的外设一样，都要先把使用到的GPIO引脚模式初始化，配置好复用功能。GPIO初始化流程如下：

(1) 使用GPIO_InitTypeDef定义GPIO初始化结构体变量， 以便下面用于存储GPIO配置；

(2) 调用库函数RCC_APB2PeriphClockCmd 来使能SPI引脚使用的GPIO端口时钟。

(3) 向GPIO初始化结构体赋值，把SCK/MOSI/MISO引脚初始化成复用推挽模式。 而CS(NSS)引脚由于使用软件控制，我们把它配置为普通的推挽输出模式。

(4) 使用以上初始化结构体的配置，调用GPIO_Init函数向寄存器写入参数， 完成GPIO的初始化。

- 配置SPI的模式

以上只是配置了SPI使用的引脚，对SPI外设模式的配置。在配置STM32的SPI模式前，我们要先了解从机端的SPI模式。

根据FLASH芯片的说明，它支持SPI模式0及模式3，支持双线全双工， 使用MSB先行模式，支持最高通讯时钟为104MHz，数据帧长度为8位。我们要把STM32的SPI外设中的这些参数配置一致。

```c
// SPI_FLASH引脚初始化
void SPI_FLASH_Init(void)
{
    // 为方便讲解，省略了SPI的GPIO初始化部分
    //......

    SPI_InitTypeDef  SPI_InitStructure;
    // SPI 模式配置
    // FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 双全工模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // SPI主模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 位宽
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // CPOL空闲时为高电平即CPOL=1
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // 数据在时钟的第二个边沿（下降沿）采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // 使用软件管理NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 4分频
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // 数据传输时高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7; // CRC计算式
    SPI_Init(FLASH_SPIx, &SPI_InitStructure);
    // 使能 SPI
    SPI_Cmd(FLASH_SPIx, ENABLE);
}
```

这段代码中，把STM32的SPI外设配置为主机端，双线全双工模式，数据帧长度为8位，使用SPI模式3(CPOL=1，CPHA=1)， NSS引脚由软件控制以及MSB先行模式。代码中把SPI的时钟频率配置成了4分频，实际上可以配置成2分频以提高通讯速率， 读者可亲自尝试一下。最后一个成员为CRC计算式，由于我们与FLASH芯片通讯不需要CRC校验，并没有使能SPI的CRC功能， 这时CRC计算式的成员值是无效的。

赋值结束后调用库函数SPI_Init把这些配置写入寄存器，并调用SPI_Cmd函数使能外设。

如果之前对结构体各参数不理解，这里我们还可以再讲诉一遍：

`SPI_InitTypeDef`结构体的参数和意义

1. **`SPI_Direction`**:
   
   `SPI_Direction = SPI_Direction_2Lines_FullDuplex;`
   
   - **SPI_Direction_2Lines_FullDuplex**: SPI工作在全双工模式下，数据可以在两个方向上同时传输。
   - 其他可选项：`SPI_Direction_1Line_Rx`（接收单线模式），`SPI_Direction_1Line_Tx`（发送单线模式）。

2. **`SPI_Mode`**:
   
   `SPI_Mode = SPI_Mode_Master;`
   
   - **SPI_Mode_Master**: SPI工作在主模式。主设备生成时钟信号并控制通信。
   - 其他可选项：`SPI_Mode_Slave`（从模式）。在从模式下，SPI设备不会生成时钟信号。

3. **`SPI_DataSize`**:
   
   `SPI_DataSize = SPI_DataSize_8b;`
   
   - **SPI_DataSize_8b**: 数据传输的位宽为8位。
   - 其他可选项：`SPI_DataSize_16b`（16位数据宽度）。

4. **`SPI_CPOL`**:
   
   `SPI_CPOL = SPI_CPOL_High;`
   
   - **SPI_CPOL_High**: SPI时钟的空闲状态为高电平。
   - 其他可选项：`SPI_CPOL_Low`（时钟空闲状态为低电平）。

5. **`SPI_CPHA`**:
   
   `SPI_CPHA = SPI_CPHA_2Edge;`
   
   - **SPI_CPHA_2Edge**: 数据在时钟的第二个边沿（下降沿）采样。
   - 其他可选项：`SPI_CPHA_1Edge`（数据在时钟的第一个边沿（上升沿）采样）。

6. **`SPI_NSS`**:
   
   `SPI_NSS = SPI_NSS_Soft;`
   
   - **SPI_NSS_Soft**: 使用软件管理NSS（片选）信号。
   - 其他可选项：`SPI_NSS_Hard`（硬件管理NSS信号）。

7. **`SPI_BaudRatePrescaler`**:
   
   `SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;`
   
   - **SPI_BaudRatePrescaler_4**: 设置SPI时钟的预分频因子为4。时钟频率是系统时钟的四分之一。
   - 其他可选项：`SPI_BaudRatePrescaler_2`, `SPI_BaudRatePrescaler_8`, `SPI_BaudRatePrescaler_16`, `SPI_BaudRatePrescaler_32`, `SPI_BaudRatePrescaler_64`, `SPI_BaudRatePrescaler_128`, `SPI_BaudRatePrescaler_256`（不同的预分频因子，影响SPI的实际时钟频率）。

8. **`SPI_FirstBit`**:
   
   `SPI_FirstBit = SPI_FirstBit_MSB;`
   
   - **SPI_FirstBit_MSB**: 数据传输时高位在前。
   - 其他可选项：`SPI_FirstBit_LSB`（数据传输时低位在前）。

9. **`SPI_CRCPolynomial`**:
   
   `SPI_CRCPolynomial = 7;`
   
   - **SPI_CRCPolynomial**: CRC的多项式值，用于数据的完整性校验。
- 使用SPI发送和介绍一个字节的数据

初始化好SPI外设后，就可以使用SPI通讯了，复杂的数据通讯都是由单个字节数据收发组成的， 我们看看它的代码实现

```c
#define Dummy_Byte 0xFF
// 使用SPI发送一个字节的数据
u8 SPI_FLASH_SendByte(u8 byte)
{
    SPITimeout = SPIT_FLAG_TIMEOUT; // 计数器

    // 等待发送缓冲区为空，TXE事件
    while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET)
    {
        if((SPITimeout--) == 0) // 超过延时时间
            return SPI_TIMEOUT_UserCallback(0);
    }

    // 写入数据寄存器，把要写入的数据写入发送缓冲区
    SPI_I2S_SendData(FLASH_SPIx, byte);
    SPITimeout = SPIT_FLAG_TIMEOUT;

    // 等待接收缓冲区非空，RXNE事件 
    while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if ((SPITimeout--) == 0) 
            return SPI_TIMEOUT_UserCallback(1);
    }
    // 读取数据寄存器，获取接收缓冲区数据
    return SPI_I2S_ReceiveData(FLASH_SPIx);
}

// 使用SPI读取一个字节的数据
u8 SPI_FLASH_ReadByte(void)
{
    return(SPI_FLASH_SendByte(Dummy_Byte));
}
```

SPI_FLASH_SendByte发送单字节函数中包含了等待事件的超时处理，这部分原理跟I2C中的一样，在此不再赘述。

SPI_FLASH_SendByte函数实现了前面讲解的“SPI通讯过程”：

(1) 本函数中不包含SPI起始和停止信号，只是收发的主要过程， 所以在调用本函数前后要做好起始和停止信号的操作；

(2) 对SPITimeout变量赋值为宏SPIT_FLAG_TIMEOUT。这个SPITimeout变量在下面的while循环中每次循环减1， 该循环通过调用库函数SPI_I2S_GetFlagStatus检测事件，若检测到事件，则进入通讯的下一阶段， 若未检测到事件则停留在此处一直检测，当检测SPIT_FLAG_TIMEOUT次都还没等待到事件则认为通讯失败， 调用的SPI_TIMEOUT_UserCallback输出调试信息，并退出通讯；

(3) 通过检测TXE标志，获取发送缓冲区的状态，若发送缓冲区为空， 则表示可能存在的上一个数据已经发送完毕；

(4) 等待至发送缓冲区为空后，调用库函数SPI_I2S_SendData把要发送的数据“byte”写入到SPI的数据寄存器DR， 写入SPI数据寄存器的数据会存储到发送缓冲区，由SPI外设发送出去；

(5) 写入完毕后等待RXNE事件，即接收缓冲区非空事件。 由于SPI双线全双工模式下MOSI与MISO数据传输是同步的(请对比“SPI通讯过程”阅读)，当接收缓冲区非空时， 表示上面的数据发送完毕，且接收缓冲区也收到新的数据；

(6) 等待至接收缓冲区非空时，通过调用库函数SPI_I2S_ReceiveData读取SPI的数据寄存器DR，就可以获取接收缓冲区中的新数据了。 代码中使用关键字“return”把接收到的这个数据作为SPI_FLASH_SendByte函数的返回值， 所以我们可以看到在下面定义的SPI接收数据函数SPI_FLASH_ReadByte，它只是简单地调用了SPI_FLASH_SendByte函数发送数据“Dummy_Byte”， 然后获取其返回值(因为不关注发送的数据，所以此时的输入参数“Dummy_Byte”可以为任意值)。 可以这样做的原因是SPI的接收过程和发送过程实质是一样的，收发同步进行，关键在于我们的上层应用中，关注的是发送还是接收的数据。

- 控制FLASH的指令

搞定SPI的基本收发单元后，还需要了解如何对FLASH芯片进行读写。FLASH芯片自定义了很多指令， 我们通过控制STM32利用SPI总线向FLASH芯片发送指令，FLASH芯片收到后就会执行相应的操作。

而这些指令，对主机端(STM32)来说，只是它遵守最基本的SPI通讯协议发送出的数据，但在设备端(FLASH芯片)把这些数据解释成不同的意义， 所以才成为指令。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI04.png)

该表中的第一列为指令名，第二列为指令编码，第三至第N列的具体内容根据指令的不同而有不同的含义。 其中带括号的字节参数，方向为FLASH向主机传输，即命令响应，不带括号的则为主机向FLASH传输。 表中“A0~A23”指FLASH芯片内部存储器组织的地址；“M0~M7”为厂商号（MANUFACTURERID）； “ID0-ID15”为FLASH芯片的ID；“dummy”指该处可为任意数据；“D0~D7”为FLASH内部存储矩阵的内容。

通过指令表中的读ID指令“JEDEC ID”可以获取这两个编号，该指令编码为“9F h”， 其中“9F h”是指16进制数“9F” (相当于C语言中的0x9F)。 紧跟指令编码的三个字节分别为FLASH芯片输出的“(M7-M0)”、“(ID15-ID8)”及“(ID7-ID0)” 。

此处我们以该指令为例，配合其指令时序图进行讲解，见图：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI009.jpeg)

主机首先通过MOSI线向FLASH芯片发送第一个字节数据为“9F h”，当FLASH芯片收到该数据后，它会解读成主机向它发送了“JEDEC指令”， 然后它就作出该命令的响应：通过MISO线把它的厂商ID(M7-M0)及芯片类型(ID15-0)发送给主机，主机接收到指令响应后可进行校验。 常见的应用是主机端通过读取设备ID来测试硬件是否连接正常，或用于识别设备。

对于FLASH芯片的其它指令，都是类似的，只是有的指令包含多个字节，或者响应包含更多的数据。

实际上，编写设备驱动都是有一定的规律可循的。首先我们要确定设备使用的是什么通讯协议。如上一章的EEPROM使用的是I2C， 本章的FLASH使用的是SPI。那么我们就先根据它的通讯协议，选择好STM32的硬件模块，并进行相应的I2C或SPI模块初始化。 接着，我们要了解目标设备的相关指令，因为不同的设备，都会有相应的不同的指令。如EEPROM中会把第一个数据解释为内部存储矩阵的地址(实质就是指令)。 而FLASH则定义了更多的指令，有写指令，读指令，读ID指令等等。最后，我们根据这些指令的格式要求，使用通讯协议向设备发送指令，达到控制设备的目标。

- 定义FLASH指令编码表

为了方便使用，我们把FLASH芯片的常用指令编码使用宏来封装起来，后面需要发送指令编码的时候我们直接使用这些宏即可

```c
/*FLASH常用命令*/
#define W25X_WriteEnable                0x06
#define W25X_WriteDisable               0x04
#define W25X_ReadStatusReg              0x05
#define W25X_WriteStatusReg             0x01
#define W25X_ReadData                   0x03
#define W25X_FastReadData               0x0B
#define W25X_FastReadDual               0x3B
#define W25X_PageProgram                0x02
#define W25X_BlockErase                 0xD8
#define W25X_SectorErase                0x20
#define W25X_ChipErase                  0xC7
#define W25X_PowerDown                  0xB9
#define W25X_ReleasePowerDown           0xAB
#define W25X_DeviceID                   0xAB
#define W25X_ManufactDeviceID           0x90
#define W25X_JedecDeviceID              0x9F
/*其它*/
#define  sFLASH_ID                      0XEF4017
#define Dummy_Byte                      0xFF
```

- 读取FLASH芯片ID

根据“JEDEC”指令的时序，我们把读取FLASH ID的过程编写成一个函数

```c
// 读取FLASH ID
u32 SPI_FLASH_ReadID(void)
{
    u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
    // 开始通讯：CS低电平
    SPI_FLASH_CS_LOW();
    // 发送JEDEC指令，读取ID
    SPI_FLASH_SendByte(W25X_JedecDeviceID);
    // 读取一个字节数据
    Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
    // 读取一个字节数据 
    Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
    // 读取一个字节数据 
    Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
    // 停止通讯：CS高电平 
    SPI_FLASH_CS_HIGH();
    // 把数据组合起来，作为函数的返回值
    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return Temp;
}
```

这段代码利用控制CS引脚电平的宏“SPI_FLASH_CS_LOW/HIGH”以及前面编写的单字节收发函数SPI_FLASH_SendByte， 很清晰地实现了“JEDEC ID”指令的时序：发送一个字节的指令编码“W25X_JedecDeviceID”，然后读取3个字节， 获取FLASH芯片对该指令的响应，最后把读取到的这3个数据合并到一个变量Temp中，然后作为函数返回值， 把该返回值与我们定义的宏“sFLASH_ID”对比，即可知道FLASH芯片是否正常。

- FLASH写使能以及读取当前状态

在向FLASH芯片存储矩阵写入数据前，首先要使能写操作，通过“Write Enable”命令即可写使能

```c
// 向FLASH发送 写使能 命令
void SPI_FLASH_WriteEnable(void)
{
    // 通讯开始：CS低 
    SPI_FLASH_CS_LOW();
    // 发送写使能命令
    SPI_FLASH_SendByte(W25X_WriteEnable);
    // 通讯结束：CS高 
    SPI_FLASH_CS_HIGH();
}
```

与EEPROM一样，由于FLASH芯片向内部存储矩阵写入数据需要消耗一定的时间，并不是在总线通讯结束的一瞬间完成的， 所以在写操作后需要确认FLASH芯片“空闲”时才能进行再次写入。为了表示自己的工作状态，FLASH芯片定义了一个状态寄存器

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI010.jpeg)

我们只关注这个状态寄存器的第0位“BUSY”，当这个位为“1”时，表明FLASH芯片处于忙碌状态， 它可能正在对内部的存储矩阵进行“擦除”或“数据写入”的操作。

利用指令表中的“Read Status Register”指令可以获取FLASH芯片状态寄存器的内容， 其时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI011.jpeg)

只要向FLASH芯片发送了读状态寄存器的指令，FLASH芯片就会持续向主机返回最新的状态寄存器内容， 直到收到SPI通讯的停止信号。据此我们编写了具有等待FLASH芯片写入结束功能的函数

```c
// WIP(busy)标志，FLASH内部正在写入
#define WIP_Flag                  0x01

// 等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
void SPI_FLASH_WaitForWriteEnd(void)
{
    u8 FLASH_Status = 0; // 标志位
    // 选择 FLASH: CS 低
    SPI_FLASH_CS_LOW();
    // 发送 读状态寄存器 命令
    SPI_FLASH_SendByte(W25X_ReadStatusReg);

    // 若FLASH忙碌，则等待
    do
    {
        // 读取FLASH芯片的状态寄存器
        FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
    }while((FLASH_Status & WIP_Flag) == SET);  // 正在写入标志

    // 停止信号  FLASH: CS 高 
    SPI_FLASH_CS_HIGH();
}
```

这段代码发送读状态寄存器的指令编码“W25X_ReadStatusReg”后，在while循环里持续获取寄存器的内容并检验它的“WIP_Flag标志”(即BUSY位)， 一直等待到该标志表示写入结束时才退出本函数，以便继续后面与FLASH芯片的数据通讯。

- FLASH扇区删除

由于FLASH存储器的特性决定了它只能把原来为“1”的数据位改写成“0”，而原来为“0”的数据位不能直接改写为“1”。 所以这里涉及到数据“擦除”的概念，在写入前，必须要对目标存储矩阵进行擦除操作，把矩阵中的数据位擦除为“1”， 在数据写入的时候，如果要存储数据“1”，那就不修改存储矩阵，在要存储数据“0”时，才更改该位。

通常，对存储矩阵擦除的基本操作单位都是多个字节进行，如本例子中的FLASH芯片支持“扇区擦除”、“块擦除”以及“整片擦除”

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI06.png)

FLASH芯片的最小擦除单位为扇区(Sector)，而一个块(Block)包含16个扇区， 其内部存储矩阵分布见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI012.png)

使用扇区擦除指令可控制FLASH芯片开始擦写， 其指令时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI013.png)

扇区擦除指令的第一个字节为指令编码，紧接着发送的3个字节用于表示要擦除的存储矩阵地址。要注意的是在扇区擦除指令前， 还需要先发送“写使能”指令，发送扇区擦除指令后，通过读取寄存器状态等待扇区擦除操作完毕

```c
// 擦除FLASH扇区
void SPI_FLASH_SectorErase(u32 SectorAddr) // 函数参数：要擦除数据的地址
{
    // 发送FLASH写使能命令
    SPI_FLASH_WriteEnable();
    SPI_FLASH_WaitForWriteEnd(); // 等待空闲
    // 擦除扇区 
    // 选择FLASH: CS低电平
    SPI_FLASH_CS_LOW();
    // 发送扇区擦除指令
    SPI_FLASH_SendByte(W25X_SectorErase);
    // 发送擦除扇区地址的高位
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    // 发送擦除扇区地址的中位
    SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    // 发送擦除扇区地址的低位
    SPI_FLASH_SendByte(SectorAddr & 0xFF);
    // 停止信号 FLASH: CS 高电平
    SPI_FLASH_CS_HIGH();
    // 等待擦除完毕
    SPI_FLASH_WaitForWriteEnd();
}
```

这段代码调用的函数在前面都已讲解，只要注意发送擦除地址时高位在前即可。调用扇区擦除指令时注意输入的地址要对齐到4KB。

- FLASH的页写入

目标扇区被擦除完毕后，就可以向它写入数据了。与EEPROM类似，FLASH芯片也有页写入命令， 使用页写入命令最多可以一次向FLASH传输256个字节的数据，我们把这个单位为页大小。 FLASH页写入的时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI014.jpeg)

从时序图可知，第1个字节为“页写入指令”编码，2-4字节为要写入的“地址A”，接着的是要写入的内容，最多个可以发送256字节数据， 这些数据将会从“地址A”开始，按顺序写入到FLASH的存储矩阵。若发送的数据超出256个，则会覆盖前面发送的数据。

与擦除指令不一样，页写入指令的地址并不要求按256字节对齐，只要确认目标存储单元是擦除状态即可(即被擦除后没有被写入过)。 所以，若对“地址x”执行页写入指令后，发送了200个字节数据后终止通讯，下一次再执行页写入指令，从“地址(x+200)”开始写入200个字节也是没有问题的(小于256均可)。 只是在实际应用中由于基本擦除单元是4KB，一般都以扇区为单位进行读写

把页写入时序封装成函数：

```c
// 对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) // 函数参数：数据指针，写入地址，数据长度
{
    // 发送FLASH写使能命令
    SPI_FLASH_WriteEnable();
    // 选择FLASH: CS低电平 
    SPI_FLASH_CS_LOW();
    // 写送写指令
    SPI_FLASH_SendByte(W25X_PageProgram);
    // 发送写地址的高位
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    // 发送写地址的中位
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    // 发送写地址的低位
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PerWritePageSize) // 如果数据过长
    {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
        FLASH_ERROR("SPI_FLASH_PageWrite too large!");
    }
    // 开始写入数据
    while (NumByteToWrite--)
    {
        // 发送当前要写入的字节数据 
        SPI_FLASH_SendByte(*pBuffer);
        // 指向下一字节数据 
        pBuffer++;
    }
    // 停止信号 FLASH: CS 高电平
    SPI_FLASH_CS_HIGH();
    // 等待写入完毕
    SPI_FLASH_WaitForWriteEnd();
}
```

这段代码的内容为：先发送“写使能”命令，接着才开始页写入时序，然后发送指令编码、地址，再把要写入的数据一个接一个地发送出去， 发送完后结束通讯，检查FLASH状态寄存器，等待FLASH内部写入结束。

- 不定量数据写入

应用的时候我们常常要写入不定量的数据，直接调用“页写入”函数并不是特别方便，所以我们在它的基础上编写了“不定量数据写入”的函数

```c
// 对FLASH不定量写入数据，调用本函数写入数据前需要先擦除扇区
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    // mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0
    Addr = WriteAddr % SPI_FLASH_PageSize;

    // 差count个数据值，刚好可以对齐到页地址
    count = SPI_FLASH_PageSize - Addr;
    // 计算出要写多少整数页
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    // mod运算求余，计算出剩余不满一页的字节数
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
    if (Addr == 0)
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr,
                                NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*先把整数页都写了*/
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr,
                                    SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            SPI_FLASH_PageWrite(pBuffer, WriteAddr,
                                NumOfSingle);
        }
    }
    /* 若地址与 SPI_FLASH_PageSize 不对齐  */
    else
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            /*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
            if (NumOfSingle > count)
            {
                temp = NumOfSingle - count;
                /*先写满当前页*/
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);

                WriteAddr +=  count;
                pBuffer += count;
                /*再写剩余的数据*/
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr,
                                    NumByteToWrite);
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*地址不对齐多出的count分开处理，不加入这个运算*/
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            /* 先写完count个数据，为的是让下一次要写的地址对齐 */
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);

            /* 接下来就重复地址对齐的情况 */
            WriteAddr +=  count;
            pBuffer += count;
            /*把整数页都写了*/
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr,
                                    SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            if (NumOfSingle != 0)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr,
                                    NumOfSingle);
            }
        }
    }
}
```

这段代码与EEPROM章节中的“快速写入多字节”函数原理是一样的，运算过程在此不再赘述。区别是页的大小以及实际数据写入的时候， 使用的是针对FLASH芯片的页写入函数，且在实际调用这个“不定量数据写入”函数时，还要注意确保目标扇区处于擦除状态。

这个函数我们简单理解就行了，学会调用才是王道，不过我们也可以稍微解释一下使该教程更加丰富：

1. **变量定义：**
   
   - `NumOfPage`：要写入的完整页数。
   - `NumOfSingle`：页外剩余的字节数。
   - `Addr`：写入地址在页内的偏移量。
   - `count`：当前页剩余的空间。
   - `temp`：临时变量，用于计算和处理页外的剩余数据。

2. **计算页对齐和剩余空间：**
   
   - `Addr`：通过`WriteAddr % SPI_FLASH_PageSize`计算，表示写入地址在当前页的偏移量。
   - `count`：当前页剩余的空间，`SPI_FLASH_PageSize - Addr`。
   - `NumOfPage`：可以写入的完整页数，`NumByteToWrite / SPI_FLASH_PageSize`。
   - `NumOfSingle`：剩余的字节数，不足一页的部分，`NumByteToWrite % SPI_FLASH_PageSize`。

3. **处理页对齐的情况：**
   
   - 如果`Addr == 0`（地址对齐）：
     - 若数据小于一页（`NumOfPage == 0`），直接写入。
     - 若数据大于一页：
       - 先写入完整的页。
       - 然后处理剩余不足一页的数据。

4. **处理页不对齐的情况：**
   
   - 如果`Addr != 0`（地址不对齐）：
     - 如果数据小于一页：
       - 如果剩余空间不足以容纳剩余数据，先写满当前页，再写剩余的数据。
       - 否则直接写入。
     - 如果数据大于一页：
       - 先处理当前页剩余的空间（`count`）。
       - 写入完整的页数据。
       - 处理剩余的数据。

5. **写入函数 `SPI_FLASH_PageWrite`**：
   
   - 此函数负责实际写入数据到SPI Flash中，每次写入一页数据或少于一页的数据。
- 从FLASH读取数据

相对于写入，FLASH芯片的数据读取要简单得多，使用读取指令“Read Data”即可， 其指令时序见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPI015.jpeg)

发送了指令编码及要读的起始地址后，FLASH芯片就会按地址递增的方式返回存储矩阵的内容，读取的数据量没有限制， 只要没有停止通讯，FLASH芯片就会一直返回数据

```c
// 读取FLASH数据
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    // 选择FLASH: CS低电平
    SPI_FLASH_CS_LOW();
    // 发送 读 指令 
    SPI_FLASH_SendByte(W25X_ReadData);
    // 发送 读 地址高位 
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    // 发送 读 地址中位 
    SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    // 发送 读 地址低位 
    SPI_FLASH_SendByte(ReadAddr & 0xFF);
    // 读取数据 
    while (NumByteToRead--)
    {
        // 读取一个字节
        *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
        // 指向下一个字节缓冲区
        pBuffer++;
    }
    // 停止信号 FLASH: CS 高电平 
    SPI_FLASH_CS_HIGH();
}
```

由于读取的数据量没有限制，所以发送读命令后一直接收NumByteToRead个数据到结束即可。

- 主函数

最后我们来编写main函数，进行FLASH芯片读写校验

```c
int main(void)
{
    LED_GPIO_Config();
    LED_BLUE;

    // 配置串口1为：115200 8-N-1
    USART_Config();
    printf("\r\n 这是一个8Mbyte串行flash(W25Q64)实验 \r\n");

    // 8M串行flash W25Q64初始化
    SPI_FLASH_Init();

    // 获取 Flash Device ID 
    DeviceID = SPI_FLASH_ReadDeviceID();
    Delay(200);

    // 获取 SPI Flash ID
    FlashID = SPI_FLASH_ReadID();
    printf("\r\n FlashID is 0x%X,\
    Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

    // 检验 SPI Flash ID 
    if (FlashID == sFLASH_ID)
    {
        printf("\r\n 检测到串行flash W25Q64 !\r\n");

        /* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
        // 这里擦除4K，即一个扇区，擦除的最小单位是扇区
        SPI_FLASH_SectorErase(FLASH_SectorToErase);

        /* 将发送缓冲区的数据写到flash中 */
        // 这里写一页，一页的大小为256个字节
        SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
        printf("\r\n 写入的数据为：%s \r\t", Tx_Buffer);

        /* 将刚刚写入的数据读出来放到接收缓冲区中 */
        SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
        printf("\r\n 读出的数据为：%s \r\n", Rx_Buffer);

        /* 检查写入的数据与读出的数据是否相等 */
        TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

        if ( PASSED == TransferStatus1 )
        {
            LED_GREEN;
            printf("\r\n 8M串行flash(W25Q64)测试成功!\n\r");
        }
        else
        {
            LED_RED;
            printf("\r\n 8M串行flash(W25Q64)测试失败!\n\r");
        }
    }// if (FlashID == sFLASH_ID)
    else// if (FlashID == sFLASH_ID)
    {
        LED_RED;
        printf("\r\n 获取不到 W25Q64 ID!\n\r");
    }

    while (1);
}
```

函数中初始化了LED、串口、SPI外设，然后读取FLASH芯片的ID进行校验，若ID校验通过则向FLASH的特定地址写入测试数据， 然后再从该地址读取数据，测试读写是否正常。

## 3. 小结

总得来说这一章不是很难，就是一个编写驱动函数的过程，在小结里，我们简化整个过程再来回顾一下：

1. **初始化SPI接口**：
   
   - 配置SPI外设的工作模式、数据宽度、时钟极性和相位等。
   - 启动SPI外设。

2. **发送和接收SPI数据**：
   
   - 使用STM32的SPI库函数或直接访问寄存器来进行数据传输。

3. **控制Flash操作**：
   
   - 根据Flash存储器的指令集执行读、写、擦除等操作。

以下是一个简单的示例代码，展示了如何在STM32上使用SPI接口与一个典型的SPI Flash存储器进行通信。这些操作包括读取Flash ID、写入数据、读取数据等。

### 1. 初始化SPI接口

```c
#include "stm32f4xx_hal.h"

SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init(&hspi1);
}
```

### 2. SPI Flash命令

以下是与SPI Flash进行通信的一些常见命令：

- 读数据命令：`0x03`
- 写数据命令：`0x02`
- 读取ID命令：`0x9F`

### 3. 读Flash ID

```c
uint8_t SPI_FLASH_ReadID(void)
{
    uint8_t id[3];
    uint8_t cmd = 0x9F; // 读取ID命令

    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, id, 3, HAL_MAX_DELAY);

    return id[0]; // 返回ID的第一个字节
}
```

### 4. 写入数据到Flash

```c
void SPI_FLASH_WriteData(uint32_t addr, uint8_t *pData, uint16_t size)
{
    uint8_t cmd[4];
    cmd[0] = 0x02; // 写数据命令
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, pData, size, HAL_MAX_DELAY);
}
```

### 5. 读取数据从Flash

```c
void SPI_FLASH_ReadData(uint32_t addr, uint8_t *pData, uint16_t size)
{
    uint8_t cmd[4];
    cmd[0] = 0x03; // 读数据命令
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, pData, size, HAL_MAX_DELAY);
}
```

### 6. 主程序示例

```c
int main(void)
{
    HAL_Init();
    MX_SPI1_Init();

    uint8_t id = SPI_FLASH_ReadID();
    // 处理ID

    uint8_t writeData[256] = {0};
    uint8_t readData[256] = {0};

    // 写入数据
    SPI_FLASH_WriteData(0x000000, writeData, sizeof(writeData));

    // 读取数据
    SPI_FLASH_ReadData(0x000000, readData, sizeof(readData));

    while (1)
    {
        // 主循环
    }
}
```

---

2024.9.5 第一次修订，后期不再维护
