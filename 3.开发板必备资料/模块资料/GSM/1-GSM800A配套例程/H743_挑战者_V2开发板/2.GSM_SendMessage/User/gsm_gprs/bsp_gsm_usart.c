/**
  ******************************************************************************
  * @file    bsp_gsm_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   GSMģ��Ĵ�������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./gsm_gprs/bsp_gsm_usart.h"
#include <stdarg.h>

 UART_HandleTypeDef GSM_UartHandle;
/*
 * ��������GSM_USART_Config
 * ����  ��GSM_USART GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void GSM_USART_Config(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	
	 /* ���ô���6ʱ��Դ*/
		RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART6;
		RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
		/* ʹ�ܴ���6ʱ�� */
		GSM_USART_CLK_ENABLE();
	
	/* ����Tx����Ϊ���ù���  */
    GPIO_InitStruct.Pin   = GSM_TX_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GSM_TX_AF;
    HAL_GPIO_Init(GSM_TX_GPIO_PORT,&GPIO_InitStruct);
    
    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStruct.Pin = GSM_RX_GPIO_PIN;
    GPIO_InitStruct.Alternate = GSM_RX_AF;
    HAL_GPIO_Init(GSM_RX_GPIO_PORT,&GPIO_InitStruct); 
    
    /* ���ô�GPS_USART ģʽ */
    GSM_UartHandle.Instance = GSM_USARTx;
    GSM_UartHandle.Init.BaudRate = GSM_USART_BAUDRATE;
    GSM_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    GSM_UartHandle.Init.StopBits = UART_STOPBITS_1;
    GSM_UartHandle.Init.Parity = UART_PARITY_NONE;
    GSM_UartHandle.Init.Mode = UART_MODE_TX_RX;
	  
	  HAL_UART_Init(&GSM_UartHandle);
	  	
				/*�����ж����ȼ�*/
		HAL_NVIC_SetPriority(GSM_USART_IRQ, 1, 0);
		 /*ʹ��DMA�ж�*/
		HAL_NVIC_EnableIRQ(GSM_USART_IRQ);
		 /*�����ж�����*/
		__HAL_USART_ENABLE_IT(&GSM_UartHandle,UART_IT_RXNE);
}

/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��GSM_USARTx
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
//int fputc(int ch, FILE *f)
//{
///* ��Printf���ݷ������� */
//  USART_SendData(GSM_USARTx, (unsigned char) ch);
//  while (!(GSM_USARTx->SR & USART_FLAG_TXE));
// 
//  return (ch);
//}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����GSM_USARTx_printf()����
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
//�жϻ��洮������
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



//��ȡ���յ������ݺͳ���
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
 * ��������GSM_USARTx_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����GSM_USARTx
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��GSM_USARTx_printf( GSM_USARTx, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
						HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)&buffer[0], 1, 0xFFFF);
					Data ++;
					break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						 HAL_UART_Transmit(&GSM_UartHandle,(uint8_t *)s, 1, 0xFFFF);
          }
					Data++;
          break;

        case 'd':										//ʮ����
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
