# 第十六章 DMA存储器到外设模式实验

## 1. 硬件设计

存储器到外设模式使用到USART1功能，具体电路设置参考USART章节，无需其他硬件设计。

## 2. 软件设计

### 2.1 编程目的

1. 配置USART通信功能；

2. 设置串口DMA工作参数；

3. 使能DMA；

4. DMA传输同时CPU可以运行其他任务。

### 2.2 代码分析

- 首先就是串口和DMA的宏定义啦

```c
// 串口工作参数宏定义
#define  DEBUG_USARTx                   USART1                 // 串口选择1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1  // 串口时钟
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd // 串口时钟使能函数
#define  DEBUG_USART_BAUDRATE           115200                 // 串口波特率

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA) // 串口GPIO时钟
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd // 串口GPIO时钟使能函数

#define  DEBUG_USART_TX_GPIO_PORT       GPIOA                  // 串口TX GPIO端口
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9             // 串口TX GPIO引脚
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA                  // 串口RX GPIO端口
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10            // 串口RX GPIO引脚

// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel4                // 使用DMA1通道4
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)            // USART数据寄存器地址
// 一次发送的数据量
#define  SENDBUFF_SIZE            5000                        // 一次发送的数据量
```

使用宏定义设置外设配置方便程序修改和升级。

USART部分设置与USART章节内容相同，可以参考USART章节内容理解。串口的发送请求对应有固定的DMA通道， 这里的外设的地址即串口的数据寄存器，一次要发送的数据量可以自定义，配置SENDBUFF_SIZE这个宏即可。

- 串口DMA传输配置

```c
// USARTx TX DMA 配置，内存到外设(USART1->DR)
void USARTx_DMA_Config(void)
{
        // 1.DMA初始化结构体
        DMA_InitTypeDef DMA_InitStructure;

        // 2.开启DMA时钟
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        // 3.设置DMA源地址：串口数据寄存器地址
        DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;

        // 4.设置内存地址(要传输的变量的指针)
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;

        // 5.设置方向：从内存到外设    
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

        // 6.传输大小    
        DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;

        // 7.外设地址不增        
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

        // 8.内存地址自增
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

        // 9.设置外设数据单位    
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

        // 10.内存数据单位
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     

        // 11.DMA模式，一次或者循环模式
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
        //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;    

        // 12.优先级：中    
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 

        // 13.禁止内存到内存的传输
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

        // 13.配置DMA通道           
        DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);    

        // 14.使能DMA
        DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}
```

首先定义一个DMA初始化变量，用来填充DMA的参数，然后使能DMA时钟。

因为数据是从存储器到串口，所以设置存储器为源地址，串口的数据寄存器为目标地址，要发送的数据有很多且都先存储在存储器中， 则存储器地址指针递增，串口数据寄存器只有一个，则外设地址地址不变，两边数据单位设置成一致，传输模式可选一次或者循环传输， 只有一个DMA请求，优先级随便设，最后调用DMA_Init函数把这些参数写到DMA的寄存器中，然后使能DMA开始传输.

- USART配置

```c
// USART1初始化配置
void USART_Config(void)
{
    // 前期准备工作
    // 1.定义GPIO初始化结构体变量
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 2.打开串口GPIO的时钟
    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    // 3.打开串口外设的时钟
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    // 4.将USART Tx的GPIO配置为推挽复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // 选择GPIO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 速度50MHz 
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIO口

    // 5.将USART Rx的GPIO配置为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // 选择GPIO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 速度50MHz 
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIO口

    // 下面开始配置串口的工作参数
    // 1.配置波特率
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 115200
    // 2.配置数据字长
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位
    // 3.配置停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位
    // 4.配置校验位
    USART_InitStructure.USART_Parity = USART_Parity_No ; // 无校验位
    // 5.配置硬件流控制
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    // 6.配置工作模式，收发一起
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发一起
    // 7.完成串口的初始化配置
    USART_Init(DEBUG_USARTx, &USART_InitStructure);    
    // 8.使能串口
    USART_Cmd(DEBUG_USARTx, ENABLE);        
}
```

这个我们应该很熟悉了，它完成USART初始化配置，包括GPIO初始化，USART通信参数设置等等

- 主函数

