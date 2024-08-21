#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "stm32f10x.h"

// 高级定时器TIM1相关参数宏定义
#define            ADVANCE_TIM                   TIM1                   // 高级定时器选择
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd // 高级定时器时钟使能函数
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1    // 高级定时器时钟

// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            ADVANCE_TIM_PERIOD            (8-1)                  // 高级定时器计数周期
#define            ADVANCE_TIM_PSC               (9-1)                  // 高级定时器分频因子
#define            ADVANCE_TIM_PULSE             4                      // 高级定时器输出脉冲宽度

// 中断向量定义
#define            ADVANCE_TIM_IRQ               TIM1_UP_IRQn           // 高级定时器更新中断
#define            ADVANCE_TIM_IRQHandler        TIM1_UP_IRQHandler     // 高级定时器更新中断处理函数

// TIM1 输出比较通道
#define            ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA   // 通道1 GPIO时钟使能函数
#define            ADVANCE_TIM_CH1_PORT          GPIOA                  // 通道1 GPIO端口
#define            ADVANCE_TIM_CH1_PIN           GPIO_Pin_8             // 通道1 GPIO引脚

// TIM1 输出比较通道的互补通道
#define            ADVANCE_TIM_CH1N_GPIO_CLK      RCC_APB2Periph_GPIOB  // 通道1N GPIO时钟使能函数
#define            ADVANCE_TIM_CH1N_PORT          GPIOB                 // 通道1N GPIO端口
#define            ADVANCE_TIM_CH1N_PIN           GPIO_Pin_13           // 通道1N GPIO引脚

// TIM1 输出比较通道的刹车通道
#define            ADVANCE_TIM_BKIN_GPIO_CLK      RCC_APB2Periph_GPIOB  // 刹车通道 GPIO时钟使能函数
#define            ADVANCE_TIM_BKIN_PORT          GPIOB                 // 刹车通道 GPIO端口
#define            ADVANCE_TIM_BKIN_PIN           GPIO_Pin_12           // 刹车通道 GPIO引脚

void ADVANCE_TIM_Init(void);


#endif	