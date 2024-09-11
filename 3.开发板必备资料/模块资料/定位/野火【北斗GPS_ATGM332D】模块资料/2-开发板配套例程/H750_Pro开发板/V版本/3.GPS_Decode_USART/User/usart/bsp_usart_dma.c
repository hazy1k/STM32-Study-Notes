/**
  ******************************************************************************
  * @file    bsp_usart_dma.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GPS����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 H750 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_usart_dma.h"
#include "./led/bsp_led.h"

DMA_HandleTypeDef DMA_Handle;
UART_HandleTypeDef GPS_UartHandle;



/* DMA���������־ */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;
/* DMA���ջ���  */
uint8_t gps_rbuff[GPS_RBUFF_SIZE];

 /**
  * @brief  GPS_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */  
void GPS_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
        
    GPS_USART_RX_GPIO_CLK_ENABLE();
    GPS_USART_TX_GPIO_CLK_ENABLE();
    
    /* ���ô���6ʱ��Դ*/
		RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART6;
		RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* ʹ�ܴ���1ʱ�� */
    GPS_USART_CLK_ENABLE();

    /* ����Tx����Ϊ���ù���  */
    GPIO_InitStruct.Pin = GPS_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPS_USART_TX_AF;
    HAL_GPIO_Init(GPS_USART_TX_GPIO_PORT, &GPIO_InitStruct);
    
    /* ����Rx����Ϊ���ù��� */
    GPIO_InitStruct.Pin = GPS_USART_RX_PIN;
    GPIO_InitStruct.Alternate = GPS_USART_RX_AF;
    HAL_GPIO_Init(GPS_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
    
    /* ���ô�GPS_USART ģʽ */
    GPS_UartHandle.Instance = GPS_USART;
    GPS_UartHandle.Init.BaudRate = GPS_USART_BAUDRATE;
    GPS_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    GPS_UartHandle.Init.StopBits = UART_STOPBITS_1;
    GPS_UartHandle.Init.Parity = UART_PARITY_NONE;
    GPS_UartHandle.Init.Mode = UART_MODE_TX_RX;
    GPS_UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    GPS_UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    GPS_UartHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    GPS_UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&GPS_UartHandle);
}

/**
  * @brief  USART_DMA_Config gps dma��������
  * @param  ��
  * @retval ��
  */
void USART_DMA_Config(void)
{
  /*����DMAʱ��*/
  GPS_USART_DMA_CLK_ENABLE();
  
  DMA_Handle.Instance = GPS_USART_DMA_STREAM;
  /* Deinitialize the stream for new transfer */
  HAL_DMA_DeInit(&DMA_Handle);
  /*usart6��Ӧ������*/	
  DMA_Handle.Init.Request = GPS_USART_DMA_REQUEST; 
  /*���򣺴��ڴ浽����*/		
  DMA_Handle.Init.Direction= DMA_PERIPH_TO_MEMORY;	
  /*�����ַ����*/	    
  DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE; 
  /*�ڴ��ַ����*/
  DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;	
  /*�������ݵ�λ*/	
  DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  /*�ڴ����ݵ�λ 8bit*/
  DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;	
  /*DMAģʽ������ѭ��*/
  DMA_Handle.Init.Mode = DMA_CIRCULAR;	 
  /*���ȼ�����*/	
  DMA_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;      
  /*����FIFO*/
  DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;        
  DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;    
  /*�洢��ͻ������ 1������*/
  DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;    
  /*����ͻ������ 1������*/
  DMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;    
  /*����DMA2��������7*/		   
  /* Configure the DMA stream */
  HAL_DMA_Init(&DMA_Handle); 
   /* Associate the DMA handle */
  __HAL_LINKDMA(&GPS_UartHandle, hdmarx, DMA_Handle);
		/*����GPSʹ�õ�DMA�ж����ȼ�*/
	HAL_NVIC_SetPriority(GPS_DMA_IRQ, 0, 0);
	 /*ʹ��DMA�ж�*/
	HAL_NVIC_EnableIRQ(GPS_DMA_IRQ);
	 /*�����ж�����*/
	__HAL_DMA_ENABLE_IT(&DMA_Handle,DMA_IT_HT|DMA_IT_TC);

}


