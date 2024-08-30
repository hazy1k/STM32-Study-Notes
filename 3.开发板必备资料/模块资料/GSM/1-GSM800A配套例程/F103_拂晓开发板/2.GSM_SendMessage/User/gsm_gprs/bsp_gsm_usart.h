#ifndef __BSP_GSM_USART_H
#define	__BSP_GSM_USART_H

#include "stm32f10x.h"
#include <stdio.h>


// GSM_GPRS使用的串口
#define  GSM_USARTx                   USART3
#define  GSM_USART_CLK                RCC_APB1Periph_USART3
#define  GSM_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  GSM_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  GSM_USART_GPIO_CLK           (RCC_APB2Periph_GPIOD)
    
#define  GSM_TX_GPIO_PORT         GPIOD
#define  GSM_TX_GPIO_PIN          GPIO_Pin_8
#define  GSM_RX_GPIO_PORT       GPIOD
#define  GSM_RX_GPIO_PIN        GPIO_Pin_9

#define  GSM_USART_IRQ                USART3_IRQn
#define  GSM_USART_IRQHandler         USART3_IRQHandler

void GSM_USART_Config(void);
void bsp_GSM_USART_IRQHandler(void);
char *get_rebuff(uint8_t *len);
void clean_rebuff(void);

void GSM_USART_printf(char *Data,...);

#endif /* __BSP_GSM_USART_H */
