#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32F7xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define SR501_INT_GPIO_PORT                GPIOE
#define SR501_INT_GPIO_CLK_ENABLE()        __GPIOE_CLK_ENABLE();
#define SR501_INT_GPIO_PIN                 GPIO_PIN_2
#define SR501_INT_EXTI_IRQ                 EXTI2_IRQn
#define SR501_IRQHandler                   EXTI2_IRQHandler

/*******************************************************/


void EXTI_SR501_Config(void);

#endif /* __EXTI_H */
