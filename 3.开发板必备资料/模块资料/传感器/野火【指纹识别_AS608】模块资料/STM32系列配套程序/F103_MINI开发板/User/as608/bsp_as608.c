/**
  ******************************************************************************
  * @file    bsp_as608.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ָ��ʶ��ģ��ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "./as608/bsp_as608.h"
#include "./usart/rx_data_queue.h"
#include "./SysTick/bsp_SysTick.h"
#include "./usart/bsp_usart.h" 

/*����������ʱ*/
#define AS608_DELAY_MS(x)     SysTick_Delay_Ms(x)

uint32_t AS608_Addr = 0xFFFFFFFF;             /*ָ��ʶ��ģ��Ĭ�ϵ�ַ*/


static void NVIC_Configuration(void);
static void AS608_SendData(uint8_t data);
static void AS608_PackHead(void);
static void SendFlag(uint8_t flag);
static void SendLength(uint16_t length);
static void Sendcmd(uint8_t cmd);
static void SendCheck(uint16_t check);


 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */	
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = AS608_USART_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
  /* �����ж�Դ��TouchOut�� */
  NVIC_InitStructure.NVIC_IRQChannel = AS608_TouchOut_INT_EXTI_IRQ;
    /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure); 
}


  /**
  * @brief  AS608_TouchOut����
  * @param  ��
  * @retval ��
  */
