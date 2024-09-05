/**
  ******************************************************************************
  * @file    bsp_as608.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   指纹识别模块实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F103-霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f4xx.h"
#include "./as608/bsp_as608.h"
#include "./usart/rx_data_queue.h"
#include "./SysTick/bsp_SysTick.h"


uint32_t AS608_Addr = 0xFFFFFFFF;             /*指纹识别模块默认地址*/


static void NVIC_Configuration(void);
static void AS608_SendData(uint8_t data);
static void AS608_PackHead(void);
static void SendFlag(uint8_t flag);
static void SendLength(uint16_t length);
static void Sendcmd(uint8_t cmd);
static void SendCheck(uint16_t check);


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */	
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = AS608_USART_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
  /* 配置中断源：TouchOut线 */
  NVIC_InitStructure.NVIC_IRQChannel = AS608_TouchOut_INT_EXTI_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);  
}


  /**
  * @brief  AS608_TouchOut配置
  * @param  无
  * @retval 无
  */
void AS608_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
	/*开启串口GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(AS608_TouchOut_INT_GPIO_CLK,ENABLE);
  /* 使能 SYSCFG 时钟 ，使用GPIO外部中断时必须使能SYSCFG时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /*打开串口外设的时钟*/
  RCC_AHB1PeriphClockCmd(AS608_USART_RX_GPIO_CLK|AS608_USART_TX_GPIO_CLK,ENABLE);
  /* 使能 USART 时钟 */
  RCC_APB1PeriphClockCmd(AS608_USART_CLK, ENABLE);

												
	/* 配置 NVIC 中断*/
	NVIC_Configuration();
  
	/* TouchOut线用到的GPIO */	
  GPIO_InitStructure.GPIO_Pin = AS608_TouchOut_INT_GPIO_PIN;
  /* 配置为浮空输入 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    /* 设置引脚不上拉也不下拉 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(AS608_TouchOut_INT_GPIO_PORT, &GPIO_InitStructure);

	/* 选择EXTI的信号源 */
  SYSCFG_EXTILineConfig(AS608_TouchOut_INT_EXTI_PORTSOURCE, AS608_TouchOut_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = AS608_TouchOut_INT_EXTI_LINE;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 上升/下降沿中断 */
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising_Falling;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
   
  /*USART GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = AS608_USART_TX_PIN  ;  
  GPIO_Init(AS608_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = AS608_USART_RX_PIN;
  GPIO_Init(AS608_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(AS608_USART_RX_GPIO_PORT,AS608_USART_RX_SOURCE,AS608_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(AS608_USART_TX_GPIO_PORT,AS608_USART_TX_SOURCE,AS608_USART_TX_AF);
  
  /* 配置串AS608_USART 模式 */
  /* 波特率设置：AS608_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = AS608_USART_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：不使用校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(AS608_USART, &USART_InitStructure); 
	
  /* 嵌套向量中断控制器NVIC配置 */
	NVIC_Configuration();
  
	/* 使能串口接收中断 */
	USART_ITConfig(AS608_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig(AS608_USART, USART_IT_IDLE, ENABLE ); /*使能串口总线空闲中断 */	

  /* 使能串口 */
  USART_Cmd(AS608_USART, ENABLE);
 	
}



/**
  * @brief  AS608_USART串口向指纹模块传递数据 
  * @param  data;传输的数据
  */
static void AS608_SendData(uint8_t data)
{
  USART_SendData(AS608_USART,data);
  while(USART_GetFlagStatus(AS608_USART,USART_FLAG_TXE) == RESET);
}


/**
  * @brief  AS608_USART串口向指纹模块命令包头格式 
  * @param  无
  */
static void AS608_PackHead(void)
{ 
  /*包头*/
  AS608_SendData(0xEF);
  AS608_SendData(0x01);	
  
  /*指纹模块地址*/
  AS608_SendData(AS608_Addr>>24);
  AS608_SendData(AS608_Addr>>16);	
  AS608_SendData(AS608_Addr>>8);
  AS608_SendData(AS608_Addr);	
}


/**
  * @brief  发送包标识
  * @param  flag:包标识位
  * @retval 无
  */
static void SendFlag(uint8_t flag)
{
  AS608_SendData(flag);
}


/**
  * @brief  发送包长度
  * @param  length:包长度
  * @retval 无
  */
static void SendLength(uint16_t length)
{
	AS608_SendData(length>>8);
  AS608_SendData(length);
}


/**
  * @brief  发送指令码
  * @param  cmd;指令码
  * @retval 无
  */
static void Sendcmd(uint8_t cmd)
{
	AS608_SendData(cmd);
}


/**
  * @brief  发送校验和
  * @param  check:检查位
  * @retval 无
  */
static void SendCheck(uint16_t check)
{
	AS608_SendData(check>>8);
	AS608_SendData(check);
}


/**
  * @brief  从缓冲区读出确认码
  * @param  *i:返回值（确认码）
  * @retval 无
  */
uint16_t  ReturnFlag( uint16_t *i)
{
   QUEUE_DATA_TYPE *rx_data;
  
   rx_data = cbRead(&rx_queue);  /*从缓冲区读取数据，进行处理*/
    
   if(rx_data != NULL)           /*缓冲队列非空*/
  { 
		/*打印环行接收到的数据*/
    QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);
		
    *i=((uint16_t)(*(rx_data->head+9)));     /*确认码*/
		
	  cbReadFinish(&rx_queue);    /*使用完数据必须调用cbReadFinish更新读指针*/
	  
		return *i;
  }
	else
  {	
	  *i=0xff;
		
	  return *i;
  }
}


