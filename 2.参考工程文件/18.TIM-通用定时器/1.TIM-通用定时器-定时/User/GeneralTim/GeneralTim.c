#include "GeneralTim.h"

void TIMx_Init(void)
{
  /* NVIC�ж����ȼ����� */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = TIM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* TIM MODE����*/
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = TIMx_Period; // ��ʱ������999
  TIM_TimeBaseStructure.TIM_Prescaler = TIMx_Prescaler; // ��ʱ����Ƶϵ��71
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ʱ�ӷ�Ƶϵ��1
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIMx, TIM_FLAG_Update);
  TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE); // ʹ�ܼ������ж�
  TIM_Cmd(TIMx, ENABLE);
}
