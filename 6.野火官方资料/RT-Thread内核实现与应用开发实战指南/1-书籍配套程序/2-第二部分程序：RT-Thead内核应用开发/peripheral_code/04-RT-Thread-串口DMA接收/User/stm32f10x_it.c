/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "board.h"
#include "rtthread.h"


// �����жϷ�����
void DEBUG_USART_IRQHandler(void)
{
	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_IDLE)!=RESET)
	{		
      Uart_DMA_Rx_Data();       /* �ͷ�һ���ź�������ʾ�����ѽ��� */
      USART_ReceiveData(DEBUG_USARTx); /* �����־λ */
	}	 
}
