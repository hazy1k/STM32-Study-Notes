#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define SR501_INT_GPIO_PORT                GPIOI
#define SR501_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOI
#define SR501_INT_GPIO_PIN                 GPIO_Pin_3
#define SR501_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOI
#define SR501_INT_EXTI_PINSOURCE           EXTI_PinSource3
#define SR501_INT_EXTI_LINE                EXTI_Line3
#define SR501_INT_EXTI_IRQ                 EXTI3_IRQn

#define SR501_IRQHandler                   EXTI3_IRQHandler


/*******************************************************/


void EXTI_SR501_Config(void);

#endif /* __EXTI_H */
