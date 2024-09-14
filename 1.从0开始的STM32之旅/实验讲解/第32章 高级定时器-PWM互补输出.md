# 第三十二章 高级定时器-PWM互补输出

## 1. 硬件设计

根据开发板引脚使用情况，并且参考定时器引脚信息 ，使用高级定时器TIM1的通道1及其互补通道作为本实验的波形输出通道，对应选择PA8和PB13引脚。将示波器的两个输入通道分别与PA8和PB13引脚连接，用于观察波形，还有注意共地。在指南者开发板里面，PA8通过一个调帽默认连接了蜂鸣器，如果调帽不拔掉的话，PA8输出的PWM信号会让蜂鸣器响。

为增加断路功能，需要用到TIM1_BKIN引脚，这里选择PB12引脚。程序我们设置该引脚为高电平有效，当BKIN引脚被置高低电平的时候，两路互补的PWM输出就被停止，就好像是刹车一样。

## 2. 软件设计

### 2.1 编程目的

1. 定时器用到的GPIO 初始化

2. 定时器时基结构体TIM_TimeBaseInitTypeDef初始化

3. 定时器输出比较结构体TIM_OCInitTypeDef初始化

4. 定时器刹车和死区结构体TIM_BDTRInitTypeDef初始化

## 2.2 代码分析

- 宏定义

```c
// 高级定时器TIM1相关参数宏定义
#define ADVANCE_TIM               TIM1                   // 高级定时器选择
#define ADVANCE_TIM_APBxClock_FUN RCC_APB2PeriphClockCmd // 高级定时器时钟使能函数
#define ADVANCE_TIM_CLK           RCC_APB2Periph_TIM1    // 高级定时器时钟

// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define ADVANCE_TIM_PERIOD        (8-1)                  // 高级定时器计数周期
#define ADVANCE_TIM_PSC           (9-1)                  // 高级定时器分频因子
#define ADVANCE_TIM_PULSE         4                      // 高级定时器输出脉冲宽度

// 中断向量定义
#define ADVANCE_TIM_IRQ           TIM1_UP_IRQn           // 高级定时器更新中断
#define ADVANCE_TIM_IRQHandler    TIM1_UP_IRQHandler     // 高级定时器更新中断处理函数

// TIM1 输出比较通道
#define ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA// 通道1 GPIO时钟使能函数
#define ADVANCE_TIM_CH1_PORT          GPIOA               // 通道1 GPIO端口
#define ADVANCE_TIM_CH1_PIN           GPIO_Pin_8          // 通道1 GPIO引脚

// TIM1 输出比较通道的互补通道
#define ADVANCE_TIM_CH1N_GPIO_CLK     RCC_APB2Periph_GPIOB// 通道1N GPIO时钟使能函数
#define ADVANCE_TIM_CH1N_PORT         GPIOB               // 通道1N GPIO端口
#define ADVANCE_TIM_CH1N_PIN          GPIO_Pin_13         // 通道1N GPIO引脚

// TIM1 输出比较通道的刹车通道
#define ADVANCE_TIM_BKIN_GPIO_CLK     RCC_APB2Periph_GPIOB// 刹车通道 GPIO时钟使能函数
#define ADVANCE_TIM_BKIN_PORT         GPIOB               // 刹车通道 GPIO端口
#define ADVANCE_TIM_BKIN_PIN          GPIO_Pin_12         // 刹车通道 GPIO引脚
```

使用宏定义非常方便程序升级、移植。有关每个宏的具体含义看程序注释即可。

- 定时器复用功能引脚初始化

```c
static void ADVANCE_TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // 输出比较通道 GPIO 初始化
  RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ADVANCE_TIM_CH1_PORT, &GPIO_InitStructure);

  // 输出比较通道互补通道 GPIO 初始化
  RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1N_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_CH1N_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ADVANCE_TIM_CH1N_PORT, &GPIO_InitStructure);

  // 输出比较通道刹车通道 GPIO 初始化
  RCC_APB2PeriphClockCmd(ADVANCE_TIM_BKIN_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_BKIN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ADVANCE_TIM_BKIN_PORT, &GPIO_InitStructure);

  // BKIN引脚默认先输出低电平
  GPIO_ResetBits(ADVANCE_TIM_BKIN_PORT, ADVANCE_TIM_BKIN_PIN);    
}
```

