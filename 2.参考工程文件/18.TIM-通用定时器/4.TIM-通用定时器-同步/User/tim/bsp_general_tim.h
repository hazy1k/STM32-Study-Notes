#ifndef __GENERAL_TIM_H
#define	__GENERAL_TIM_H

#include "stm32f10x.h"

// 定时器参数宏定义
#define GENERAL_TIM_MASTER     		  TIM2                // 主定时器--TIM2
#define GENERAL_TIM_MASTER_CLK        RCC_APB1Periph_TIM2 // 主定时器时钟--RCC_APB1Periph_TIM2

#define GENERAL_TIM_SLAVE1     		  TIM3                // 从定时器1--TIM3
#define GENERAL_TIM_SLAVE1_CLK        RCC_APB1Periph_TIM3 // 从定时器1时钟--RCC_APB1Periph_TIM3

#define GENERAL_TIM_SLAVE11     	  TIM4                // 从定时器11--TIM4
#define GENERAL_TIM_SLAVE11_CLK       RCC_APB1Periph_TIM4 // 从定时器11时钟--RCC_APB1Periph_TIM4

// TIM主模式PWM输出引脚
#define GENERAL_MASTER_PIN            GPIO_Pin_10          // 主定时器PWM输出引脚             
#define GENERAL_MASTER_GPIO_PORT      GPIOB                // 主定时器PWM输出端口             
#define GENERAL_MASTER_GPIO_CLK       RCC_APB2Periph_GPIOB // 主定时器PWM输出端口时钟
#define GENERAL_MASTER_PINSOURCE	  GPIO_PinSource5      // 主定时器PWM输出引脚源
#define GENERAL_MASTER_AF			  GPIO_AF_TIM2         // 主定时器PWM输出引脚复用功能

// TIM从模式PWM输出引脚
#define GENERAL_SLAVE1_PIN            GPIO_Pin_6             
#define GENERAL_SLAVE1_GPIO_PORT      GPIOC                      
#define GENERAL_SLAVE1_GPIO_CLK       RCC_APB2Periph_GPIOC
#define GENERAL_SLAVE1_PINSOURCE	  GPIO_PinSource6
#define GENERAL_SLAVE1_AF			  GPIO_AF_TIM3

// TIM从模式的从模式PWM输出引脚 
#define GENERAL_SLAVE11_PIN           GPIO_Pin_6             
#define GENERAL_SLAVE11_GPIO_PORT     GPIOB                      
#define GENERAL_SLAVE11_GPIO_CLK      RCC_APB2Periph_GPIOB
#define GENERAL_SLAVE11_PINSOURCE	  GPIO_PinSource6
#define GENERAL_SLAVE11_AF			  GPIO_AF_TIM4

void TIMx_Configuration(void);

#endif 
