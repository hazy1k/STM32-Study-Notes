# 第十一章 RCC-使用HSE/HSI配置时钟

## 1. 导入

    RCC ：reset clock control  复位和时钟控制器。本章我们主要讲解时钟部分， 特别是要着重理解时钟树，理解了时钟树，STM32的一切时钟的来龙去脉都会了如指掌。

## 2. RCC主要作用-时钟部分

    设置系统时钟SYSCLK、设置AHB分频因子（决定HCLK等于多少）、设置APB2分频因子（决定PCLK2等于多少）、 设置APB1分频因子（决定PCLK1等于多少）、设置各个外设的分频因子；控制AHB、APB2和APB1这三条总线时钟的开启、 控制每个外设的时钟的开启。对于SYSCLK、HCLK、PCLK2、PCLK1这四个时钟的配置一般是： PCLK2 = HCLK = SYSCLK=PLLCLK = 72M， PCLK1=HCLK/2 = 36M。这个时钟配置也是库函数的标准配置，我们用的最多的就是这个。

    控制AHB、APB2和APB1这三条总线时钟的开启、 控制每个外设的时钟的开启。这个我们应该很熟悉咯，已经用到过几次。想要使用GPIO就得先把外部时钟给开了。

    更多内容参考：[从小白到 Pro | RCC时钟基础知识和常见问题 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/145221712)

## 3. RCC框图剖析—时钟部分

     我们这里选取库函数时钟系统时钟函数：**SetSysClockTo72();** 以这个函数的编写流程来讲解时钟树， 这个函数也是我们用库的时候默认的系统时钟设置函数。 该函数的功能是利用HSE把时钟设置为：PCLK2 = HCLK = SYSCLK = 72M，PCLK1=HCLK/2 = 36M。 下面我们就以这个代码的流程为主线，来分析时钟树，对应的是图中的黄色部分，代码流程在时钟树中以数字的大小顺序标识。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/RCC002.png)

### 3.1 系统时钟

#### 3.1.1 HSE高速外部时钟信号

    HSE是高速的外部时钟信号，可以由有源晶振或者无源晶振提供，频率从4-16MHZ不等。当使用有源晶振时， 时钟从OSC_IN引脚进入，OSC_OUT引脚悬空，当选用无源晶振时，时钟从OSC_IN和OSC_OUT进入，并且要配谐振电容。

    HSE最常使用的就是8M的无源晶振。当确定PLL时钟来源的时候，HSE可以不分频或者2分频， 这个由时钟配置寄存器CFGR的位17：PLLXTPRE设置，我们设置为HSE不分频。

- PLL时钟源

    PLL时钟来源可以有两个，一个来自HSE，另外一个是HSI/2，具体用哪个由时钟配置寄存器CFGR的位16：PLLSRC设置。 HSI是内部高速的时钟信号，频率为8M，根据温度和环境的情况频率会有漂移，一般不作为PLL的时钟来源。这里我们选HSE作为PLL的时钟来源。

- PLL时钟PLLCLK

    通过设置PLL的倍频因子，可以对PLL的时钟来源进行倍频，倍频因子可以是:[2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]， 具体设置成多少，由时钟配置寄存器CFGR的位21-18：PLLMUL[3:0]设置。我们这里设置为9倍频， 因为上一步我们设置PLL的时钟来源为HSE=8M，所以经过PLL倍频之后的PLL时钟：PLLCLK = 8M *9 = 72M。 72M是ST官方推荐的稳定运行时钟，如果你想超频的话，增大倍频因子即可，最高为128M。 我们这里设置PLL时钟：PLLCLK = 8M *9 = 72M。

- 系统时钟SYSCLK

    系统时钟来源可以是：HSI、PLLCLK、HSE，具体的时钟配置寄存器CFGR的位1-0：SW[1:0]设置。 我们这里设置系统时钟：SYSCLK = PLLCLK = 72M。

- AHB总线时钟HCLK

    系统时钟SYSCLK经过AHB预分频器分频之后得到时钟叫APB总线时钟，即HCLK，分频因子可以是:[1,2,4，8，16，64，128，256，512]， 具体的由时钟配置寄存器CFGR的位7-4  ：HPRE[3:0]设置。片上大部分外设的时钟都是经过HCLK分频得到， 至于AHB总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置， 我们这里只需粗线条的设置好APB的时钟即可。我们这里设置为1分频，即HCLK=SYSCLK=72M。

