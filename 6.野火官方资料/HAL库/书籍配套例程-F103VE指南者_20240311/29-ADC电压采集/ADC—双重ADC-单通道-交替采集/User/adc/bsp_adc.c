
/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ADC
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./adc/bsp_adc.h"

ADC_HandleTypeDef hadcx1;
ADC_HandleTypeDef hadcx2;
DMA_HandleTypeDef hdma_adcx;


void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hadc->Instance==ADCx1)
  {
    /* ����ʱ��ʹ�� */
    ADCx1_RCC_CLK_ENABLE();
    
    /* ADת��ͨ������ʱ��ʹ�� */
    ADC_GPIO_ClK_ENABLE();
    
    /* DMAʱ��ʹ�� */
    DMAx1_RCC_CLK_ENABLE();
      
    /* ADת��ͨ�����ų�ʼ�� */
    GPIO_InitStruct.Pin = ADC_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct);

    /* DMA�����ʼ������ */  
    hdma_adcx.Instance = ADC_DMAx_CHANNELn;
    hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adcx.Init.MemInc = DMA_MINC_DISABLE;
    hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adcx.Init.Mode = DMA_CIRCULAR;
    hdma_adcx.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_adcx);
    /* ����DMA */
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adcx);
    
    /* �����ж����ȼ����ú�ʹ���ж� */
    HAL_NVIC_SetPriority(ADC_DMAx_CHANNELn_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_DMAx_CHANNELn_IRQn);
  }
  else if(hadc->Instance==ADCx2)
  {
    /* ����ʱ��ʹ�� */
    ADCx2_RCC_CLK_ENABLE();
    
    /* ADת��ͨ������ʱ��ʹ�� */
    ADC_GPIO_ClK_ENABLE();
      
    /* ADת��ͨ�����ų�ʼ�� */
    GPIO_InitStruct.Pin = ADC_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(ADC_GPIO, &GPIO_InitStruct);
  }
}

void ADCx_Init(void)
{
  ADC_MultiModeTypeDef multimode;
  ADC_ChannelConfTypeDef sConfig;

  // ADC��������
  hadcx1.Instance = ADCx1;
  hadcx1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadcx1.Init.ContinuousConvMode = ENABLE;
  hadcx1.Init.DiscontinuousConvMode = DISABLE;
  hadcx1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadcx1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadcx1.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadcx1);  
  
    /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_DUALMODE_INTERLSLOW;
  HAL_ADCEx_MultiModeConfigChannel(&hadcx1, &multimode);
  
  // ���ò���ͨ��
  sConfig.Channel = ADC_CHANNEL;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx1, &sConfig);
  
  // ADC��������
  hadcx2.Instance = ADCx2;
  hadcx2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadcx2.Init.ContinuousConvMode = ENABLE;
  hadcx2.Init.DiscontinuousConvMode = DISABLE;
  hadcx2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadcx2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadcx2.Init.NbrOfConversion = 1;
  HAL_ADC_Init(&hadcx2);  
  
    /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_DUALMODE_INTERLSLOW;
  HAL_ADCEx_MultiModeConfigChannel(&hadcx2, &multimode);
  
  // ���ò���ͨ��
  sConfig.Channel = ADC_CHANNEL;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadcx2, &sConfig);
}



