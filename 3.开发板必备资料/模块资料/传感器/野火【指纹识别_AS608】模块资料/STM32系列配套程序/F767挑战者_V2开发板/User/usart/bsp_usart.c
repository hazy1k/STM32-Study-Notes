/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   使用串口1，重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32F767 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_usart.h"

UART_HandleTypeDef UartHandle;

 /**
  * @brief  USARTx GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */  
void UARTx_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
        
    UARTx_RX_GPIO_CLK_ENABLE();
    UARTx_TX_GPIO_CLK_ENABLE();
    
    /* 配置串口1时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UARTx;
    RCC_PeriphClkInit.Usart1ClockSelection = RCC_UARTxCLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* 使能 UART 时钟 */
    UARTx_CLK_ENABLE();

    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10    ------> USART1_RX 
    */
    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = UARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = UARTx_TX_AF;
    HAL_GPIO_Init(UARTx_TX_GPIO_PORT, &GPIO_InitStruct);
    
    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = UARTx_RX_PIN;
    GPIO_InitStruct.Alternate = UARTx_RX_AF;
    HAL_GPIO_Init(UARTx_RX_GPIO_PORT, &GPIO_InitStruct); 
    
    /* 配置串USARTx 模式 */
    UartHandle.Instance = UARTx;
    UartHandle.Init.BaudRate = UARTx_BAUDRATE;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;

    /*防止奇怪产生溢出错误错误*/
	  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
	  UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

    HAL_UART_Init(&UartHandle);

}
/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit( &UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}
//重定向c库函数printf到串口USARTx，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口USARTx */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
	return (ch);
}

//重定向c库函数scanf到串口USARTx，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{	
    int ch;
    /* 等待串口输入数据 */
    while(__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) == RESET);
    HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
    //	__HAL_UART_CLEAR_OREFLAG(&UartHandle);
    return (ch);
}
/*********************************************END OF FILE**********************/
