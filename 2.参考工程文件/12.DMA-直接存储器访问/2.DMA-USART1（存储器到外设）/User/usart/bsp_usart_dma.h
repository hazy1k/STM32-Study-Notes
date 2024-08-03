#ifndef __USARTDMA_H
#define	__USARTDMA_H


#include "stm32f10x.h"
#include <stdio.h>


// 串口工作参数宏定义
#define  DEBUG_USARTx                   USART1                 // 串口选择1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1  // 串口时钟
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd // 串口时钟使能函数
#define  DEBUG_USART_BAUDRATE           115200                 // 串口波特率

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA) // 串口GPIO时钟
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd // 串口GPIO时钟使能函数
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA                  // 串口TX GPIO端口
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9             // 串口TX GPIO引脚
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA                  // 串口RX GPIO端口
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10            // 串口RX GPIO引脚

// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel4                // 使用DMA1通道14
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)            // USART数据寄存器地址
// 一次发送的数据量
#define  SENDBUFF_SIZE            5000                        // 一次发送的数据量


// 函数声明
void USART_Config(void);
void USARTx_DMA_Config(void);

#endif