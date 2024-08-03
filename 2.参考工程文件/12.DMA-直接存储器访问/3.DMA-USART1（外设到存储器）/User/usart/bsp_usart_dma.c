#include "bsp_usart_dma.h"

uint8_t ReceiveBuff[RECEIVEBUFF_SIZE]; // 接收缓冲区

// 串口配置函数
void USART_Config(void)
{
	// 结构体定义
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStruct;

	// 1.打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 2.打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
 	
	// 3.配置NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置抢占优先级2
	NVIC_InitStruct.NVIC_IRQChannel = DEBUG_USART_IRQ; // 设置中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // 使能中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3; // 响应优先级
	NVIC_Init(&NVIC_InitStruct); // 初始化NVIC

	// 4.将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN; // 设置Tx引脚-PA9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        // 设置复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 设置GPIO速度为50MHz
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIO

    // 5.将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN; // 设置Rx引脚-PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 设置浮空输入
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure); // 初始化GPIO
	
	// 配置串口的工作参数
	// 1.配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE; // 设置波特率115200
	// 2.配置数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 设置数据字长8位
	// 3.配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 设置停止位1
	// 4.配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No; // 设置无校验位
	// 5.配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 设置无硬件流控制
	// 6.配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 设置收发一起工作模式
	// 7.完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure); // 初始化串口
	// 8.使能空闲中断
	USART_ITConfig(DEBUG_USARTx,USART_IT_IDLE,ENABLE);
	// 9.使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}


/*****************  发送一个字节 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}

// USARTx TX DMA 配置，内存到外设(USART1->DR)
void USARTx_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		// 开启DMA时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	    
		// 设置DMA源地址：串口数据寄存器地址*/
        DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// 内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuff;
		// 方向：从外设到内存	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// 传输大小	
		DMA_InitStructure.DMA_BufferSize = RECEIVEBUFF_SIZE;
		// 外设地址不增	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// 内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// 外设数据单位	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		// 内存数据单位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMA模式，一次或者循环模式
//		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环模式
		// 优先级：中	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// 禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// 配置DMA通道		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
		// 使能DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
}