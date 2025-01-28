#include "AdvanceTim.h"

static void ATIM_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = ATIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void ATIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
}

static void ATIM_Mode_Init(void)
{
	ATIM_APBxClock(ATIM_CLK, ENABLE); // 72 MHz
	// ʱ���ṹ��ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 999
	TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 71
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
	// ���벶��ṹ��ʼ��
	// ����PWMʱ��Ҫ��������Ĵ�����һ�������ڣ�һ����ռ�ձ�
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = ATIM_IC1PWM_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // ����������
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0; // ���˲�
	TIM_PWMIConfig(ATIMx, &TIM_ICInitStructure);
	/* ��������PWM����ģʽʱ��ֻ��Ҫ���ô����źŵ���һ·���ɣ����ڲ�������
	   ����һ·����������ռ�ձȣ�����Ӳ���Դ����ã����Բ���Ҫ���á�*/
	// ����ͨ��IC2����
	/*TIM_ICInitStructure.TIM_Channel = ATIM_IC1PWM_CHANNEL;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // �����½���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0; 
	TIM_PWMIConfig(ATIMx, &TIM_ICInitStructure);*/
	// ѡ�����벶��Ĵ����ź�
	TIM_SelectInputTrigger(ATIMx, TIM_TS_TI1FP1); // ѡ��TI1FP1������Ϊ�����ź�
	// ѡ���ģʽ����λģʽ
	// PWM����ģʽʱ����ģʽ�����ڸ�λģʽ��������ʼʱ��������CNT��λ
	TIM_SelectSlaveMode(ATIMx, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(ATIMx, TIM_MasterSlaveMode_Enable); // ʹ������ģʽ
	TIM_ITConfig(ATIMx, TIM_IT_CC1, ENABLE);
	TIM_ClearITPendingBit(ATIMx, TIM_IT_CC1);
	TIM_Cmd(ATIMx, ENABLE);
}

void ATIMx_Init(void)
{
	ATIM_NVIC_Init();
	ATIM_GPIO_Init();
	ATIM_Mode_Init();
}
