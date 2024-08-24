# 第十章 EXTI控制实验

## 1. 硬件设计

    轻触按键在按下时会使得引脚接通，通过电路设计可以使得按下时产生电平变化

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/EXTI004.png)

## 2. 软件设计

我们需要完成的任务：

前置配置LED我们就不必多说了

1. 初始化用来产生中断的GPIO，初始化EXTI

```c
#ifndef __EXTI_H
#define __EXTI_H


#include "stm32f10x.h"

// 配置KEY1和KEY2的GPIO
#define KEY1_INT_GPIO_PORT         GPIOA // 选择GPIOA作为KEY1的输出口
#define KEY1_INT_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO) // 使能GPIOA时钟和AFIO时钟
#define KEY1_INT_GPIO_PIN          GPIO_Pin_0 // 选择GPIOA0作为KEY1的输出引脚
#define KEY1_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA // 选择GPIOA作为KEY1的外部中断源
#define KEY1_INT_EXTI_PINSOURCE    GPIO_PinSource0 // 选择GPIOA0作为KEY1的外部中断源
#define KEY1_INT_EXTI_LINE         EXTI_Line0 // 选择GPIOA0作为KEY1的外部中断线
#define KEY1_INT_EXTI_IRQ          EXTI0_IRQn // 选择GPIOA0作为KEY1的外部中断中断源

#define KEY1_IRQHandler            EXTI0_IRQHandler


#define KEY2_INT_GPIO_PORT         GPIOC // 选择GPIOC作为KEY2的输出口
#define KEY2_INT_GPIO_CLK          (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO) // 使能GPIOC时钟和AFIO时钟
#define KEY2_INT_GPIO_PIN          GPIO_Pin_13 // 选择GPIOC13作为KEY2的输出引脚
#define KEY2_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOC // 选择GPIOC作为KEY2的外部中断源
#define KEY2_INT_EXTI_PINSOURCE    GPIO_PinSource13 // 选择GPIOC13作为KEY2的外部中断源
#define KEY2_INT_EXTI_LINE         EXTI_Line13 // 选择GPIOC13作为KEY2的外部中断线
#define KEY2_INT_EXTI_IRQ          EXTI15_10_IRQn // 选择GPIOC13作为KEY2的外部中断中断源

#define KEY2_IRQHandler            EXTI15_10_IRQHandler

void EXTI_Key_Config(void);

#endif
```

在上面的宏定义中，我们除了开GPIO的端口时钟外，我们还打开了AFIO的时钟， 这是因为等下配置EXTI信号源的时候需要用到AFIO的外部中断控制寄存器AFIO_EXTICRx。

2. 配置NVIC；

```c
// 配置嵌套向量中断控制器NVIC
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  // 1.配置NVIC为优先级组1
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 使用库函数misc.C中的函数-NVIC_PriorityGroupConfig()配置NVIC为优先级组1

  // 2.配置中断源：按键1
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ; // 实际上是使用的是EXTI_IRQn

  // 3.配置中断子优先级：1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;

  // 4.配置子优先级：1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

  // 5.使能中断通道
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // 6.配置中断源：按键2，其他使用上面相关配置
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
  NVIC_Init(&NVIC_InitStructure);
}
```

    这里我们配置两个的中断软件优先级一样，如果出现了两个按键同时按下的情况，那怎么办，到底该执行哪一个中断？当两个中断的软件优先级一样的时候， 中断来临时，具体先执行哪个中断服务函数由硬件的中断编号决定，编号越小，优先级越高。

    当然，我们也可以把抢占优先级设置成一样，子优先级设置成不一样，这样就可以区别两个按键同时按下的情况，而不用硬件去对比硬件编号。

3. 编写中断服务函数；

```c
// 配置IO为EXTI中断口，并设置中断优先级
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体变量
    EXTI_InitTypeDef EXTI_InitStructure; // 定义EXTI初始化结构体变量

    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(KEY1_INT_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(KEY2_INT_GPIO_CLK, ENABLE);

    // 配置NVIC中断
    NVIC_Configuration();

/*--------------------------KEY1配置-----------------------------*/
    // 1.选择按键用到的GPIO引脚
  GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;

  // 2.配置为浮空输入
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);

    // 3.选择EXTI的信号源
  GPIO_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE, KEY1_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;

    // 4.配置EXTI为中断模式
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

    // 5.上升沿中断
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;

  // 6.使能中断    
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /*--------------------------KEY2配置-----------------------------*/
    /* 选择按键用到的GPIO */    
  GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;
  /* 配置为浮空输入 */    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);

    /* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE, KEY2_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;

    /* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* 下降沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  /* 使能中断 */    
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
```

