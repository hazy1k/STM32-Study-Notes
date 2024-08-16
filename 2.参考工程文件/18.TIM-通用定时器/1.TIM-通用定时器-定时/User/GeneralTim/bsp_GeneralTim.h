#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H

#include "stm32f10x.h"

// ͨ�ö�ʱ��TIM�������壬ֻ��TIM2��3��4��5
#define GENERAL_TIM2    1
#define GENERAL_TIM3    0
#define GENERAL_TIM4    0
#define GENERAL_TIM5    0

#if  GENERAL_TIM2
#define            GENERAL_TIM                   TIM2                   // ѡ��ʹ�õĶ�ʱ��--TIM2
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd // ѡ����صĶ�ʱ��ʱ��--RCC_APB1Periph_TIM2
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM2    // ѡ��ʹ�õĶ�ʱ��ʱ��--RCC_APB1Periph_TIM2
#define            GENERAL_TIM_Period            (1000-1)               // ��ʱ������--1000-1
#define            GENERAL_TIM_Prescaler         71                     // ��ʱ����Ƶϵ��--71
#define            GENERAL_TIM_IRQ               TIM2_IRQn              // ѡ��ʹ�õĶ�ʱ���ж�--TIM2_IRQn
#define            GENERAL_TIM_IRQHandler        TIM2_IRQHandler        // ѡ��ʹ�õĶ�ʱ���жϴ�����--TIM2_IRQHandler

#elif  GENERAL_TIM3
#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_Period            (1000-1)
#define            GENERAL_TIM_Prescaler         71
#define            GENERAL_TIM_IRQ               TIM3_IRQn
#define            GENERAL_TIM_IRQHandler        TIM3_IRQHandler

#elif   GENERAL_TIM4
#define            GENERAL_TIM                   TIM4
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM4
#define            GENERAL_TIM_Period            (1000-1)
#define            GENERAL_TIM_Prescaler         71
#define            GENERAL_TIM_IRQ               TIM4_IRQn
#define            GENERAL_TIM_IRQHandler        TIM4_IRQHandler

#elif   GENERAL_TIM5
#define            GENERAL_TIM                   TIM5
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM5
#define            GENERAL_TIM_Period            (1000-1)
#define            GENERAL_TIM_Prescaler         71
#define            GENERAL_TIM_IRQ               TIM5_IRQn
#define            GENERAL_TIM_IRQHandler        TIM5_IRQHandler

#endif

void GENERAL_TIM_Init(void);


#endif