/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ͨ�ö�ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_general_tim.h"

void TIM_SetTIMxCompare(TIM_TypeDef *TIMx,uint32_t channel,uint32_t compare);
void TIM_SetPWM_period(TIM_TypeDef* TIMx,uint32_t TIM_period);

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
 GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
	__HAL_RCC_GPIOA_CLK_ENABLE();
  
  /* ��ʱ��ͨ��1��������IO��ʼ�� */
	/*�����������*/
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	/*������������ */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*ѡ��Ҫ���Ƶ�GPIO����*/	
	GPIO_InitStruct.Pin = GENERAL_TIM_CH1_PIN;
  HAL_GPIO_Init(GENERAL_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GENERAL_TIM_CH2_PIN;	
  HAL_GPIO_Init(GENERAL_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);

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
TIM_HandleTypeDef  TIM_TimeBaseStructure;
static void TIM_PWMOUTPUT_Config(void)
{
  TIM_OC_InitTypeDef  TIM_OCInitStructure;  
	int tim_per=5000;//��ʱ������
	
  /*ʹ�ܶ�ʱ��*/
  GENERAL_TIM_CLK_ENABLE();
	
  TIM_TimeBaseStructure.Instance = GENERAL_TIM;
  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������5000����Ϊ5000�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.Init.Period = tim_per;
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
	//				PCLK1 = HCLK / 2 
	//				=> TIMxCLK=HCLK/2=SystemCoreClock/2*2=72MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10KHz
  TIM_TimeBaseStructure.Init.Prescaler = 7200-1;	
	/*������ʽ*/
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
	/*����ʱ�ӷ�Ƶ*/
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	/*��ʼ����ʱ��*/
  HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);
  
	/*PWMģʽ����*/
  TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;//����ΪPWMģʽ1
  TIM_OCInitStructure.Pulse = tim_per/2;//Ĭ��ռ�ձ�Ϊ50%
  TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ*/
  TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;	
	
	/*����PWMͨ��*/
  HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure, &TIM_OCInitStructure, TIM_CHANNEL_1);
	/*��ʼ���PWM*/
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,TIM_CHANNEL_1);
	
	/*��������*/
  TIM_OCInitStructure.Pulse = tim_per/2;//Ĭ��ռ�ձ�Ϊ50%
	/*����PWMͨ��*/
  HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure, &TIM_OCInitStructure, TIM_CHANNEL_2);
	/*��ʼ���PWM*/
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,TIM_CHANNEL_2);
	

}

/**
  * @brief  ����TIMͨ����ռ�ձ�
	* @param  channel		ͨ��	��1,2,3,4��
	* @param  compare		ռ�ձ�
	*	@note 	��
  * @retval ��
  */
void TIM2_SetPWM_pulse(int channel,int compare)
{
		switch(channel)
	{
		case 1:  	__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,TIM_CHANNEL_1,compare);break;
		case 2:	  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,TIM_CHANNEL_2,compare);break;
		case 3:	  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,TIM_CHANNEL_3,compare);break;
		case 4:	  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,TIM_CHANNEL_4,compare);break;
	}
}


/**
  * @brief  ��ʼ������ͨ�ö�ʱ��
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{
	TIMx_GPIO_Config();
  
  TIM_PWMOUTPUT_Config();
}




/*********************************************END OF FILE**********************/
