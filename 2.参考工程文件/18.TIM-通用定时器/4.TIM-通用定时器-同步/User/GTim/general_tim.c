#include "general_tim.h"

// ��ʼ��GPIO�������붨ʱ����ص�����
static void TIM_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  // ʹ���붨ʱ����ص�GPIOʱ��
  RCC_APB2PeriphClockCmd(GTIM_MASTER_GPIO_CLK | GTIM_SLAVE1_GPIO_CLK | GTIM_SLAVE2_GPIO_CLK, ENABLE);
  // ʹ��AFIOʱ�ӣ����ڴ������Ÿ���
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  // ����TIM2��������ӳ�䣬��TIM2_CH3ӳ�䵽PB10����
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); // ��ӳ��TIM2���� TIM2_CH3 -> PB10
  // ����TIM3��������ӳ�䣬��TIM3_CH1ӳ�䵽PC6����
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); // ��ӳ��TIM3���� TIM3_CH1 -> PC6
  // ���ôӶ�ʱ��1��TIM3����GPIO���ţ�PC6
  GPIO_InitStructure.GPIO_Pin = TIM_SLAVE1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ���������ٶ�
  GPIO_Init(TIM_SLAVE1_PORT, &GPIO_InitStructure); // ��ʼ��TIM3_CH1��PC6��
  // ���ôӶ�ʱ��2��TIM4����GPIO���ţ�PB6
  GPIO_InitStructure.GPIO_Pin = TIM_SLAVE2_PIN;
  GPIO_Init(TIM_SLAVE2_PORT, &GPIO_InitStructure); // ��ʼ��TIM4_CH2��PB6��
  // ��������ʱ����TIM2����GPIO���ţ�PB10
  GPIO_InitStructure.GPIO_Pin = TIM_MASTER_PIN;
  GPIO_Init(TIM_MASTER_PORT, &GPIO_InitStructure); // ��ʼ��TIM2_CH3��PB10��
}

// ��ʼ����ʱ���Ĺ���ģʽ�����Ӷ�ʱ��ģʽ��
static void TIM_Mode_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  // ʹ������ʱ���ʹӶ�ʱ����ʱ��
  RCC_APB1PeriphClockCmd(TIM_MASTER_CLK | TIM_SLAVE1_CLK | TIM_SLAVE2_CLK, ENABLE);
  // ��������ʱ��ʱ������
  TIM_TimeBaseStructure.TIM_Period = 72; // ���ö�ʱ�����Զ�����ֵ������Ϊ72��
  TIM_TimeBaseStructure.TIM_Prescaler = 0; // ��ʹ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; // ʱ�ӷ�Ƶϵ��Ϊ0
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // �������ϼ���
  TIM_TimeBaseInit(TIM_MASTER, &TIM_TimeBaseStructure); // ��ʼ������ʱ��
  // ���ôӶ�ʱ��1ʱ����������������Ϊ4��
  TIM_TimeBaseStructure.TIM_Period = 4; // ��������Ϊ4
  TIM_TimeBaseInit(TIM_SLAVE1, &TIM_TimeBaseStructure); // ��ʼ���Ӷ�ʱ��1
  // ���ôӶ�ʱ��2ʱ����������������Ϊ4��
  TIM_TimeBaseStructure.TIM_Period = 4; // ��������Ϊ4
  TIM_TimeBaseInit(TIM_SLAVE2, &TIM_TimeBaseStructure); // ��ʼ���Ӷ�ʱ��2
  // ��������ʱ��ΪPWMģʽ
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ʹ�����
  TIM_OCInitStructure.TIM_Pulse = 5; // ����PWMռ�ձ�Ϊ5
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // �������Ϊ��
  TIM_OC3Init(TIM_MASTER, &TIM_OCInitStructure); // ��������ʱ����PWM���ͨ��3
  // ��������ʱ��Ϊ����ģʽ
  TIM_SelectMasterSlaveMode(TIM_MASTER, TIM_MasterSlaveMode_Enable); // ʹ������ģʽ
  TIM_SelectInputTrigger(TIM_MASTER, TIM_TRGOSource_Update); // �����������ԴΪ�����¼�
  // ���ôӶ�ʱ��1ΪPWMģʽ
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ʹ�����
  TIM_OCInitStructure.TIM_Pulse = 4; // ����PWMռ�ձ�Ϊ4
  TIM_OC1Init(TIM_SLAVE1, &TIM_OCInitStructure); // ���ôӶ�ʱ��1��PWM���ͨ��1
  // ���ôӶ�ʱ��1Ϊ����ģʽ
  TIM_SelectMasterSlaveMode(TIM_SLAVE1, TIM_MasterSlaveMode_Enable);
  // ���ôӶ�ʱ��1Ϊ�ſ�ͬ��ģʽ
  TIM_SelectSlaveMode(TIM_SLAVE1, TIM_SlaveMode_Gated); // ����Ϊ�ſ�ͬ��ģʽ
  TIM_SelectInputTrigger(TIM_SLAVE1, TIM_TS_ITR1); // ���ô���ԴΪ����ʱ���ĸ����¼�
  // ���ôӶ�ʱ��2Ϊ�ſ�ͬ��ģʽ
  TIM_SelectSlaveMode(TIM_SLAVE2, TIM_SlaveMode_Gated); // ����Ϊ�ſ�ͬ��ģʽ
  TIM_SelectInputTrigger(TIM_SLAVE2, TIM_TS_ITR2); // ���ô���ԴΪ��һ�����봥��Դ
  // ��������ʱ���ʹӶ�ʱ��
  TIM_Cmd(TIM_MASTER, ENABLE); // ��������ʱ��
  TIM_Cmd(TIM_SLAVE1, ENABLE); // �����Ӷ�ʱ��1
  TIM_Cmd(TIM_SLAVE2, ENABLE); // �����Ӷ�ʱ��2
}

// ��ʼ����ʱ����ص�Ӳ��
void TIMx_Init(void)
{
  TIM_GPIO_Init(); // ����GPIO��ʼ������
  TIM_Mode_Init(); // ���ö�ʱ��ģʽ��ʼ������
}
