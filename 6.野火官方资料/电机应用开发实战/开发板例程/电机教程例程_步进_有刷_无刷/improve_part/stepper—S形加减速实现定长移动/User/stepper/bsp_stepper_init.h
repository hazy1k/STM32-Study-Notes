#ifndef __BSP_STEPPER_INIT_H
#define	__BSP_STEPPER_INIT_H

#include "stm32f1xx_hal.h"

/*�궨��*/
/*******************************************************/
//�궨���Ӧ������Ľӿ� 1 ��2
#define CHANNEL_SW 1

#if(CHANNEL_SW == 1)
//Motor ���� 
//Motor ���� 
#define MOTOR_DIR_PIN                  	GPIO_PIN_14   
#define MOTOR_DIR_GPIO_PORT            	GPIOB                    
#define MOTOR_DIR_GPIO_CLK_ENABLE()   	__HAL_RCC_GPIOB_CLK_ENABLE()

//Motor ʹ�� 
#define MOTOR_EN_PIN                  	GPIO_PIN_12
#define MOTOR_EN_GPIO_PORT            	GPIOB                       
#define MOTOR_EN_GPIO_CLK_ENABLE()    	__HAL_RCC_GPIOB_CLK_ENABLE()
	
//Motor ����
#define MOTOR_PUL_IRQn                  TIM8_CC_IRQn
#define MOTOR_PUL_IRQHandler            TIM8_CC_IRQHandler

#define MOTOR_PUL_TIM                   TIM8
#define MOTOR_PUL_CLK_ENABLE()  		    __HAL_RCC_TIM8_CLK_ENABLE()

#define MOTOR_PUL_PORT       			      GPIOC
#define MOTOR_PUL_PIN             		  GPIO_PIN_6
#define MOTOR_PUL_GPIO_CLK_ENABLE()		  __HAL_RCC_GPIOC_CLK_ENABLE()
#define MOTOR_PUL_CHANNEL_x             TIM_CHANNEL_1

#define MOTOR_TIM_IT_CCx                TIM_IT_CC1
#define MOTOR_TIM_FLAG_CCx              TIM_FLAG_CC1
#elif(CHANNEL_SW == 2)

//Motor ���� 
#define MOTOR_DIR_PIN                  	GPIO_PIN_15
#define MOTOR_DIR_GPIO_PORT            	GPIOD          
#define MOTOR_DIR_GPIO_CLK_ENABLE()   	__HAL_RCC_GPIOD_CLK_ENABLE()

//Motor ʹ�� 
#define MOTOR_EN_PIN                  	GPIO_PIN_13
#define MOTOR_EN_GPIO_PORT            	GPIOC            
#define MOTOR_EN_GPIO_CLK_ENABLE()    	__HAL_RCC_GPIOC_CLK_ENABLE()
	
//Motor ����
#define MOTOR_PUL_IRQn                  TIM8_CC_IRQn
#define MOTOR_PUL_IRQHandler            TIM8_CC_IRQHandler

#define MOTOR_PUL_TIM                   TIM8
#define MOTOR_PUL_CLK_ENABLE()  		    __HAL_RCC_TIM8_CLK_ENABLE()

#define MOTOR_PUL_PORT       			      GPIOC
#define MOTOR_PUL_PIN             	  	GPIO_PIN_7
#define MOTOR_PUL_GPIO_CLK_ENABLE()		  __HAL_RCC_GPIOC_CLK_ENABLE()
#define MOTOR_PUL_CHANNEL_x             TIM_CHANNEL_2

#define MOTOR_TIM_IT_CCx                TIM_IT_CC2
#define MOTOR_TIM_FLAG_CCx              TIM_FLAG_CC2

#endif

//��ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/(TIM_PRESCALER+1)
//���� �߼���ʱ���� Ƶ��Ϊ72MHz,������ʱ��Ϊ36MHz
//72/(2)=36Mhz
//������Ҫ��Ƶ�ʿ����Լ�����
#define TIM_PRESCALER               (2 - 1)
// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFF
#define TIM_PERIOD                   0xFFFF

/****************************************************************/

#define HIGH GPIO_PIN_SET	      //�ߵ�ƽ
#define LOW  GPIO_PIN_RESET	    //�͵�ƽ

#define ON  HIGH	              //��
#define OFF LOW	                //��

#define CW 	LOW		              //˳ʱ��
#define CCW HIGH      	        //��ʱ��

//����ʹ������
/* ���κ꣬��������������һ��ʹ�� */
#define MOTOR_EN(x)					HAL_GPIO_WritePin(MOTOR_EN_GPIO_PORT,MOTOR_EN_PIN,x)
#define MOTOR_PUL(x)				HAL_GPIO_WritePin(MOTOR_PUL_GPIO_PORT,MOTOR_PUL_PIN,x)
#define MOTOR_DIR(x)				HAL_GPIO_WritePin(MOTOR_DIR_GPIO_PORT,MOTOR_DIR_PIN,x)

#define MOTOR_EN_TOGGLE     HAL_GPIO_TogglePin(MOTOR_EN_GPIO_PORT,MOTOR_EN_PIN)
#define MOTOR_PUL_TOGGLE    HAL_GPIO_TogglePin(MOTOR_PUL_PORT,MOTOR_PUL_PIN)


extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void stepper_Init(void);

#endif
