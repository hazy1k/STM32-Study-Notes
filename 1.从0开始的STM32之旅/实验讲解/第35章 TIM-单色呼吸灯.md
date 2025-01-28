# 第三十五章 TIM-单色呼吸灯

## 1. 硬件设计

使用定时器输出控制LED灯呈呼吸效果，无其他硬件设计

## 2. 软件设计

### 2.1 编程大纲

1) 初始化PWM输出通道，初始化PWM工作模式；

2) 计算获取PWM数据表；

3) 编写中断服务函数，在中断服务函数根据PWM数据表切换比较寄存器的值；

### 2.2 代码分析

- 定时器参数宏定义

```c
#define BRE_TIMx                    TIM3
#define BRE_TIM_APBxClock_FUN       RCC_APB1PeriphClockCmd
#define BRE_TIM_CLK                 RCC_APB1Periph_TIM3
#define BRE_TIM_GPIO_APBxClock_FUN  RCC_APB2PeriphClockCmd
#define BRE_TIM_GPIO_CLK            (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
```

- 通道宏定义

```c
// 红灯的引脚需要重映射
#define  BRE_GPIO_REMAP_FUN()     GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                 
#define  BRE_TIM_LED_PORT         GPIOB
#define  BRE_TIM_LED_PIN          GPIO_Pin_5
#define  BRE_TIM_OCxInit          TIM_OC2Init          // 通道选择，1~4
#define  BRE_TIM_OCxPreloadConfig TIM_OC2PreloadConfig 
#define  BRE_CCRx                 CCR2
#define  BRE_TIMx_IRQn            TIM3_IRQn            // 中断
#define  BRE_TIMx_IRQHandler      TIM3_IRQHandler
```

为方便切换LED灯的颜色，它定义了三组宏，通过修改代码中的“#define LIGHT_COLOR RED_LIGHT”语句， 可以切换使用红、绿、蓝三种颜色的呼吸灯。

在每组宏定义中，与全彩LED灯实验中的类似，定义了定时器编号、定时器时钟使能库函数、引脚重映射操作、GPIO端口和引脚号、 通道对应的比较寄存器名以及中断通道和中断服务函数名。

与全彩LED灯实验不同，本实验中定时器的比较寄存器CCRx在控制呼吸灯的单个周期内需要切换为PWM表中不同的数值，所以需要利用定时器中断。

- PWM表

```c
// LED亮度等级 PWM表,指数曲线 ，此表使用工程目录下的python脚本index_wave.py生成
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
};

//计算PWM表有多少个元素
uint16_t POINT_NUM = sizeof(indexWave) / sizeof(indexWave[0]); 
```

- TIM GPIO初始化

```c
// 配置TIM复用输出PWM时用到的I/O
static void TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(BRE_TIM_GPIO_CLK, ENABLE);      // 使能GPIO时钟 
  BRE_TIM_GPIO_APBxClock_FUN(BRE_TIM_GPIO_CLK, ENABLE ); // 使能GPIO时钟
  BRE_GPIO_REMAP_FUN(); // 使能GPIO映射

  // 配置呼吸灯用到的引脚
  GPIO_InitStructure.GPIO_Pin =  BRE_TIM_LED_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( BRE_TIM_LED_PORT, &GPIO_InitStructure );
}
```

由于本实验直接使用定时器输出通道的脉冲信号控制LED灯，此处代码把GPIO相关的引脚配置成了复用推挽输出模式。其中由于红灯使用的引脚需要用到第二功能， 本代码使用宏BRE_GPIO_REMAP_FUN ()进行了该引脚的功能重定义操作

- NVIC初始化

```c
// 配置嵌套向量中断控制器NVIC
static void NVIC_Config_PWM(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 设置中断优先级组1

  // 配置TIM3_IRQ中断为中断源
  NVIC_InitStructure.NVIC_IRQChannel = BRE_TIMx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
```

- TIM输出PWM模式配置

```c
static void TIMx_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;                                                                                

  // 设置TIM3CLK 时钟
  BRE_TIM_APBxClock_FUN(BRE_TIM_CLK, ENABLE); 

  // 基本定时器配置   
  TIM_TimeBaseStructure.TIM_Period = (1024-1);;              // 当定时器从0计数到 TIM_Period+1 ，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = (200-1);             // 设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;   // 设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 向上计数模式
  TIM_TimeBaseInit(BRE_TIMx, &TIM_TimeBaseStructure);

  // PWM模式配置 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;            // 配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// 使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;                           // 设置初始PWM脉冲宽度为0    
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;     // 当定时器计数值小于CCR1_Val时为低电平

  BRE_TIM_OCxInit(BRE_TIMx, &TIM_OCInitStructure );            // 使能通道
  BRE_TIM_OCxPreloadConfig(BRE_TIMx, TIM_OCPreload_Enable);    // 使能预装载    
  TIM_ARRPreloadConfig(BRE_TIMx, ENABLE);                      // 使能TIM重载寄存器ARR
  TIM_Cmd(BRE_TIMx, ENABLE);                                   // 使能定时器    
  TIM_ITConfig(BRE_TIMx, TIM_IT_Update, ENABLE);               // 使能update中断
  NVIC_Config_PWM();                                           // 配置中断优先级        
}
```

