/**
  ******************************************************************************
  * @file    bsp_stepper_init.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ���������ʼ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_stepper_init.h"

TIM_HandleTypeDef TIM_StepperHandle;

Stepper_TypeDef step_motor[2] = 
{
  {X_MOTOR_PUL_PIN, X_MOTOR_DIR_PIN, X_MOTOR_EN_PIN, X_MOTOR_PUL_CHANNEL, X_MOTOR_PUL_PORT, X_MOTOR_DIR_GPIO_PORT, X_MOTOR_EN_GPIO_PORT},
  {Y_MOTOR_PUL_PIN, Y_MOTOR_DIR_PIN, Y_MOTOR_EN_PIN, Y_MOTOR_PUL_CHANNEL, Y_MOTOR_PUL_PORT, Y_MOTOR_DIR_GPIO_PORT, Y_MOTOR_EN_GPIO_PORT},
};

/**
  * @brief  �ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
  /* �����ж����� */
  HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
}

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void Stepper_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��ȡ����Ԫ�ظ��� */
  uint8_t member_count = sizeof(step_motor)/sizeof(Stepper_TypeDef);
  
  X_MOTOR_DIR_GPIO_CLK_ENABLE();
  X_MOTOR_EN_GPIO_CLK_ENABLE();
  X_MOTOR_PUL_GPIO_CLK_ENABLE();

  Y_MOTOR_DIR_GPIO_CLK_ENABLE();
  Y_MOTOR_EN_GPIO_CLK_ENABLE();
  Y_MOTOR_PUL_GPIO_CLK_ENABLE();

  for(uint8_t i = 0; i < member_count; i++)
  {
    /*ѡ��Ҫ���Ƶ�GPIO����*/															   
    GPIO_InitStruct.Pin = step_motor[i].dir_pin;	
    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
    GPIO_InitStruct.Pull =GPIO_PULLUP;
    /*������������Ϊ���� */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    /*Motor �������� ��ʼ��*/
    HAL_GPIO_Init(step_motor[i].dir_port, &GPIO_InitStruct);
    MOTOR_DIR(step_motor[i].dir_port, step_motor[i].dir_pin, CW);
    
    /*Motor ʹ������ ��ʼ��*/
    GPIO_InitStruct.Pin = step_motor[i].en_pin;
    HAL_GPIO_Init(step_motor[i].en_port, &GPIO_InitStruct);
    MOTOR_OFFLINE(step_motor[i].en_port, step_motor[i].en_pin, LOW);
    
    /* ��ʱ�����ͨ����������IO��ʼ�� */
    /*�����������*/
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    /*������������ */ 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    /*���ø���*/
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /*ѡ��Ҫ���Ƶ�GPIO����*/	
    GPIO_InitStruct.Pin = step_motor[i].pul_pin;
    /*Motor �������� ��ʼ��*/
    HAL_GPIO_Init(step_motor[i].pul_port, &GPIO_InitStruct);
    MOTOR_PUL(step_motor[i].pul_port, step_motor[i].pul_pin, LOW);
  }
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
static void TIM_PWMOUTPUT_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;

  /* ��ȡ����Ԫ�ظ��� */
  uint8_t member_count = sizeof(step_motor)/sizeof(Stepper_TypeDef);
  
	/*ʹ�ܶ�ʱ��*/
	MOTOR_PUL_CLK_ENABLE();
  
	TIM_StepperHandle.Instance = MOTOR_PUL_TIM;    
	/* �ۼ� TIM_Period�������һ�����»����ж�*/		
	//����ʱ����0������TIM_PERIOD����ΪTIM_PERIOD�Σ�Ϊһ����ʱ����
	TIM_StepperHandle.Init.Period = TIM_PERIOD;
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK = 168MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=28MHz
	TIM_StepperHandle.Init.Prescaler = TIM_PRESCALER-1;

	/*������ʽ*/
	TIM_StepperHandle.Init.CounterMode = TIM_COUNTERMODE_UP;            
	/*����ʱ�ӷ�Ƶ*/	
	TIM_StepperHandle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_StepperHandle.Init.RepetitionCounter = 0;
	/*��ʼ����ʱ��Ϊ����Ƚ�ģʽ*/
  HAL_TIM_Base_Init(&TIM_StepperHandle);

	/*PWMģʽ����--��������ΪPWMģʽ2*/
	TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM2;
	/*�Ƚ�����ļ���ֵ*/
	TIM_OCInitStructure.Pulse = TIM_PERIOD;
	/*����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ*/
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	/*���û���ͨ������ļ���*/
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH; 
	/*����ģʽ����*/
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*���е�ƽ*/
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	/*����ͨ������*/
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  for(uint8_t i = 0; i < member_count; i++)
  {
    /* ��������Ƚ�ͨ�� */
    HAL_TIM_OC_ConfigChannel(&TIM_StepperHandle, &TIM_OCInitStructure, step_motor[i].pul_channel);
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[i].pul_channel, TIM_CCx_DISABLE);
  }
}

/**
  * @brief  ���������ʼ��
  * @param  ��
	*	@note   ��
  * @retval ��
  */
void stepper_Init(void)
{
	/*���IO����*/
	Stepper_GPIO_Config();
	/*��ʱ��PWM�������*/
	TIM_PWMOUTPUT_Config();
	/*�ж�����*/
	TIMx_NVIC_Configuration();
}
