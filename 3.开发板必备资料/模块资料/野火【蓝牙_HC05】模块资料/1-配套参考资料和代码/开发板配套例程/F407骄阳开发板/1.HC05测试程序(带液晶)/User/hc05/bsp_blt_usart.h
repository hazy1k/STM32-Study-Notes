#ifndef  _BSP_BLT_USART_
#define  _BSP_BLT_USART_

#include "stm32f4xx.h"
#include <stdio.h>

//Òý½Å¶¨Òå
/*******************************************************/
#define BLT_USART_BAUD_RATE                       38400


#define BLT_USARTx                            UART5
#define BLT_UARTx_CLK_ENABLE()                __UART5_CLK_ENABLE();

//#define BLT_RCC_PERIPHCLK_UARTx               RCC_PERIPHCLK_USART16
//#define BLT_RCC_UARTxCLKSOURCE_SYSCLK         RCC_USART16CLKSOURCE_D2PCLK2

#define BLT_UARTx_RX_GPIO_PORT                GPIOD
#define BLT_UARTx_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOD_CLK_ENABLE()
#define BLT_UARTx_RX_PIN                      GPIO_PIN_2
#define BLT_UARTx_RX_AF                       GPIO_AF8_UART5


#define BLT_UARTx_TX_GPIO_PORT                GPIOC
#define BLT_UARTx_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BLT_UARTx_TX_PIN                      GPIO_PIN_12
#define BLT_UARTx_TX_AF                       GPIO_AF8_UART5

#define BLT_UARTx_IRQHandler                  UART5_IRQHandler
#define BLT_UARTx_IRQ                 		    UART5_IRQn

/*******************************************************/


void BLT_USART_Config(void);
void BLT_Usart_SendString(uint8_t *str);

void bsp_USART_Process(void);
char *get_rebuff(uint16_t *len);
void clean_rebuff(void);






#endif

