#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H

#include "stm32f10x.h"

/* ��ʱ���������� */
#define TIM_BASE TIM6
#define TIM_BASE_Period    999// ARRֵ
#define TIM_BASE_Prescaler 71 // Ԥ��Ƶϵ��
/* �ж���� */
#define TIM_BASE_IRQ TIM6_IRQn
#define TIM_BASE_IRQHandler TIM6_IRQHandler

void TIM_BASE_Init(void);

#endif
