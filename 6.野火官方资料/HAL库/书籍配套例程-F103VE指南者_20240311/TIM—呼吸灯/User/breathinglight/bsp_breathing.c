/**
  ******************************************************************************
  * @file    bsp_breathing.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 �Ե������� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./breathinglight/bsp_breathing.h"

TIM_HandleTypeDef BRE_TIM;

/* LED���ȵȼ� PWM��,ָ������ ���˱�ʹ�ù���Ŀ¼�µ�python�ű�index_wave.py����*/
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
	
};

//����PWM���ж��ٸ�Ԫ��
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]); 



 /**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct;
    
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
  BRE_TIM_GPIO_RCC_CLK_ENABLE();

  /* ��ʱ��ͨ��3��������IO��ʼ�� */
  GPIO_InitStruct.Pin = BRE_TIM_CH3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BRE_TIM_CH3_PORT, &GPIO_InitStruct);
}

/**
  * @brief  ������ʱ��Ӳ����ʼ������
  * @param  ��
  * @retval ��
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	 /* ������ʱ������ʱ��ʹ�� */
    BRE_TIM_RCC_CLK_ENABLE();
    
    /* ���ö�ʱ���ж����ȼ���ʹ�� */
    HAL_NVIC_SetPriority(BRE_TIM_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(BRE_TIM_IRQn);
}
/**
  * @brief  ����TIM�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */
void TIMx_Mode_Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  
  BRE_TIM.Instance = BRE_TIMx;
  BRE_TIM.Init.Prescaler = BRE_TIM_PRESCALER;
  BRE_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
  BRE_TIM.Init.Period = BRE_TIM_PERIOD;
  BRE_TIM.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&BRE_TIM);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&BRE_TIM, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&BRE_TIM, &sMasterConfig);
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.Pulse = indexWave[0];
  HAL_TIM_PWM_ConfigChannel(&BRE_TIM, &sConfigOC, TIM_CHANNEL_3);

  HAL_TIM_MspPostInit(&BRE_TIM);
}



/*********************************************END OF FILE**********************/
