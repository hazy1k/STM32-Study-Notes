#ifndef __BSP_STEP_MOTOR_INIT_H
#define	__BSP_STEP_MOTOR_INIT_H

#include "stm32f1xx_hal.h"

/* ��������ṹ�� */
typedef struct{
  uint16_t pul_pin;
  uint16_t dir_pin;
  uint16_t en_pin;
  uint32_t pul_channel;
  GPIO_TypeDef *pul_port;
  GPIO_TypeDef *dir_port;
  GPIO_TypeDef *en_port;
  uint16_t oc_pulse_num;
}Stepper_TypeDef;




/*�궨��*/
/*******************************************************/
#define MOTOR_PUL_TIM                        TIM8
#define MOTOR_PUL_IRQn                       TIM8_UP_TIM13_IRQn
#define MOTOR_PUL_IRQHandler                 TIM8_UP_TIM13_IRQHandler
#define MOTOR_PUL_CLK_ENABLE()               __HAL_RCC_TIM8_CLK_ENABLE()

/*********************X�������Ŷ���*******************/
//Motor ����
#define X_MOTOR_DIR_PIN                      GPIO_PIN_14
#define X_MOTOR_DIR_GPIO_PORT                GPIOB
#define X_MOTOR_DIR_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ʹ��
#define X_MOTOR_EN_PIN                       GPIO_PIN_12
#define X_MOTOR_EN_GPIO_PORT                 GPIOB
#define X_MOTOR_EN_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ����
#define X_MOTOR_PUL_PORT                     GPIOC
#define X_MOTOR_PUL_PIN                      GPIO_PIN_6
#define X_MOTOR_PUL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

//��ʱ��ͨ��
#define X_MOTOR_PUL_CHANNEL                  TIM_CHANNEL_1

/*********************Y�������Ŷ���*******************/
//Motor ����
#define Y_MOTOR_DIR_PIN                      GPIO_PIN_15
#define Y_MOTOR_DIR_GPIO_PORT                GPIOB        
#define Y_MOTOR_DIR_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ʹ��
#define Y_MOTOR_EN_PIN                       GPIO_PIN_13
#define Y_MOTOR_EN_GPIO_PORT                 GPIOB                       
#define Y_MOTOR_EN_GPIO_CLK_ENABLE()      	 __HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ����
#define Y_MOTOR_PUL_PORT       			         GPIOC
#define Y_MOTOR_PUL_PIN             		     GPIO_PIN_7
#define Y_MOTOR_PUL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

//��ʱ��ͨ��
#define Y_MOTOR_PUL_CHANNEL                  TIM_CHANNEL_2


//����ʹ������
/* ���κ꣬��������������һ��ʹ�� */
#define MOTOR_PUL(port, pin, x)              HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_DIR(port, pin, x)              HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_OFFLINE(port, pin, x)          HAL_GPIO_WritePin(port, pin, x)
#define MOTOR_START(tim, channel, status)    TIM_CCxChannelCmd(tim, channel, status)
#define MOTOR_STOP(tim, channel, status)     TIM_CCxChannelCmd(tim, channel, status)

/*Ƶ����ز���*/
//��ʱ��ʵ��ʱ��Ƶ��Ϊ��168MHz/TIM_PRESCALER
//���� �߼���ʱ���� Ƶ��Ϊ168MHz,������ʱ��Ϊ84MHz
//168/TIM_PRESCALER=28MHz
//������Ҫ��Ƶ�ʿ����Լ�����
#define TIM_PRESCALER                3
// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFF
#define TIM_PERIOD                   0xFFFF

/************************************************************/
#define HIGH GPIO_PIN_SET	      //�ߵ�ƽ
#define LOW  GPIO_PIN_RESET	    //�͵�ƽ

#define ON  HIGH	              //��
#define OFF LOW	                //��

#define CW 	LOW		              //˳ʱ��
#define CCW HIGH      	        //��ʱ��


//����ʹ������
#define MOTOR_EN(x)					HAL_GPIO_WritePin(X_MOTOR_EN_GPIO_PORT,X_MOTOR_EN_PIN,x);\
                            HAL_GPIO_WritePin(Y_MOTOR_EN_GPIO_PORT,Y_MOTOR_EN_PIN,x)

extern TIM_HandleTypeDef TIM_StepperHandle;
extern Stepper_TypeDef step_motor[2];

void stepper_Init(void);

#endif /* __STEP_MOTOR_INIT_H */
