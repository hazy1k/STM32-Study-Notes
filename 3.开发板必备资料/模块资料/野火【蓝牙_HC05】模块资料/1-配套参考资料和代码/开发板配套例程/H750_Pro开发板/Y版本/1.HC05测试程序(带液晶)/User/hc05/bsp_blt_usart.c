/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   HC05串口驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 挑战者 STM32H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "./hc05/bsp_blt_usart.h"
#include <stdarg.h>
#include "./led/bsp_led.h"

UART_HandleTypeDef BLT_UartHandle;
//中断缓存串口数据
#define UART_BUFF_SIZE      1024
volatile    uint16_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];
/*
 * 函数名：USARTx_Config
 * 描述  ：USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void BLT_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	
	
	BLT_UARTx_RX_GPIO_CLK_ENABLE();
	BLT_UARTx_TX_GPIO_CLK_ENABLE();
	
	/* 配置串口6时钟源*/
	RCC_PeriphClkInit.PeriphClockSelection = BLT_RCC_PERIPHCLK_UARTx;
	RCC_PeriphClkInit.Usart16ClockSelection = BLT_RCC_UARTxCLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
	
		/* 使能 UART 时钟 */
	BLT_UARTx_CLK_ENABLE();  
	
	/* 配置Tx引脚为复用功能  */
	GPIO_InitStruct.Pin = BLT_UARTx_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = BLT_UARTx_TX_AF;
	HAL_GPIO_Init(BLT_UARTx_TX_GPIO_PORT, &GPIO_InitStruct);
	
	/* 配置Rx引脚为复用功能 */
	GPIO_InitStruct.Pin = BLT_UARTx_RX_PIN;
	GPIO_InitStruct.Alternate = BLT_UARTx_RX_AF;
	HAL_GPIO_Init(BLT_UARTx_RX_GPIO_PORT, &GPIO_InitStruct); 
	  
	/* 配置串USARTx 模式 */
	BLT_UartHandle.Instance = BLT_USARTx;
	BLT_UartHandle.Init.BaudRate = BLT_USART_BAUD_RATE;
	BLT_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	BLT_UartHandle.Init.StopBits = UART_STOPBITS_1;
	BLT_UartHandle.Init.Parity = UART_PARITY_NONE;
	BLT_UartHandle.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&BLT_UartHandle);

	/*串口6中断初始化 */
	HAL_NVIC_SetPriority(BLT_UARTx_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(BLT_UARTx_IRQ);
	/*配置串口接收中断 */
	__HAL_UART_ENABLE_IT(&BLT_UartHandle,USART_IT_RXNE);  
}

/*****************  发送字符串 **********************/
void BLT_Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit( &BLT_UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}	
	
 

void bsp_USART_Process(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(__HAL_UART_GET_IT(&BLT_UartHandle,UART_IT_RXNE) != RESET)
					{		
						HAL_UART_Receive(&BLT_UartHandle, (uint8_t *)&uart_buff[uart_p], 1, 1000);  
            uart_p++;						
					}
    }
		else
		{
			clean_rebuff();     
		}
	HAL_UART_IRQHandler(&BLT_UartHandle);			
}


//获取接收到的数据和长度
char *get_rebuff(uint16_t *len) 
{
    *len = uart_p;
    return (char *)&uart_buff;
}

//清空缓冲区
void clean_rebuff(void)
{

    uint16_t i=UART_BUFF_SIZE+1;
    uart_p = 0;
	while(i)
		uart_buff[--i]=0;

}



/******************* (C) COPYRIGHT  WildFire Team *****END OF FILE************/


