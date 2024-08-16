#include "bsp_GeneralTim.h"

// TIMx ͨ��x GPIO ����
static void GENERAL_TIM_GPIO_Config(void) 
{
	// 1.����һ��GPIO_InitTypeDef�ṹ�����
  	GPIO_InitTypeDef GPIO_InitStructure;

  	// 2.����Ƚ�ͨ��1 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE); // ����ͨ��1 GPIOʱ��
  	GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;       // ѡ��ͨ��1����
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // �����������ģʽ
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         // 50MHz
  	GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);     // ��ʼ��GPIO
	
	// ����Ƚ�ͨ��2 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH2_GPIO_CLK, ENABLE);
  	GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH2_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStructure);
	
	// ����Ƚ�ͨ��3 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH3_GPIO_CLK, ENABLE);
  	GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);
	
	// ����Ƚ�ͨ��4 GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH4_GPIO_CLK, ENABLE);
  	GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH4_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GENERAL_TIM_CH4_PORT, &GPIO_InitStructure);
}

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

// TIMģʽ����
static void GENERAL_TIM_Mode_Config(void)
{
  	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);

/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	// �������ڣ���������Ϊ100K
	// 1.����һ��TIM_TimeBaseInitTypeDef�ṹ�����
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 2.�Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;	
	// 3.����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_Prescaler;	
	// 4.ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	// 5.����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		
	// 6.�ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

/*---------------------����ȽϽṹ���ʼ��--------------------*/	
	// 1.ռ�ձ�����
	uint16_t CCR1_Val = 5; // ͨ��1��ռ�ձ�Ϊ50%
	uint16_t CCR2_Val = 4; // ͨ��2��ռ�ձ�Ϊ40%
	uint16_t CCR3_Val = 3; // ͨ��3��ռ�ձ�Ϊ30%
	uint16_t CCR4_Val = 2; // ͨ��4��ռ�ձ�Ϊ20%
	
	// 2.����һ��TIM_OCInitTypeDef�ṹ�����
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 3.����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 4.���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 5.���ͨ����ƽ��������	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// ����Ƚ�ͨ�� 1
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;                // ����Ƚ�ֵ
	TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);          // ��ʼ������Ƚ�ͨ��1
	TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable); // ʹ��Ԥװ��
	
	// ����Ƚ�ͨ�� 2
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
	TIM_OC2Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// ����Ƚ�ͨ�� 3
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
	TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// ����Ƚ�ͨ�� 4
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
	TIM_OC4Init(GENERAL_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// ʹ�ܼ�����
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_Mode_Config();		
}