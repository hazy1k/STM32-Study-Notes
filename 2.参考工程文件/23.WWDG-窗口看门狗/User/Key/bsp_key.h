#ifndef __BSP_KEY_H
#define	__BSP_KEY_H


#include "stm32f10x.h"

// 使用KEY1，如果要使用KEY2的话，把宏KEY1_PA0注释掉即可
#define   KEYI_PA0

#ifdef    KEYI_PA0
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOA
#define               macKEY1_GPIO_PORT    	                GPIOA			   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_0

#else    //KEY2_PC13
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOC
#define               macKEY1_GPIO_PORT    	                GPIOC		   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_13

#endif

#define KEY_ON	1
#define KEY_OFF	0

void SOFT_Delay(__IO u32 nCount);
void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);


#endif /* __BSP_KEY_H */

