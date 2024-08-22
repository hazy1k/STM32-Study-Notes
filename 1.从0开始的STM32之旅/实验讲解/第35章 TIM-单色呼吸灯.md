# 第三十五章 TIM-单色呼吸灯

## 1. 硬件设计

使用定时器输出控制LED灯呈呼吸效果，无其他硬件设计

## 2. 软件设计

- 定时器参数宏定义

```c
#define   BRE_TIMx                     TIM3
#define   BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd
#define   BRE_TIM_CLK                  RCC_APB1Periph_TIM3
#define   BRE_TIM_GPIO_APBxClock_FUN   RCC_APB2PeriphClockCmd
#define   BRE_TIM_GPIO_CLK             (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
```

- 通道宏定义

```c
//红灯的引脚需要重映射
#define   BRE_GPIO_REMAP_FUN()		    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				
#define  BRE_TIM_LED_PORT               GPIOB
#define  BRE_TIM_LED_PIN                GPIO_Pin_5
#define  BRE_TIM_OCxInit                TIM_OC2Init          // 通道选择，1~4
#define  BRE_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig 
#define  BRE_CCRx                       CCR2
#define   BRE_TIMx_IRQn                 TIM3_IRQn              //中断
#define   BRE_TIMx_IRQHandler           TIM3_IRQHandler
```

- PWM表

```c
// LED亮度等级 PWM表,指数曲线 ，此表使用工程目录下的python脚本index_wave.py生成
uint16_t indexWave[] = {
1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
15, 17, 19, 22, 25, 28, 32, 36,
41, 47, 53, 61, 69, 79, 89, 102,
116, 131, 149, 170, 193, 219, 250,
284, 323, 367, 417, 474, 539, 613,
697, 792, 901, 1024, 1024, 901, 792,
697, 613, 539, 474, 417, 367, 323,
284, 250, 219, 193, 170, 149, 131, 
116, 102, 89, 79, 69, 61, 53, 47, 41,
36, 32, 28, 25, 22, 19, 17, 15, 13, 
11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
2, 2, 2, 2, 1, 1, 1, 1
};

//计算PWM表有多少个元素
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]); 
```

- TIM GPIO初始化

```c
// 配置TIM复用输出PWM时用到的I/O
static void TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(BRE_TIM_GPIO_CLK, ENABLE); // 使能GPIO时钟 
  BRE_TIM_GPIO_APBxClock_FUN(BRE_TIM_GPIO_CLK, ENABLE ); // 使能GPIO时钟
		
	BRE_GPIO_REMAP_FUN(); // 使能GPIO映射

  // 配置呼吸灯用到的引脚
  GPIO_InitStructure.GPIO_Pin =  BRE_TIM_LED_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( BRE_TIM_LED_PORT, &GPIO_InitStructure );
}
```

- NVIC初始化

```c
// 配置嵌套向量中断控制器NVIC
static void NVIC_Config_PWM(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 设置中断优先级组1
  
  // 配置TIM3_IRQ中断为中断源
  NVIC_InitStructure.NVIC_IRQChannel = BRE_TIMx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
```

- TIM输出PWM模式配置

```c
static void TIMx_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
  // 设置TIM3CLK 时钟
  BRE_TIM_APBxClock_FUN(BRE_TIM_CLK, ENABLE); 

  /* 基本定时器配置 */		  
  TIM_TimeBaseStructure.TIM_Period = (1024-1);;       		  // 当定时器从0计数到 TIM_Period+1 ，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = (200-1);	    	  // 设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	  // 设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseInit(BRE_TIMx, &TIM_TimeBaseStructure);

  /* PWM模式配置 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    		// 配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	// 使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;				 		    // 设置初始PWM脉冲宽度为0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	// 当定时器计数值小于CCR1_Val时为低电平

  BRE_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	 		// 使能通道
  BRE_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );	// 使能预装载	
  TIM_ARRPreloadConfig(BRE_TIMx, ENABLE);			 			// 使能TIM重载寄存器ARR
  TIM_Cmd(BRE_TIMx, ENABLE);                   					// 使能定时器	
  TIM_ITConfig(BRE_TIMx, TIM_IT_Update, ENABLE);			    // 使能update中断
  NVIC_Config_PWM();										    // 配置中断优先级		
}
```


