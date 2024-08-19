#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "stm32f10x.h"


/********************高级定时器TIM参数定义，只限TIM1、8************/
#define ADVANCE_TIM1 // 如果使用TIM8，注释掉这个宏即可

#ifdef  ADVANCE_TIM1 // 使用高级定时器TIM1

// TIM1参数定义
#define            ADVANCE_TIM                   TIM1                   // 高级定时器TIM1
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd // 高级定时器挂载APB2时钟
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1    // 高级定时器时钟源
#define            ADVANCE_TIM_Period            (1000-1)               // 高级定时器周期
#define            ADVANCE_TIM_Prescaler         71                     // 高级定时器分频系数
#define            ADVANCE_TIM_IRQ               TIM1_UP_IRQn           // 高级定时器中断向量
#define            ADVANCE_TIM_IRQHandler        TIM1_UP_IRQHandler     // 高级定时器中断服务函数

#else  // 使用高级定时器TIM8
#define            ADVANCE_TIM                   TIM8
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM8
#define            ADVANCE_TIM_Period            (1000-1)
#define            ADVANCE_TIM_Prescaler         71
#define            ADVANCE_TIM_IRQ               TIM8_UP_IRQn
#define            ADVANCE_TIM_IRQHandler        TIM8_UP_IRQHandler

#endif

void ADVANCE_TIM_Init(void);


#endif