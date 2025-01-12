#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

// USARTx ���ź궨��
#define USARTx_GPIO_CLK (RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)
#define USARTx_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
// TX-���������������
#define USARTx_TX_GPIO_PORT GPIOC
#define USARTx_TX_GPIO_PIN GPIO_Pin_12
// RX-����������������
#define USARTx_RX_GPIO_PORT GPIOD
#define USARTx_RX_GPIO_PIN GPIO_Pin_2

// USARTx ��ز����궨��
#define USARTx  UART5
#define USARTx_CLK RCC_APB1Periph_UART5
#define USARTx_APBxCLKCmd RCC_APB1PeriphClockCmd
#define USARTx_BaudRate 115200

// NVIC����
#define USARTx_IRQ  UART5_IRQn
#define USARTx_IRQHandler UART5_IRQHandler

// һЩ��������
void USART_Config(void);
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len);
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr);
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);

#endif
