#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f1xx.h"
#include <stdio.h>

//���ڲ�����
#define DEBUG_USART_BAUDRATE                    115200

//���Ŷ���
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __HAL_RCC_USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOB
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_7

#define DEBUG_USART_TX_GPIO_PORT                GPIOB
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_6

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		    USART1_IRQn

//������1���õ�PB6\PB7��
#define DEBUG_USART_AF_ENABLE()					      	__HAL_AFIO_REMAP_USART1_ENABLE()
/************************************************************/

void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
extern UART_HandleTypeDef UartHandle;
#endif /* __USART1_H */
