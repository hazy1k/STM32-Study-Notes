#ifndef __BSP_MOTOR_TIM_H
#define	__BSP_MOTOR_TIM_H

#include "stm32f1xx.h"
#include "./led/bsp_led.h"   
#include ".\bldcm_control\bsp_bldcm_control.h"
#include <stdio.h>

/* �������תʵ�ֽṹ�� */

#define SPEED_FILTER_NUM      30    // �ٶ��˲�����

typedef struct
{
  int32_t timeout;            // ��ʱ�����¼���
  float speed;                // ����ٶ� rps��ת/���ӣ�
  int32_t enable_flag;        // ���ʹ�ܱ�־
  int32_t speed_group[SPEED_FILTER_NUM];
}motor_rotate_t;

/* ������ƶ�ʱ�� */
#define MOTOR_TIM           				      TIM1
#define MOTOR_TIM_CLK_ENABLE()  			    __HAL_RCC_TIM1_CLK_ENABLE()
extern TIM_HandleTypeDef  htimx_bldcm;

/* �ۼ� TIM_Period�������һ�����»����ж�		
	����ʱ����0������2399����Ϊ2400�Σ�Ϊһ����ʱ���� */
#define PWM_PERIOD_COUNT     (2400)

#define PWM_MAX_PERIOD_COUNT    (PWM_PERIOD_COUNT - 100)

/* �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK = 72MHz 
	 �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(PWM_PRESCALER_COUNT)/PWM_PERIOD_COUNT = 15KHz*/
#define PWM_PRESCALER_COUNT     (2)

#define MOTOR_OCPWM1_AF_ENABLE()          __HAL_AFIO_REMAP_TIM1_ENABLE();

/* TIM1ͨ��1������� */
#define MOTOR_OCPWM1_PIN           		    GPIO_PIN_9
#define MOTOR_OCPWM1_GPIO_PORT     		    GPIOE
#define MOTOR_OCPWM1_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1ͨ��2������� */
#define MOTOR_OCPWM2_PIN           		    GPIO_PIN_11
#define MOTOR_OCPWM2_GPIO_PORT     		    GPIOE
#define MOTOR_OCPWM2_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1ͨ��3������� */
#define MOTOR_OCPWM3_PIN           		    GPIO_PIN_13
#define MOTOR_OCPWM3_GPIO_PORT     		    GPIOE
#define MOTOR_OCPWM3_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1ͨ��1����������� */
#define MOTOR_OCNPWM1_PIN            		  GPIO_PIN_8
#define MOTOR_OCNPWM1_GPIO_PORT      		  GPIOE
#define MOTOR_OCNPWM1_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1ͨ��2����������� */
#define MOTOR_OCNPWM2_PIN            		  GPIO_PIN_10
#define MOTOR_OCNPWM2_GPIO_PORT      		  GPIOE
#define MOTOR_OCNPWM2_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOE_CLK_ENABLE()

/* TIM1ͨ��3����������� */
#define MOTOR_OCNPWM3_PIN            		  GPIO_PIN_12
#define MOTOR_OCNPWM3_GPIO_PORT      		  GPIOE
#define MOTOR_OCNPWM3_GPIO_CLK_ENABLE()	  __HAL_RCC_GPIOE_CLK_ENABLE()

#define TIM_COM_TS_ITRx                   TIM_TS_ITR3    // �ڲ���������(TIM1->ITR3->TIM4)

/* ������������ʱ�� */
#define HALL_TIM           				      TIM4
#define HALL_TIM_CLK_ENABLE()  			    __HAL_RCC_TIM4_CLK_ENABLE()

extern TIM_HandleTypeDef htimx_hall;

/* �ۼ� TIM_Period�������һ�����»����ж�		
	����ʱ����0������65535����Ϊ65535�Σ�Ϊһ���������� */
#define HALL_PERIOD_COUNT     (0xFFFF)

/* ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK = 72MHz
	 �趨��ʱ��Ƶ��Ϊ = TIMxCLK / (PWM_PRESCALER_COUNT) / PWM_PERIOD_COUNT = 9.987Hz
   ���� T = 100ms */
#define HALL_PRESCALER_COUNT     (110)

#define HALL_INPUT_AF_ENABLE()          	__HAL_AFIO_REMAP_TIM4_ENABLE();

/* TIM4 ͨ�� 1 ���� */
#define HALL_INPUTU_PIN           		    GPIO_PIN_12
#define HALL_INPUTU_GPIO_PORT     		    GPIOD
#define HALL_INPUTU_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOD_CLK_ENABLE()

/* TIM4 ͨ�� 2 ���� */
#define HALL_INPUTV_PIN           		    GPIO_PIN_13
#define HALL_INPUTV_GPIO_PORT     		    GPIOD
#define HALL_INPUTV_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOD_CLK_ENABLE()

/* TIM4 ͨ�� 3 ���� */
#define HALL_INPUTW_PIN           		    GPIO_PIN_14
#define HALL_INPUTW_GPIO_PORT     		    GPIOD
#define HALL_INPUTW_GPIO_CLK_ENABLE() 	  __HAL_RCC_GPIOD_CLK_ENABLE()

#define HALL_TIM_IRQn                    TIM4_IRQn
#define HALL_TIM_IRQHandler              TIM4_IRQHandler

extern TIM_HandleTypeDef TIM_TimeBaseStructure;

void PWM_TIMx_Configuration(void);
void stop_pwm_output(void);
void set_pwm_pulse(uint16_t pulse);
float get_motor_speed(void);

void hall_enable(void);
void hall_disable(void);
void hall_tim_config(void);

#endif   /* __BSP_MOTOR_TIM_H */
