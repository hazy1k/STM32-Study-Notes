#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32h7xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define SR501_INT_GPIO_PORT                GPIOA
#define SR501_INT_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE();
#define SR501_INT_GPIO_PIN                 GPIO_PIN_12
#define SR501_INT_EXTI_IRQ                 EXTI15_10_IRQn
#define SR501_IRQHandler                   EXTI15_10_IRQHandler

/*******************************************************/


void EXTI_SR501_Config(void);

#endif /* __EXTI_H */
