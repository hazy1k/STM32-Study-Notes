#ifndef __BSP_GSM_USART_H
#define	__BSP_GSM_USART_H

#include "stm32h7xx.h"
#include <stdio.h>


// GSM_GPRS使用的串口
#define  GSM_USARTx                            USART6
#define  GSM_USART_CLK_ENABLE()              __USART6_CLK_ENABLE()
#define  GSM_USART_BAUDRATE                    115200

// USART GPIO 引脚宏定义
#define  GSM_USART_TX_GPIO_CLK               __HAL_RCC_GPIOC_CLK_ENABLE()  
#define  GSM_TX_GPIO_PORT         		       GPIOC   
#define  GSM_TX_GPIO_PIN          		       GPIO_PIN_6
#define  GSM_TX_AF             			         GPIO_AF7_USART6


#define  GSM_USART_RX_GPIO_CLK               __HAL_RCC_GPIOC_CLK_ENABLE()  
#define  GSM_RX_GPIO_PORT       			       GPIOC
#define  GSM_RX_GPIO_PIN        			       GPIO_PIN_7
#define  GSM_RX_AF             			         GPIO_AF7_USART6



#define  GSM_USART_IRQ                       USART6_IRQn
#define  GSM_USART_IRQHandler                USART6_IRQHandler

void GSM_USART_Config(void);
void bsp_GSM_USART_IRQHandler(void);
char *get_rebuff(uint8_t *len);
void clean_rebuff(void);

void GSM_USART_printf(char *Data,...);

#endif /* __BSP_GSM_USART_H */
