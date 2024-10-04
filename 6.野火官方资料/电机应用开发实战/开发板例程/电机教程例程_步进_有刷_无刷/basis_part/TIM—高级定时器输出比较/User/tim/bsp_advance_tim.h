#ifndef __ADVANCE_TIM_H
#define	__ADVANCE_TIM_H

#include "stm32f1xx.h"

/* �߼���ʱ������ */
/* �߼���ʱ������ */
#define ADVANCE_TIM           		      TIM8
#define ADVANCE_TIM_CLK_ENABLE()  	    __HAL_RCC_TIM8_CLK_ENABLE()
#define ADVANCE_TIM_IRQn		            TIM8_CC_IRQn
#define ADVANCE_TIM_IRQHandler          TIM8_CC_IRQHandler

/* TIM8ͨ��1������� */
#define CHANNEL1_OC_PIN           		  GPIO_PIN_6              
#define CHANNEL1_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL1_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_1           TIM_CHANNEL_1

/* TIM8ͨ��2������� */
#define CHANNEL2_OC_PIN           		  GPIO_PIN_7              
#define CHANNEL2_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL2_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_2           TIM_CHANNEL_2

/* TIM8ͨ��3������� */
#define CHANNEL3_OC_PIN           		  GPIO_PIN_8              
#define CHANNEL3_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL3_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_3           TIM_CHANNEL_3

/* TIM8ͨ��4������� */
#define CHANNEL4_OC_PIN           		  GPIO_PIN_9              
#define CHANNEL4_OC_GPIO_PORT     		  GPIOC                     
#define CHANNEL4_OC_GPIO_CLK_ENABLE() 	__HAL_RCC_GPIOC_CLK_ENABLE()
#define ADVANCE_TIM_CHANNEL_4           TIM_CHANNEL_4

/*Ƶ����ز���*/
//��ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/(TIM_PRESCALER-1)
//168/(TIM_PRESCALER-1)=1Mhz
//������Ҫ��Ƶ�ʿ����Լ�����
#define TIM_PRESCALER                72
// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFF
#define TIM_PERIOD                   0xFFFF

extern TIM_HandleTypeDef TIM_AdvanceHandle;

void TIMx_AdvanceConfig(void);

#endif /* __ADVANCE_TIM_H */


