#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H

#include "stm32f10x.h"

#define BASIC_TIM6 // 如果使用TIM7，注释掉这个宏即可

#ifdef  BASIC_TIM6 // 使用基本定时器TIM6
#define            BASIC_TIM                   TIM6                   // 选择使用的定时器--TIM6
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd // 使能定时器时钟--APB1
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM6    // 定时器时钟--TIM6
#define            BASIC_TIM_Period            1000-1                 // 定时器周期--1000-1
#define            BASIC_TIM_Prescaler         71                     // 定时器分频器--71
#define            BASIC_TIM_IRQ               TIM6_IRQn              // 定时器中断向量号--TIM6_IRQn
#define            BASIC_TIM_IRQHandler        TIM6_IRQHandler        // 定时器中断服务程序--TIM6_IRQHandler

#else  // 使用基本定时器TIM7
#define            BASIC_TIM                   TIM7
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM7
#define            BASIC_TIM_Period            1000-1
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM7_IRQn
#define            BASIC_TIM_IRQHandler        TIM7_IRQHandler

#endif

void BASIC_TIM_Init(void);


#endif
