#include "bsp_usart_dma.h"

uint8_t ReceiveBuff[RECEIVEBUFF_SIZE]; // ���ջ�����

// �������ú���
void USART_Config(void)
{
	// �ṹ�嶨��
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

	// 1.�򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 2.�򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
 	
	// 3.����NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ������ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannel = DEBUG_USART_IRQ; // �����ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // ʹ���ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3; // ��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct); // ��ʼ��NVIC

	// 4.��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // ����Tx����-PA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // ���ø����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // ����GPIO�ٶ�Ϊ50MHz
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); // ��ʼ��GPIO

    // 5.��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // ����Rx����-PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // ���ø�������
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure); // ��ʼ��GPIO
	
	// ���ô��ڵĹ�������
	// 1.���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // ���ò�����115200
	// 2.���������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // ���������ֳ�8λ
	// 3.����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // ����ֹͣλ1
	// 4.����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No; // ������У��λ
	// 5.����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ������Ӳ��������
	// 6.���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // �����շ�һ����ģʽ
	// 7.��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure); // ��ʼ������
	// 8.ʹ�ܿ����ж�
	USART_ITConfig(DEBUG_USARTx,USART_IT_IDLE,ENABLE);
	// 9.ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}


/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ����8λ������ ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}

// USARTx TX DMA ���ã��ڴ浽����(USART1->DR)
void USARTx_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	    
		// ����DMAԴ��ַ���������ݼĴ�����ַ*/
        DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// �ڴ��ַ(Ҫ����ı�����ָ��)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff;
		// ���򣺴����赽�ڴ�	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// �����С	
		DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE;
		// �����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMAģʽ��һ�λ���ѭ��ģʽ
//		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // ѭ��ģʽ
		// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// ��ֹ�ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// ����DMAͨ��		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
		// ʹ��DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}