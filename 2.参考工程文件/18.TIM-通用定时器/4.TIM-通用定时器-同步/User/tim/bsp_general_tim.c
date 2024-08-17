#include "./tim/bsp_general_tim.h"

// TIM所使用的GPIO初始化
static void TIMx_GPIO_Config(void) 
{
  // 1.先定义一个GPIO_InitTypeDef结构体变量
	GPIO_InitTypeDef GPIO_InitStructure;

  // 2.使能GPIO时钟
  RCC_APB2PeriphClockCmd(GENERAL_MASTER_GPIO_CLK |GENERAL_SLAVE1_GPIO_CLK | GENERAL_SLAVE11_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); // 使能AFIO时钟
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);   // 完全重映像TIM2_CH3->PB10
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);   // 完全重映像TIM3_CH1->PC6

  // 3.GPIO 配置: PC6(TIM3 CH1) 作为推挽复用输出功能
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(GENERAL_SLAVE1_GPIO_PORT, &GPIO_InitStructure);

  // 4.GPIO 配置: PB6(TIM4 CH1) 作为推挽复用输出功能
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE11_PIN;
  GPIO_Init(GENERAL_SLAVE11_GPIO_PORT, &GPIO_InitStructure);

  // 5.GPIO 配置: PB10(TIM2 CH3) 作为推挽复用输出功能
  GPIO_InitStructure.GPIO_Pin = GENERAL_MASTER_PIN;
  GPIO_Init(GENERAL_MASTER_GPIO_PORT, &GPIO_InitStructure);
}

// TIM模式配置
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// 使能TIM时钟
  RCC_APB1PeriphClockCmd(GENERAL_TIM_MASTER_CLK | GENERAL_TIM_SLAVE1_CLK | GENERAL_TIM_SLAVE11_CLK, ENABLE);
  // 时基配置
  TIM_TimeBaseStructure.TIM_Period = 72;      
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(GENERAL_TIM_MASTER, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE1, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE11, &TIM_TimeBaseStructure);

  // 主定时器设置成PWM1模式
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 36; // 占空比
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(GENERAL_TIM_MASTER, &TIM_OCInitStructure); // 初始化 TIM2 输出通道3

  // 使能主从模式
  TIM_SelectMasterSlaveMode(GENERAL_TIM_MASTER, TIM_MasterSlaveMode_Enable);
	// 输出触发源选项
  TIM_SelectOutputTrigger(GENERAL_TIM_MASTER, TIM_TRGOSource_Update);

	// 从定时器设置成PWM1模式
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1; // 占空比
  TIM_OC1Init(GENERAL_TIM_SLAVE1, &TIM_OCInitStructure);  // 初始化 TIM3 输出通道1
	TIM_OC1Init(GENERAL_TIM_SLAVE11, &TIM_OCInitStructure); // 初始化 TIM4 输出通道1
  // 使能主从模式
  TIM_SelectMasterSlaveMode(GENERAL_TIM_SLAVE1, TIM_MasterSlaveMode_Enable);
  // 输出触发源选项 
  TIM_SelectOutputTrigger(GENERAL_TIM_SLAVE1, TIM_TRGOSource_Update);
	
  TIM_SelectSlaveMode(GENERAL_TIM_SLAVE1, TIM_SlaveMode_Gated); // 从模式配置 : TIM3
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE1, TIM_TS_ITR1);      // 设置输入触发源
	
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);               // 从模式配置 : TIM4
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE11, TIM_TS_ITR2);     // 设置输入触发源
  
  // TIM 计数器溢出中断启用
  TIM_Cmd(GENERAL_TIM_SLAVE1, ENABLE);
  TIM_Cmd(GENERAL_TIM_MASTER, ENABLE);
  TIM_Cmd(GENERAL_TIM_SLAVE11, ENABLE);
}

void TIMx_Configuration(void)
{  
	TIMx_GPIO_Config();	
  TIM_Mode_Config();
}
