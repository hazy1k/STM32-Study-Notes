/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ������ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_basic_tim.h"

extern volatile uint32_t time ; // ms ��ʱ����
TIM_HandleTypeDef htimx;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
   /* ������ʱ������ʱ��ʹ�� */
    BASIC_TIM_RCC_CLK_ENABLE();

    /* �����ж����� */
    HAL_NVIC_SetPriority(BASIC_TIM_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BASIC_TIM_IRQ);
 
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
void BASIC_TIMx_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig;

  htimx.Instance = BASIC_TIMx;
  htimx.Init.Prescaler = BASIC_TIMx_PRESCALER;
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx.Init.Period = BASIC_TIMx_PERIOD;
  HAL_TIM_Base_Init(&htimx);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimx, &sMasterConfig);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  time++;
}

/*********************************************END OF FILE**********************/
