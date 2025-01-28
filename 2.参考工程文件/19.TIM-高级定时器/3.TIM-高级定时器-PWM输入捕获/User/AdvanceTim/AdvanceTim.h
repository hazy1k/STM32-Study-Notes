#ifndef __ADVANCE_TIM_H
#define __ADVANCE_TIM_H

#include "stm32f10x.h"

/* TIM1�������� */
#define ATIMx TIM1
#define ATIM_APBxClock RCC_APB2PeriphClockCmd
#define ATIM_CLK RCC_APB2Periph_TIM1
#define ATIM_Period 999
#define ATIM_Prescaler 71
/* ���벶��ͨ������ */
#define ATIM_CH1_GPIO_CLK RCC_APB2Periph_GPIOA
#define ATIM_CH1_GPIO_PORT GPIOA
#define ATIM_CH1_GPIO_PIN GPIO_Pin_8
#define ATIM_IC1PWM_CHANNEL TIM_Channel_1
#define ATIM_IC2PWM_CHANNEL TIM_Channel_2
/* �ж�������� */
#define ATIM_IRQ TIM1_CC_IRQn
#define ATIM_IRQHandler TIM1_CC_IRQHandler

void ATIMx_Init(void);

#endif /* __ADVANCE_TIM_H */
