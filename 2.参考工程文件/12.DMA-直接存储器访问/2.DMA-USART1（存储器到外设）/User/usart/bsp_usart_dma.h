#ifndef __USARTDMA_H
#define	__USARTDMA_H


#include "stm32f10x.h"
#include <stdio.h>


// ���ڹ��������궨��
#define  DEBUG_USARTx                   USART1                 // ����ѡ��1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1  // ����ʱ��
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd // ����ʱ��ʹ�ܺ���
#define  DEBUG_USART_BAUDRATE           115200                 // ���ڲ�����

// USART GPIO ���ź궨��
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA) // ����GPIOʱ��
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd // ����GPIOʱ��ʹ�ܺ���
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA                  // ����TX GPIO�˿�
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9             // ����TX GPIO����
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA                  // ����RX GPIO�˿�
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10            // ����RX GPIO����

// ���ڶ�Ӧ��DMA����ͨ��
#define  USART_TX_DMA_CHANNEL     DMA1_Channel4                // ʹ��DMA1ͨ��14
// ����Ĵ�����ַ
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)            // USART���ݼĴ�����ַ
// һ�η��͵�������
#define  SENDBUFF_SIZE            5000                        // һ�η��͵�������


// ��������
void USART_Config(void);
void USARTx_DMA_Config(void);

#endif