void AS608_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
	/*��������GPIO�ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(AS608_TouchOut_INT_GPIO_CLK,ENABLE);
  AS608_USART_GPIO_APBxClkCmd(AS608_USART_GPIO_CLK, ENABLE);
  /*�򿪴��������ʱ��*/
	AS608_USART_APBxClkCmd(AS608_USART_CLK, ENABLE);
												
	/* ���� NVIC �ж�*/
	NVIC_Configuration();
  
	/* TouchOut���õ���GPIO */	
  GPIO_InitStructure.GPIO_Pin = AS608_TouchOut_INT_GPIO_PIN;
  /* ����Ϊ�������� */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(AS608_TouchOut_INT_GPIO_PORT, &GPIO_InitStructure);

	/* ѡ��EXTI���ź�Դ */
  GPIO_EXTILineConfig(AS608_TouchOut_INT_EXTI_PORTSOURCE, AS608_TouchOut_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = AS608_TouchOut_INT_EXTI_LINE;
	
	/* EXTIΪ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* ����/�½����ж� */
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Rising_Falling;
  /* ʹ���ж� */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /*��USART Tx��GPIO����Ϊ���츴��ģʽ*/
  GPIO_InitStructure.GPIO_Pin = AS608_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(AS608_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /*��USART Rx��GPIO����Ϊ��������ģʽ*/
	GPIO_InitStructure.GPIO_Pin = AS608_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(AS608_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/*���ô��ڵĹ�������*/
	/*���ò�����*/
	USART_InitStructure.USART_BaudRate = AS608_USART_BAUDRATE;
	/*���� �������ֳ�*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	/*����ֹͣλ*/
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/*����У��λ*/
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	/*����Ӳ��������*/
	USART_InitStructure.USART_HardwareFlowControl = 
	                                USART_HardwareFlowControl_None;
	/*���ù���ģʽ���շ�һ��*/
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/*��ɴ��ڵĳ�ʼ������*/
	USART_Init(AS608_USART, &USART_InitStructure);
	
	/*ʹ�ܴ��ڽ����ж�*/
	USART_ITConfig(AS608_USART, USART_IT_RXNE, ENABLE);	
	USART_ITConfig(AS608_USART, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	
	
	/*ʹ�ܴ���*/
	USART_Cmd(AS608_USART, ENABLE);	
 	
}



/**
  * @brief  AS608_USART������ָ��ģ�鴫������ 
  * @param  data;���������
  */
static void AS608_SendData(uint8_t data)
{
  USART_SendData(AS608_USART,data);
  while(USART_GetFlagStatus(AS608_USART,USART_FLAG_TXE) == RESET);
}


/**
  * @brief  AS608_USART������ָ��ģ�������ͷ��ʽ 
  * @param  ��
  */
static void AS608_PackHead(void)
{ 
  /*��ͷ*/
  AS608_SendData(0xEF);
  AS608_SendData(0x01);	
  
  /*ָ��ģ���ַ*/
  AS608_SendData(AS608_Addr>>24);
  AS608_SendData(AS608_Addr>>16);	
  AS608_SendData(AS608_Addr>>8);
  AS608_SendData(AS608_Addr);	
}


/**
  * @brief  ���Ͱ���ʶ
  * @param  flag:����ʶλ
  * @retval ��
  */
static void SendFlag(uint8_t flag)
{
  AS608_SendData(flag);
}


/**
  * @brief  ���Ͱ�����
  * @param  length:������
  * @retval ��
  */
static void SendLength(uint16_t length)
{
	AS608_SendData(length>>8);
  AS608_SendData(length);
}


/**
  * @brief  ����ָ����
  * @param  cmd;ָ����
  * @retval ��
  */
static void Sendcmd(uint8_t cmd)
{
	AS608_SendData(cmd);
}


/**
  * @brief  ����У���
  * @param  check:���λ
  * @retval ��
  */
static void SendCheck(uint16_t check)
{
	AS608_SendData(check>>8);
	AS608_SendData(check);
}


/**
  * @brief  �ӻ���������ȷ����
  * @param  *i:����ֵ��ȷ���룩
  * @retval ��
  */
uint16_t  ReturnFlag( uint16_t *i)
{
   QUEUE_DATA_TYPE *rx_data;
  
   rx_data = cbRead(&rx_queue);  /*�ӻ�������ȡ���ݣ����д���*/
    
   if(rx_data != NULL)           /*������зǿ�*/
  { 
		/*��ӡ���н��յ�������*/
    QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);
		
    *i=((uint16_t)(*(rx_data->head+9)));     /*ȷ����*/
		
	  cbReadFinish(&rx_queue);    /*ʹ�������ݱ������cbReadFinish���¶�ָ��*/
	  
		return *i;
  }
	else
  {	
	  *i=0xff;
		
	  return *i;
  }
}


/**
  * @brief   ̽����ָ��̽�⵽��¼��ָ��ͼ����� ImageBuffer
  * @param   ��
  * @retval  ȷ����=00H ��ʾ¼��ɹ���ȷ����=01H ��ʾ�հ��д�
             ȷ����=02H ��ʾ������������ָ��ȷ����=03H ��ʾ¼�벻�ɹ� 
  */
uint16_t PS_GetImage(void)
{
  uint16_t temp;
  uint16_t sure,p=0;
	
	AS608_DELAY_MS(3000);       /*��ָ�����붯��Ԥ��ʱ��*/
	
	AS608_PackHead();
	SendFlag(0x01);             /*�������ʶ*/
	SendLength(0x03);
	Sendcmd(0x01);              /*¼ָ��ָ��*/
  temp=0x01+0x03+0x01;
	SendCheck(temp);
	
  AS608_DELAY_MS(500);        /*�ȴ�ָ��ʶ��ģ�鴦������*/
  
	sure=ReturnFlag(&p);
  
  return  sure;
}


/**
  * @brief  ��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CHARBUFFER1��CHARBUFFER2
  * @param  BufferID(������������)
  * @retval ȷ����=00H ��ʾ���������ɹ���ȷ����=01H ��ʾ�հ��д�
            ȷ����=06H ��ʾָ��ͼ��̫�Ҷ�������������ȷ����=07H ��ʾָ��ͼ����������������̫�ٶ�������������
            ȷ����=15H ��ʾͼ�񻺳�����û����Чԭʼͼ��������ͼ��
  */
uint16_t PS_GenChar(uint8_t BufferID)
{
  uint16_t temp;
  uint16_t  sure,p=0;
 
  AS608_PackHead();
  SendFlag(0x01);          
  SendLength(0x04);
  Sendcmd(0x02);              /*��������ָ��*/   
  AS608_SendData(BufferID);
  temp = 0x01+0x04+0x02+BufferID;
  SendCheck(temp);
	
  AS608_DELAY_MS(600);
	
  sure=ReturnFlag(&p);
  
  return  sure;
}


/**
  * @brief  ���STM32��ָ��ģ���ͨ������
  * @param  PS_Addrָ��ģ���ַ
  * @retval ����ֵ0ͨѶ�ɹ�;1��ʾͨѶ���ɹ�
  */
uint16_t PS_Connect(uint32_t *PS_Addr)
{
	QUEUE_DATA_TYPE *rx_data;	
	
	AS608_PackHead();
	AS608_SendData(0X01);
	AS608_SendData(0X00);
	AS608_SendData(0X00);
	
  AS608_DELAY_MS(100);
	
	rx_data = cbRead(&rx_queue);  /*�ӻ�������ȡ���ݣ����д���*/ 
	if(rx_data != NULL)           /*������зǿ�*/
	{ 
		/*��ӡ���н��յ�������*/
		QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);
		
		if(/*�ж��ǲ���ģ�鷵�ص�Ӧ���*/
   			*(rx_data->head)	== 0XEF
				  && *(rx_data->head+1)==0X01
				  && *(rx_data->head+6)==0X07)  
		  {
			AS608_INFO("ָ��ģ��ĵ�ַΪ:0x%x%x%x%x\r\n",*(rx_data->head+2),
																						      *(rx_data->head+3),
																						      *(rx_data->head+4),
																					   	    *(rx_data->head+5));
		  }
			cbReadFinish(&rx_queue);  /*ʹ�������ݱ������cbReadFinish���¶�ָ��*/
		  
			return 0;	
	}	
	
  return 1;		
}


/**
  * @brief  ��ȷ�ȶ� CHARBUFFER1��CHARBUFFER2�е������ļ�
  * @param  ��
  * @retval ȷ����=00H ��ʾָ��ƥ�䣻ȷ����=01H ��ʾ�հ��д�ȷ����=08H ��ʾָ�Ʋ�ƥ��
  */
uint16_t PS_Match(void)
{
	uint16_t temp;
  uint16_t  sure,p=0;
	
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x03);
	Sendcmd(0x03);                /*��ȷ�ȶ�ָ��*/
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	
  AS608_DELAY_MS(500);
	
  sure=ReturnFlag(&p);
  
  return  sure;

}


