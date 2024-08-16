#include "bsp_GeneralTim.h" 

// NVIC中断优先级配置
static void GENERAL_TIM_NVIC_Config(void)
{
    // 1.定义NVIC_InitTypeDef结构体
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 2.设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 3.设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQ ;	
		// 4.设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 5.设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    // 6.使能中断	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // 7.初始化NVIC
    NVIC_Init(&NVIC_InitStructure);
}

// 定时器模式配置
static void GENERAL_TIM_Mode_Config(void)
{
    // 1.定义TIM_TimeBaseInitTypeDef结构体
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		// 2.开启定时器时钟,即内部时钟CK_INT=72M
    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);	
		// 3.自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;
	  // 4.时钟预分频数
    TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_Prescaler;	
		// 5.时钟分频因子 ，没用到不用管
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
		// 6.计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		
		// 7.重复计数器的值，没用到不用管
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	  // 8.初始化定时器
    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
		// 9.清除计数器中断标志位
    TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
	  
		// 10.开启计数器中断
    TIM_ITConfig(GENERAL_TIM, TIM_IT_Update, ENABLE);
		
		// 11.使能计数器
    TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_NVIC_Config();
	GENERAL_TIM_Mode_Config();		
}