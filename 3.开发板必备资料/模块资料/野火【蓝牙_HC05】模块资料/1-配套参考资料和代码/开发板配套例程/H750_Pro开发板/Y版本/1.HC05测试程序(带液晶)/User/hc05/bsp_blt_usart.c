/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   HC05��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ��ս�� STM32H750 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "./hc05/bsp_blt_usart.h"
#include <stdarg.h>
#include "./led/bsp_led.h"

UART_HandleTypeDef BLT_UartHandle;
//�жϻ��洮������
#define UART_BUFF_SIZE      1024
volatile    uint16_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];
/*
 * ��������USARTx_Config
 * ����  ��USART GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void BLT_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	
	
	BLT_UARTx_RX_GPIO_CLK_ENABLE();
	BLT_UARTx_TX_GPIO_CLK_ENABLE();
	
	/* ���ô���6ʱ��Դ*/
	RCC_PeriphClkInit.PeriphClockSelection = BLT_RCC_PERIPHCLK_UARTx;
	RCC_PeriphClkInit.Usart16ClockSelection = BLT_RCC_UARTxCLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
	
		/* ʹ�� UART ʱ�� */
	BLT_UARTx_CLK_ENABLE();  
	
	/* ����Tx����Ϊ���ù���  */
	GPIO_InitStruct.Pin = BLT_UARTx_TX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = BLT_UARTx_TX_AF;
	HAL_GPIO_Init(BLT_UARTx_TX_GPIO_PORT, &GPIO_InitStruct);
	
	/* ����Rx����Ϊ���ù��� */
	GPIO_InitStruct.Pin = BLT_UARTx_RX_PIN;
	GPIO_InitStruct.Alternate = BLT_UARTx_RX_AF;
	HAL_GPIO_Init(BLT_UARTx_RX_GPIO_PORT, &GPIO_InitStruct); 
	  
	/* ���ô�USARTx ģʽ */
	BLT_UartHandle.Instance = BLT_USARTx;
	BLT_UartHandle.Init.BaudRate = BLT_USART_BAUD_RATE;
	BLT_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	BLT_UartHandle.Init.StopBits = UART_STOPBITS_1;
	BLT_UartHandle.Init.Parity = UART_PARITY_NONE;
	BLT_UartHandle.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&BLT_UartHandle);

	/*����6�жϳ�ʼ�� */
	HAL_NVIC_SetPriority(BLT_UARTx_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(BLT_UARTx_IRQ);
	/*���ô��ڽ����ж� */
	__HAL_UART_ENABLE_IT(&BLT_UartHandle,USART_IT_RXNE);  
}

/*****************  �����ַ��� **********************/
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


//��ȡ���յ������ݺͳ���
char *get_rebuff(uint16_t *len) 
{
    *len = uart_p;
    return (char *)&uart_buff;
}

//��ջ�����
void clean_rebuff(void)
{

    uint16_t i=UART_BUFF_SIZE+1;
    uart_p = 0;
	while(i)
		uart_buff[--i]=0;

}



/******************* (C) COPYRIGHT  WildFire Team *****END OF FILE************/