/**
  * @brief  ��CHARBUFFER1��CHARBUFFER2 �е������ļ��ϲ�����ģ�壬
            �������CHARBUFFER1��CHARBUFFER2��
  * @param  ��
  * @retval ȷ����=00H ��ʾ�ϲ��ɹ���ȷ����=01H ��ʾ�հ��д�
            ȷ����=0aH ��ʾ�ϲ�ʧ�ܣ���öָ�Ʋ�����ͬһ��ָ��
  */
uint16_t PS_RegModel(void)
{
	uint16_t temp;
  uint16_t sure,p=0;
  	
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x03);
	Sendcmd(0x05);                /*�ϲ�����ָ��*/
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	
  AS608_DELAY_MS(500);
	
  sure=ReturnFlag(&p);
  
  return  sure;

}


/**
  * @brief  ��������ID
  * @param  ��
  * @retval ��������ֵ
  */
uint32_t GET_NUM(void)
{
	uint32_t num;
	scanf("%d", &num);
	AS608_INFO("�����ID:num=%d\r\n",num);
	return num;
}


/**
  * @brief  �� CHARBUFFER1 �� CHARBUFFER2 �е�ģ���ļ��浽 PageID ��flash ���ݿ�λ�á�
  * @param  BufferID:�������ţ�
  * @param 	PageID:ָ�ƿ�λ�ú�
  * @retval ȷ����=00H ��ʾ����ɹ���ȷ����=01H ��ʾ�հ��д�
            ȷ����=0bH ��ʾ PageID ����ָ�ƿⷶΧ��ȷ����=18H ��ʾд FLASH ����
  */
uint16_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  uint16_t sure,p=0;
 
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x06);
	Sendcmd(0x06);                /*�洢ģ��ָ��*/
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
  * @brief  ��CHARBUFFER1��CHARBUFFER2�е������ļ��������������򲿷�ָ�ƿ�
  * @param  BufferID:��������
  * @param  StartPage:��ʼҳ
  * @param  PageNum:ҳ��
  * @param  p:����ָ��
  * @retval ensure:ȷ����,*p:ҳ�루����ָ��ģ�壩
  */
uint16_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,uint16_t *p)
{
	uint16_t temp;
  uint16_t ensure;
  QUEUE_DATA_TYPE *rx_data;
	
	AS608_PackHead();
	SendFlag(0x01);
	SendLength(0x08);
	Sendcmd(0x1b);                 /*��������ָ�ƿ�ָ��*/
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
	
  rx_data = cbRead(&rx_queue);  /*�ӻ�������ȡ���ݣ����д���*/
  if(rx_data != NULL)           /*������зǿ�*/
  {
		/*��ӡ���н��յ�������*/
	  QUEUE_DEBUG_ARRAY((uint8_t*)rx_data->head,rx_data->len);
    
    ensure=((uint16_t)(*(rx_data->head+9)));   /*ȷ����*/
    
    /*����ҳ�루��ƥ���ָ��ģ��ID��*/
	  *p=((*(rx_data->head+10))<<8)+(*(rx_data->head+11));
    
	  cbReadFinish(&rx_queue);    /*ʹ�������ݱ������cbReadFinish���¶�ָ��*/
    
	  return ensure;
  }
	else
	{	
	  ensure=0xff;
    
	  return ensure;
	}
}



