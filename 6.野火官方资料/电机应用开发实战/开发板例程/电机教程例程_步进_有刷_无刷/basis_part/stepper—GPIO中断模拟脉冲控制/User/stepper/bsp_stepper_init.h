#ifndef __BSP_STEP_MOTOR_INIT_H
#define __BSP_STEP_MOTOR_INIT_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

#define MOTOR_PUL_TIM                   TIM2
#define MOTOR_PUL_CLK_ENABLE()          __HAL_RCC_TIM2_CLK_ENABLE()

#define MOTOR_PUL_IRQn                  TIM2_IRQn
#define MOTOR_PUL_IRQHandler            TIM2_IRQHandler

//���Ŷ���
//Motor ʹ�� 
#define MOTOR_EN_PIN                    GPIO_PIN_12
#define MOTOR_EN_GPIO_PORT              GPIOB
#define MOTOR_EN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ���� 
#define MOTOR_DIR_PIN                   GPIO_PIN_14
#define MOTOR_DIR_GPIO_PORT             GPIOB
#define MOTOR_DIR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ����
#define MOTOR_PUL_PIN                   GPIO_PIN_6
#define MOTOR_PUL_GPIO_PORT             GPIOC
#define MOTOR_PUL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()


/************************************************************/
#define HIGH GPIO_PIN_SET       //�ߵ�ƽ
#define LOW  GPIO_PIN_RESET     //�͵�ƽ

#define ON  LOW                 //��
#define OFF HIGH                //��

#define CW  HIGH                //˳ʱ��
#define CCW LOW                 //��ʱ��

//����ʹ������
/* ���κ꣬��������������һ��ʹ�� */
#define MOTOR_EN(x)         HAL_GPIO_WritePin(MOTOR_EN_GPIO_PORT,MOTOR_EN_PIN,x)
#define MOTOR_PUL(x)        HAL_GPIO_WritePin(MOTOR_PUL_GPIO_PORT,MOTOR_PUL_PIN,x)
#define MOTOR_DIR(x)        HAL_GPIO_WritePin(MOTOR_DIR_GPIO_PORT,MOTOR_DIR_PIN,x)
                          
#define MOTOR_PUL_T()       HAL_GPIO_TogglePin(MOTOR_PUL_GPIO_PORT,MOTOR_PUL_PIN);
  

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
void TIMx_Configuration(void);
extern void stepper_Init(void);
extern void stepper_turn(int tim,float angle,float subdivide,uint8_t dir);
#endif /* __STEP_MOTOR_INIT_H */
