#include "stm32f10x.h"
#include "bsp_usart.h"

int main(void)
{	
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  USART_Config();
	
	/* ����һ���ַ��� */
	Usart_SendString(DEBUG_USARTx, "����һ�������жϽ��ջ���ʵ��\n");
	printf("��ӭʹ��Ұ��STM32������\n\n\n\n");
	
  // ����������д�õĺ���

  // Usart_SendByte() ����������USARTx ���ںţ����͵��ֽ�
  // Usart_SendArray() ����������USARTx ���ںţ����͵����飬���鳤��
  // Usart_SendString() ����������USARTx ���ںţ����͵��ַ���
  // Usart_SendHalfWord() ����������USARTx ���ںţ����͵İ���

  while(1)
	{	
		
	}	
}