/**
  * @brief   探测手指，探测到后录入指纹图像存于 ImageBuffer
  * @param   无
  * @retval  确认码=00H 表示录入成功；确认码=01H 表示收包有错；
             确认码=02H 表示传感器上无手指；确认码=03H 表示录入不成功 
  */
uint16_t PS_GetImage(void)
{
  uint16_t temp;
  uint16_t sure,p=0;
	
	AS608_DELAY_MS(3000);       /*给指纹输入动作预留时间*/
	
	AS608_PackHead();
	SendFlag(0x01);             /*命令包标识*/
	SendLength(0x03);
	Sendcmd(0x01);              /*录指纹指令*/
  temp=0x01+0x03+0x01;
	SendCheck(temp);
	
  AS608_DELAY_MS(500);        /*等待指纹识别模块处理数据*/
  
	sure=ReturnFlag(&p);
  
  return  sure;
}


/**
  * @brief  将ImageBuffer中的原始图像生成指纹特征文件存于CHARBUFFER1或CHARBUFFER2
  * @param  BufferID(特征缓冲区号)
  * @retval 确认码=00H 表示生成特征成功；确认码=01H 表示收包有错；
            确认码=06H 表示指纹图像太乱而生不成特征；确认码=07H 表示指纹图像正常，但特征点太少而生不成特征；
            确认码=15H 表示图像缓冲区内没有有效原始图而生不成图像
  */
uint16_t PS_GenChar(uint8_t BufferID)
{
  uint16_t temp;
  uint16_t  sure,p=0;
 
  AS608_PackHead();
  SendFlag(0x01);          
  SendLength(0x04);
  Sendcmd(0x02);              /*生成特征指令*/   
  AS608_SendData(BufferID);
  temp = 0x01+0x04+0x02+BufferID;
  SendCheck(temp);
	
  AS608_DELAY_MS(500);
	
  sure=ReturnFlag(&p);
  
  return  sure;
}


/**
  * @brief  检查STM32与指纹模块的通信连接
  * @param  PS_Addr指纹模块地址
  * @retval 返回值0通讯成功;1表示通讯不成功
  */
uint16_t PS_Connect(uint32_t *PS_Addr)
{
	QUEUE_DATA_TYPE *rx_data;	
	
	AS608_PackHead();
	AS608_SendData(0x01);
	AS608_SendData(0x00);
	AS608_SendData(0x00);
	
  AS608_DELAY_MS(100);
	
	rx_data = cbRead(&rx_queue);  /*从缓冲区读取数据，进行处理*/ 
	if(rx_data != NULL)           /*缓冲队列非空*/
	{ 
		/*打印环行接收到的数据*/
		QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);
		
		if(/*判断是不是模块返回的应答包*/
   			*(rx_data->head)	== 0XEF
				  && *(rx_data->head+1)==0X01
				  && *(rx_data->head+6)==0X07)  
		  {
			AS608_INFO("指纹模块的地址为:0x%x%x%x%x\r\n",*(rx_data->head+2),
																						      *(rx_data->head+3),
																						      *(rx_data->head+4),
																					   	    *(rx_data->head+5));
		  }
			cbReadFinish(&rx_queue);  /*使用完数据必须调用cbReadFinish更新读指针*/
		  
			return 0;	
	}	
	
  return 1;		
}


