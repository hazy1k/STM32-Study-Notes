#include "usart.h"

static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	USARTx_GPIO_APBxClkCmd(USARTx_GPIO_CLK, ENABLE);
	USARTx_APBxCLKCmd(USARTx_CLK, ENABLE);
	// TX��������
	GPIO_InitStructure.GPIO_Pin = USARTx_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);
    // RX��������
	GPIO_InitStructure.GPIO_Pin = USARTx_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ���븡��
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);
	// USART ����
	USART_InitStructure.USART_BaudRate = USARTx_BaudRate; // ������115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // �ֳ�8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; // ��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // �շ�ģʽ
	USART_Init(USARTx, &USART_InitStructure);
	// NVIC����
	NVIC_Configuration();
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE); // ʹ�ܴ��ڽ����ж�
	USART_Cmd(USARTx, ENABLE); // ʹ�ܴ���
}

// ---------��д�ӷ����ݺ���----------

// ����һ���ֽں���
// ����������USARTx�����ںţ�ch�����͵��ֽ�
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
	USART_SendData(pUSARTx, ch);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET); // �ȴ��������
}

// ����һ��8λ���麯��
// ����������USARTx�����ںţ�data�����͵�8λ����
void USART_SendArray(USART_TypeDef *pUSARTx, uint8_t *pArray, uint16_t len)
{
	uint8_t i;
	for(i = 0; i < len; i++)
	{
		USART_SendByte(pUSARTx, pArray[i]); // ѭ��д������
	}
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET); // �ȴ��������
}

// ����һ���ַ�������
// ����������USARTx�����ںţ�pStr�����͵��ַ���
void USART_SendString(USART_TypeDef *pUSARTx, uint8_t *pStr)
{
	unsigned int k = 0;
	do{
		USART_SendByte(pUSARTx, *(pStr+k)); // ѭ��д���ַ���
		k++;
	}while(*(pStr+k)!= '\0'); // ֱ��������������\0��
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

// ����һ��16λ���ݺ���
// ����������USARTx�����ںţ�ch�����͵�16λ����
void USART_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l; // 16λ����Ҫ��Ϊ��λ�͵�λ�����η���
	temp_h = (ch & 0xff00) >> 8; // ȡ��λ
	temp_l = ch & 0x00ff; // ȡ��λ
	USART_SendByte(pUSARTx, temp_h); // ���͸�λ
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	USART_SendByte(pUSARTx, temp_l); // ���͵�λ
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

// �ض���printf��scanf����������
int USART_PutChar(int ch, FILE *f) // ����������ch��Ҫ���͵��ַ���f���ļ�ָ��
{
	USART_SendByte(USARTx, (uint8_t)ch); // �����ַ�
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	return ch; // ���ط��͵��ַ�
}

int USART_GetChar(FILE *f) // ����������f���ļ�ָ��
{
	while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USARTx); // �����ַ�
}
