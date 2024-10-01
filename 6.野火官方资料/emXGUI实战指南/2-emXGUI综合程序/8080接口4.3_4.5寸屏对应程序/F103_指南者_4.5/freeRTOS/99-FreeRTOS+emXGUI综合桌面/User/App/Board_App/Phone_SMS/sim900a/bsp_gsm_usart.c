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
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./sim900a/bsp_gsm_usart.h"
#include <stdarg.h>
#include "emXGUI_Arch.h"

/// ����USART�����ж�
static void GSM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = GSM_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������GSM_USART_Config
 * ����  ��GSM_USART GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void GSM_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config GSM_USARTx clock */
	RCC_APB2PeriphClockCmd(GSM_USART_GPIO_CLK, ENABLE);
	GSM_USART_APBxClkCmd(GSM_USART_CLK, ENABLE);

	/* USART GPIO config */
   /* Configure USART Tx  as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GSM_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GSM_TX_GPIO_PORT, &GPIO_InitStructure);
	    
  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GSM_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GSM_RX_GPIO_PORT, &GPIO_InitStructure);
	  
	/* USART mode config */
	USART_InitStructure.USART_BaudRate = GSM_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(GSM_USARTx, &USART_InitStructure); 
	
	/*	�����ж����ȼ� */
	GSM_NVIC_Config();
  
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(GSM_USARTx, USART_IT_RXNE, ENABLE);
  USART_ITConfig(GSM_USARTx, USART_IT_IDLE, ENABLE);    // ʹ�ܿ����ж�
	
	USART_Cmd(GSM_USARTx, ENABLE);
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

/* �����ⲿ���� */
extern GUI_SEM *Call_Sem;
extern uint8_t Sim900aReceiveAcc;    // �������

void GSM_USART_IRQHandler(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(USART_GetITStatus(GSM_USARTx, USART_IT_RXNE) != RESET)
        {
            uart_buff[uart_p] = USART_ReceiveData(GSM_USARTx);
            uart_p++;
        }
    }
    
    if(USART_GetITStatus(GSM_USARTx, USART_IT_IDLE) != RESET)
    {
      USART_ReceiveData(GSM_USART);
//      GUI_SemPostISR(Call_Sem);
      USART_ClearITPendingBit(GSM_USART, USART_IT_IDLE);    // ����жϹ���λ
      USART_ClearFlag(GSM_USART, USART_IT_IDLE);            // ��������ж�
      Sim900aReceiveAcc=1;
    }
}



//��ȡ���յ������ݺͳ���
char *gsm_get_rebuff(uint16_t *len)
{
    *len = uart_p;
    return (char *)&uart_buff;
}

void gsm_clean_rebuff(void)
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
					USART_SendData(GSM_USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
					USART_SendData(GSM_USARTx, 0x0a);	
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
						USART_SendData(GSM_USARTx,*s);
						while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//ʮ����
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(GSM_USARTx,*s);
						while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(GSM_USARTx, *Data++);
		while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
	}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