/**
  * @brief  ɾ�� flash ���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
  * @param  PageID:ָ�ƿ�ģ���
  * @param  N:ɾ����ģ�����
  * @retval ȷ����=00H ��ʾɾ��ģ��ɹ���ȷ����=01H ��ʾ�հ��д�
            ȷ����=10H ��ʾɾ��ģ��ʧ��
  */
uint16_t PS_DeletChar(uint16_t PageID,uint16_t N)
{
	uint16_t temp;
  uint16_t sure,p=0;
 
	
	AS608_PackHead();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x0C);                /*ɾ��ָ��ָ��ģ��ָ��*/
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
  * @brief  ɾ�� flash ���ݿ�������ָ��ģ��
  * @param  ��
  * @retval ȷ����=00H ��ʾ��ճɹ���ȷ����=01H ��ʾ�հ��д�ȷ����=11H ��ʾ���ʧ��
  */
uint16_t  PS_Empty(void)
{
	uint16_t temp;
  uint16_t sure,p=0;
 
	
	AS608_PackHead();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	
	AS608_DELAY_MS(400);
	
  sure=ReturnFlag(&p);
  
  return  sure;

}


/**
  * @brief  ��ʾ������Ϣ
  * @param  ensure:ȷ����
  * @retval ��
  */
void  ShowErrMessage( uint16_t  ensure) 
{
  switch(ensure)
	{
    case  0x00:
			AS608_INFO("OK\r\n");
		break;
		
	  case  0x01:
			AS608_INFO("���ݰ����մ���\r\n");
		break;
		
	  case  0x02:
	    AS608_INFO("ָ��ģ��û�м�⵽ָ�ƣ�\r\n");
		break;
		
	  case  0x03:
	    AS608_INFO("¼��ָ��ͼ��ʧ��\r\n\r\n");
		break;
		
	  case  0x04:
	    AS608_INFO("ָ��ͼ��̫�ɡ�̫��������������\r\n\r\n");
		break;
	  
		case  0x05:
	    AS608_INFO("ָ��ͼ��̫ʪ��̫��������������\r\n\r\n");
		break;
		
	  case  0x06:
	    AS608_INFO("ָ��ͼ��̫�Ҷ�����������\r\n\r\n");
		break;
		
	  case  0x07:
	    AS608_INFO("ָ��ͼ����������������̫�٣������̫С��������������\r\n");
		break;
		
	  case  0x08:
	    AS608_INFO("ָ�Ʋ�ƥ��\r\n\r\n");
		break;
		
	  case  0x09:
      AS608_INFO("�Ա�ָ��ʧ�ܣ�ָ�ƿⲻ���ڴ�ָ�ƣ�\r\n\r\n");
		break;
		
		case  0x0a:
	    AS608_INFO("�����ϲ�ʧ��\r\n");
		break;
		
		case  0x0b:
      AS608_INFO("����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ\r\n");
		break;
		
		case  0x10:
	    AS608_INFO("ɾ��ģ��ʧ��\r\n");
		break;
		
		case  0x11:
      AS608_INFO("���ָ�ƿ�ʧ��\r\n");
		break;	
		
		case  0x15:
		  AS608_INFO("��������û����Чԭʼͼ��������ͼ��\r\n");
		break;
		
		case  0x18:
		  AS608_INFO("��д FLASH ����\r\n");
		break;
		
		case  0x19:
	    AS608_INFO("δ�������\r\n");
		break;
		
		case  0x1a:
	    AS608_INFO("��Ч�Ĵ�����\r\n");
		break;
		
		case  0x1b:
			AS608_INFO("�Ĵ����趨���ݴ���\r\n");
		break;
		
		case  0x1c:
		  AS608_INFO("���±�ҳ��ָ������\r\n");
		break;
		
		case  0x1f:
      AS608_INFO("ָ�ƿ���\r\n");
		break;
		
		case  0x20:
      AS608_INFO("��ַ����\r\n");
		break;
		
		default :
      AS608_INFO("ģ�鷵��ȷ��������\r\n");
		break;	
  }

}






/*********************************************END OF FILE**********************/

