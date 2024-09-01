/**
  ******************************************************************************
  * @file    wifi_config.c
  * @author  fire
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   WiFi模块接口配置驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO-STM32 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include "wifi_config.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"


struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


/**
  * @brief  WiFi_Config wifi 初始化
  * @param  无
  * @retval 无
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
	
		/*开启DMA时钟*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
		//NVIC_Config();	   			//配置DMA中断

		/*设置DMA源：串口数据寄存器地址*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( USART2->DR ) );	   

		/*内存地址(要传输的变量的指针)*/
//		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;
//	  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxBuff;
	  DMA_InitStructure.DMA_MemoryBaseAddr = ( u32 ) ( & ( USART1->DR ) );

		/*方向：从内存到外设*/		
//		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

		/*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
//		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
//	  DMA_InitStructure.DMA_BufferSize = RxBUFF_SIZE;
      DMA_InitStructure.DMA_BufferSize = 1;

		/*外设地址不增*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*内存地址自增*/
//		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

		/*外设数据单位*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*内存数据单位 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMA模式：不断循环*/
//		  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*优先级：中*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  

		/*禁止内存到内存的传输	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*配置DMA1的4通道*/		   
//		DMA_Init(DMA1_Channel4, &DMA_InitStructure); 	  
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
		
		/*使能DMA*/
//		DMA_Cmd (DMA1_Channel4,ENABLE);
    DMA_Cmd (DMA1_Channel6,ENABLE);		
		//DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);  //配置DMA发送完成后产生中断
		
}


/// 配置NVIC中的UART2中断
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
