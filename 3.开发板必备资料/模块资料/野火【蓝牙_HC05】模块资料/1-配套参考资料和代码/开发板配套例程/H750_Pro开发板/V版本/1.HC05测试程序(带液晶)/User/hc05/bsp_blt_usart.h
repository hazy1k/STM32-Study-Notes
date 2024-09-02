#ifndef  _BSP_BLT_USART_
#define  _BSP_BLT_USART_

#include "stm32h7xx.h"
#include <stdio.h>

//Òý½Å¶¨Òå
/*******************************************************/
#define BLT_USART_BAUD_RATE                       38400


#define BLT_USARTx                            USART6
#define BLT_UARTx_CLK_ENABLE()                __USART6_CLK_ENABLE();

#define BLT_RCC_PERIPHCLK_UARTx               RCC_PERIPHCLK_USART16
#define BLT_RCC_UARTxCLKSOURCE_SYSCLK         RCC_USART16CLKSOURCE_D2PCLK2

#define BLT_UARTx_RX_GPIO_PORT                GPIOC
#define BLT_UARTx_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BLT_UARTx_RX_PIN                         GPIO_PIN_7
#define BLT_UARTx_RX_AF                          GPIO_AF7_USART6


#define BLT_UARTx_TX_GPIO_PORT                GPIOC
#define BLT_UARTx_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BLT_UARTx_TX_PIN                      GPIO_PIN_6
#define BLT_UARTx_TX_AF                       GPIO_AF7_USART6

#define BLT_USART_IRQHandler                  USART6_IRQHandler
#define BLT_UARTx_IRQ                 		    USART6_IRQn

/*******************************************************/


void BLT_USART_Config(void);
void BLT_Usart_SendString(uint8_t *str);

void bsp_USART_Process(void);
char *get_rebuff(uint16_t *len);
void clean_rebuff(void);






#endif

