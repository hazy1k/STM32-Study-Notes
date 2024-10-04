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

TIM_HandleTypeDef  TIM_TimeBaseStructure;
TIM_OC_InitTypeDef TIM_OCInitStructure;

__IO uint16_t ChannelPulse = 500;

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
	ADVANCE_OCPWM_GPIO_CLK_ENABLE();
	ADVANCE_OCNPWM_GPIO_CLK_ENABLE();
	ADVANCE_BKIN_GPIO_CLK_ENABLE(); 

	/* ��ʱ���������ų�ʼ�� */															   
	GPIO_InitStructure.Pin = ADVANCE_OCPWM_PIN;	
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;    
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
	HAL_GPIO_Init(ADVANCE_OCPWM_GPIO_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.Pin = ADVANCE_OCNPWM_PIN;	
	HAL_GPIO_Init(ADVANCE_OCNPWM_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = ADVANCE_BKIN_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADVANCE_BKIN_GPIO_PORT, &GPIO_InitStructure);
	
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
	TIM_BreakDeadTimeConfigTypeDef TIM_BDTRInitStructure;
	// ����TIMx_CLK,x[1,8] 
	ADVANCE_TIM_CLK_ENABLE(); 
	/* ���嶨ʱ���ľ����ȷ����ʱ���Ĵ����Ļ���ַ*/
	TIM_TimeBaseStructure.Instance = ADVANCE_TIM;
	/* �ۼ� TIM_Period�������һ�����»����ж�*/		
	//����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.Init.Period = 1000-1;
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=168MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
	TIM_TimeBaseStructure.Init.Prescaler = 72-1;	
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	// ������ʽ
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
	// �ظ�������
	TIM_TimeBaseStructure.Init.RepetitionCounter=0;	
	// ��ʼ����ʱ��TIMx, x[1,8]
	HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);

	/*PWMģʽ����*/
	//����ΪPWMģʽ1
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
	TIM_OCInitStructure.Pulse = ChannelPulse;
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	//��ʼ��ͨ��1���PWM 
	HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure,&TIM_OCInitStructure,TIM_CHANNEL_1);

	/* �Զ����ʹ�ܣ���·������ʱ����������� */
	TIM_BDTRInitStructure.OffStateRunMode = TIM_OSSR_ENABLE;
	TIM_BDTRInitStructure.OffStateIDLEMode = TIM_OSSI_ENABLE;
	TIM_BDTRInitStructure.LockLevel = TIM_LOCKLEVEL_1;
	TIM_BDTRInitStructure.DeadTime = 11;
	TIM_BDTRInitStructure.BreakState = TIM_BREAK_ENABLE;
	TIM_BDTRInitStructure.BreakPolarity = TIM_BREAKPOLARITY_LOW;
	TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&TIM_TimeBaseStructure, &TIM_BDTRInitStructure);

	/* ��ʱ��ͨ��1���PWM */
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,TIM_CHANNEL_1);
	/* ��ʱ��ͨ��1�������PWM */
	HAL_TIMEx_PWMN_Start(&TIM_TimeBaseStructure,TIM_CHANNEL_1);
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1s����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{
	TIMx_GPIO_Config();	

	TIM_Mode_Config();
}

/*********************************************END OF FILE**********************/