本配置主体与全彩LED灯实验中的类似，代码中初始化了控制RGB灯用的定时器，它被配置为向上计数， PWM通道输出也被配置成当计数器CNT的值小于输出比较寄存器CCRx的值时，PWM通道输出低电平，点亮LED灯。 在函数的最后还使能了定时器中断，每当定时器的一个计数周期完成时，产生中断，配合中断服务函数， 即可切换CCRx比较寄存器的值。

代码中的TIM_Period和TIM_Prescaler是关键配置。

其中TIMPeriod被配置为(1024-1)，它控制控制定时器的定时周期，定时器的计数寄存器CNT从0开始，每个时钟会对计数器加1， 计数至1023时完成一次计数，产生中断，也就是说一共1024个计数周期，与PWM表元素中的最大值相同。若定时器的输出比较寄存器CCRx被赋值为PWM表中的元素， 即可改变输出对应占空比的PWM波，控制LED灯，如：

![屏幕截图 2024 08 23 105037](https://img.picgo.net/2024/08/23/-2024-08-23-10503727c809d7f7479a00.png)

根据本工程中的PWM表更新CCRx的值，即可输出占空比呈呼吸特性曲线变化的PWM波形，达到呼吸灯的效果。

最终，拟合曲线的周期由TIMPeriod、PWM表的点数、TIM_Prescaler以及下面中断服务函数的period_cnt比较值共同决定， 本工程需要调整这些参数使得拟合曲线的周期约为3秒，从而达到较平缓的呼吸效果。

## 3. 小结

本章实际上就是利用TIM控制高低电平时间来模拟一个呼吸灯，稍微总结一下：

### 实现步骤

1. **配置 TIM 定时器**：设置一个定时器产生 PWM 信号。
2. **配置 PWM 输出**：通过 PWM 信号调节 LED 的亮度。
3. **实现呼吸效果**：在主循环中调整 PWM 的占空比，使 LED 的亮度随时间变化。

### 示例代码

以下是如何用 TIM3 产生单色呼吸灯的示例代码。假设你使用的是 STM32F103 系列单片机。

```c
#include "stm32f10x.h"

// 定义 PWM 参数
#define PWM_FREQUENCY 1000  // PWM 频率 1 kHz
#define PWM_PERIOD (SystemCoreClock / PWM_FREQUENCY) // PWM 周期

void TIM3_Config(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 启用 TIM3 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置 GPIO 引脚（假设为 GPIOA 的 PA6）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 TIM3 时间基数
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置 TIM3 通道 1 为 PWM 模式 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始脉冲宽度为 0

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 使能 TIM3
    TIM_Cmd(TIM3, ENABLE);
}

void Delay(__IO uint32_t nTime)
{
    // 简单延时函数实现
    for (; nTime != 0; nTime--);
}

int main(void)
{
    // 系统初始化
    SystemInit();

    // 配置 TIM3
    TIM3_Config();

    uint16_t pwm_duty_cycle;
    int8_t direction = 1;  // 1: 增加亮度, -1: 减少亮度

    while (1)
    {
        // 更新 PWM 占空比，模拟呼吸灯效果
        for (pwm_duty_cycle = 0; pwm_duty_cycle < PWM_PERIOD; pwm_duty_cycle += 10)
        {
            TIM_SetCompare1(TIM3, pwm_duty_cycle);
            Delay(100); // 控制呼吸灯的速度
        }

        // 反向减少亮度
        for (pwm_duty_cycle = PWM_PERIOD; pwm_duty_cycle > 0; pwm_duty_cycle -= 10)
        {
            TIM_SetCompare1(TIM3, pwm_duty_cycle);
            Delay(100); // 控制呼吸灯的速度
        }
    }
}
```

### 代码说明

1. **TIM 配置**：配置 TIM3 为 PWM 模式，并设置 PWM 频率为 1 kHz。
2. **GPIO 配置**：将 GPIOA 的 PA6 引脚设置为 TIM3 的 PWM 输出引脚。
3. **PWM 占空比控制**：通过更新 TIM3 通道 1 的比较值来控制 LED 的亮度。
4. **呼吸效果**：在主循环中，通过逐渐增加和减少占空比实现 LED 亮度的渐变，模拟呼吸效果。

---

2024.9.17 第一次修订，后期不再维护