ADVANCE_TIM_GPIO_Config()函数初始化了定时器用到的相关的GPIO，当使用不同的GPIO的时候，只需要修改头文件里面的宏定义即可，而不需要修改这个函数。

- 定时器模式配置

```c
// 定时器模式配置
static void ADVANCE_TIM_Mode_Config(void)
{
    // 开启定时器时钟,即内部时钟CK_INT=72M
    ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/
    // 1.时基结构体初始化
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    // 2.自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_PERIOD;
    // 3.驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler= ADVANCE_TIM_PSC;    
    // 4.时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        
    // 5.计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        
    // 6.重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;    
    // 7.初始化定时器
    TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

/*--------------------输出比较结构体初始化-------------------*/
    // 1.输出比较结构体初始化        
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    // 2.配置为PWM模式1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    // 3.输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 4.互补输出使能
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
    // 5.设置占空比大小
    TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_PULSE;
    // 6.输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // 7.互补输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    // 8.输出通道空闲电平极性配置
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    // 9.互补输出通道空闲电平极性配置
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    // 10.初始化输出比较1
    TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable); // 使能预装载寄存器

/*-------------------刹车和死区结构体初始化-------------------*/
    // 有关刹车和死区结构体的成员具体可参考BDTR寄存器的描述
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    // 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
    // 这里配置的死区时间为152ns
    TIM_BDTRInitStructure.TIM_DeadTime = 11;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    // 当BKIN引脚检测到高电平的时候，输出比较信号被禁止，就好像是刹车一样
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);

    // 使能计数器
    TIM_Cmd(ADVANCE_TIM, ENABLE);    
    // 主输出使能，当使用的是通用定时器时，这句不需要
    TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
}
```

ADVANCE_TIM_Mode_Config()函数中初始化了三个结构体，有关这三个结构体成员的具体含义可参考“定时器初始化结构体详解”小节， 剩下的程序参考注释阅读即可。如果需要修改PWM的周期和占空比，修改头文件里面的ADVANCE_TIM_PERIOD、 ADVANCE_TIM_PSC和ADVANCE_TIM_PULSE这三个宏即可。 PWM信号的频率的计算公司为：F =TIM_CLK/{(ARR+1)*(PSC+1)}， 其中TIM_CLK等于72MHZ，ARR即自动重装载寄存器的值，对应ADVANCE_TIM_PERIOD这个宏，PSC即计数器时钟的分频因子，对应ADVANCE_TIM_PSC这个宏。

- 主函数

```c
// TIM—高级定时器-PWM互补输出带死区时间应用
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_AdvanceTim.h"  

int main(void)
{    
    // 高级定时器初始化
    ADVANCE_TIM_Init();

  while(1)
  {      
  }
}
```

Main函数很简单，调用了ADVANCE_TIM_Init()函数，该函数调用了ADVANCE_TIM_GPIO_Config()和ADVANCE_TIM_Mode_Config()这两个函数完成了定时器GPIO引脚和工作模式的初始化， 这时，相应的GPIO引脚上就可以检测到互补输出的PWM信号，而且带死区时间，如果程序运行的过程中， BKIN引脚被拉高的话，PWM输出会被禁止，就好像是断路或者刹车一样。

## 3. 小结

看似比较复杂，其实就是一个定时器配置的问题，相比与前面这一章多了一个死区时间的配置而已，下面我们总结一下流程：

1. **定时器时钟配置**：

```c
ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK, ENABLE);
```

这行代码用于开启定时器的时钟。具体的时钟源和配置会依赖于你的硬件和定时器的设置。

