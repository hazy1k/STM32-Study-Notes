#ifndef __USART2_H
#define	__USART2_H


#include "stm32f10x.h"
#include <stdio.h>

#define             BLT_USART_BAUD_RATE                       38400

#define             BLT_USARTx                                USART3
#define             BLT_USART_APBxClock_FUN                 RCC_APB1PeriphClockCmd
#define             BLT_USART_CLK                             RCC_APB1Periph_USART3
#define             BLT_USART_GPIO_APBxClock_FUN            RCC_APB2PeriphClockCmd
#define             BLT_USART_GPIO_CLK                       (RCC_APB2Periph_GPIOB )     
#define             BLT_USART_TX_PORT                         GPIOB   
#define             BLT_USART_TX_PIN                          GPIO_Pin_10
#define             BLT_USART_RX_PORT                         GPIOB
#define             BLT_USART_RX_PIN                          GPIO_Pin_11
#define             BLT_USART_IRQ                             USART3_IRQn
#define             BLT_USART_IRQHandler                         USART3_IRQHandler

#define UART_BUFF_SIZE      1024


typedef struct 
{
  volatile    uint16_t datanum;
  uint8_t     uart_buff[UART_BUFF_SIZE];		
  uint8_t     receive_data_flag;
}ReceiveData;


void BLT_USART_Config(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

char *get_rebuff(uint16_t *len);
void clean_rebuff(void);

#endif /* __USART2_H */
