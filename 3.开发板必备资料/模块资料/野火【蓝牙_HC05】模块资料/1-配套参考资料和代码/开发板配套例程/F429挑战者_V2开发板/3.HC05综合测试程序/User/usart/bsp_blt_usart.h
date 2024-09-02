#ifndef __USART6_H
#define	__USART6_H


#include "stm32f4xx.h"
#include <stdio.h>


#define             BLT_USART_BAUD_RATE                       38400

#define             BLT_USARTx                                USART6
#define             BLT_USART_CLK                             RCC_APB2Periph_USART6
#define             BLT_USART_GPIO_CLK                       (RCC_AHB1Periph_GPIOC )     

#define             BLT_USART_TX_PORT                         GPIOC   
#define             BLT_USART_TX_PIN                          GPIO_Pin_6
#define 		    BLT_USART_TX_AF                       		GPIO_AF_USART6
#define             BLT_USART_TX_SOURCE                  		GPIO_PinSource6


#define             BLT_USART_RX_PORT                         GPIOC 
#define             BLT_USART_RX_PIN                          GPIO_Pin_7
#define 		    BLT_USART_RX_AF                       		GPIO_AF_USART6
#define             BLT_USART_RX_SOURCE                  		GPIO_PinSource7

#define             BLT_USART_IRQ                             USART6_IRQn
#define             BLT_USART_IRQHandler                     USART6_IRQHandler


/* 蓝牙串口数据缓存最大长度 */
#define     UART_BUFF_SIZE      1024


void BLT_USART_Config(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

void bsp_USART_Process(void);
char *get_rebuff(uint16_t *len);
void clean_rebuff(void);

#endif /* __USART6_H */
