/**
  ******************************************************************************
  * @file    bsp_gsm_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   GSM模块的串口驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./gsm_gprs/bsp_gsm_usart.h"
#include <stdarg.h>

 UART_HandleTypeDef GSM_UartHandle;
/*
 * 函数名：GSM_USART_Config
 * 描述  ：GSM_USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void GSM_USART_Config(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	
	 /* 配置串口6时钟源*/
		RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART6;
		RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
		/* 使能串口6时钟 */
		GSM_USART_CLK_ENABLE();
	
	/* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin   = GSM_TX_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GSM_TX_AF;
    HAL_GPIO_Init(GSM_TX_GPIO_PORT,&GPIO_InitStruct);
    
    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = GSM_RX_GPIO_PIN;
    GPIO_InitStruct.Alternate = GSM_RX_AF;
    HAL_GPIO_Init(GSM_RX_GPIO_PORT,&GPIO_InitStruct); 
    
    /* 配置串GPS_USART 模式 */
    GSM_UartHandle.Instance = GSM_USARTx;
    GSM_UartHandle.Init.BaudRate = GSM_USART_BAUDRATE;
    GSM_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    GSM_UartHandle.Init.StopBits = UART_STOPBITS_1;
    GSM_UartHandle.Init.Parity = UART_PARITY_NONE;
    GSM_UartHandle.Init.Mode = UART_MODE_TX_RX;
	  
	  HAL_UART_Init(&GSM_UartHandle);
	  	
				/*配置中断优先级*/
		HAL_NVIC_SetPriority(GSM_USART_IRQ, 1, 0);
		 /*使能DMA中断*/
		HAL_NVIC_EnableIRQ(GSM_USART_IRQ);
		 /*配置中断条件*/
		__HAL_USART_ENABLE_IT(&GSM_UartHandle,UART_IT_RXNE);
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到GSM_USARTx
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
//int fputc(int ch, FILE *f)
//{
///* 将Printf内容发往串口 */
//  USART_SendData(GSM_USARTx, (unsigned char) ch);
//  while (!(GSM_USARTx->SR & USART_FLAG_TXE));
// 
//  return (ch);
//}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被GSM_USARTx_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


#if 1
//中断缓存串口数据
#define UART_BUFF_SIZE      255
volatile    uint8_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];

void GSM_USART_IRQHandler(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(__HAL_UART_GET_IT(&GSM_UartHandle, USART_IT_RXNE) != RESET)
        {
            uart_buff[uart_p] = GSM_UartHandle.Instance->RDR;
            uart_p++;
        }
    }
		HAL_UART_IRQHandler(&GSM_UartHandle);	
}



//获取接收到的数据和长度
char *get_rebuff(uint8_t *len)
{
    *len = uart_p;
    return (char *)&uart_buff;
}

void clean_rebuff(void)
{
	uint16_t i=UART_BUFF_SIZE+1;
    uart_p = 0;
	while(i)
		uart_buff[--i]=0;
}

#endif

/*
 * 函数名：GSM_USARTx_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即GSM_USARTx
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用GSM_USARTx_printf( GSM_USARTx, "\r\n this is a demo \r\n" );
 *            		 GSM_USARTx_printf( GSM_USARTx, "\r\n %d \r\n", i );
 *            		 GSM_USARTx_printf( GSM_USARTx, "\r\n %s \r\n", j );
 */
uint8_t buffer[2] = {0x0d,0x0a};
void GSM_USART_printf(char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
						HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)&buffer[0], 1, 0xFFFF);
					Data ++;
					break;

				case 'n':							          //换行符
					  HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)&buffer[1], 1, 0xFFFF);
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						 HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)s, 1, 0xFFFF);
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						 HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)s, 1, 0xFFFF);
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else
		{
			HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)Data, 1, 0xFFFF); 
			Data++;
		}
		while ( __HAL_USART_GET_FLAG (&GSM_UartHandle, USART_FLAG_TXE ) == RESET );
	}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
