#ifndef __TOUCHPAD_H_
#define	__TOUCHPAD_H_

#include "stm32f1xx.h"
#define TOUCHPAD_GATE_VAL 	70
// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��TOUCHPAD_TIMx_PRESCALER+1��
#define TOUCHPAD_TIM_PRESCALER              47 //1.5MHz
// ���嶨ʱ������
#define TOUCHPAD_TIM_ARR                     0xFFFF


/******************** TPAD �������ò������� **************************/
#define TOUCHPAD_TIMx                        TIM5
#define TOUCHPAD_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM5_CLK_ENABLE()
#define TOUCHPAD_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM5_CLK_DISABLE()

#define TOUCHPAD_GPIO_RCC_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define TOUCHPAD_GPIO_PIN                    GPIO_PIN_1
#define TOUCHPAD_GPIO                        GPIOA
#define TOUCHPAD_TIM_CHANNEL                 TIM_CHANNEL_2
#define TOUCHPAD_TIM_FLAG_CCR                TIM_FLAG_CC2

/************************** TPAD ��������********************************/
uint8_t TPAD_Init(void);
uint8_t TPAD_Scan(uint8_t mode);
 

#endif /* __TOUCHPAD_H_ */

