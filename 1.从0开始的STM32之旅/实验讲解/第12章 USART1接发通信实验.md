# 第十二章 USART1接发通信实验

## 1. 硬件设计

为利用USART实现开发板与电脑通信，需要用到一个USB转USART的IC，我们选择CH340G芯片来实现这个功能，CH340G是一个USB总线的转接芯片， 实现USB转USART、USB转lrDA红外或者USB转打印机接口，我们使用其USB转USART功能。

我们将CH340G的TXD引脚与USART1的RX引脚连接，CH340G的RXD引脚与USART1的TX引脚连接。CH340G芯片集成在开发板上，其地线(GND)已与控制器的GND连通。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/USART011.png)

## 2. 软件设计

### 2.1 编程目标

1. 使能RX和TX引脚GPIO时钟和USART时钟；

2. 初始化GPIO，并将GPIO复用到USART上；

3. 配置USART参数；

4. 配置中断控制器并使能USART接收中断；

5. 使能USART；

6. 在USART接收中断服务函数实现数据接收和发送。

### 2.2 代码分析

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

使用宏定义方便程序移植和升级 。开发板中的CH340G的收发引脚默认通过跳帽连接到USART1，如果想使用其他串口， 可以把CH340G跟USART1直接的连接跳帽拔掉，然后再把其他串口的IO用杜邦线接到CH340G的收发引脚即可。

这里我们使用USART1，设定波特率为115200，选定USART的GPIO为PA9和PA10。

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

在中断章节已对嵌套向量中断控制器的工作机制做了详细的讲解，这里我们就直接使用，配置USART作为中断源，因为本实验没有使用其他中断，对优先级没什么具体要求。

- 串口中断服务函数

```c
// 串口中断服务函数
void DEBUG_USART_IRQHandler(void)
{
  uint8_t ucTemp;
	if(USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(DEBUG_USARTx);
    USART_SendData(DEBUG_USARTx,ucTemp);    
	}	 
}
```

关于中断函数我们要详细说明一下，因为出现了新的库函数：

1. **中断服务函数定义**：
   
   - `DEBUG_USART_IRQHandler()` 是一个串口中断服务函数，用于处理指定的串口（如 `DEBUG_USARTx`）的中断事件。

2. **局部变量**：
   
   - `uint8_t ucTemp;`：定义了一个 8 位无符号整型变量，用于存储接收到的数据。

3. **中断检查**：
   
   - `if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET)`：
     - `USART_GetITStatus()` 函数用于检查指定 USART 模块的中断状态。`USART_IT_RXNE` 是接收数据寄存器非空中断的标志。
     - 如果接收到的数据寄存器非空中断被触发，函数返回值将不为 `RESET`，中断服务程序将继续执行。

4. **读取数据**：
   
   - `ucTemp = USART_ReceiveData(DEBUG_USARTx);`：
     - `USART_ReceiveData()` 函数从接收数据寄存器中读取一个字节的数据，并将其存储在 `ucTemp` 变量中。

5. **发送数据**：
   
   - `USART_SendData(DEBUG_USARTx, ucTemp);`：
     - `USART_SendData()` 函数将数据 `ucTemp` 发送到指定的 USART 发送数据寄存器。这通常用于回显收到的数据。

6. **等待数据发送完成**：
   
   - `while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TC) == RESET);`：
     - `USART_GetFlagStatus()` 函数用于检查指定的 USART 标志位。`USART_FLAG_TC` 是传输完成标志。
     - 这个 `while` 循环会阻塞，直到发送完成标志位 `USART_FLAG_TC` 被设置为 `SET`，即数据发送完成。这个操作确保数据完全发送出去。
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

使用GPIO_InitTypeDef和USART_InitTypeDef结构体定义一个GPIO初始化变量以及一个USART初始化变量，这两个结构体内容我们之前已经有详细讲解。

调用RCC_APB2PeriphClockCmd函数开启GPIO端口时钟，使用GPIO之前必须开启对应端口的时钟。使用RCC_APB2PeriphClockCmd函数开启USART时钟。

使用GPIO之前都需要初始化配置它，并且还要添加特殊设置，因为我们使用它作为外设的引脚，一般都有特殊功能。 我们在初始化时需要把它的模式设置为复用功能。这里把串口的Tx引脚配置为复用推挽输出，Rx引脚为浮空输入，数据完全由外部输入决定。

接下来，我们配置USART1通信参数为：波特率115200，字长为8，1个停止位，没有校验位，不使用硬件流控制，收发一体工作模式，然后调用USART初始化函数完成配置。

程序用到USART接收中断，需要配置NVIC，这里调用NVIC_Configuration函数完成配置。配置完NVIC之后调用USART_ITConfig函数使能USART接收中断。

最后调用USART_Cmd函数使能USART，这个函数最终配置的是USART_CR1的UE位，具体的作用是开启USART的工作时钟，没有时钟那USART这个外设自然就工作不了。

照例，我们应该分析一下新出现的库函数：

1. **`NVIC_Configuration();`**：
   
   - 这个函数通常用于配置嵌套向量中断控制器（NVIC）的中断优先级。通过设置中断优先级和中断使能状态，它控制哪个中断源的优先级更高，以及在中断请求时如何响应。

2. **`USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);`**：
   
   - `USART_ITConfig` 函数用于启用或禁用特定 USART 的中断。在这里，`USART_IT_RXNE` 表示接收数据寄存器非空中断，使能这个中断后，每当接收到新数据时，会触发中断请求。`ENABLE` 表示启用这个中断。

3. **`USART_Cmd(DEBUG_USARTx, ENABLE);`**：
   
   - `USART_Cmd` 函数用于使能或禁用指定的 USART 外设。`ENABLE` 参数表示启用 USART，使其开始工作，接收和发送数据。
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

Usart_SendByte函数用来在指定USART发送一个ASCLL码值字符，它有两个形参，第一个为USART，第二个为待发送的字符。 它是通过调用库函数USART_SendData来实现的，并且增加了等待发送完成功能。 通过使用USART_GetFlagStatus函数来获取USART事件标志来实现发送完成功能等待，它接收两个参数，一个是USART， 一个是事件标志。这里我们循环检测发送数据寄存器为空这个标志，当跳出while循环时说明发送数据寄存器为空这个事实。

Usart_SendString函数用来发送一个字符串，它实际是调用Usart_SendByte函数发送每个字符，直到遇到空字符才停止发送。 最后使用循环检测发送完成的事件标志TC来实现保证数据发送完成后才退出函数。

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
    Usart_SendByte(pUSARTx, *(str + k) ); // 发送一个字节数据到USART
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

---

2024.8.24 第一次修订，后期不再维护
