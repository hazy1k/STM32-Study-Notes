# 第三十六章 TIM-全彩呼吸灯

## 1. 硬件设计

与上一章一样

## 2. 软件设计

### 2.1 编程目标

1. 在单色呼吸灯的基础上，增加PWM输出通道，三个通道分别控制红绿蓝颜色；

2. 编写中断服务函数，增加对拟合波形幅值的控制；

3. 计算获取新的PWM数据表；

### 2.2 代码分析

- 硬件相关宏定义

```c
// 电压幅值等级数
#define AMPLITUDE_CLASS 256

// 控制输出波形的频率
extern __IO uint16_t period_class ;

// PWM表中的点数
extern uint16_t POINT_NUM;

// TIM3宏定义
#define BRE_TIMx TIM3
//中断相关
#define BRE_TIMx_IRQn       TIM3_IRQn             
#define BRE_TIMx_IRQHandler TIM3_IRQHandler
//时钟
#define BRE_TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define BRE_TIM_CLK           RCC_APB1Periph_TIM3
#define BRE_TIM_GPIO_CLK     (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
// 红灯的引脚需要重映射
#define BRE_GPIO_REMAP_FUN() GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                 
// 红灯
#define BRE_RED_TIM_LED_PORT          GPIOB
#define BRE_RED_TIM_LED_PIN           GPIO_Pin_5
#define BRE_RED_TIM_OCxInit           TIM_OC2Init          // 通道初始化函数
#define BRE_RED_TIM_OCxPreloadConfig  TIM_OC2PreloadConfig // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIMx->BRE_RED_CCRx ，可访问该通道的比较寄存器
#define BRE_RED_CCRx                   CCR2        

// 绿灯
#define BRE_GREEN_TIM_LED_PORT         GPIOB
#define BRE_GREEN_TIM_LED_PIN          GPIO_Pin_0
#define BRE_GREEN_TIM_OCxInit          TIM_OC3Init          // 通道初始化函数
#define BRE_GREEN_TIM_OCxPreloadConfig TIM_OC3PreloadConfig // 通道重载配置函数
#define BRE_GREEN_CCRx                 CCR3

// 蓝灯
#define BRE_BLUE_TIM_LED_PORT          GPIOB
#define BRE_BLUE_TIM_LED_PIN           GPIO_Pin_1
#define BRE_BLUE_TIM_OCxInit           TIM_OC4Init           // 通道初始化函数
#define BRE_BLUE_TIM_OCxPreloadConfig  TIM_OC4PreloadConfig  // 通道重载配置函数
#define BRE_BLUE_CCRx                  CCR4
```

- 初始化GPIO

```c
// GPIO 初始化
static void TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(BRE_TIM_GPIO_CLK, ENABLE); // 定时器GPIO时钟使能 
  BRE_GPIO_REMAP_FUN(); // 引脚映射设置
  // 配置LED灯用到的引脚
  //红
  GPIO_InitStructure.GPIO_Pin =  BRE_RED_TIM_LED_PIN ;    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BRE_RED_TIM_LED_PORT, &GPIO_InitStructure);
  //绿
  GPIO_InitStructure.GPIO_Pin =  BRE_GREEN_TIM_LED_PIN ;
  GPIO_Init(BRE_GREEN_TIM_LED_PORT, &GPIO_InitStructure);
  //蓝
  GPIO_InitStructure.GPIO_Pin =  BRE_BLUE_TIM_LED_PIN ;
  GPIO_Init(BRE_BLUE_TIM_LED_PORT, &GPIO_InitStructure);
}
```

本实验对GPIO的初始化也相对单色呼吸灯实验作了修改，同时初始化三个通道。

- 定义PWM表

