#include "usart.h"

// USART GPIO ����,������������
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_GPIO_APBxClkCmd(USART_GPIO_CLK, ENABLE);
	USART_APBxClkCmd(USART_CLK, ENABLE);
	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);
  	// ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);
	// ���ô��ڵĹ�������
	USART_InitStructure.USART_BaudRate = USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ʼ������
	USART_Init(USARTx, &USART_InitStructure);
	USART_Cmd(USARTx, ENABLE);	    
}

// �ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ����� */
	USART_SendData(USARTx, (uint8_t) ch);	
	/* �ȴ�������� */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);		
	return (ch);
}

// �ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
	/* �ȴ������������� */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USARTx);
}
