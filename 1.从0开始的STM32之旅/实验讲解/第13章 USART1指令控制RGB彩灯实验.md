# 第十三章 USART1指令控制RGB彩灯实验

## 1. 硬件设计

同前一章实验和已经介绍过的led

## 2. 软件设计

### 2.1 编程目标

1. 初始化配置RGB彩色灯GPIO；

2. 使能RX和TX引脚GPIO时钟和USART时钟；

3. 初始化GPIO，并将GPIO复用到USART上；

4. 配置USART参数；

5. 使能USART；

6. 获取指令输入，根据指令控制RGB彩色灯。

### 2.2 代码分析

USART1和led控制我们已经详细的分析过了，下面回顾一下重要部分，讲解主函数

- 串口宏定义

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

- 串口初始化

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

该配置函数跟上一个实验的基本一样，不一样的地方是没有使用接收中断。

- 重定向printf和scanf函数

```c
// 重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口 */
    USART_SendData(DEBUG_USARTx, (uint8_t) ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);

    return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
    /* 等待串口输入数据 */
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(DEBUG_USARTx);
}
```

在C语言标准库中，fputc函数是printf函数内部的一个函数，功能是将字符ch写入到文件指针f所指向文件的当前写指针位置， 简单理解就是把字符写入到特定文件中。我们使用USART函数重新修改fputc函数内容，达到类似“写入”的功能。

fgetc函数与fputc函数非常相似，实现字符读取功能。在使用scanf函数时需要注意字符输入格式。

还有一点需要注意的，使用fput和fgetc函数达到重定向C语言标准库输入输出函数必须在MDK的工程选项把“Use MicroLIB”勾选上， MicoroLIB是缺省C库的备选库，它对标准C库进行了高度优化使代码更少，占用更少资源。

为使用printf、scanf函数需要在文件中包含stdio.h头文件。

- 主函数

```c
static void Show_Message(void); // 打印指令输入提示信息

int main(void)
{    
  char i;

  // 1.初始化led
  LED_GPIO_Config();

  // 2.初始化串口
  USART_Config();

  // 3.打印提示信息
  Show_Message();
  while(1)
    {    
    /* 获取字符指令 */
    ch = getchar();
    printf("接收到字符：%c\n", ch); // 打印字符而不是ASCII值

      /* 根据字符指令控制RGB彩灯颜色 */
      switch(ch)
      {
        case '1': // 使用字符进行比较
          LED_RED; // 1.红色
          break;
        case '2':
          LED_GREEN; // 2.绿色
          break; 
        case '3':
          LED_BLUE; // 3.蓝色
          break;
        case '4':
          LED_YELLOW; // 4.黄色
          break;
        case '5':
          LED_PURPLE; // 5.紫色
          break;
        case '6':
          LED_CYAN; // 6.青色
          break;
        case '7':
          LED_WHITE; // 7.白色
          break;
        case '8':
          LED_RGBOFF; // 8.灭
          break;
        default:
          Show_Message();
          break;      
      }   
    }    
}

// 打印指令输入提示信息
static void Show_Message(void)
{
  printf("\r\n   这是一个通过串口通信指令控制RGB彩灯实验 \n");
  printf("使用  USART  参数为：%d 8-N-1 \n", DEBUG_USART_BAUDRATE);
  printf("开发板接到指令后控制RGB彩灯颜色，指令对应如下：\n");
  printf("   指令   ------ 彩灯颜色 \n");
  printf("     1    ------    红 \n");
  printf("     2    ------    绿 \n");
  printf("     3    ------    蓝 \n");
  printf("     4    ------    黄 \n");
  printf("     5    ------    紫 \n");
  printf("     6    ------    青 \n");
  printf("     7    ------    白 \n");
  printf("     8    ------    灭 \n");  
}
```

主函数倒是很简单，就算我们刚刚学习C语言也能看懂，主要打印一个菜单嘛，然后使用一个switch循环，根据指令来实现不同功能（调用led函数）

Show_Message函数全部是调用printf函数，“打印”实验操作信息到串口调试助手。

首先我们定义一个字符变量来存放接收到的字符。

接下来调用LED_GPIO_Config函数完成RGB彩色GPIO初始化配置，该函数定义在bsp_led.c文件内。

调用USART_Config函完成USART初始化配置。

Show_Message函数使用printf函数打印实验指令说明信息。

getchar函数用于等待获取一个字符，并返回字符。我们使用ch变量保持返回的字符，接下来判断ch内容执行对应的程序。

我们使用switch语句判断ch变量内容，并执行对应的功能程序。

## 3. 小结

理解了USART1的配置后，这章应该就简单了，我们再把我们之前写的led函数拿过来实现联动，具体就是：初始化led->初始化串口->主函数中调用两个函数就行啦，在串口收发的时候我们发送指令，不同指令对应不同led

---

2024.8.24 第一次修订，后期不再维护