```c
/********全彩呼吸灯实验的指数曲线表**********/
/* LED亮度等级 PWM表,指数曲线 ，
此表使用工程目录下的python脚本index_wave.py生成*/
const uint16_t indexWave[] = {
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
    2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
    5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
    11, 12, 12, 13, 14, 15, 17, 18,
    19, 20, 22, 23, 25, 27, 29, 31,
    33, 36, 38, 41, 44, 47, 51, 54,
    58, 63, 67, 72, 77, 83, 89, 95,
    102, 110, 117, 126, 135, 145, 156,
    167, 179, 192, 206, 221, 237, 254,
    272, 292, 313, 336, 361, 387, 415,
    445, 477, 512, 512, 477, 445, 415,
    387, 361, 336, 313, 292, 272, 254,
    237, 221, 206, 192, 179, 167, 156,
    145, 135, 126, 117, 110, 102, 95,
    89, 83, 77, 72, 67, 63, 58, 54, 51,
    47, 44, 41, 38, 36, 33, 31, 29, 27,
    25, 23, 22, 20, 19, 18, 17, 15, 14,
    13, 12, 12, 11, 10, 9, 9, 8, 8, 7, 7,
    6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1
};

/*********SPWM实验的正弦曲线表*************/
/* SPWM表,正弦曲线，此表使用工程目录下的python脚本sin_wave.py生成*/
const uint16_t indexWave[] = {
    0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 89, 98,
    107, 116, 125, 133, 142, 151, 159, 168, 176,
    184, 193, 201, 209, 218, 226, 234, 242, 249,
    257, 265, 273, 280, 288, 295, 302, 310, 317,
    324, 331, 337, 344, 351, 357, 364, 370, 376,
    382, 388, 394, 399, 405, 410, 416, 421, 426,
    431, 436, 440, 445, 449, 454, 458, 462, 465,
    469, 473, 476, 479, 482, 485, 488, 491, 493,
    496, 498, 500, 502, 503, 505, 506, 508, 509,
    510, 510, 511, 512, 512, 512, 512, 512, 512,
    511, 510, 510, 509, 508, 506, 505, 503, 502,
    500, 498, 496, 493, 491, 488, 485, 482, 479,
    476, 473, 469, 465, 462, 458, 454, 449, 445,
    440, 436, 431, 426, 421, 416, 410, 405, 399,
    394, 388, 382, 376, 370, 364, 357, 351, 344,
    337, 331, 324,  317, 310, 302, 295, 288, 280,
    273, 265, 257, 249, 242, 234, 226, 218, 209,
    201, 193, 184, 176, 168, 159, 151, 142, 133,
    125, 116, 107, 98, 89, 81, 72, 63, 54, 45, 36,
    27, 18, 9, 0
};

//计算PWM表有多少个元素
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]);
```

代码中列出的PWM表内元素的最大值均为512，元素个数均为128， 把两个表绘制成成曲线如图：呼吸曲线和正弦曲线

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SPWM010.jpeg)

- NVIC配置

```c
// 配置NVIC中断优先级
static void NVIC_Config_PWM(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 设置中断优先级组1

  // 配置TIM3_IRQ中断为中断源
  NVIC_InitStructure.NVIC_IRQChannel = BRE_TIMx_IRQn; // 选择中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断
  NVIC_Init(&NVIC_InitStructure); // 初始化
}
```

- 定时器PWM配置

```c
static void TIMx_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;                                                                                

  // 设置TIMCLK 时钟
  BRE_TIM_APBxClock_FUN(BRE_TIM_CLK, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = (512-1);                // 当定时器从0计数到 TIM_Period+1 为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = (10-1);              // 设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;   // 设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;// 向上计数模式
  TIM_TimeBaseInit(BRE_TIMx, &TIM_TimeBaseStructure); // 初始化定时器

  // PWM模式配置
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;            // 配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// 使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;                           // 设置初始PWM脉冲宽度为0    
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;     // 当定时器计数值小于CCR_Val时为低电平

  // RGB通道
  // 红色
  BRE_RED_TIM_OCxInit(BRE_TIMx, &TIM_OCInitStructure);    
  BRE_RED_TIM_OCxPreloadConfig(BRE_TIMx, TIM_OCPreload_Enable); // 使能预装载    
  // 绿色
  BRE_GREEN_TIM_OCxInit(BRE_TIMx, &TIM_OCInitStructure);    
  BRE_GREEN_TIM_OCxPreloadConfig(BRE_TIMx, TIM_OCPreload_Enable); // 使能预装载    
  // 蓝色
  BRE_BLUE_TIM_OCxInit(BRE_TIMx, &TIM_OCInitStructure);    
  BRE_BLUE_TIM_OCxPreloadConfig(BRE_TIMx, TIM_OCPreload_Enable); // 使能预装载    

  TIM_ARRPreloadConfig(BRE_TIMx, ENABLE); // 使能TIM重载寄存器ARR
  TIM_Cmd(BRE_TIMx, ENABLE); // 使能定时器    
  TIM_ITConfig(BRE_TIMx, TIM_IT_Update, ENABLE); // 使能update中断
  NVIC_Config_PWM(); // 配置中断优先级        
}
```

