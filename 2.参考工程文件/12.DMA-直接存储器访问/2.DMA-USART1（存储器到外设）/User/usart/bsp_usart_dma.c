#include "bsp_usart_dma.h"

uint8_t SendBuff[SENDBUFF_SIZE]; // ���ͻ�����

// USART1��ʼ������
void USART_Config(void)
{
	// ǰ��׼������
	// 1.����GPIO��ʼ���ṹ�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 2.�򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 3.�򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 4.��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // ѡ��GPIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // �ٶ�50MHz 
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); // ��ʼ��GPIO��

    // 5.��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // ѡ��GPIO��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // �ٶ�50MHz 
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure); // ��ʼ��GPIO��
	
	// ���濪ʼ���ô��ڵĹ�������
	// 1.���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 115200
	// 2.���������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8λ����λ
	// 3.����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1λֹͣλ
	// 4.����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ; // ��У��λ
	// 5.����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ��������
	// 6.���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // �շ�һ��
	// 7.��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);	
	// 8.ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

// USART1���ܺ���
/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef *pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx, ch);
		
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
		// 1.DMA��ʼ���ṹ��
		DMA_InitTypeDef DMA_InitStructure;
	
		// 2.����DMAʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		// 3.����DMAԴ��ַ���������ݼĴ�����ַ
    	DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;

		// 4.�����ڴ��ַ(Ҫ����ı�����ָ��)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;

		// 5.���÷��򣺴��ڴ浽����	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

		// 6.�����С	
		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;

		// 7.�����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

		// 8.�ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

		// 9.�����������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		// 10.�ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		// 11.DMAģʽ��һ�λ���ѭ��ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

		// 12.���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 

		// 13.��ֹ�ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		// 13.����DMAͨ��		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);	
			
		// 14.ʹ��DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}