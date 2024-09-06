#ifndef __MPU_EXTI_H
#define	__MPU_EXTI_H

#include "stm32h7xx.h"


#define MPU_INT_GPIO_PORT                GPIOD
#define MPU_INT_GPIO_CLK_ENABLE()        __GPIOD_CLK_ENABLE()
#define MPU_INT_GPIO_PIN                 GPIO_PIN_2
#define MPU_INT_EXTI_IRQ                 EXTI2_IRQn

#define MPU_IRQHandler                   EXTI2_IRQHandler

void EXTI_MPU_Config(void);

#endif /* __EXTI_H */
