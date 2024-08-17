#include "./tim/bsp_general_tim.h"

// TIM��ʹ�õ�GPIO��ʼ��
static void TIMx_GPIO_Config(void) 
{
  // 1.�ȶ���һ��GPIO_InitTypeDef�ṹ�����
	GPIO_InitTypeDef GPIO_InitStructure;

  // 2.ʹ��GPIOʱ��
  RCC_APB2PeriphClockCmd(GENERAL_MASTER_GPIO_CLK |GENERAL_SLAVE1_GPIO_CLK | GENERAL_SLAVE11_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); // ʹ��AFIOʱ��
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);   // ��ȫ��ӳ��TIM2_CH3->PB10
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);   // ��ȫ��ӳ��TIM3_CH1->PC6

  // 3.GPIO ����: PC6(TIM3 CH1) ��Ϊ���츴���������
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(GENERAL_SLAVE1_GPIO_PORT, &GPIO_InitStructure);

  // 4.GPIO ����: PB6(TIM4 CH1) ��Ϊ���츴���������
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE11_PIN;
  GPIO_Init(GENERAL_SLAVE11_GPIO_PORT, &GPIO_InitStructure);

  // 5.GPIO ����: PB10(TIM2 CH3) ��Ϊ���츴���������
  GPIO_InitStructure.GPIO_Pin = GENERAL_MASTER_PIN;
  GPIO_Init(GENERAL_MASTER_GPIO_PORT, &GPIO_InitStructure);
}

// TIMģʽ����
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// ʹ��TIMʱ��
  RCC_APB1PeriphClockCmd(GENERAL_TIM_MASTER_CLK | GENERAL_TIM_SLAVE1_CLK | GENERAL_TIM_SLAVE11_CLK, ENABLE);
  // ʱ������
  TIM_TimeBaseStructure.TIM_Period = 72;      
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(GENERAL_TIM_MASTER, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE1, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE11, &TIM_TimeBaseStructure);

  // ����ʱ�����ó�PWM1ģʽ
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 36; // ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(GENERAL_TIM_MASTER, &TIM_OCInitStructure); // ��ʼ�� TIM2 ���ͨ��3

  // ʹ������ģʽ
  TIM_SelectMasterSlaveMode(GENERAL_TIM_MASTER, TIM_MasterSlaveMode_Enable);
	// �������Դѡ��
  TIM_SelectOutputTrigger(GENERAL_TIM_MASTER, TIM_TRGOSource_Update);

	// �Ӷ�ʱ�����ó�PWM1ģʽ
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1; // ռ�ձ�
  TIM_OC1Init(GENERAL_TIM_SLAVE1, &TIM_OCInitStructure);  // ��ʼ�� TIM3 ���ͨ��1
	TIM_OC1Init(GENERAL_TIM_SLAVE11, &TIM_OCInitStructure); // ��ʼ�� TIM4 ���ͨ��1
  // ʹ������ģʽ
  TIM_SelectMasterSlaveMode(GENERAL_TIM_SLAVE1, TIM_MasterSlaveMode_Enable);
  // �������Դѡ�� 
  TIM_SelectOutputTrigger(GENERAL_TIM_SLAVE1, TIM_TRGOSource_Update);
	
  TIM_SelectSlaveMode(GENERAL_TIM_SLAVE1, TIM_SlaveMode_Gated); // ��ģʽ���� : TIM3
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE1, TIM_TS_ITR1);      // �������봥��Դ
	
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);               // ��ģʽ���� : TIM4
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE11, TIM_TS_ITR2);     // �������봥��Դ
  
  // TIM ����������ж�����
  TIM_Cmd(GENERAL_TIM_SLAVE1, ENABLE);
  TIM_Cmd(GENERAL_TIM_MASTER, ENABLE);
  TIM_Cmd(GENERAL_TIM_SLAVE11, ENABLE);
}

void TIMx_Configuration(void)
{  
	TIMx_GPIO_Config();	
  TIM_Mode_Config();
}
