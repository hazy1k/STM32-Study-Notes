#ifndef __USART1_H
#define	__USART1_H


#include "stm32f10x.h"
#include <stdio.h>



/**************************USART参数定义********************************/
#define             macUSART_BAUD_RATE                       115200

#define             macUSARTx                                USART1
#define             macUSART_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define             macUSART_CLK                             RCC_APB2Periph_USART1
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        RCC_APB2Periph_GPIOB    
#define             macUSART_TX_PORT                         GPIOB
#define             macUSART_TX_PIN                          GPIO_Pin_6
#define             macUSART_RX_PORT                         GPIOB
#define             macUSART_RX_PIN                          GPIO_Pin_7
#define             macUSART_IRQ                             USART1_IRQn
#define             macUSART_INT_FUN                         USART1_IRQHandler



void                USARTx_Config                           ( void );



#endif /* __USART1_H */
