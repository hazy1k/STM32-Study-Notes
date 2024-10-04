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

TIM_HandleTypeDef  TIM_PWMOUTPUT_Handle;
TIM_HandleTypeDef  TIM_PWMINPUT_Handle;
__IO uint16_t IC2Value = 0;
__IO uint16_t IC1Value = 0;
__IO float DutyCycle = 0;
__IO float Frequency = 0;

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_PWMGPIO_Cong(void) 
{
 GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
  GENERAL_OCPWM_GPIO_CLK_ENABLE();
  
  /* ��ʱ��ͨ��1��������IO��ʼ�� */
	/*�����������*/
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	/*������������ */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*ѡ��Ҫ���Ƶ�GPIO����*/	
	GPIO_InitStruct.Pin = GENERAL_OCPWM_PIN;
  HAL_GPIO_Init(GENERAL_OCPWM_GPIO_PORT, &GPIO_InitStruct);
}



/**
  * @brief  ����TIM�������벶��ʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /*������ʱ����ص�GPIO����ʱ��*/
  ADVANCE_ICPWM_GPIO_CLK_ENABLE(); 
  /* �߼���ʱ�����벶������ */
  GPIO_InitStructure.Pin = ADVANCE_ICPWM_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;    
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADVANCE_ICPWM_GPIO_PORT, &GPIO_InitStructure);	
}
 /**
  * @brief  �߼����ƶ�ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
  //������ռ���ȼ��������ȼ�
  HAL_NVIC_SetPriority(ADVANCE_TIM_IRQn, 0, 3);
  // �����ж���Դ
  HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQn);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode       TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
TIM_HandleTypeDef  TIM_TimeBaseStructure;
static void TIM_PWMOUTPUT_Config(void)
{
  TIM_OC_InitTypeDef  TIM_OCInitStructure;  
	
  /*ʹ�ܶ�ʱ��*/
  GENERAL_TIM_CLK_ENABLE();
	
  TIM_TimeBaseStructure.Instance = GENERAL_TIM;
  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������5000����Ϊ5000�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.Init.Period = 5000-1;
	//��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
	//				PCLK1 = HCLK / 2 
	//				=> TIMxCLK=HCLK/2=SystemCoreClock/2*2=72MHz
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10KHz
  TIM_TimeBaseStructure.Init.Prescaler = 720-1;	
	/*������ʽ*/
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
	/*����ʱ�ӷ�Ƶ*/
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	/*��ʼ����ʱ��*/
  HAL_TIM_PWM_Init(&TIM_TimeBaseStructure);
  
	/*PWMģʽ����*/
  TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;//����ΪPWMģʽ1
  TIM_OCInitStructure.Pulse = 2500;//Ĭ��ռ�ձ�Ϊ50%
  TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	/*����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ*/
  TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;	
	
	/*����PWMͨ��*/
  HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure, &TIM_OCInitStructure, TIM_CHANNEL_1);
	/*��ʼ���PWM*/
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,TIM_CHANNEL_1);
	
	/*��������*/
  TIM_OCInitStructure.Pulse = 2500;//Ĭ��ռ�ձ�Ϊ50%
	/*����PWMͨ��*/
  HAL_TIM_PWM_ConfigChannel(&TIM_TimeBaseStructure, &TIM_OCInitStructure, TIM_CHANNEL_2);
	/*��ʼ���PWM*/
	HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,TIM_CHANNEL_2);
	

}
    
static void TIM_PWMINPUT_Config(void)
{ 
  TIM_IC_InitTypeDef    TIM_ICInitStructure;
  TIM_SlaveConfigTypeDef  TIM_SlaveConfigStructure;
//  TIM_MasterConfigTypeDef TIM_MasterConfigStructure;
  
  // ����TIMx_CLK,x[1,8] 
  ADVANCE_TIM_CLK_ENABLE(); 
  /* ���嶨ʱ���ľ����ȷ����ʱ���Ĵ����Ļ���ַ*/
  TIM_PWMINPUT_Handle.Instance = ADVANCE_TIM;
  TIM_PWMINPUT_Handle.Init.Period = 0xFFFF;   
  // �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=72MHz 
  // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_PWMINPUT_Handle.Init.Prescaler = 72-1; 
  // ����ʱ�ӷ�Ƶ
  TIM_PWMINPUT_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  // ������ʽ
  TIM_PWMINPUT_Handle.Init.CounterMode=TIM_COUNTERMODE_UP;  
  // ��ʼ����ʱ��TIMx, x[1,8]
  HAL_TIM_IC_Init(&TIM_PWMINPUT_Handle);
  
  /* IC1���������ش��� TI1FP1 */
  TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;
  TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.ICFilter = 0x0;
  HAL_TIM_IC_ConfigChannel(&TIM_PWMINPUT_Handle,&TIM_ICInitStructure,ADVANCE_IC1PWM_CHANNEL);

  /* IC2�����½��ش��� TI1FP2 */  
  TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_FALLING;
  TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.ICFilter = 0x0;
  HAL_TIM_IC_ConfigChannel(&TIM_PWMINPUT_Handle,&TIM_ICInitStructure,ADVANCE_IC2PWM_CHANNEL);
  
  /* ѡ���ģʽ: ��λģʽ */
  TIM_SlaveConfigStructure.SlaveMode = TIM_SLAVEMODE_RESET;
  /* ѡ��ʱ�����봥��: TI1FP1 */
  TIM_SlaveConfigStructure.InputTrigger = TIM_TS_TI1FP1;
  HAL_TIM_SlaveConfigSynchro(&TIM_PWMINPUT_Handle,&TIM_SlaveConfigStructure);
  
  /* ʹ�ܲ���/�Ƚ�2�ж����� */
  HAL_TIM_IC_Start_IT(&TIM_PWMINPUT_Handle,TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&TIM_PWMINPUT_Handle,TIM_CHANNEL_2);
}
/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{
	TIMx_PWMGPIO_Cong();
	
  TIMx_GPIO_Config();
  
  TIMx_NVIC_Configuration();  
  
  TIM_PWMOUTPUT_Config();
  
  TIM_PWMINPUT_Config();
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  htim : hadc handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    /* ��ȡ���벶��ֵ */
    IC1Value = HAL_TIM_ReadCapturedValue(&TIM_PWMINPUT_Handle,ADVANCE_IC1PWM_CHANNEL);
    IC2Value = HAL_TIM_ReadCapturedValue(&TIM_PWMINPUT_Handle,ADVANCE_IC2PWM_CHANNEL);  
    if (IC1Value != 0)
    {
      /* ռ�ձȼ��� */
      DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);
      /* Ƶ�ʼ��� */
      Frequency = 72000000/72/(float)(IC1Value+1);
    }
    else
    {
      DutyCycle = 0;
      Frequency = 0;
    }

  }
}
/*********************************************END OF FILE**********************/
