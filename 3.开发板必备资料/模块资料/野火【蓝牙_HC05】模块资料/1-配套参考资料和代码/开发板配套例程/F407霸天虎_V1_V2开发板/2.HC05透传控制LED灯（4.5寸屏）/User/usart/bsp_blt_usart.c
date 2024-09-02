/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SPP_CA串口驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "./usart/bsp_blt_usart.h"
#include <stdarg.h>


extern ReceiveData BLT_USART_ReceiveData;



/// 配置USART接收中断
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
 * 函数名：USARTx_Config
 * 描述  ：USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void BLT_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(BLT_USART_GPIO_CLK, ENABLE);
	/* config USART clock */
	RCC_APB1PeriphClockCmd(BLT_USART_CLK, ENABLE); 
 
	  /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(BLT_USART_TX_PORT,BLT_USART_TX_SOURCE, BLT_USART_TX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
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
	
	/*	配置中断优先级 */
	NVIC_Configuration();
	/* 使能串口接收中断 */
	USART_ITConfig(BLT_USARTx, USART_IT_RXNE, ENABLE);
  //使能串口总线空闲中断 	
	USART_ITConfig (BLT_USARTx, USART_IT_IDLE, ENABLE );

	USART_Cmd(BLT_USARTx, ENABLE);
	USART_ClearFlag(BLT_USARTx, USART_FLAG_TC);
}

/***************** 发送一个字符  **********************/
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  发送指定长度的字符串 **********************/
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen )
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}


/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}






//获取接收到的数据和长度
char *get_rebuff(uint16_t *len) 
{
    *len = BLT_USART_ReceiveData.datanum;
    return (char *)&BLT_USART_ReceiveData.uart_buff;
}

//清空缓冲区
void clean_rebuff(void)
{

    uint16_t i=UART_BUFF_SIZE;
    BLT_USART_ReceiveData.datanum = 0;
	  BLT_USART_ReceiveData.receive_data_flag = 0;
	while(i)
		BLT_USART_ReceiveData.uart_buff[--i]=0;
	  

}







/******************* (C) COPYRIGHT  WildFire Team *****END OF FILE************/