本配置中同时使能三个PWM通道，而定时器初始化中的TIM_Period成员被配置为(512-1)，即PWM表元素的极大值，TIM_Prescaler被配置为(10-1)， 相对于单色呼吸灯实验，提高了定时器的时钟频率。

## 3. 小结

本章与上面一章单色呼吸灯大同小异，就是多加两个通道，用来进行混色，下面我们来复盘一下：

### 实验步骤：

1. **硬件连接**：
   
   - 连接LED的红、绿、蓝引脚到STM32F103的PWM输出引脚（如TIM1、TIM2等）。
   - 通过适当的电阻限流。

2. **配置时钟**：
   
   - 配置系统时钟，确保TIM的时钟频率正确。

3. **配置定时器（TIM）**：
   
   - 设置TIM为PWM模式。
   - 配置TIM的预分频器和自动重装载寄存器（ARR）以获得所需的PWM频率。
   - 配置通道1、2和3来控制红、绿、蓝LED。

4. **编写代码**：
   
   - 初始化TIM。
   - 设置PWM输出。
   - 实现呼吸灯效果的逻辑，通过调整PWM占空比来实现渐变效果。

### 代码详解：

```c
#include "stm32f10x.h"

// 定义周期和预分频器的值
#define PERIOD  1000
#define PRESCALER  72

// 初始化TIM
void TIM_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    TIM_TimeBaseInitStruct.TIM_Prescaler = PRESCALER - 1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = PERIOD - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = PERIOD / 2; // 初始占空比50%
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM1, &TIM_OCInitStruct);
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);
    TIM_OC3Init(TIM1, &TIM_OCInitStruct);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

// 设置PWM占空比
void SetPWM_Pulse(uint32_t channel, uint32_t pulse) {
    if (channel == TIM_Channel_1) TIM1->CCR1 = pulse;
    if (channel == TIM_Channel_2) TIM1->CCR2 = pulse;
    if (channel == TIM_Channel_3) TIM1->CCR3 = pulse;
}

// 呼吸灯效果
void BreathingLight(void) {
    uint32_t pulse;
    while (1) {
        for (pulse = 0; pulse < PERIOD; pulse++) {
            SetPWM_Pulse(TIM_Channel_1, pulse);
            SetPWM_Pulse(TIM_Channel_2, PERIOD - pulse);
            SetPWM_Pulse(TIM_Channel_3, (pulse + PERIOD / 2) % PERIOD); // 简单变化的蓝色
            Delay(10); // 延时函数，根据需要调整
        }
    }
}

// 简单延时函数
void Delay(uint32_t time) {
    for (volatile uint32_t i = 0; i < time * 1000; i++);
}

int main(void) {
    SystemInit();
    TIM_Config();
    BreathingLight();
    while (1);
}
```

### 代码说明：

- **`TIM_Config`**: 配置定时器TIM1为PWM模式，设置预分频器和周期。`TIM_OCInitStruct` 用于配置PWM的占空比和极性。
- **`SetPWM_Pulse`**: 调节指定通道的PWM占空比。
- **`BreathingLight`**: 实现呼吸灯效果，通过调整PWM占空比来使LED颜色变化。
- **`Delay`**: 一个简单的延时函数，用于调整亮度变化的速度。

---

2024.9.18 第一次修订，后期不再维护