- APB2总线时钟PCLK2

    APB2总线时钟PCLK2由HCLK经过高速APB2预分频器得到，分频因子可以是:[1,2,4，8，16]，具体由时钟配置寄存器CFGR的位13-11：PPRE2[2:0]决定。 PCLK2属于高速的总线时钟，片上高速的外设就挂载到这条总线上，比如全部的GPIO、USART1、SPI1等。至于APB2总线上的外设的时钟设置为多少， 得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好APB2的时钟即可。我们这里设置为1分频，即PCLK2 = HCLK = 72M。

- APB1总线时钟PCLK1

    APB1总线时钟PCLK1由HCLK经过低速APB预分频器得到，分频因子可以是:[1,2,4，8，16]，具体的由时钟配置寄存器CFGR的位10-8：PRRE1[2:0]决定。 PCLK1属于低速的总线时钟，最高为36M，片上低速的外设就挂载到这条总线上，比如USART2/3/4/5、SPI2/3，I2C1/2等。 至于APB1总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好APB1的时钟即可。 我们这里设置为2分频，即PCLK1 = HCLK/2 = 36M。

    更多参考：[【STM32】系统时钟RCC详解(超详细，超全面)_rcc时钟-CSDN博客](https://blog.csdn.net/as480133937/article/details/98845509)

#### 3.1.2 设置系统时钟库函数

    上面的7个步骤对应的设置系统时钟库函数如下，该函数截取自固件库文件system_stm32f10x.c。为了方便阅读， 我已把互联型相关的代码删掉，把英文注释翻译成了中文，并把代码标上了序号，总共七个步骤。该函数是直接操作寄存器的， 有关寄存器部分请参考数据手册的RCC的寄存器描述部分。

```c
static void SetSysClockTo72(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    // ① 使能HSE，并等待HSE稳定
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    // 等待HSE启动稳定，并做超时处理
    do {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0)
        &&(StartUpCounter !=HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }
    // HSE启动成功，则继续往下处理
    if (HSEStatus == (uint32_t)0x01) {

        //-----------------------------------------------------------
        // 使能FLASH 预存取缓冲区 */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
        // 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
        // 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
        // 0：0 < SYSCLK <= 24M
        // 1：24< SYSCLK <= 48M
        // 2：48< SYSCLK <= 72M */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
        //------------------------------------------------------------

        // ② 设置AHB、APB2、APB1预分频因子
        // HCLK = SYSCLK
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
        //PCLK2 = HCLK
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
        //PCLK1 = HCLK/2
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

        // ③ 设置PLL时钟来源，设置PLL倍频因子，PLLCLK = HSE * 9 = 72 MHz
        RCC->CFGR &= (uint32_t)((uint32_t)
                                ~(RCC_CFGR_PLLSRC
                                | RCC_CFGR_PLLXTPRE
                                | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE
                                | RCC_CFGR_PLLMULL9);

        // ④ 使能 PLL
        RCC->CR |= RCC_CR_PLLON;

        // ⑤ 等待PLL稳定
        while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
        }

        // ⑥ 选择PLL作为系统时钟来源
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        // ⑦ 读取时钟切换状态位，确保PLLCLK被选为系统时钟
        while ((RCC->CFGR&(uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08){
        }
    } else {// 如果HSE启动失败，用户可以在这里添加错误代码出来
    }
}
```

### 3.2 其他时钟

- USB时钟

    USB时钟是由PLLCLK经过USB预分频器得到，分频因子可以是：[1,1.5]，具体的由时钟配置寄存器CFGR的位22：USBPRE配置。 USB的时钟最高是48M，根据分频因子反推过来算，PLLCLK只能是48M或者是72M。一般我们设置PLLCLK=72M，USBCLK=48M。 USB对时钟要求比较高，所以PLLCLK只能是由HSE倍频得到，不能使用HSI倍频。

- Cortex系统时钟

    Cortex系统时钟由HCLK 8分频得到，等于9M， Cortex系统时钟用来驱动内核的系统定时器SysTick，SysTick一般用于操作系统的时钟节拍，也可以用做普通的定时。

- ADC时钟

    ADC时钟由PCLK2经过ADC预分频器得到，分频因子可以是[2,4,6,8]，具体的由时钟配置寄存器CFGR的位15-14：ADCPRE[1:0]决定。 很奇怪的是怎么没有1分频。ADC时钟最高只能是14M，如果采样周期设置成最短的1.5个周期的话，ADC的转换时间可以达到最短的1us。 如果真要达到最短的转换时间1us的话，那ADC的时钟就得是14M，反推PCLK2的时钟只能是：28M、56M、84M、112M， 鉴于PCLK2最高是72M，所以只能取28M和56M。

- RTC时钟、独立看门狗时钟

    RTC时钟可由HSE/128分频得到，也可由低速外部时钟信号LSE提供，频率为32.768KHZ，也可由低速内部时钟信号LSI提供， 具体选用哪个时钟由备份域控制寄存器BDCR的位9-8：RTCSEL[1:0]配置。独立看门狗的时钟由LSI提供， 且只能是由LSI提供，LSI是低速的内部时钟信号，频率为30~60KHZ直接不等，一般取40KHZ。

- MCO时钟输出

    MCO是microcontroller clock output的缩写，是微控制器时钟输出引脚，在STM32 F1系列中 由 PA8复用所得， 主要作用是可以对外提供时钟，相当于一个有源晶振。MCO的时钟来源可以是：PLLCLK/2、HSI、HSE、SYSCLK， 具体选哪个由时钟配置寄存器CFGR的位26-24：MCO[2:0]决定。除了对外提供时钟这个作用之外， 我们还可以通过示波器监控MCO引脚的时钟输出来验证我们的系统时钟配置是否正确。

## 4. 配置系统时钟实验

### 4.1 使用HSE

    一般情况下，我们都是使用HSE，然后HSE经过PLL倍频之后作为系统时钟。通常的配置是：HSE=8M，PLL的倍频因子为：9， 系统时钟就设置成:SYSCLK = 8M * 9 = 72M。使用HSE，系统时钟SYSCLK最高是128M。我们使用的库函数就是这么干的， 当程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用SystemInit()函数把系统时钟初始化成72MHZ， SystemInit()在库文件：system_stm32f10x.c中定义。如果我们想把系统时钟设置低一点或者超频的话，可以修改底层的库文件， 但是为了维持库的完整性，我们可以根据时钟树的流程自行写一个。

    更多内容参考：[STM32入坑（六）使用HSE配置系统时钟_stm32使用hse时钟-CSDN博客](https://blog.csdn.net/m0_46195580/article/details/111402501)

### 4.2 使用HSI

    当HSE故障的时候，如果PLL的时钟来源是HSE，那么当HSE故障的时候，不仅HSE不能使用，连PLL也会被关闭，这个时候系统会自动切换HSI作为系统时钟， 此时SYSCLK=HSI=8M，如果没有开启CSS和CSS中断的话，那么整个系统就只能在低速率运行，这是系统跟瘫痪没什么两样。如果开启了CSS功能的话， 那么可以当HSE故障时，在CSS中断里面采取补救措施，使用HSI，并把系统时钟设置为更高的频率，最高是64M，64M的频率足够一般的外设使用， 如：ADC、SPI、I2C等。但是这里就又有一个问题了，原来SYSCLK=72M，现在因为故障改成64M，那么那些外设的时钟肯定被改变了， 那么外设工作就会被打乱，那我们是不是在设置HSI时钟的时候，也重新调整外设总线的分频因子，即AHB，APB2和APB1的分频因子， 使外设的时钟达到跟HSE没有故障之前一样。但是这个也不是最保障的办法，毕竟不能一直使用HSI，所以当HSE故障时还是要采取报警措施。

    还有一种情况是，有些用户不想用HSE，想用HSI，但是又不知道怎么用HSI来设置系统时钟，因为调用库函数都是使用HSE， 下面我们给出个使用HSI配置系统时钟例子，起个抛砖引玉的作用。

    更多内容参考：[STM32入坑（七）使用HSI配置系统时钟_iar stm32f1 hsi-CSDN博客](https://blog.csdn.net/m0_46195580/article/details/111409666)

### 4.3 硬件设计

- RCC

- LED一个 

### 4.4 软件设计

    我们编写两个RCC驱动文件，bsp_clkconfig.h和bsp_clkconfig.c，用来存放RCC系统时钟配置函数。

#### 4.4.1 编程概要

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

#### 4.4.2 代码分析

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
		LED1( ON );			  // 亮
		Delay(0x0FFFFF);
		LED1( OFF );		  // 灭 
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

    RCC即复位和时钟控制，mt32时钟源分高速（HSE）和低速（HSI），我们通常使用HSE，但是出现故障时系统会自动使用HSI。

    STM32的分频和倍频功能非常强大，可将时钟源通过分频与倍频技术，使各APB总线时钟频率配置为指定值，供各外设使用。重点就是PLL（锁相环）的几个参数：PLL_M、PLL_N、PLL_P、PLL_Q，也就是倍频、分频因子。
