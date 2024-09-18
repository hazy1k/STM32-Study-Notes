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

TIM_HandleTypeDef TIM_TimeBaseStructure;

 /**
  * @brief  ʱ�����ж�����
  * @param  ��
  * @retval ��
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{  
    /* ������ʱ������ʱ��ʹ�� */
    ADVANCED_TIM_RCC_CLK_ENABLE();

    /* �����ж����� */
    HAL_NVIC_SetPriority(ADVANCE_TIM_IRQn , 1, 0);
    HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQn );  
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
 void TIM_Mode_Config(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  
  TIM_TimeBaseStructure.Instance = ADVANCED_TIMx;
  
   // ʱ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.Init.Prescaler = ADVANCED_TIM_PRESCALER;
  // ����������ģʽ������Ϊ���ϼ���
  TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
  // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
  TIM_TimeBaseStructure.Init.Period = ADVANCED_TIM_PERIOD;
  // ʱ�ӷ�Ƶ���� ��û�õ����ù�
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  // �ظ���������ֵ��û�õ����ù�
  TIM_TimeBaseStructure.Init.RepetitionCounter = ADVANCED_TIM_REPETITIONCOUNTER;
  // ��ʼ����ʱ��
  HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  
  HAL_TIM_ConfigClockSource(&TIM_TimeBaseStructure, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  HAL_TIMEx_MasterConfigSynchronization(&TIM_TimeBaseStructure, &sMasterConfig);
  
  /* ���ж�ģʽ��������ʱ�� */
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);
}



/*********************************************END OF FILE**********************/
