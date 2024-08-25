#ifndef __BSP_KEY_H
#define	__BSP_KEY_H

#include "stm32f10x.h"

//  Òý½Å¶¨Òå
#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    KEY1_GPIO_PORT    GPIOA			   
#define    KEY1_GPIO_PING	 GPIO_Pin_0

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOC
#define    KEY2_GPIO_PORT    GPIOC		   
#define    KEY2_GPIO_PING	 GPIO_Pin_13

#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif 