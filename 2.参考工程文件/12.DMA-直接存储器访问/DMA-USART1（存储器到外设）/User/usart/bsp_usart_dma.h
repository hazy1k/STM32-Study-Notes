#ifndef __USARTDMA_H
#define	__USARTDMA_H


#include "stm32f10x.h"
#include <stdio.h>


// ���ڹ��������궨��
#define  DEBUG_USARTx                   USART1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  DEBUG_USART_BAUDRATE           115200

// USART GPIO ���ź궨��
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

// ���ڶ�Ӧ��DMA����ͨ��
#define  USART_TX_DMA_CHANNEL     DMA1_Channel4
// ����Ĵ�����ַ
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)
// һ�η��͵�������
#define  SENDBUFF_SIZE            5000


void USART_Config(void);
void USARTx_DMA_Config(void);

#endif /* __USARTDMA_H */
