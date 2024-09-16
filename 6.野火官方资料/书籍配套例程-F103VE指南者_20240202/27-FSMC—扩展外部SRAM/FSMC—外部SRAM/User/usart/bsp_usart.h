#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f10x.h"
#include <stdio.h>


/** 
  * ���ں궨�壬��ͬ�Ĵ��ڹ��ص����߲�һ������ֲʱ��Ҫ�޸��⼸����
  */
#define  DEBUG_USART                   	USART1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART_APBxClkCmd        RCC_APB2PeriphClockCmd
#define  DEBUG_USART_BAUDRATE          115200

// USART GPIO ���ź궨��
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10




void Debug_USART_Config(void);
//int fputc(int ch, FILE *f);

#endif /* __USART1_H */
