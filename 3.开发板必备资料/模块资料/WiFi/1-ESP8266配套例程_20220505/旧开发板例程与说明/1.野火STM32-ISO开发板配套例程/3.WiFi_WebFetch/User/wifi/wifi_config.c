/**
  ******************************************************************************
  * @file    wifi_config.c
  * @author  fire
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   WiFiģ��ӿ���������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO-STM32 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include "wifi_config.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"


struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


/**
  * @brief  WiFi_Config wifi ��ʼ��
  * @param  ��
  * @retval ��
  */
void WiFi_Config( void )
{
	WiFi_RST_INIT();
  WiFi_USART1_INIT(); 
	WiFi_USART2_INIT(); 
	USART2_DMA_Config();
	WiFi_NVIC_INIT();
  
}

void USART2_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		/*����DMAʱ��*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
		//NVIC_Config();	   			//����DMA�ж�

		/*����DMAԴ���������ݼĴ�����ַ*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( USART2->DR ) );	   

		/*�ڴ��ַ(Ҫ����ı�����ָ��)*/
//		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;
//	  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxBuff;
	  DMA_InitStructure.DMA_MemoryBaseAddr = ( u32 ) ( & ( USART1->DR ) );

		/*���򣺴��ڴ浽����*/		
//		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

		/*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
//		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
//	  DMA_InitStructure.DMA_BufferSize = RxBUFF_SIZE;
      DMA_InitStructure.DMA_BufferSize = 1;

		/*�����ַ����*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*�ڴ��ַ����*/
//		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

		/*�������ݵ�λ*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*�ڴ����ݵ�λ 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMAģʽ������ѭ��*/
//		  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*���ȼ�����*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  

		/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*����DMA1��4ͨ��*/		   
//		DMA_Init(DMA1_Channel4, &DMA_InitStructure); 	  
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
		
		/*ʹ��DMA*/
//		DMA_Cmd (DMA1_Channel4,ENABLE);
    DMA_Cmd (DMA1_Channel6,ENABLE);		
		//DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�
		
}


/// ����NVIC�е�UART2�ж�
void NVIC_Configuration( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


/*********************************************************end of file**************************************************/