2. **时基结构体初始化**：

```c
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_PERIOD;
TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_PSC;
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
```

- `TIM_Period`：定时器的自动重装载寄存器的值，即定时器计数器溢出的周期。
- `TIM_Prescaler`：设置计数器的分频系数，影响计数器的时钟频率。
- `TIM_ClockDivision`：配置时钟的分频因子，通常用于配置死区时间等。
- `TIM_CounterMode`：设置计数模式，这里是向上计数模式。
- `TIM_RepetitionCounter`：用于设置重复计数器的值，通常不使用时设为0。
3. **输出比较结构体初始化**：

```c
TIM_OCInitTypeDef TIM_OCInitStructure;
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_PULSE;
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);


```

- `TIM_OCMode`：配置为PWM模式1，用于产生PWM信号。
- `TIM_OutputState`：使能输出比较功能。
- `TIM_OutputNState`：使能互补输出功能。
- `TIM_Pulse`：设置PWM信号的占空比。
- `TIM_OCPolarity`：设置PWM信号的极性。
- `TIM_OCNPolarity`：设置互补PWM信号的极性。
- `TIM_OCIdleState`：设置PWM信号在空闲状态下的电平。
- `TIM_OCNIdleState`：设置互补PWM信号在空闲状态下的电平。
- `TIM_OC1PreloadConfig`：使能预装载寄存器，用于在下一个计数周期装载新的比较值。

**刹车和死区结构体初始化**：

```c
TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
TIM_BDTRInitStructure.TIM_DeadTime = 11;
TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);
```

- `TIM_OSSRState` 和 `TIM_OSSIState`：配置定时器的刹车功能。
- `TIM_LOCKLevel`：配置定时器锁定级别，防止修改配置寄存器。
- `TIM_DeadTime`：设置死区时间，这里设置为11个计数周期。
- `TIM_Break`：使能刹车功能，当BKIN引脚检测到高电平时，输出信号被禁止。
- `TIM_BreakPolarity`：设置刹车信号的极性。
- `TIM_AutomaticOutput`：使能自动输出功能。

**使能计数器和主输出**：

```c
TIM_Cmd(ADVANCE_TIM, ENABLE);
TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
```

- `TIM_Cmd`：使能定时器计数器。
- `TIM_CtrlPWMOutputs`：使能PWM输出，通常在使用高级定时器时需要启用。

---

### **基本原理**

1. **PWM信号生成**：
   
   - PWM（脉宽调制）信号是通过调节信号的高电平持续时间（脉宽）来控制功率。定时器产生PWM信号通过设置比较寄存器的值和计数器的周期来实现。

2. **刹车功能**：
   
   - 刹车功能在定时器的刹车事件发生时关闭输出信号，保护电路或负载。当刹车信号被激活时，定时器的输出被迫置为低电平。

3. **死区时间**：
   
   - 死区时间用于避免PWM信号的两个互补输出同时导通，防止短路。它在两个互补信号之间引入一个小的时间间隔。

### **详细代码示例**

以下是一个高级定时器的配置示例（以STM32系列为例）：

```c
#include "stm32f4xx.h"

void TIM_Config(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

    // 1. 使能定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM1, ENABLE);

    // 2. 定时器时基配置
    TIM_TimeBaseStructure.TIM_Period = 999;          // 自动重装载寄存器周期
    TIM_TimeBaseStructure.TIM_Prescaler = 83;         // 预分频器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // 3. 输出比较配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 499;              // 占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // 4. 刹车和死区时间配置
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIM_BDTRInitStructure.TIM_DeadTime = 11;          // 死区时间
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

    // 5. 使能定时器
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

```

### **应用**

- **电机控制**：在电机驱动中，PWM信号用于控制电机的速度和转向，刹车功能保护电机免受过流损坏。
- **风扇控制**：通过PWM信号调节风扇的转速。
- **LED调光**：使用PWM调节LED的亮度。

---

2024.9.14 第一次修订，后期不再维护