/**
  * @brief  精确比对 CHARBUFFER1与CHARBUFFER2中的特征文件
  * @param  无
  * @retval 确认码=00H 表示指纹匹配；确认码=01H 表示收包有错；确认码=08H 表示指纹不匹配
  */
uint16_t PS_Match(void)
{
	uint16_t temp;
  uint16_t  sure,p=0;
	
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x03);
	Sendcmd(0x03);                /*精确比对指令*/
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	
  AS608_DELAY_MS(500);
	
  sure=ReturnFlag(&p);
  
  return  sure;

}


/**
  * @brief  将CHARBUFFER1与CHARBUFFER2 中的特征文件合并生成模板，
            结果存于CHARBUFFER1与CHARBUFFER2。
  * @param  无
  * @retval 确认码=00H 表示合并成功；确认码=01H 表示收包有错；
            确认码=0aH 表示合并失败（两枚指纹不属于同一手指）
  */
uint16_t PS_RegModel(void)
{
	uint16_t temp;
  uint16_t sure,p=0;
  	
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x03);
	Sendcmd(0x05);                /*合并特征指令*/
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	
  AS608_DELAY_MS(500);
	
  sure=ReturnFlag(&p);
  
  return  sure;

}


/**
  * @brief  键盘输入ID
  * @param  无
  * @retval 返回输入值
  */
uint32_t GET_NUM(void)
{
	uint32_t num;
	scanf("%d", &num);
	AS608_INFO("输入的ID:num=%d\r\n",num);
	return num;
}


/**
  * @brief  将 CHARBUFFER1 或 CHARBUFFER2 中的模板文件存到 PageID 号flash 数据库位置。
  * @param  BufferID:缓冲区号，
  * @param 	PageID:指纹库位置号
  * @retval 确认码=00H 表示储存成功；确认码=01H 表示收包有错；
            确认码=0bH 表示 PageID 超出指纹库范围；确认码=18H 表示写 FLASH 出错
  */
uint16_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  uint16_t sure,p=0;
 
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x06);
	Sendcmd(0x06);                /*存储模板指令*/
	AS608_SendData(BufferID);
  AS608_SendData(PageID>>8);
	AS608_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	       +(PageID>>8)+(uint8_t)PageID;
	SendCheck(temp);
	
	AS608_DELAY_MS(500);
	
  sure=ReturnFlag(&p);
  
  return  sure;

	
}

/**
  * @brief  以CHARBUFFER1或CHARBUFFER2中的特征文件高速搜索整个或部分指纹库
  * @param  BufferID:缓冲区号
  * @param  StartPage:起始页
  * @param  PageNum:页数
  * @param  p:传递指针
  * @retval ensure:确认字,*p:页码（相配指纹模板）
  */
uint16_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t *p)
{
	uint16_t temp;
  uint16_t ensure;
  QUEUE_DATA_TYPE *rx_data;
	
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x08);
	Sendcmd(0x1b);                 /*高速搜索指纹库指令*/
	AS608_SendData(BufferID);
	AS608_SendData(StartPage>>8);
	AS608_SendData(StartPage);
	AS608_SendData(PageNum>>8);
	AS608_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	       +(StartPage>>8)+(uint8_t)StartPage
	       +(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	
 	AS608_DELAY_MS(500);
	
  rx_data = cbRead(&rx_queue);  /*从缓冲区读取数据，进行处理*/
  if(rx_data != NULL)           /*缓冲队列非空*/
  {
		/*打印环行接收到的数据*/
	  QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);
    
    ensure=((uint16_t)(*(rx_data->head+9)));   /*确认码*/
    
    /*返回页码（相匹配的指纹模板ID）*/
	  *p=((*(rx_data->head+10))<<8)+(*(rx_data->head+11));
    
	  cbReadFinish(&rx_queue);    /*使用完数据必须调用cbReadFinish更新读指针*/
    
	  return ensure;
  }
	else
	{	
	  ensure=0xff;
    
	  return ensure;
	}
}



