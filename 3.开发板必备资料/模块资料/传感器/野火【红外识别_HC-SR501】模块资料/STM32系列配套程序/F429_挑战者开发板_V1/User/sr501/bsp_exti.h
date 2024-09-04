#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define SR501_INT_GPIO_PORT                GPIOA
#define SR501_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define SR501_INT_GPIO_PIN                 GPIO_Pin_4
#define SR501_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOA
#define SR501_INT_EXTI_PINSOURCE           EXTI_PinSource4
#define SR501_INT_EXTI_LINE                EXTI_Line4
#define SR501_INT_EXTI_IRQ                 EXTI4_IRQn

#define SR501_IRQHandler                   EXTI4_IRQHandler


/*******************************************************/


void EXTI_SR501_Config(void);

#endif /* __EXTI_H */
