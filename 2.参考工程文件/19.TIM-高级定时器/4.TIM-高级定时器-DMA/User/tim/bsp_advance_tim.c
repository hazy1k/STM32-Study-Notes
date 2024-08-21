#include "./tim/bsp_advance_tim.h"

uint16_t aSRC_Buffer[TIM_DMA_BUFSIZE] = {700,700,100};

// 高级定时器GPIO初始化
static void TIMx_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
	
	/*开启定时器相关的GPIO外设时钟*/
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1_GPIO_CLK, ENABLE);
	
	/* 定时器功能引脚初始化 */															   
	GPIO_InitStructure.GPIO_Pin = ADVANCE_TIM_CH1_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(ADVANCE_TIM_CH1_PORT, &GPIO_InitStructure);

	// DMA初始化
	// DMA时钟使能
  RCC_AHBPeriphClockCmd(TIM_DMA_CLK, ENABLE);	
	DMA_DeInit(TIM_DMA_STREAM);

  // 设置DMA源地址：TIM的CCR寄存器地址
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(TIM_DMA_ADDRESS) ;
	// 内存地址(要传输的变量的指针)
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)aSRC_Buffer;
	// 方向：从内存到外设	
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	// 传输数目	
  DMA_InitStructure.DMA_BufferSize = TIM_DMA_BUFSIZE;
	// 外设地址不增	
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 内存地址自增
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 外设数据单位	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 内存数据单位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	// DMA模式，一次或者循环模式
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// 优先级：中
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	// 禁止内存到内存的传输
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable ;
	// 配置DMA通道	
  DMA_Init(TIM_DMA_STREAM, &DMA_InitStructure);
}

static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// 开启TIMx_CLK,x[1,8] 
  RCC_APB2PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到1023，即为1024次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 1024-1;
	// 高级控制定时器时钟源TIMxCLK = HCLK=72MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;		
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  // 重复计数器
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
	
  /*PWM模式配置*/
	//配置为PWM模式1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 0;
	// 输出通道电平极性配置	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  //使能通道1
  TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
		
	/* Enable preload feature */
  TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);
	
	// 使能定时器
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	
	/* DMA enable*/
  DMA_Cmd(TIM_DMA_STREAM, ENABLE);
	
	/* TIM Update DMA Request enable */
  TIM_DMACmd(ADVANCE_TIM, TIM_DMA_CC1, ENABLE);	

	/* 主动输出使能 */
  TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
}

void TIMx_Configuration(void)
{  
	TIMx_GPIO_Config();
	
  TIM_Mode_Config();
}
