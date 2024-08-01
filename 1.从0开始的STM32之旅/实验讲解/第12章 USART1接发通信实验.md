# 第十二章 USART1接发通信实验

## 1. 硬件设计

为利用USART实现开发板与电脑通信，需要用到一个USB转USART的IC，我们选择CH340G芯片来实现这个功能，CH340G是一个USB总线的转接芯片， 实现USB转USART、USB转lrDA红外或者USB转打印机接口，我们使用其USB转USART功能。具体电路设计见图 [USB转串口硬件设计](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/USART.html#usb) 。

我们将CH340G的TXD引脚与USART1的RX引脚连接，CH340G的RXD引脚与USART1的TX引脚连接。CH340G芯片集成在开发板上，其地线(GND)已与控制器的GND连通。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART011.png)

## 2. 软件设计

- USART1头文件配置-设置端口、引脚、中断

```c
// 串口1-USART1
#define  DEBUG_USARTx                   USART1                 // 使用USART1  
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1  // 串口1时钟
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd // 使能串口1时钟
#define  DEBUG_USART_BAUDRATE           115200                 // 串口1波特率    

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA) // APB2
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd // 使能GPIO时钟

// TX-发送数据输出引脚定义配置
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA                  // 输出端口GPIOA
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9             // 输出引脚-PA9

// RX-接收数据输入引脚定义配置
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA                  // 输入端口GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10            // 输入引脚-PA10

// 配置NVIC中断
#define  DEBUG_USART_IRQ                USART1_IRQn            // 串口1中断号
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler      // 串口1中断服务函数
```

- 配置NVIC

```c
// 配置嵌套向量中断控制器NVIC
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; // 定义NVIC初始化结构体变量

  // 1.嵌套向量中断控制器组选择
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  // 2.配置USART为中断源
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;

  // 3.设置抢占优先级和子优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

  // 4.使能中断
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  // 5.初始化配置NVIC
  NVIC_Init(&NVIC_InitStructure);
}
```

- 配置USART GPIO工作参数设置

```c
// USART GPIO 配置,工作参数配置
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体变量
    USART_InitTypeDef USART_InitStructure; // 定义USART初始化结构体变量

    // 1.打开串口GPIO的时钟
    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    // 2.打开串口外设的时钟
    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    // 3.将USART Tx的GPIO配置为推挽复用模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // 使用GPIO_Pin_9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 速度为50MHz

    // 4.初始化GPIO结构体
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); 

    // 5.将USART Rx的GPIO配置为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // 使用GPIO_Pin_10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入模式

    // 6.初始化GPIO结构体
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // 配置串口的工作参数
    // 7.配置波特率
    USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 波特率为115200

    // 8.配置数据字长
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位

    // 9.配置停止位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1位停止位

    // 10.配置校验位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位

    // 11.配置硬件流控制
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制

    // 12.配置工作模式
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发一起工作

    // 13.完成串口的初始化配置
    USART_Init(DEBUG_USARTx, &USART_InitStructure);

    // 串口中断优先级配置
    NVIC_Configuration();
    // 使能串口接收中断
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);    
    // 使能串口
    USART_Cmd(DEBUG_USARTx, ENABLE);        
}
```

- 首先编写发送一个字节（8位）函数

```c
// 发送一个字节函数
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch) // 函数参数：串口号、待发送的字节
{
    // 发送一个字节数据到USART
    USART_SendData(pUSARTx, ch);

    // 等待发送完成
    whbie (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);    
}
```

- 编写发送一个8位数组函数

```c
// 发送一个8位的数组函数
void Usart_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num) // 函数参数：串口号、待发送的数组、数组长度
{
  uint8_t i;

    for(i = 0; i < num; i++)
      {
        // 发送一个字节数据到USART并且通过循环写入到数组中
        Usart_SendByte(pUSARTx, array[i]);    
      }

    // 等待发送完成
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}
```

- 发送一个字符串函数

```c
// 发送一个字符串函数
void Usart_SendString( USART_TypeDef *pUSARTx, char *str) // 函数参数：串口号、待发送的字符串
{
  unsigned int k=0;
  do 
  {
    Usart_SendByte( pUSARTx, *(str + k) ); // 发送一个字节数据到USART
    k++;
  } while(*(str + k)!='\0'); // 直到字符串结束

  // 等待发送完成
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET)
  {

  }
}
```

- 发送一个16位数函数

```c
// 发送一个16位数函数
void Usart_SendHalfWord( USART_TypeDef *pUSARTx, uint16_t ch) // 函数参数：串口号、待发送的16位数
{
    uint8_t temp_h, temp_l;

    // 1.取高八位
    temp_h = (ch&0XFF00) >> 8;
    // 2.取出低八位
    temp_l = ch&0XFF;

    // 3.发送高八位
    USART_SendData(pUSARTx, temp_h);    
    // 4.等待发送完成
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

    // 5.发送低八位
    USART_SendData(pUSARTx,temp_l);    
    // 6.等待发送完成
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);    
}
```

- 重定向printf和scanf函数到串口-目的是让串口也可以使用

```c
// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f) // 函数参数：待发送的字符、文件指针
{
        // 发送一个字节数据到串口
        USART_SendData(DEBUG_USARTx, (uint8_t)ch);

        // 等待发送完毕
        while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);        

        // 返回发送的字符
        return (ch);
}

// 重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f) // 函数参数：文件指针
{
        // 等待串口输入数据
        while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

        // 返回接收到的数据
        return (int)USART_ReceiveData(DEBUG_USARTx);
}
```

## 3. 小结

注释已经写得很详细了，我看过一些教程也都没我写得这样细致。

不过，既然是小结，我们还得总结一下基本流程吧：

- 首先就算USART1的头文件了，我们需要在这里宏定义各种串口需要的资源（GPIO、RCC、NVIC）

- 接着写我们的串口函数了，首先就算初始化NVIV和GPIO了，接着再写我们的功能函数

- 主函数就简单了，直接引用我们已经写好的函数就ok咯

```c
#include "stm32f10x.h"
#include "bsp_usart.h"

int main(void)
{    
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  USART_Config();

    /* 发送一个字符串 */
    Usart_SendString(DEBUG_USARTx, "这是一个串口中断接收回显实验\n");
    printf("欢迎使用野火STM32开发板\n\n\n\n");

  // 下面是我们写好的函数

  // Usart_SendByte() 函数参数：USARTx 串口号，发送的字节
  // Usart_SendArray() 函数参数：USARTx 串口号，发送的数组，数组长度
  // Usart_SendString() 函数参数：USARTx 串口号，发送的字符串
  // Usart_SendHalfWord() 函数参数：USARTx 串口号，发送的半字

  while(1)
    {    

    }    
}
```
