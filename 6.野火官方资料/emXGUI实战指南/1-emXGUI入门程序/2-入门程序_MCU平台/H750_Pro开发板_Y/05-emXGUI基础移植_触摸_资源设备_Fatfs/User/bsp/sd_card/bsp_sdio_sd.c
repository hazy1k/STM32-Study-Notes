/**
  ******************************************************************************
  * @file    bsp_sdio_sd.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   SDIO sd�����������������ļ�ϵͳ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 H743 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./sd_card/bsp_sdio_sd.h"
#include "./led/bsp_led.h"   

#include "FreeRTOS.h"
#include "task.h"

SD_HandleTypeDef uSdHandle;

uint8_t Buffer_Block_Tx[MULTI_BUFFER_SIZE/4];
uint8_t Buffer_Block_Rx[MULTI_BUFFER_SIZE/4];
//���ͱ�־λ
volatile uint8_t TX_Flag=0;
//���ܱ�־λ
volatile uint8_t RX_Flag=0; 
/**
  * @brief  SD_Card���Ժ���
  * @param  ��
  * @retval ��
  */
void SD_Test(void)
{
    LED_BLUE;
    /*------------------------------ SD ��ʼ�� ------------------------------ */
    if(BSP_SD_Init() != HAL_OK)
    {    
        LED_RED;
        printf("SD����ʼ��ʧ�ܣ���ȷ��SD������ȷ���뿪���壬��һ��SD�����ԣ�\n");
    }
    else
    {
        printf("SD����ʼ���ɹ���\n");	     
        LED_BLUE;
        /*��������*/
        SD_EraseTest();

        LED_BLUE;
        /*single block ��д����*/
        SD_SingleBlockTest();

        LED_BLUE;
        /*muti block ��д����*/
        SD_MultiBlockTest(); 
    } 
} 
void BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params)
{
  /* ����SDIC�ж�*/
  HAL_NVIC_DisableIRQ(SDMMC1_IRQn);

  /* ����SDMMC1ʱ�� */
  __HAL_RCC_SDMMC1_CLK_DISABLE();

}
uint8_t BSP_SD_DeInit(void)
{ 
  uint8_t sd_state = HAL_OK;
 
  uSdHandle.Instance = SDMMC1;
  
  /* ȡ����ʼ��SD���豸 */
  if(HAL_SD_DeInit(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  /* ȡ����ʼ��SD�ײ����� */
  uSdHandle.Instance = SDMMC1;
  BSP_SD_MspDeInit(&uSdHandle, NULL);
  
  return  sd_state;
}

/**
  * @brief  ��ʼ��SDMMC1��SD��
  * @param  ��
  * @retval HAL_OK����ʼ���ɹ���HAL_ERROR����ʼ��ʧ��
  */
HAL_StatusTypeDef BSP_SD_Init(void)
{ 
  uint8_t sd_state = HAL_OK;
  //BSP_SD_DeInit();
  /* ����SDMMC��� */
  uSdHandle.Instance = SDMMC1;
  
	/* SDMMC�ں�ʱ��200Mhz, SDCardʱ��25Mhz  */
  uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_4B;
  uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  uSdHandle.Init.ClockDiv            = 0;
  
  /* ��ʼ��SD�ײ����� */
  BSP_SD_MspInit();

  /* HAL SD ��ʼ�� */
  if(HAL_SD_Init(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }
//    /* ����SD����λ�� */
//  if(sd_state == HAL_OK)
//  {
//    /* ����Ϊ4bitģʽ */
//    if(HAL_SD_ConfigWideBusOperation(&uSdHandle, uSdHandle.Init.BusWide) != HAL_OK)
//    {
//      sd_state = HAL_ERROR;
//    }
//    else
//    {
//      sd_state = HAL_OK;
//    }
//  }

  return  sd_state;
}
/**
  * @brief  SD����������
  * @param  ��
  * @retval ��
  */
static void SD_EraseTest(void)
{
    HAL_StatusTypeDef Status = HAL_OK;
    HAL_StatusTypeDef EraseStatus = HAL_OK;
    if (Status == HAL_OK)
    {
        Status = HAL_SD_Erase(&uSdHandle, 0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));
        if(Wait_SDCARD_Ready() != HAL_OK)
        {
            EraseStatus = HAL_ERROR;
        }
    }     
    if(EraseStatus == HAL_OK)
    {    
      LED_GREEN;
      printf("SD���������Գɹ���\n");
    }
    else
    {
      LED_RED;
      printf("SD����������ʧ�ܣ�\n");
      printf("��ܰ��ʾ������SD����֧�ֲ������ԣ���SD����ͨ�������single��д���ԣ�����ʾSD���ܹ�����ʹ�á�\n");
    }    
}

/**
  * @brief  SD�������д����
  * @param  ��
  * @retval ��
  */
void SD_SingleBlockTest(void)
{
    HAL_StatusTypeDef Status =  HAL_OK;
    HAL_StatusTypeDef TransferStatus1 = HAL_ERROR;
    /* Fill the buffer to send */
    Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE/4, 0);    
    SCB_CleanDCache_by_Addr((uint32_t*)Buffer_Block_Tx, BLOCK_SIZE/4);
    if(Status == HAL_OK)
    {
        /* Write block of 512 bytes on address 0 */
        Status = HAL_SD_WriteBlocks_DMA(&uSdHandle, Buffer_Block_Tx, 0x00, 1);
        while(TX_Flag == 0);
    } 
    /* Fill the buffer to reception */
    Fill_Buffer(Buffer_Block_Rx, BLOCK_SIZE/4, 0);   
    SCB_CleanDCache_by_Addr((uint32_t*)Buffer_Block_Rx, BLOCK_SIZE/4); 
    if(Status == HAL_OK)
    {
        /* Read block of 512 bytes from address 0 */
        Status = HAL_SD_ReadBlocks_DMA(&uSdHandle, Buffer_Block_Rx,0, 1);
        while(RX_Flag == 0);
    }    
    if (Status == HAL_OK)
    {
        TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE/4);
    }
    if(TransferStatus1 == HAL_OK)
    {
        LED_GREEN;
        printf("Single block ���Գɹ���\n");
    }
    else
    {
        LED_RED;
        printf("Single block ����ʧ�ܣ���ȷ��SD����ȷ���뿪���壬��һ��SD�����ԣ�\n");
    }
}
/**
  * @brief  SD������д����
  * @param  ��
  * @retval ��
  */
