#include "bsp_GeneralTim.h" 

// NVIC�ж����ȼ�����
static void GENERAL_TIM_NVIC_Config(void)
{
    // 1.����NVIC_InitTypeDef�ṹ��
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 2.�����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 3.�����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQ ;	
		// 4.���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 5.������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    // 6.ʹ���ж�	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // 7.��ʼ��NVIC
    NVIC_Init(&NVIC_InitStructure);
}

// ��ʱ��ģʽ����
static void GENERAL_TIM_Mode_Config(void)
{
    // 1.����TIM_TimeBaseInitTypeDef�ṹ��
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		// 2.������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);	
		// 3.�Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;
	  // 4.ʱ��Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
		// 5.ʱ�ӷ�Ƶ���� ��û�õ����ù�
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
		// 6.����������ģʽ������Ϊ���ϼ���
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		
		// 7.�ظ���������ֵ��û�õ����ù�
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	  // 8.��ʼ����ʱ��
    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
		// 9.����������жϱ�־λ
    TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
	  
		// 10.�����������ж�
    TIM_ITConfig(GENERAL_TIM, TIM_IT_Update, ENABLE);
		
		// 11.ʹ�ܼ�����
    TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_NVIC_Config();
	GENERAL_TIM_Mode_Config();		
}