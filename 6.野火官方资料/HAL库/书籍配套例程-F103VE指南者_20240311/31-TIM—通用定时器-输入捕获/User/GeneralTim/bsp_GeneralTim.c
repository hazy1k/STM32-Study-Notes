
#include "bsp_GeneralTim.h"

// ��ʱ�����벶���û��Զ�������ṹ�嶨��
STRUCT_CAPTURE TIM_ICUserValueStructure = {0,0,0,0};

TIM_HandleTypeDef  TIM_TimeBase;

// �������ж����ȼ�����
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;
 
  /* ������ʱ������ʱ��ʹ�� */
  GENERAL_TIM_RCC_CLK_ENABLE();
  
  KEY1_RCC_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = KEY1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KEY1_GPIO, &GPIO_InitStruct);
  
  /* ���ö�ʱ���ж����ȼ���ʹ�� */
  HAL_NVIC_SetPriority(GENERAL_TIM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQn);
  
}

///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)
void GENERAL_TIM_Mode_Config(void)
{
  TIM_ClockConfigTypeDef CLKSource;
  TIM_MasterConfigTypeDef MasterConfig;
  TIM_IC_InitTypeDef ICConfig;
  
   TIM_TimeBase.Instance = GENERAL_TIMx;
  // ����CNT��������ʱ�� = Fck_int/(psc+1)
   TIM_TimeBase.Init.Prescaler = GENERAL_TIM_PRESCALER;
  // ����������ģʽ������Ϊ���ϼ���
   TIM_TimeBase.Init.CounterMode = TIM_COUNTERMODE_UP;
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
   TIM_TimeBase.Init.Period = GENERAL_TIM_PERIOD;
  // ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
   TIM_TimeBase.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(& TIM_TimeBase);

  CLKSource.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(& TIM_TimeBase, &CLKSource);

  MasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  MasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(& TIM_TimeBase, &MasterConfig);
  
  ICConfig.ICPolarity = GENERAL_TIM_STRAT_ICPolarity;
  ICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
  ICConfig.ICPrescaler = TIM_ICPSC_DIV1;
  ICConfig.ICFilter = 0;
  
  HAL_TIM_IC_ConfigChannel(& TIM_TimeBase, &ICConfig, GENERAL_TIM_CHANNELx);
}


/*********************************************END OF FILE**********************/
