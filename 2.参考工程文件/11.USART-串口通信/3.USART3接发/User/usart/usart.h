#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

// USARTx ���ź궨��
#define USARTx_GPIO_CLK RCC_APB2Periph_GPIOB
#define USARTx_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
// TX-���������������
#define USARTx_TX_GPIO_PORT GPIOB
#define USARTx_TX_GPIO_PIN GPIO_Pin_10
// RX-����������������
#define USARTx_RX_GPIO_PORT GPIOB
#define USARTx_RX_GPIO_PIN GPIO_Pin_11

// USARTx ��ز����궨��
#define USARTx  USART3
#define USARTx_CLK RCC_APB1Periph_USART3
#define USARTx_APBxCLKCmd RCC_APB1PeriphClockCmd
#define USARTx_BaudRate 115200

// NVIC����
#define USARTx_IRQ  USART3_IRQn
#define USARTx_IRQHandler USART3_IRQHandler

// һЩ��������
void USART_Config(void);
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len);
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr);
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);

#endif
