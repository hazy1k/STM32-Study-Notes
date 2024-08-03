#ifndef __USARTDMA_H
#define	__USARTDMA_H

#include "stm32f10x.h"
#include <stdio.h>


// 串口工作参数宏定义
#define  DEBUG_USARTx                   USART1                 // 使用USART1作为调试串口 
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1  // USART1时钟
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd // USART1时钟使能函数
#define  DEBUG_USART_BAUDRATE           115200                 // 波特率

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA) // GPIO时钟
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd // GPIO时钟使能函数
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA                  // 串口发送GPIO端口
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9             // 串口发送GPIO引脚
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA                  // 串口接收GPIO端口
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10            // 串口接收GPIO引脚

#define  DEBUG_USART_IRQ                USART1_IRQn            // USART1中断源
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler      // USART1中断处理函数

// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel5                // 使用通道5  
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)            // USART数据寄存器地址
// 一次发送的数据量
#define  RECEIVEBUFF_SIZE            5000


void USART_Config(void);
void USARTx_DMA_Config(void);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
#endif 