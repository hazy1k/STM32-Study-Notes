#include "bsp_GeneralTim.h"

// 定时器输入捕获用户自定义变量结构体定义
TIM_ICUserValueTypeDef TIM_ICUserValueStructure = {0,0,0,0};

// 中断优先级配置
static void GENERAL_TIM_NVIC_Config(void)
{
	// 1.定义一个NVIC_InitTypeDef类型的结构体变量，用于配置NVIC
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
    NVIC_Init(&NVIC_InitStructure); // 7.根据配置结构体初始化NVIC
}

// 输入捕获通道 GPIO 配置
static void GENERAL_TIM_GPIO_Config(void) 
{
	// 输入捕获通道 GPIO 端口初始化
  	GPIO_InitTypeDef GPIO_InitStructure;

  	// 输入捕获通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(GENERAL_TIM_CH1_GPIO_CLK, ENABLE); // 使能GPIO时钟
  	GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;       // 选择GPIO引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     // 设置为浮空输入模式
  	GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);	  // 初始化GPIO
}

// 定时器模式配置
static void GENERAL_TIM_Mode_Config(void)
{
  	// 开启定时器时钟,即内部时钟CK_INT=72M
	GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

/*--------------------时基结构体初始化-------------------------*/	
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_PERIOD;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_PSC;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

	/*--------------------输入捕获结构体初始化-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	// 配置输入捕获的通道，需要根据具体的GPIO来配置
	TIM_ICInitStructure.TIM_Channel = GENERAL_TIM_CHANNEL_x;
	// 输入捕获信号的极性配置
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_TIM_STRAT_ICPolarity;
	// 输入通道和捕获通道的映射关系，有直连和非直连两种
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// 输入的需要被捕获的信号的分频系数
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// 输入的需要被捕获的信号的滤波系数
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// 定时器输入捕获初始化
	TIM_ICInit(GENERAL_TIM, &TIM_ICInitStructure);
	
	// 清除更新和捕获中断标志位
  	TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update|GENERAL_TIM_IT_CCx);	
  	// 开启更新和捕获中断  
	TIM_ITConfig(GENERAL_TIM, TIM_IT_Update | GENERAL_TIM_IT_CCx, ENABLE);
	
	// 使能计数器
	TIM_Cmd(GENERAL_TIM, ENABLE);
}

void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_GPIO_Config();
	GENERAL_TIM_NVIC_Config();
	GENERAL_TIM_Mode_Config();		
}