void SD_MultiBlockTest(void)
{ 
    HAL_StatusTypeDef Status =  HAL_OK;
    HAL_StatusTypeDef TransferStatus1 = HAL_ERROR;
    TX_Flag = 0;
    RX_Flag = 0; 
    /* Fill the buffer to send */
    Fill_Buffer(Buffer_Block_Tx, MULTI_BUFFER_SIZE/4, 0);    
    SCB_CleanDCache_by_Addr((uint32_t*)Buffer_Block_Tx, MULTI_BUFFER_SIZE/4);
    if(Status == HAL_OK)
    {
        /* Write block of 512 bytes on address 0 */
        Status = HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t *)Buffer_Block_Tx, 0,  NUMBER_OF_BLOCKS);
        while(TX_Flag == 0);
    } 
    /* Fill the buffer to reception */
    Fill_Buffer(Buffer_Block_Rx, MULTI_BUFFER_SIZE/4, 0);   
    SCB_CleanDCache_by_Addr((uint32_t*)Buffer_Block_Rx, MULTI_BUFFER_SIZE/4); 
    if(Status == HAL_OK)
    {
        /* Read block of 512 bytes from address 0 */
        Status = HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)Buffer_Block_Rx,0, NUMBER_OF_BLOCKS);
        while(RX_Flag == 0);
    }    
    if (Status == HAL_OK)
    {
        TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, MULTI_BUFFER_SIZE/4);
    }
    if(TransferStatus1 == HAL_OK)
    {
        LED_GREEN;
        printf("Multi block ���Գɹ���\n");
    }
    else
    {
        LED_RED;
        printf("Multi block ����ʧ�ܣ���ȷ��SD����ȷ���뿪���壬��һ��SD�����ԣ�\n");
    }
}

/**
  * @brief  SD���ȴ�������ɺ���
  * @param  ��
  * @retval HAL_OK�������ɹ���HAL_ERROR������ʧ��
  */
static HAL_StatusTypeDef Wait_SDCARD_Ready(void)
{
    uint32_t loop = SD_TIMEOUT;
    
    /* Wait for the Erasing process is completed */
    /* Verify that SD card is ready to use after the Erase */
    while(loop > 0)
    {
      loop--;
      if(HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER)
      {
          return HAL_OK;
      }
    }
    return HAL_ERROR;
}
/**
  * @brief  ����ƥ���⺯��
  * @param  pBuffer1���������飻pBuffer2���������飻BufferLength�����鳤��
  * @retval HAL_OK��ƥ�䣻HAL_ERROR����ƥ��
  */
