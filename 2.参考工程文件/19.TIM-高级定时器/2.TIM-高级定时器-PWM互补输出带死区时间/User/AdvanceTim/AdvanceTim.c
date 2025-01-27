#include "AdvanceTim.h"

static void ATIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// ����Ƚ�ͨ��GPIO��ʼ��
	RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
	// ����Ƚ�ͨ���Ļ���ͨ��GPIO��ʼ��
	RCC_APB2PeriphClockCmd(ATIM_CH1N_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_CH1N_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATIM_CH1N_GPIO_PORT, &GPIO_InitStructure);
	// ����Ƚ�ͨ����ɲ��ͨ��GPIO��ʼ��
	RCC_APB2PeriphClockCmd(ATIM_BKIN_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ATIM_BKIN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ATIM_BKIN_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(ATIM_BKIN_GPIO_PORT, ATIM_BKIN_GPIO_PIN); // BKINĬ���ǵ͵�ƽ״̬  
}

static void ATIM_Mode_Init(void)
{
	ATIM_APBxClock(ATIM_CLK, ENABLE); // 72MHz
	// ʱ���ṹ��ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = ATIM_Period; // 8-1
	TIM_TimeBaseStructure.TIM_Prescaler = ATIM_Prescaler; // 9-1
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
	// ����ȽϽṹ��ʼ��
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ����ΪPWM1ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ���ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // �������ʹ��
	TIM_OCInitStructure.TIM_Pulse = ATIM_Pulse; // ռ�ձ�4 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; // ����ͨ��������Ը�
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set; // ���ͨ������״̬��λ
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; // ����ͨ������״̬��λ
	TIM_OC1Init(ATIMx, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(ATIMx, TIM_OCPreload_Enable); // Ԥװ��ʹ��
	// �����ṹ��ʼ��
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable; // ������װ��
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable; // ����ֹͣ
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; // ����
	TIM_BDTRInitStructure.TIM_DeadTime = 11; // ����ʱ��11
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable; // ��������
	// ��BKIN���ż�⵽�ߵ�ƽ��ʱ������Ƚ��ź���������ֹ��ֱ����һ�������¼�������
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High; // �������Ը�
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable; // �����Զ����
	TIM_BDTRConfig(ATIMx, &TIM_BDTRInitStructure);
	TIM_Cmd(ATIMx, ENABLE);
	TIM_CtrlPWMOutputs(ATIMx, ENABLE); // ʹ��PWM���
}

void ATIMx_Config(void)
{
	ATIM_GPIO_Init();
	ATIM_Mode_Init();
}
