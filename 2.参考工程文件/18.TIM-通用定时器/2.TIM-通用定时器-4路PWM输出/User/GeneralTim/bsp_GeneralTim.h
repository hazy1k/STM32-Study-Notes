#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H


#include "stm32f10x.h"

// 通用定时器参数宏定义-TIM3
#define            GENERAL_TIM                   TIM3                   // 选择使用的通用定时器-TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd // 选择要挂载的时钟-APB1
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3    // 选择时钟源-APB1时钟_TIM3
#define            GENERAL_TIM_Period            9                      // 定时器周期-9
#define            GENERAL_TIM_Prescaler         71                     // 定时器分频系数-71
// TIM3 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

// TIM3 输出比较通道2
#define            GENERAL_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH2_PORT          GPIOA
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7

// TIM3 输出比较通道3
#define            GENERAL_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH3_PORT          GPIOB
#define            GENERAL_TIM_CH3_PIN           GPIO_Pin_0

// TIM3 输出比较通道4
#define            GENERAL_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH4_PORT          GPIOB
#define            GENERAL_TIM_CH4_PIN           GPIO_Pin_1


void GENERAL_TIM_Init(void);


#endif