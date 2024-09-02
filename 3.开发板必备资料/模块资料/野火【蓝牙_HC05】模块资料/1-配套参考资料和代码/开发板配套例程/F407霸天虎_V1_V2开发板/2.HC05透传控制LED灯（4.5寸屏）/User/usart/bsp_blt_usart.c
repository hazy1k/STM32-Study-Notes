/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SPP_CA��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "./usart/bsp_blt_usart.h"
#include <stdarg.h>


extern ReceiveData BLT_USART_ReceiveData;



/// ����USART�����ж�
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = BLT_USART_IRQ; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������USARTx_Config
 * ����  ��USART GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void BLT_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(BLT_USART_GPIO_CLK, ENABLE);
	/* config USART clock */
	RCC_APB1PeriphClockCmd(BLT_USART_CLK, ENABLE); 
 
	  /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(BLT_USART_TX_PORT,BLT_USART_TX_SOURCE, BLT_USART_TX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(BLT_USART_RX_PORT,BLT_USART_RX_SOURCE,BLT_USART_RX_AF); 

	
	/* USARTconfig */
  /* Configure USART as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = BLT_USART_TX_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(BLT_USART_TX_PORT, &GPIO_InitStructure);
	    
  /* Configure USART as input floating */
  GPIO_InitStructure.GPIO_Pin = BLT_USART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(BLT_USART_RX_PORT, &GPIO_InitStructure);
	  
	/* USART mode config */
	USART_InitStructure.USART_BaudRate = BLT_USART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(BLT_USARTx, &USART_InitStructure); 
	
	/*	�����ж����ȼ� */
	NVIC_Configuration();
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(BLT_USARTx, USART_IT_RXNE, ENABLE);
  //ʹ�ܴ������߿����ж� 	
	USART_ITConfig (BLT_USARTx, USART_IT_IDLE, ENABLE );

	USART_Cmd(BLT_USARTx, ENABLE);
	USART_ClearFlag(BLT_USARTx, USART_FLAG_TC);
}

/***************** ����һ���ַ�  **********************/
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  ����ָ�����ȵ��ַ��� **********************/
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen )
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}


/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}






//��ȡ���յ������ݺͳ���
char *get_rebuff(uint16_t *len) 
{
    *len = BLT_USART_ReceiveData.datanum;
    return (char *)&BLT_USART_ReceiveData.uart_buff;
}

//��ջ�����
void clean_rebuff(void)
{

    uint16_t i=UART_BUFF_SIZE;
    BLT_USART_ReceiveData.datanum = 0;
	  BLT_USART_ReceiveData.receive_data_flag = 0;
	while(i)
		BLT_USART_ReceiveData.uart_buff[--i]=0;
	  

}







/******************* (C) COPYRIGHT  WildFire Team *****END OF FILE************/