```c
#include "stm32f10x.h"
#include "bsp_usart_dma.h"
#include "bsp_led.h"

extern uint8_t SendBuff[SENDBUFF_SIZE];
static void Delay(__IO u32 nCount); 

int main(void)
{
  uint16_t i;
  USART_Config(); 
  USARTx_DMA_Config();
  LED_GPIO_Config();

  // 填充将要发送的数据
  for(i = 0; i < SENDBUFF_SIZE; i++)
  {
    SendBuff[i] = 'P';
  }

  /*为演示DMA持续运行而CPU还能处理其它事情，持续使用DMA发送数据，量非常大，
  *长时间运行可能会导致电脑端串口调试助手会卡死，鼠标乱飞的情况，
  *或把DMA配置中的循环模式改为单次模式*/        

  // USART1 向 DMA发出TX请求
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);

  // 此时CPU是空闲的，可以干其他的事情
  // 例如同时控制LED
  while(1)
  {
    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}

static void Delay(__IO uint32_t nCount)     //简单的延时函数
{
    for(; nCount != 0; nCount--);
}
```

使用for循环填充源数据，SendBuff[SENDBUFF_SIZE]是定义在bsp_usart_dma.c中的一个全局无符号8位整数数组，是DMA传输的源数据， 在USART_DMA_Config函数中已经被设置为存储器地址。

USART_DMACmd函数用于控制USART的DMA请求的启动和关闭。它接收三个参数，第一个参数用于设置串口外设，可以是USART1/2/3和UART4/5这5个参数可选， 第二个参数设置串口的具体DMA请求，有串口发送请求USART_DMAReq_Tx和接收请求USART_DMAReq_Rx可选， 第三个参数用于设置启动请求ENABLE或者关闭请求DISABLE。运行该函数后USART的DMA发送传输就开始了，根据配置存储器的数据会发送到串口。

DMA传输过程是不占用CPU资源的，可以一边传输一次运行其他任务。

## 3. 小结

有了上一章的基础，这一次应该更容易理解，对照注释清晰明了，不同的是我们这是内部存储到外设，改一下配置就行啦，其他无异

主函数里有一个库函数我们需要详细说明一下：

### 函数原型

```c
void USART_DMACmd(USART_TypeDef* USARTx, FunctionalState NewState);
```

### 参数解释

- **`USARTx`**: 这是 USART 外设的指针。例如，`USART1`、`USART2` 等。
- **`USART_DMAReq`**: 这是 DMA 请求的类型。通常有两种类型：`USART_DMAReq_Tx`（发送请求）和 `USART_DMAReq_Rx`（接收请求）。
- **`NewState`**: 指定要启用还是禁用 DMA 请求。可以是 `ENABLE`（启用）或 `DISABLE`（禁用）。

### 工作原理

1. **启用 DMA 请求**: 当启用 DMA 请求时，USART 在发送或接收数据时会触发 DMA 控制器，从而自动处理数据的传输，而不是由 CPU 直接进行传输。这使得数据传输更高效，特别是在大量数据传输时。

2. **禁用 DMA 请求**: 禁用 DMA 请求后，USART 将不再自动使用 DMA 进行数据传输，数据将由 CPU 直接处理。

### 示例代码

下面是一个简单的例子，展示如何启用 USART1 的 DMA 发送请求：

```c
// 使能 USART1 的 DMA 发送请求
USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

// 假设 USART1 已经被初始化并且 DMA 也配置好了
// 现在可以将数据通过 DMA 发送

// 发送数据
const uint8_t data[] = "Hello, DMA!";
DMA_ConfigureForUSART1Tx(data, sizeof(data)); // 这是一个假设的函数，用于配置 DMA 发送数据

// 启动 DMA 传输
DMA_Cmd(DMA1_Channel4, ENABLE); // 使能 DMA 通道，假设使用的是 DMA1 通道4
```

### 解释

1. **`USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);`**: 启用 USART1 的 DMA 发送请求。
2. **`DMA_ConfigureForUSART1Tx(data, sizeof(data));`**: 配置 DMA 以便它可以发送 `data` 数组中的数据。实际代码中，需要设置 DMA 的源地址、目的地址、传输长度等参数。
3. **`DMA_Cmd(DMA1_Channel4, ENABLE);`**: 启动 DMA 通道以开始数据传输。

---

2024.8.29 第一次修订，后期不在维护