static HAL_StatusTypeDef Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
    while (BufferLength--)
    {
      if (*pBuffer1 != *pBuffer2)
      {      
        return HAL_ERROR;      
      }
      else{

        pBuffer1++;
        pBuffer2++;
      }
    }
    return HAL_OK;
}


/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint32_t index = 0;
  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++)
  {
    pBuffer[index] = index + Offset;
  }
}
/**
  * @brief  ����WIFIģ��
  * @param  ��
  * @param  ��
  * @retval ��
  */
void WIFI_PDN_INIT(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStruct;
    /*ʹ������ʱ��*/	
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /*ѡ��Ҫ���Ƶ�GPIO����*/															   
    GPIO_InitStruct.Pin = GPIO_PIN_2;	
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;      
    /*��������Ϊ����ģʽ*/
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    /*������������Ϊ���� */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
    /*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	
    /*����WiFiģ��*/
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);  
}
/**
  * @brief  ����ѯģʽ�´�SD���е�ָ����ַ��ȡ��
  * @param  pData: ָ�򽫰���Ҫ��������ݵĻ�������ָ��
  * @param  ReadAddr: �������ȡ���ݵĵ�ַ  
  * @param  NumOfBlocks: ��ȡ�������� 
  * @retval SD ״̬
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t NumOfBlocks)
{
  
  if(HAL_SD_ReadBlocks(&uSdHandle,(uint8_t *)pData, ReadAddr,NumOfBlocks,SD_TIMEOUT) == HAL_OK)
  {
    return HAL_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  ����ѯģʽ�´�SD���е�ָ����ַд��� 
  * @param  pData: ָ�򽫰���Ҫ��������ݵĻ�������ָ��
  * @param  WriteAddr: �������ȡ���ݵĵ�ַ  
  * @param  NumOfBlocks: ��ȡ�������� 
  * @retval SD ״̬
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr,uint32_t NumOfBlocks)
{
  if(HAL_SD_WriteBlocks(&uSdHandle,(uint8_t *)pData, WriteAddr,NumOfBlocks,SD_TIMEOUT) == HAL_OK)
  {
    
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  ��DMA��ʽ��SD���е�ָ����ַ��ȡ��
  * @param  pData: ָ�򽫰���Ҫ��������ݵĻ�������ָ��
  * @param  ReadAddr: �������ȡ���ݵĵ�ַ  
  * @param  NumOfBlocks: ��ȡ�������� 
  * @retval SD ״̬
  */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint64_t ReadAddr, uint32_t NumOfBlocks)
{ 
  taskENTER_CRITICAL();
  /* ��DMA��ʽ��SD���е�ָ����ַ��ȡ�� */
  if(HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)pData, ReadAddr,NumOfBlocks) == HAL_OK)
  {
    taskEXIT_CRITICAL();	
    return MSD_OK;
  }
  else
  {
    taskEXIT_CRITICAL();	
    return MSD_ERROR;
  }
}

/**
  * @brief  ��DMA��ʽ��SD���е�ָ����ַд���
  * @param  pData: ָ�򽫰���Ҫ��������ݵĻ�������ָ��
  * @param  ReadAddr: ������д�����ݵĵ�ַ  
  * @param  NumOfBlocks: ��ȡ�������� 
  * @retval SD ״̬
  */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint64_t WriteAddr, uint32_t NumOfBlocks)
{  

  /* ��DMA��ʽ��SD���е�ָ����ַд��� */
  if(HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks) == HAL_OK)
  {

    return MSD_OK;
  }
  else
  {

    return MSD_ERROR;
  }
}
/**
  * @brief  ��ʼ��SD����
  * @param  ��
  * @param  ��
  * @retval None
  */
void BSP_SD_MspInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* ʹ�� SDMMC ʱ�� */
    __HAL_RCC_SDMMC1_CLK_ENABLE();
//    HAL_RCCEx_GetPLL1ClockFreq(&PLL1_Clocks);
    /* ʹ�� GPIOs ʱ�� */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
  
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    //����WIFIģ��
    WIFI_PDN_INIT();
        
    HAL_NVIC_SetPriority(SDMMC1_IRQn,0x7,0);  //����SDMMC1�ж�
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);        //ʹ��SDMMC1�ж�
    
    HAL_NVIC_SetPriority(SysTick_IRQn, 0x0E ,0);
}

void SDMMC1_IRQHandler(void)
{
//  uint32_t ulReturn;
//  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
//  ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
  HAL_SD_IRQHandler(&uSdHandle);
		
  /* �˳��ٽ�� */
//  taskEXIT_CRITICAL_FROM_ISR( ulReturn );  
}
/************************END OF FILE*******************/
