#ifndef __EXTI_H
#define	__EXTI_H


#include "stm32f10x.h"


//Òý½Å¶¨Òå
#define SR501_INT_GPIO_PORT         GPIOB
#define SR501_INT_GPIO_CLK          (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
#define SR501_INT_GPIO_PIN          GPIO_Pin_0
#define SR501_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOB
#define SR501_INT_EXTI_PINSOURCE    GPIO_PinSource0
#define SR501_INT_EXTI_LINE         EXTI_Line0
#define SR501_INT_EXTI_IRQ          EXTI0_IRQn

#define SR501_IRQHandler            EXTI0_IRQHandler


void EXTI_SR501_Config(void);


#endif /* __EXTI_H */