/**
  * @brief  GPS_Config gps ��ʼ��
  * @param  ��
  * @retval ��
  */
void GPS_Config(void)
{
  GPS_USART_Config();
  USART_DMA_Config();  
}


/**
  * @brief  GPS_ProcessDMAIRQ GPS DMA�жϷ�����
  * @param  None.
  * @retval None.
  */
void GPS_ProcessDMAIRQ(void)
{
	
	if(__HAL_DMA_GET_FLAG(&DMA_Handle,DMA_FLAG_HTIF3_7) != RESET)        /* DMA �봫����� */
  { 
    GPS_HalfTransferEnd = 1;                //���ð봫����ɱ�־λ
		LED1_ON;
  }
  else if(__HAL_DMA_GET_FLAG(&DMA_Handle,DMA_FLAG_TCIF3_7)!= RESET)     /* DMA ������� */
  {
     LED1_OFF;
    GPS_TransferEnd = 1;                    //���ô�����ɱ�־λ
  }
	HAL_DMA_IRQHandler(&DMA_Handle);
}


/**
  * @brief  trace �ڽ���ʱ��������GPS���
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void trace(const char *str, int str_size)
{
  #ifdef __GPS_DEBUG    //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
  
    printf("\n");
  #endif
}

/**
  * @brief  error �ڽ������ʱ�����ʾ��Ϣ
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void error(const char *str, int str_size)
{
    #ifdef __GPS_DEBUG   //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ

    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
    #endif
}

/**
  * @brief  error �ڽ������ʱ�����ʾ��Ϣ
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void gps_info(const char *str, int str_size)
{

    uint16_t i;
    printf("\r\nInfo: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
}



/******************************************************************************************************** 
**     ��������:            bit        IsLeapYear(uint8_t    iYear) 
**    ��������:            �ж�����(�������2000�Ժ�����) 
**    ��ڲ�����            iYear    ��λ���� 
**    ���ڲ���:            uint8_t        1:Ϊ����    0:Ϊƽ�� 
********************************************************************************************************/ 
static uint8_t IsLeapYear(uint8_t iYear) 
{ 
    uint16_t    Year; 
    Year    =    2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
     return 0; 
} 

/******************************************************************************************************** 
**     ��������:            void    GMTconvert(uint8_t *DT,uint8_t GMT,uint8_t AREA) 
**    ��������:            ��������ʱ�任�������ʱ��ʱ�� 
**    ��ڲ�����            *DT:    ��ʾ����ʱ������� ��ʽ YY,MM,DD,HH,MM,SS 
**                        GMT:    ʱ���� 
**                        AREA:    1(+)���� W0(-)���� 
********************************************************************************************************/ 
void    GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //������ʱ�����ݴ���� 
     
    if(GMT==0)    return;                //�������0ʱ��ֱ�ӷ��� 
    if(GMT>12)    return;                //ʱ�����Ϊ12 �����򷵻�         

    YY    =    SourceTime->year;                //��ȡ�� 
    MM    =    SourceTime->mon;                 //��ȡ�� 
    DD    =    SourceTime->day;                 //��ȡ�� 
    hh    =    SourceTime->hour;                //��ȡʱ 
    mm    =    SourceTime->min;                 //��ȡ�� 
    ss    =    SourceTime->sec;                 //��ȡ�� 

    if(AREA)                        //��(+)ʱ������ 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+GMT-24;        //�ȵó�ʱ�� 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //���·�(12�µ�������) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //С�·�2�µ�������) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //����2�·� 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //��������������2��29�� ���߲�����������2��28�� 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //����12�·� 
            { 
                if(DD<31)    DD++; 
                else        //�������һ�� 
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)    hh    -=    GMT;    //������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+24-GMT;        //�ȵó�ʱ�� 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //�����Ǵ��·�(1�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //������С�·�2�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //�����ϸ�����2�·� 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //�������� 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //����1�·� 
            { 
                if(DD>1)    DD--; 
                else        //�����һ�� 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //������ 
    ConvertTime->mon    =    MM;                //������ 
    ConvertTime->day    =    DD;                //������ 
    ConvertTime->hour   =    hh;                //����ʱ 
    ConvertTime->min    =    mm;                //���·� 
    ConvertTime->sec    =    ss;                //������ 
}  



/*********************************************END OF FILE**********************/
