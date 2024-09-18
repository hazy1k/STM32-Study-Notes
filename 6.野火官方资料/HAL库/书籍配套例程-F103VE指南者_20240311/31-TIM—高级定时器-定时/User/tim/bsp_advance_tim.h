#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

// ��ʱ��Ԥ��Ƶ
#define ADVANCED_TIM_PRESCALER            71 
// �߼���ʱ���ظ������Ĵ���ֵ
#define ADVANCED_TIM_REPETITIONCOUNTER    9
// ��ʱ������
#define ADVANCED_TIM_PERIOD               1000 


#define ADVANCED_TIMx                     TIM1
#define ADVANCED_TIM_RCC_CLK_ENABLE()     __HAL_RCC_TIM1_CLK_ENABLE()
#define ADVANCED_TIM_RCC_CLK_DISABLE()    __HAL_RCC_TIM1_CLK_DISABLE()

#define ADVANCE_TIM_IRQn                  TIM1_UP_IRQn
#define ADVANCE_TIM_IRQHandler          TIM1_UP_IRQHandler




extern TIM_HandleTypeDef TIM_TimeBaseStructure;

 void TIM_Mode_Config(void);

#endif /* __ADVANCE_TIM_H */

