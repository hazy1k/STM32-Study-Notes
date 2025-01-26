#ifndef __GENERAL_TIM_H
#define __GENERAL_TIM_H

#include "stm32f10x.h"

/* ���Ӷ�ʱ������ */
#define TIM_MASTER TIM2 // ����ʱ��--TIM2
#define TIM_MASTER_CLK RCC_APB1Periph_TIM2
#define TIM_SLAVE1 TIM3 // �Ӷ�ʱ��1--TIM3
#define TIM_SLAVE1_CLK RCC_APB1Periph_TIM3
#define TIM_SLAVE2 TIM4 // �Ӷ�ʱ��2--TIM4
#define TIM_SLAVE2_CLK RCC_APB1Periph_TIM4

/* ���Ӷ�ʱ��ͨ������ */
#define TIM_MASTER_PIN GPIO_Pin_10 // TIM2-PB10
#define TIM_MASTER_PORT GPIOB
#define GTIM_MASTER_GPIO_CLK RCC_APB2Periph_GPIOB

/* �Ӷ�ʱ�����ͨ������ */
#define TIM_SLAVE1_PIN GPIO_Pin_6 // TIM3-PC6
#define TIM_SLAVE1_PORT GPIOC
#define GTIM_SLAVE1_GPIO_CLK RCC_APB2Periph_GPIOC
#define TIM_SLAVE2_PIN GPIO_Pin_6 // TIM4-PB6
#define TIM_SLAVE2_PORT GPIOB
#define GTIM_SLAVE2_GPIO_CLK RCC_APB2Periph_GPIOB

void TIMx_Init(void);

#endif /* __GENERAL_TIM_H */
