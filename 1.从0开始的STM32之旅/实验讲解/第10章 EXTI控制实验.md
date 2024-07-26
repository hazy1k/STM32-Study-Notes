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
#define	__EXTI_H


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
	GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体变量
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

## 3. 小结

我们代码注释已经写得足够清楚了，感觉没什么可讲，复盘一下大概流程吧：

- 首先肯定是要宏定义配置GPIO和EXTI初始化了，就拿key1来说，先选择用哪个GPIO口，之后马上打开时钟，但选择GPIO了不行，我们接着再选择了输出引脚，外部中断源。至于EXTI配置，我们只要关注两个-EXTI_Linex（中断输入线）和EXTIX_IRQn（中断线对应的）

- 之后的key2同理啦，只是改改引脚，配置

- 配置NVIC：5件套-优先组，中断源，中断子优先级，子优先级，使能中断

- 中断服务函数：首先肯定是要先初始化GPIO和EXTI两个结构体啦，再有打开时钟不能完，初始化一下NVIC。前期工作完成，正式来配置key1（key2同理）这个也比较公式化了再配合EXTI框图应该比较容易，1.选择引脚；2.配置输入模式（浮空输入）；3.选择EXTI作为信号源；4.配置EXTI为中断模式；5.使EXTI上升沿中断；6.使能中断

达成的效果：当我们按下按键->产生中断->led状态翻转（我们这里使用按键控制）