/**
  * @brief  删除 flash 数据库中指定ID号开始的N个指纹模板
  * @param  PageID:指纹库模板号
  * @param  N:删除的模板个数
  * @retval 确认码=00H 表示删除模板成功；确认码=01H 表示收包有错；
            确认码=10H 表示删除模板失败
  */
uint16_t PS_DeletChar(uint16_t PageID,uint16_t N)
{
	uint16_t temp;
  uint16_t sure,p=0;
 
	
	AS608_PackHead();
	SendFlag(0x01);//命令包标识
	SendLength(0x07);
	Sendcmd(0x0C);                /*删除指定指纹模板指令*/
	AS608_SendData(PageID>>8);
  AS608_SendData(PageID);
	AS608_SendData(N>>8);
	AS608_SendData(N);
	temp = 0x01+0x07+0x0C
	       +(PageID>>8)+(uint8_t)PageID
	       +(N>>8)+(uint8_t)N;
	SendCheck(temp);
	
	AS608_DELAY_MS(400);
	
	sure=ReturnFlag(&p);
  
  return  sure;

	
}
/**
  * @brief  删除 flash 数据库中所有指纹模板
  * @param  无
  * @retval 确认码=00H 表示清空成功；确认码=01H 表示收包有错；确认码=11H 表示清空失败
  */
uint16_t  PS_Empty(void)
{
	uint16_t temp;
  uint16_t sure,p=0;
 
	
	AS608_PackHead();
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	
	AS608_DELAY_MS(400);
	
  sure=ReturnFlag(&p);
  
  return  sure;

}


/**
  * @brief  显示错误信息
  * @param  ensure:确认码
  * @retval 无
  */
void  ShowErrMessage( uint16_t  ensure) 
{
  switch(ensure)
	{
    case  0x00:
			AS608_INFO("OK\r\n");
		break;
		
	  case  0x01:
			AS608_INFO("数据包接收错误\r\n");
		break;
		
	  case  0x02:
	    AS608_INFO("指纹模块没有检测到指纹！\r\n");
		break;
		
	  case  0x03:
	    AS608_INFO("录入指纹图像失败\r\n\r\n");
		break;
		
	  case  0x04:
	    AS608_INFO("指纹图像太干、太淡而生不成特征\r\n\r\n");
		break;
	  
		case  0x05:
	    AS608_INFO("指纹图像太湿、太糊而生不成特征\r\n\r\n");
		break;
		
	  case  0x06:
	    AS608_INFO("指纹图像太乱而生不成特征\r\n\r\n");
		break;
		
	  case  0x07:
	    AS608_INFO("指纹图像正常，但特征点太少（或面积太小）而生不成特征\r\n");
		break;
		
	  case  0x08:
	    AS608_INFO("指纹不匹配\r\n\r\n");
		break;
		
	  case  0x09:
      AS608_INFO("对比指纹失败，指纹库不存在此指纹！\r\n\r\n");
		break;
		
		case  0x0a:
	    AS608_INFO("特征合并失败\r\n");
		break;
		
		case  0x0b:
      AS608_INFO("访问指纹库时地址序号超出指纹库范围\r\n");
		break;
		
		case  0x10:
	    AS608_INFO("删除模板失败\r\n");
		break;
		
		case  0x11:
      AS608_INFO("清空指纹库失败\r\n");
		break;	
		
		case  0x15:
		  AS608_INFO("缓冲区内没有有效原始图而生不成图像\r\n");
		break;
		
		case  0x18:
		  AS608_INFO("读写 FLASH 出错\r\n");
		break;
		
		case  0x19:
	    AS608_INFO("未定义错误\r\n");
		break;
		
		case  0x1a:
	    AS608_INFO("无效寄存器号\r\n");
		break;
		
		case  0x1b:
			AS608_INFO("寄存器设定内容错误\r\n");
		break;
		
		case  0x1c:
		  AS608_INFO("记事本页码指定错误\r\n");
		break;
		
		case  0x1f:
      AS608_INFO("指纹库满\r\n");
		break;
		
		case  0x20:
      AS608_INFO("地址错误\r\n");
		break;
		
		default :
      AS608_INFO("模块返回确认码有误\r\n");
		break;	
  }

}






/*********************************************END OF FILE**********************/

