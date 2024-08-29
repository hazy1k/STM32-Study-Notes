# 第十七章 DMA外设到存储器实验

## 1. 硬件设计

外设到存储器使用到USART1功能，具体电路设置参考USART章节，无需其他硬件设计。

## 2. 软件设计

### 2.1 编程目的

- 配置USART通信功能；

- 设置串口DMA工作参数；

- 使能DMA；

- DMA传输同时CPU可以运行其他任务。

### 2.2 代码分析

- 串口配置宏定义

```c
// 串口工作参数宏定义
#define  DEBUG_USARTx                   USART1                 // 使用USART1作为调试串口 
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1  // USART1时钟
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd // USART1时钟使能函数
#define  DEBUG_USART_BAUDRATE           115200                 // 波特率

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA) // GPIO时钟
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd // GPIO时钟使能函数

#define  DEBUG_USART_TX_GPIO_PORT       GPIOA                  // 串口发送GPIO端口
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9             // 串口发送GPIO引脚
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA                  // 串口接收GPIO端口
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10            // 串口接收GPIO引脚

#define  DEBUG_USART_IRQ                USART1_IRQn            // USART1中断源
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler      // USART1中断处理函数

// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel5                // 使用通道5  
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)            // USART数据寄存器地址
// 一次发送的数据量
#define  RECEIVEBUFF_SIZE            5000
```

- 串口配置

```c
// 串口配置函数
void USART_Config(void)
{
    // 结构体定义
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1.打开串口GPIO的时钟
    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
    // 2.打开串口外设的时钟
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
    // 3.配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置抢占优先级2
    NVIC_InitStruct.NVIC_IRQChannel = DEBUG_USART_IRQ; // 设置中断源
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3; // 响应优先级
    NVIC_Init(&NVIC_InitStruct); // 初始化NVIC
    // 4.将USART Tx的GPIO配置为推挽复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // 设置Tx引脚-PA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 设置复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 设置GPIO速度为50MHz
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIO
    // 5.将USART Rx的GPIO配置为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // 设置Rx引脚-PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 设置浮空输入
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIO

    // 配置串口的工作参数
    // 1.配置波特率
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 设置波特率115200
    // 2.配置数据字长
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 设置数据字长8位
    // 3.配置停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 设置停止位1
    // 4.配置校验位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 设置无校验位
    // 5.配置硬件流控制
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 设置无硬件流控制
    // 6.配置工作模式，收发一起
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 设置收发一起工作模式
    // 7.完成串口的初始化配置
    USART_Init(DEBUG_USARTx, &USART_InitStructure); // 初始化串口
    // 8.使能空闲中断
    USART_ITConfig(DEBUG_USARTx,USART_IT_IDLE,ENABLE);
    // 9.使能串口
    USART_Cmd(DEBUG_USARTx, ENABLE);        
}
```

- DMA配置

```c
// USARTx TX DMA 配置，内存到外设(USART1->DR)
void USARTx_DMA_Config(void)
{
        DMA_InitTypeDef DMA_InitStructure;
        // 开启DMA时钟
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        // 设置DMA源地址：串口数据寄存器地址*/
        DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
        // 内存地址(要传输的变量的指针)
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff;
        // 方向：从外设到内存    
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        // 传输大小    
        DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE;
        // 外设地址不增        
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        // 内存地址自增
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        // 外设数据单位    
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        // 内存数据单位
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;     
        // DMA模式，一次或者循环模式
//        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式
        // 优先级：中    
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
        // 禁止内存到内存的传输
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        // 配置DMA通道           
        DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);        
        // 使能DMA
        DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}
```

- 主函数

```c
int main(void)
{
  USART_Config(); 
  USARTx_DMA_Config();
  LED_GPIO_Config();

  printf("\r\nDMA外设到存储器模式，用电脑向开发板串口发送数据，数据会返回到电脑。\r\n");

  // USART1 向 DMA发出RX请求
  USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE); // 函数参数：选用的串口，DMA请求，使能/禁止
  // 用电脑向开发板串口发送数据，数据会返回到电脑。
  while(1)
  {
    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}
```

## 3. 小结

这章功能就好像我们已经讲过的串口接发实验一样，但是使用DMA不会占用cpu资源，适合大型项目，更多内容参考注释。DMA实验也已经写了三章，再来复习一下配置吧，以这个实验为例：

DMA 从外设（如串口）到存储器的配置允许外设数据自动传输到内存中，无需 CPU 干预。以下是配置步骤和示例

### 配置步骤

1. **开启 DMA 时钟**：

```c
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
```

2. **配置 DMA 通道**：
   设置 DMA 外设地址（如串口的数据寄存器地址）、内存地址、数据方向等。

```c
DMA_InitTypeDef DMA_InitStructure;
DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR)); // 外设地址
DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff; // 内存地址
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // 外设到内存
DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE; // 数据缓冲区大小
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不增
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址增
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据大小
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式
DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 优先级
DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 仅外设到内存
```

3. **初始化 DMA**：

```c
DMA_Init(DMA1_Channel5, &DMA_InitStructure); // 初始化特定 DMA 通道
```

4. **使能 DMA**：

```c
DMA_Cmd(DMA1_Channel5, ENABLE);
```

5. **使能 USART DMA 接收**：

```c
USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
```

### 示例代码

假设你要将 USART1 的接收到的数据存储到 `ReceiveBuff` 缓冲区中：

```c
#include "stm32f10x.h"

#define RECEIVEBUFF_SIZE  256
uint8_t ReceiveBuff[RECEIVEBUFF_SIZE];

void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 使能 DMA 时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // DMA 配置
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR)); // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff; // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // 从外设到内存
    DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE; // 缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据大小
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 仅外设到内存
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // 使能 DMA
    DMA_Cmd(DMA1_Channel5, ENABLE);

    // 使能 USART DMA 接收
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

int main(void)
{
    // USART 和 DMA 配置
    USART_InitTypeDef USART_InitStructure;
    // 配置 USART1 (省略具体配置)
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    DMA_Config();

    while (1)
    {
        // 主循环
        // 你可以在这里处理接收到的数据
    }
}

```

---

2024.8.29 第一次修订，后期不再维护


