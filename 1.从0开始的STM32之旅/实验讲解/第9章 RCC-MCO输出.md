# 第九章 RCC-MCO输出

### 1. 使用HSE

    一般情况下，我们都是使用HSE，然后HSE经过PLL倍频之后作为系统时钟。通常的配置是：HSE=8M，PLL的倍频因子为：9， 系统时钟就设置成:SYSCLK = 8M * 9 = 72M。使用HSE，系统时钟SYSCLK最高是128M。我们使用的库函数就是这么干的， 当程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用SystemInit()函数把系统时钟初始化成72MHZ， SystemInit()在库文件：system_stm32f10x.c中定义。如果我们想把系统时钟设置低一点或者超频的话，可以修改底层的库文件， 但是为了维持库的完整性，我们可以根据时钟树的流程自行写一个。

    更多内容参考：[STM32入坑（六）使用HSE配置系统时钟_stm32使用hse时钟-CSDN博客](https://blog.csdn.net/m0_46195580/article/details/111402501)

### 2. 使用HSI

    当HSE故障的时候，如果PLL的时钟来源是HSE，那么当HSE故障的时候，不仅HSE不能使用，连PLL也会被关闭，这个时候系统会自动切换HSI作为系统时钟， 此时SYSCLK=HSI=8M，如果没有开启CSS和CSS中断的话，那么整个系统就只能在低速率运行，这是系统跟瘫痪没什么两样。如果开启了CSS功能的话， 那么可以当HSE故障时，在CSS中断里面采取补救措施，使用HSI，并把系统时钟设置为更高的频率，最高是64M，64M的频率足够一般的外设使用， 如：ADC、SPI、I2C等。但是这里就又有一个问题了，原来SYSCLK=72M，现在因为故障改成64M，那么那些外设的时钟肯定被改变了， 那么外设工作就会被打乱，那我们是不是在设置HSI时钟的时候，也重新调整外设总线的分频因子，即AHB，APB2和APB1的分频因子， 使外设的时钟达到跟HSE没有故障之前一样。但是这个也不是最保障的办法，毕竟不能一直使用HSI，所以当HSE故障时还是要采取报警措施。

    还有一种情况是，有些用户不想用HSE，想用HSI，但是又不知道怎么用HSI来设置系统时钟，因为调用库函数都是使用HSE， 下面我们给出个使用HSI配置系统时钟例子，起个抛砖引玉的作用。

    更多内容参考：[STM32系统时钟的配置方法——内部高速时钟HSI作为系统时钟源_stm32 hsi-CSDN博客](https://blog.csdn.net/qq_33066921/article/details/137297817)

## 3. 硬件设计

- RCC

       这个是内部资源，不用关

- LED一个
  
  关于开发板上面的LED电路我们已经分析过了 

### 4. 软件设计

    我们编写两个RCC驱动文件，bsp_clkconfig.h和bsp_clkconfig.c，用来存放RCC系统时钟配置函数。

#### 4.1 编程概要

编程要点对应着时钟树图中的序号。

> 1、开启HSE/HSI ，并等待 HSE/HSI 稳定
> 
> 2、设置 AHB、APB2、APB1的预分频因子
> 
> 3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
> 
> 4、开启PLL，并等待PLL稳定
> 
> 5、把PLLCK切换为系统时钟SYSCLK
> 
> 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟

#### 4.2 代码分析

使用HSE/HSI配置系统时钟

```c
#include "bsp_clkconfig.h"
#include "stm32f10x_rcc.h"

/*
使用HSE时，设置系统时钟的步骤
1、开启HSE ，并等待 HSE 稳定
2、设置 AHB、APB2、APB1的预分频因子
3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
4、开启PLL，并等待PLL稳定
5、把PLLCK切换为系统时钟SYSCLK
6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
*/

/* 
设置系统时钟:SYSCLK, AHB总线时钟:HCLK, APB2总线时钟:PCLK2, APB1总线时钟:PCLK1
PCLK2 = HCLK = SYSCLK
PCLK1 = HCLK/2,最高只能是36M
参数说明：pllmul是PLL的倍频因子，在调用的时候可以是：RCC_PLLMul_x , x:[2,3,...16]
举例：User_SetSysClock(RCC_PLLMul_9);  则设置系统时钟为：8MHZ * 9 = 72MHZ
      User_SetSysClock(RCC_PLLMul_16); 则设置系统时钟为：8MHZ * 16 = 128MHZ，超频慎用
 HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法
 */

void HSE_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

  // 把RCC外设初始化成复位状态，这句是必须的
  RCC_DeInit();

  // 使能HSE，开启外部晶振，野火开发板用的是8M
  RCC_HSEConfig(RCC_HSE_ON);

  // 等待 HSE 启动稳定
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  // 只有 HSE 稳定之后则继续往下执行
  if (HSEStartUpStatus == SUCCESS)
  {
    // 使能FLASH 预存取缓冲区
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
        // 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
        // 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
        // 0：0 < SYSCLK <= 24M
        // 1：24< SYSCLK <= 48M
        // 2：48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);

    // AHB预分频因子设置为1分频，HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 

    // APB2预分频因子设置为1分频，PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1预分频因子设置为1分频，PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);

//-----------------设置各种频率主要就是在这里设置-------------------
    // 设置PLL时钟来源为HSE，设置PLL倍频因子
        // PLLCLK = 8MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);

    // 开启PLL 
    RCC_PLLCmd(ENABLE);

    // 等待 PLL稳定
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // 读取时钟切换状态位，确保PLLCLK被选为系统时钟
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // 如果HSE开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
        // 当HSE开启失败或者故障的时候，单片机会自动把HSI设置为系统时钟，
        // HSI是内部的高速时钟，8MHZ
    while (1)
    {
    }
  }
}

/*
使用HSI时，设置系统时钟的步骤
1、开启HSI ，并等待 HSI 稳定
2、设置 AHB、APB2、APB1的预分频因子
3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置
4、开启PLL，并等待PLL稳定
5、把PLLCK切换为系统时钟SYSCLK
6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
*/

/*
设置 系统时钟:SYSCLK, AHB总线时钟:HCLK, APB2总线时钟:PCLK2, APB1总线时钟:PCLK1
PCLK2 = HCLK = SYSCLK
PCLK1 = HCLK/2,最高只能是36M
参数说明：pllmul是PLL的倍频因子，在调用的时候可以是：RCC_PLLMul_x , x:[2,3,...16]
举例：HSI_SetSysClock(RCC_PLLMul_9);  则设置系统时钟为：4MHZ * 9 = 36MHZ
     HSI_SetSysClock(RCC_PLLMul_16); 则设置系统时钟为：4MHZ * 16 = 64MHZ

HSI作为时钟来源，经过PLL倍频作为系统时钟，这是在HSE故障的时候才使用的方法
HSI会因为温度等原因会有漂移，不稳定，一般不会用HSI作为时钟来源，除非是迫不得已的情况
如果HSI要作为PLL时钟的来源的话，必须二分频之后才可以，即HSI/2，而PLL倍频因子最大只能是16
所以当使用HSI的时候，SYSCLK最大只能是4M*16=64M
 */

void HSI_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t HSIStartUpStatus = 0;

  // 把RCC外设初始化成复位状态，这句是必须的
  RCC_DeInit();

  // 使能HSI
  RCC_HSICmd(ENABLE);

  // 等待 HSI稳定
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {
  }    
  // 读取 HSI 就绪状态
  HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

    // 只有 HSI就绪之后则继续往下执行
  if (HSIStartUpStatus == RCC_CR_HSIRDY)
  {

    // 使能FLASH 预存取缓冲区
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
        // 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
        // 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
        // 0：0 < SYSCLK <= 24M
        // 1：24< SYSCLK <= 48M
        // 2：48< SYSCLK <= 72M
    FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//

    // AHB预分频因子设置为1分频，HCLK = SYSCLK 
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 

    // APB2预分频因子设置为1分频，PCLK2 = HCLK
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    // APB1预分频因子设置为1分频，PCLK1 = HCLK/2 
    RCC_PCLK1Config(RCC_HCLK_Div2);

//-----------------设置各种频率主要就是在这里设置-------------------//
    // 设置PLL时钟来源为HSI，设置PLL倍频因子
        // PLLCLK = 4MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);

    // 开启PLL 
    RCC_PLLCmd(ENABLE);

    // 等待 PLL稳定
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    // 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // 读取时钟切换状态位，确保PLLCLK被选为系统时钟
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // 如果HSI开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
        // 当HSE开启失败或者故障的时候，单片机会自动把HSI设置为系统时钟，
        // HSI是内部的高速时钟，8MHZ
    while (1)
    {
    }
  }
}
```

MCO输出

```c
#include "bsp_mcooutput.h"
#include "stm32f10x_rcc.h"

/*
 * 初始化MCO引脚PA8
 * 在F1系列中MCO引脚只有一个，即PA8，在F4系列中，MCO引脚会有两个
 */
void MCO_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 开启GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 选择GPIO8引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

    //设置为复用功能推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    //设置IO的翻转速率为50M
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // 初始化GPIOA8
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
```

主函数

```c
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_clkconfig.h"
#include "bsp_mcooutput.h"

// 软件延时函数，使用不同的系统时钟，延时不一样
void Delay(__IO u32 nCount); 

int main(void)
{    
    // 程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用
    // SystemInit()函数把系统时钟初始化成72MHZ
    // SystemInit()在system_stm32f10x.c中定义
    // 如果用户想修改系统时钟，可自行编写程序修改

    // 重新设置系统时钟，这时候可以选择使用HSE还是HSI

    // 使用HSE时，SYSCLK = 8M * RCC_PLLMul_x, x:[2,3,...16],最高是128M
    //HSE_SetSysClock(RCC_PLLMul_9);

    // 使用HSI时，SYSCLK = 4M * RCC_PLLMul_x, x:[2,3,...16],最高是64MH
    HSI_SetSysClock(RCC_PLLMul_16);

    // MCO 引脚初始化
    MCO_GPIO_Config();

    // 设置MCO引脚输出时钟，用示波器即可在PA8测量到输出的时钟信号，
    // 我们可以把PLLCLK/2作为MCO引脚的时钟来检测系统时钟是否配置准确
    // MCO引脚输出可以是HSE,HSI,PLLCLK/2,SYSCLK    
    // RCC_MCOConfig(RCC_MCO_HSE);                             
    // RCC_MCOConfig(RCC_MCO_HSI);                       
    // RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);        
    RCC_MCOConfig(RCC_MCO_SYSCLK);              

    // LED 端口初始化
    LED_GPIO_Config();
    while (1)
    {
        LED1( ON );              // 亮
        Delay(0x0FFFFF);
        LED1( OFF );          // 灭 
        Delay(0x0FFFFF);        
    }
}


//  软件延时函数，使用不同的系统时钟，延时不一样
void Delay(__IO uint32_t nCount)    
{
    for(; nCount != 0; nCount--);
}
```

    在主函数中，可以调用HSE_SetSysClock()或者HSI_SetSysClock()这两个函数把系统时钟设置成各种常用的时钟，然后通过MCO引脚监控， 或者通过LED闪烁的快慢体验不同的系统时钟对同一个软件延时函数的影响。

## 5. 小结

主要就是一个配置问题，可以参考时钟树来理解，比如我们以配置HSE为例：

1、开启HSE ，并等待 HSE 稳定

    如何开启？答：使用库函数就行了呀

```c
// 把RCC外设初始化成复位状态
RCC_DeInit(); // 库函数 RCC_DeInit()

//使能HSE，开启外部晶振，野火开发板用的是8M
RCC_HSEConfig(RCC_HSE_ON); // 库函数 RCC_HSEConfig()

// 等待 HSE 启动稳定
HSEStartUpStatus = RCC_WaitForHSEStartUp(); // 库函数 RCC_WaitForHSEStartUp()
```

2、设置 AHB、APB2、APB1的预分频因子

    参考时钟树设置预分频因子：

```c
// AHB预分频因子设置为1分频，HCLK = SYSCLK 
RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
// APB2预分频因子设置为1分频，PCLK2 = HCLK
RCC_PCLK2Config(RCC_HCLK_Div1); 

// APB1预分频因子设置为1分频，PCLK1 = HCLK/2 
RCC_PCLK1Config(RCC_HCLK_Div2);
```

3、设置PLL的时钟来源，和PLL的倍频因子，设置各种频率主要就是在这里设置

```c
// 设置PLL时钟来源为HSE，设置PLL倍频因子
// PLLCLK = 8MHz * pllmul
RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
```

4、开启PLL，并等待PLL稳定

```c
// 开启PLL 
RCC_PLLCmd(ENABLE); // 使用库函数 RCC_PLLCmd()
// 等待 PLL稳定
while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
{
      
}
```

5、把PLLCK切换为系统时钟SYSCLK

```c
// 当PLL稳定之后，把PLL时钟切换为系统时钟SYSCLK
RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // PLLCLK经过我们上面设置了，现在只需要把它作为系统时钟
```

6、读取时钟切换状态位，确保PLLCLK被选为系统时钟

```c
// 读取时钟切换状态位，确保PLLCLK被选为系统时钟
// 库函数 RCC_GetSYSCLKSource() 用于读取系统时钟源
// 参数代表系统时钟源，0x08代表PLLCLK
while (RCC_GetSYSCLKSource() != 0x08)
{
      // 等待时钟切换完成
}
    // 系统时钟设置完成
```


