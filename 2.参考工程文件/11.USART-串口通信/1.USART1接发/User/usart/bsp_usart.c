#include "bsp_usart.h"

// ����Ƕ�������жϿ�����NVIC
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; // ����NVIC��ʼ���ṹ�����
  
  // 1.Ƕ�������жϿ�������ѡ��
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  // 2.����USARTΪ�ж�Դ
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;

  // 3.������ռ���ȼ��������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

  // 4.ʹ���ж�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  // 5.��ʼ������NVIC
  NVIC_Init(&NVIC_InitStructure);
}

// USART GPIO ����,������������
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; // ����GPIO��ʼ���ṹ�����
	USART_InitTypeDef USART_InitStructure; // ����USART��ʼ���ṹ�����

	// 1.�򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 2.�򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 3.��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // ʹ��GPIO_Pin_9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // �������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // �ٶ�Ϊ50MHz

	// 4.��ʼ��GPIO�ṹ��
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); 

    // 5.��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // ʹ��GPIO_Pin_10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // ��������ģʽ

	// 6.��ʼ��GPIO�ṹ��
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// 7.���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // ������Ϊ115200

	// 8.���������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8λ����λ

	// 9.����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1λֹͣλ

	// 10.����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No; // ��У��λ

	// 11.����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ��������

	// 12.���ù���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // �շ�һ����

	// 13.��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// �����ж����ȼ�����
	NVIC_Configuration();
	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	// ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

// ����һ���ֽں���
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch) // �������������ںš������͵��ֽ�
{
	// ����һ���ֽ����ݵ�USART
	USART_SendData(pUSARTx, ch);
		
	// �ȴ��������
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

// ����һ��8λ�����麯��
void Usart_SendArray(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t num) // �������������ںš������͵����顢���鳤��
{
  uint8_t i;
	
	for(i = 0; i < num; i++)
  	{
	    // ����һ���ֽ����ݵ�USART����ͨ��ѭ��д�뵽������
	    Usart_SendByte(pUSARTx, array[i]);	
  	}

	// �ȴ��������
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

// ����һ���ַ�������
void Usart_SendString( USART_TypeDef *pUSARTx, char *str) // �������������ںš������͵��ַ���
{
  unsigned int k=0;
  do 
  {
    Usart_SendByte( pUSARTx, *(str + k) ); // ����һ���ֽ����ݵ�USART
    k++;
  } while(*(str + k)!='\0'); // ֱ���ַ�������
  
  // �ȴ��������
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET)
  {

  }
}

// ����һ��16λ������
void Usart_SendHalfWord( USART_TypeDef *pUSARTx, uint16_t ch) // �������������ںš������͵�16λ��
{
	uint8_t temp_h, temp_l;
	
	// 1.ȡ�߰�λ
	temp_h = (ch&0XFF00) >> 8;
	// 2.ȡ���Ͱ�λ
	temp_l = ch&0XFF;
	
	// 3.���͸߰�λ
	USART_SendData(pUSARTx, temp_h);	
	// 4.�ȴ��������
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	// 5.���͵Ͱ�λ
	USART_SendData(pUSARTx,temp_l);	
	// 6.�ȴ��������
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

// �ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f) // ���������������͵��ַ����ļ�ָ��
{
		// ����һ���ֽ����ݵ�����
		USART_SendData(DEBUG_USARTx, (uint8_t)ch);
		
		// �ȴ��������
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		// ���ط��͵��ַ�
		return (ch);
}

// �ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f) // �����������ļ�ָ��
{
		// �ȴ�������������
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);
		
		// ���ؽ��յ�������
		return (int)USART_ReceiveData(DEBUG_USARTx);
}

