/**
  ******************************************************************************
  * @file    bsp_advance_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �߼����ƶ�ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_advance_tim.h"

TIM_HandleTypeDef TIM_AdvanceHandle;

__IO uint16_t OC_Pulse_num_Channel1 = 25;    /* ͨ��1�ıȽ�ֵ */
__IO uint16_t OC_Pulse_num_Channel2 = 51;    /* ͨ��2�ıȽ�ֵ */
__IO uint16_t OC_Pulse_num_Channel3 = 77;    /* ͨ��3�ıȽ�ֵ */
__IO uint16_t OC_Pulse_num_Channel4 = 103;   /* ͨ��4�ıȽ�ֵ */

/**
  * @brief  �߼����ƶ�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
	/* ������ռ���ȼ��������ȼ� */
	HAL_NVIC_SetPriority(ADVANCE_TIM_IRQn, 0, 0);
	/* �����ж�Դ */
	HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQn);
}

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������ʱ����ص�GPIO����ʱ��*/
	CHANNEL1_OC_GPIO_CLK_ENABLE();
	CHANNEL2_OC_GPIO_CLK_ENABLE();
  CHANNEL3_OC_GPIO_CLK_ENABLE();
  CHANNEL4_OC_GPIO_CLK_ENABLE();

	/* ��ʱ���������ų�ʼ�� */															   
	GPIO_InitStructure.Pin = CHANNEL1_OC_PIN;	
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(CHANNEL1_OC_GPIO_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.Pin = CHANNEL2_OC_PIN;	
	HAL_GPIO_Init(CHANNEL2_OC_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.Pin = CHANNEL3_OC_PIN;	
	HAL_GPIO_Init(CHANNEL3_OC_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.Pin = CHANNEL4_OC_PIN;	
	HAL_GPIO_Init(CHANNEL4_OC_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;
  
	/*ʹ�ܶ�ʱ��*/
	ADVANCE_TIM_CLK_ENABLE();

	TIM_AdvanceHandle.Instance = ADVANCE_TIM;    
	/* �ۼ� TIM_Period�������һ�����»����ж� */		
	//����ʱ����0������TIM_PERIOD����ΪTIM_PERIOD�Σ�Ϊһ����ʱ����
	TIM_AdvanceHandle.Init.Period = TIM_PERIOD; 
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=168MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_PRESCALER-1)
	TIM_AdvanceHandle.Init.Prescaler = TIM_PRESCALER-1;
	/* ������ʽ */
	TIM_AdvanceHandle.Init.CounterMode = TIM_COUNTERMODE_UP;            
	/* ����ʱ�ӷ�Ƶ */	
	TIM_AdvanceHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_AdvanceHandle.Init.RepetitionCounter = 0 ;  		
	/* ��ʼ����ʱ�� */
	HAL_TIM_OC_Init(&TIM_AdvanceHandle);

	/* PWMģʽ����--��������Ϊ����Ƚ�ģʽ */
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE; 
	/* �Ƚ�����ļ���ֵ */
	TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel1;
	/* ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ */
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/* ���û���ͨ������ļ��� */
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW; 
	/* ����ģʽ���� */
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;   
	/* ���е�ƽ */
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	/* ����ͨ������ */
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET; 
	HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_1);
  
  /* ͨ��2���� */
  TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel2;
  HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_2);
    
  /* ͨ��3���� */
  TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel3;
  HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_3);
  
  /* ͨ��4���� */
  TIM_OCInitStructure.Pulse = OC_Pulse_num_Channel4;
  HAL_TIM_OC_ConfigChannel(&TIM_AdvanceHandle, &TIM_OCInitStructure, ADVANCE_TIM_CHANNEL_4);

	/* �����Ƚ������ʹ���ж� */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_1);
  /* �����Ƚ������ʹ���ж� */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_2);
  /* �����Ƚ������ʹ���ж� */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_3);
  /* �����Ƚ������ʹ���ж� */
	HAL_TIM_OC_Start_IT(&TIM_AdvanceHandle, ADVANCE_TIM_CHANNEL_4);
  
	/* ʹ�ܱȽ�ͨ��1 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_1, TIM_CCx_ENABLE);
  /* ʹ�ܱȽ�ͨ��2 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_2, TIM_CCx_ENABLE);
  /* ʹ�ܱȽ�ͨ��3 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_3, TIM_CCx_ENABLE);
  /* ʹ�ܱȽ�ͨ��4 */
	TIM_CCxChannelCmd(ADVANCE_TIM, ADVANCE_TIM_CHANNEL_4, TIM_CCx_ENABLE);
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ������
  * @param  ��
  * @retval ��
  */
void TIMx_AdvanceConfig(void)
{
	TIMx_GPIO_Config();	

	TIM_Mode_Config();
  
  TIMx_NVIC_Configuration();
}


/**
  * @brief  ��ʱ���Ƚ��ж�
  * @param  htim����ʱ�����ָ��
	*	@note 	��
  * @retval ��
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  __IO uint16_t count;
  
  /*��ȡ��ǰ����*/
  count = __HAL_TIM_GET_COUNTER(htim);
  
  /*�жϴ����жϵ����ͨ���������µıȽ���ֵ*/
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_1, count + OC_Pulse_num_Channel1);
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_2, count + OC_Pulse_num_Channel2);
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_3, count + OC_Pulse_num_Channel3);
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
    __HAL_TIM_SET_COMPARE(htim, ADVANCE_TIM_CHANNEL_4, count + OC_Pulse_num_Channel4);
  }
}

/*********************************************END OF FILE**********************/
