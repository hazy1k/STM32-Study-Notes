#include "bsp_AdvanceTim.h" 

// 中断优先级配置
static void ADVANCE_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQ ;	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// 高级定时器模式配置
static void ADVANCE_TIM_Mode_Config(void)
{
    // 1.定义一个定时器结构体
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		// 2.开启定时器时钟,即内部时钟CK_INT=72M
    ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK, ENABLE);	
		// 3.自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_Period;
	  // 4.时钟预分频数
    TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_Prescaler;	
		// 5.时钟分频因子 ，没用到不用管
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
		// 6.计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		
		// 7.重复计数器的值，没用到不用管
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	  // 8.初始化定时器
    TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
	
		// 9.清除计数器中断标志位
    TIM_ClearFlag(ADVANCE_TIM, TIM_FLAG_Update);
	  
		// 10.开启计数器中断
    TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update, ENABLE);
		
		// 11.使能计数器
    TIM_Cmd(ADVANCE_TIM, ENABLE);
}

void ADVANCE_TIM_Init(void)
{
	ADVANCE_TIM_NVIC_Config();
	ADVANCE_TIM_Mode_Config();		
}