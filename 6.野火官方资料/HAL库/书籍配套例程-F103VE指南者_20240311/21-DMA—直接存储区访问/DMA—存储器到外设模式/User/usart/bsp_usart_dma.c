/**
  ******************************************************************************
  * @file    bsp_usart_dma.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����c��printf������usart�˿�,ʹ��DMAģʽ��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_usart_dma.h"

uint8_t SendBuff[SENDBUFF_SIZE];

DMA_HandleTypeDef  DMA_Handle;      //DMA���
UART_HandleTypeDef UartHandle;      //UART���

 /**
  * @brief  USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
      
  DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
  /* ʹ�� UART ʱ�� */
  DEBUG_USART_CLK_ENABLE();
  
   /* ����Tx����Ϊ���ù���  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);
  
  
  UartHandle.Instance          = DEBUG_USART;
  UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  
  HAL_UART_Init(&UartHandle); 
}

///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

/**
  * @brief  USART1 TX DMA ���ã��ڴ浽����(USART1->DR)
  * @param  ��
  * @retval ��
  */
void USART_DMA_Config(void)
{
  DEBUG_USART_DMA_CLK_ENABLE();  
     
  DMA_Handle.Instance=DEBUG_USART_DMA_STREAM;                            //������ѡ��
                             
  DMA_Handle.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������HAL_DMA_Init(&DMA_Handle);
  DMA_Handle.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ/* Associate the DMA handle */
  DMA_Handle.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ__HAL_LINKDMA(&UartHandle, hdmatx, DMA_Handle); 
  DMA_Handle.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
  DMA_Handle.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
  DMA_Handle.Init.Mode=DMA_NORMAL;                            //������ͨģʽ
  DMA_Handle.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
  
  HAL_DMA_Init(&DMA_Handle);
  /* Associate the DMA handle */
  __HAL_LINKDMA(&UartHandle, hdmatx, DMA_Handle); 
  
}


/*********************************************END OF FILE**********************/
