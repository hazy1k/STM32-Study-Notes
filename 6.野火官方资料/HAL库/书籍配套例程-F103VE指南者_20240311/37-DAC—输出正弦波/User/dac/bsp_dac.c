/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 Ӧ��bsp / DMA ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32  ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./dac/bsp_dac.h"

DAC_HandleTypeDef DAC_InitStructure;
DMA_HandleTypeDef DMA_InitStructure;
TIM_HandleTypeDef TIM_Time;


/* �������� ---------------------------------------------------------*/
uint16_t Sine12bit[POINT_NUM] = {
	2048	, 2460	, 2856	, 3218	, 3532	, 3786	, 3969	, 4072	,
	4093	, 4031	, 3887	, 3668	, 3382	, 3042	,	2661	, 2255	, 
	1841	, 1435	, 1054	, 714		, 428		, 209		, 65		, 3			,
	24		, 127		, 310		, 564		, 878		, 1240	, 1636	, 2048
};


/**
  * @brief  ʹ��DAC��ʱ�ӣ���ʼ��GPIO
  * @param  ��
  * @retval ��
  */
void HAL_DAC_MspInit(DAC_HandleTypeDef*  DAC_InitStructure)
{
  GPIO_InitTypeDef GPIO_InitStruct;
 
  /* DAC����ʱ��ʹ�� */
  DACx_CLK_ENABLE();
  
  /* DMA������ʱ��ʹ�� */
  __HAL_RCC_DMA2_CLK_ENABLE();
  
  /* DACͨ�����Ŷ˿�ʱ��ʹ�� */
  DACx_CHANNEL_GPIO_CLK_ENABLE();
  
  /* DACͨ���������� */
  GPIO_InitStruct.Pin = DACx_CHANNEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DACx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
  
  /* DMA��������ʼ�� */  
  DMA_InitStructure.Instance = DACx_DMAx_CHANNELn;
  DMA_InitStructure.Init.Direction = DMA_MEMORY_TO_PERIPH;
  DMA_InitStructure.Init.PeriphInc = DMA_PINC_DISABLE;
  DMA_InitStructure.Init.MemInc = DMA_MINC_ENABLE;
  DMA_InitStructure.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  DMA_InitStructure.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  DMA_InitStructure.Init.Mode = DMA_CIRCULAR;
  DMA_InitStructure.Init.Priority = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&DMA_InitStructure);
  __HAL_LINKDMA( DAC_InitStructure,DMA_Handle2,DMA_InitStructure); 
}


/**
  * @brief  ʹ��DAC����
  * @param  ��
  * @retval ��
  */
 void DAC_Config(void)
{
  DAC_ChannelConfTypeDef sConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  
   __HAL_RCC_TIM6_CLK_ENABLE();
  
  /* DMA�ж����� */
  HAL_NVIC_SetPriority(DACx_DMAx_CHANNELn_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DACx_DMAx_CHANNELn_IRQn);
  
  /* DAC��ʼ�� */
   DAC_InitStructure.Instance = DACx;
  HAL_DAC_Init(& DAC_InitStructure);

  /* DACͨ��������� */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  HAL_DAC_ConfigChannel(& DAC_InitStructure, &sConfig, DACx_CHANNEL);
  
  /* ��ʼ����ʱ�������ڴ���DAC���� */
  TIM_Time.Instance = TIM6;
  TIM_Time.Init.Prescaler = 71;
  TIM_Time.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_Time.Init.Period = 1000;
  HAL_TIM_Base_Init(&TIM_Time);
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM_Time, &sMasterConfig);
}

