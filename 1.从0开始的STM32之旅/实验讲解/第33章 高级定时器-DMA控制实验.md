# 第三十四章 高级定时器-DMA控制实验

## 1. 硬件设计

TIM和DMA均输入内部资源，无需硬件设计，还使用了按键，已经分析过了

## 2. 软件设计

- 高级定时器及通道宏定义

```c
// 定时器
#define ADVANCE_TIM           		  TIM1
#define ADVANCE_TIM_CLK       		  RCC_APB2Periph_TIM1                      

// TIM1 输出比较通道
#define ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define ADVANCE_TIM_CH1_PORT          GPIOA
#define ADVANCE_TIM_CH1_PIN           GPIO_Pin_8
```

- DMA相关参数宏定义

```c
// DMA配置  
#define TIM_DMA_ADDRESS               TIM1_BASE+0x34
#define TIM_DMA_BUFSIZE               3
#define TIM_DMA_CLK                   RCC_AHBPeriph_DMA1
#define TIM_DMA_STREAM                DMA1_Channel2
```

- 高级定时器初始化

```c
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
```

- DMA初始化

```c
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
```

- 高级定时器模式配置

```c
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
```

- 主函数

```c
#include "stm32f10x.h"
#include "./tim/bsp_advance_tim.h"
#include "./key/bsp_key.h" 

extern uint16_t aSRC_Buffer[3];

int main(void)
{	
	/* 初始化按键GPIO */
  Key_GPIO_Config();
	
  /* 初始化高级控制定时器，设置PWM模式 */
	TIMx_Configuration();
  
  while(1)
  {       
		/* 扫描KEY1 */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			aSRC_Buffer[0]=1000;
			aSRC_Buffer[1]=1000;
			aSRC_Buffer[2]=100;
		}   
    /* 扫描KEY2 */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			aSRC_Buffer[0]=300;
			aSRC_Buffer[1]=300;
			aSRC_Buffer[2]=100;
		}   
  }
}

```


