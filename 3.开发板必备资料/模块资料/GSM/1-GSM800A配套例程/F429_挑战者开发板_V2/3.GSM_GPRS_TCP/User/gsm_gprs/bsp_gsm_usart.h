#ifndef __BSP_GSM_USART_H
#define	__BSP_GSM_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


// GSM_GPRS使用的串口
#define  GSM_USARTx                   USART6
#define  GSM_USART_CLK                RCC_APB2Periph_USART6
#define  GSM_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  GSM_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  GSM_USART_GPIO_CLK           (RCC_AHB1Periph_GPIOC)
    
#define  GSM_TX_GPIO_PORT         		GPIOC   
#define  GSM_TX_GPIO_PIN          		GPIO_Pin_6
#define  GSM_TX_AF             			  GPIO_AF_USART6
#define  GSM_TX_SOURCE         			  GPIO_PinSource6

#define  GSM_RX_GPIO_PORT       			GPIOC
#define  GSM_RX_GPIO_PIN        			GPIO_Pin_7
#define  GSM_RX_AF             			  GPIO_AF_USART6
#define  GSM_RX_SOURCE         			  GPIO_PinSource7


#define  GSM_USART_IRQ                USART6_IRQn
#define  GSM_USART_IRQHandler         USART6_IRQHandler

void GSM_USART_Config(void);
void bsp_GSM_USART_IRQHandler(void);
char *get_rebuff(uint8_t *len);
void clean_rebuff(void);

void GSM_USART_printf(char *Data,...);

#endif /* __BSP_GSM_USART_H */