出现了-新的库函数！GPIO_EXTILineConfig()，来讲解一下吧：

### 2.1.1 函数原型

```c
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
```

### 2.1.2 参数说明

1. **`GPIO_PortSource`**：
- 类型：`uint8_t`
- 说明：指定外部中断源的 GPIO 端口。这通常是一个与 GPIO 端口相关的常量，例如 `GPIO_PortSourceGPIOA`、`GPIO_PortSourceGPIOB` 等。这些常量用于选择具体的 GPIO 端口（A、B、C、D、E）。
2. **`GPIO_PinSource`**：
   
   - 类型：`uint8_t`
   - 说明：指定外部中断源的 GPIO 引脚。这通常是一个与 GPIO 引脚相关的常量，例如 `GPIO_PinSource0`、`GPIO_PinSource1` 等。这些常量用于选择具体的 GPIO 引脚（0-15）。

### 2.1.3 功能

`GPIO_EXTILineConfig()` 函数将指定的 GPIO 引脚配置为外部中断源。这样，GPIO 引脚的状态变化（如上升沿、下降沿或两者）将触发相应的 EXTI 线中断请求。要使此配置生效，还需要对 EXTI 线进行进一步的配置，包括中断触发方式、优先级等。

### 2.1.4 使用步骤

1. **选择 GPIO 端口和引脚**：
   在调用 `GPIO_EXTILineConfig()` 函数之前，确定要配置的 GPIO 端口和引脚。例如，假设你想将 GPIOB 的引脚 5 作为外部中断源。

2. **配置 GPIO 引脚**：
   你需要首先配置 GPIO 引脚为输入模式，并使能其上拉或下拉电阻（如果需要）。

3. **调用 `GPIO_EXTILineConfig()` 函数**：
   使用 `GPIO_EXTILineConfig()` 函数将 GPIO 引脚映射到相应的 EXTI 线。例如：

```c
GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
```

4. **配置 EXTI**：
   之后，你需要配置 EXTI 线的触发方式（上升沿、下降沿或双边沿）、中断优先级等。这些配置通常通过 `EXTI_Init()` 函数来完成。

### 2.1.5 举例

以下是一个简单的代码示例，展示如何配置 GPIOB 的引脚 5 为外部中断源：

```c
#include "stm32f4xx.h"

void EXTI_Config(void) {
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 启用 GPIOB 和 SYSCFG 时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // 配置 GPIOB 引脚 5 为输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置 GPIO 引脚 5 作为 EXTI 线的源
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

    // 配置 EXTI 线 5
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 或 EXTI_Trigger_Falling, EXTI_Trigger_Rising_Falling
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置 NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

在这个示例中，我们完成了以下几个步骤：

1. 启用 GPIOB 和 SYSCFG 的时钟。
2. 配置 GPIOB 的引脚 5 为输入模式。
3. 使用 `GPIO_EXTILineConfig()` 函数将 GPIOB 的引脚 5 配置为 EXTI 线的源。
4. 配置 EXTI 线 5 的中断触发方式和使能中断。
5. 配置 NVIC 以响应 EXTI 中断。

## 3. 小结

我们代码注释已经写得足够清楚了，感觉没什么可讲，复盘一下大概流程吧：

- 首先肯定是要宏定义配置GPIO和EXTI初始化了，就拿key1来说，先选择用哪个GPIO口，之后马上打开时钟，但选择GPIO了不行，我们接着再选择了输出引脚，外部中断源。至于EXTI配置，我们只要关注两个-EXTI_Linex（中断输入线）和EXTIX_IRQn（中断线对应的）

- 之后的key2同理啦，只是改改引脚，配置

- 配置NVIC：5件套-优先组，中断源，中断子优先级，子优先级，使能中断

- 中断服务函数：首先肯定是要先初始化GPIO和EXTI两个结构体啦，再有打开时钟不能完，初始化一下NVIC。前期工作完成，正式来配置key1（key2同理）这个也比较公式化了再配合EXTI框图应该比较容易，1.选择引脚；2.配置输入模式（浮空输入）；3.选择EXTI作为信号源；4.配置EXTI为中断模式；5.使EXTI上升沿中断；6.使能中断

达成的效果：当我们按下按键->产生中断->led状态翻转（我们这里使用按键控制）

也没有出现新的库函数，在中断应用中我们就已经介绍过了，

---

2024.8.21 第一次修订，后期不再维护
