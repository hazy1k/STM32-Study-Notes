# 第三十四章 TIM-全彩LED灯

## 1. 硬件设计

该实验主要就是用TIM产生三个通道，一个通道对应一个彩灯。

TIM无需硬件设计，led我们也早已分析过了

## 2. 软件设计

- 定时器参数宏定义

```c
// 定时器配置
#define   COLOR_TIMx                    TIM3                                        // 选择使用的定时器
#define   COLOR_TIM_APBxClock_FUN       RCC_APB1PeriphClockCmd                      // 使能定时器时钟
#define   COLOR_TIM_CLK                 RCC_APB1Periph_TIM3                         // 定时器时钟
#define   COLOR_TIM_GPIO_CLK            (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)  // 定时器GPIO时钟
```

- 三种灯及通道宏定义

```c
//红灯的引脚需要重映射
#define   COLOR_GPIO_REMAP_FUN()		GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				
// 红灯
#define  COLOR_RED_TIM_LED_PORT               GPIOB
#define  COLOR_RED_TIM_LED_PIN                GPIO_Pin_5
#define  COLOR_RED_TIM_OCxInit                TIM_OC2Init          // 通道初始化函数
#define  COLOR_RED_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_RED_CCRx ，可访问该通道的比较寄存器
#define  COLOR_RED_CCRx                       CCR2		

// 绿灯
#define  COLOR_GREEN_TIM_LED_PORT             GPIOB
#define  COLOR_GREEN_TIM_LED_PIN              GPIO_Pin_0
#define  COLOR_GREEN_TIM_OCxInit              TIM_OC3Init          // 通道初始化函数
#define  COLOR_GREEN_TIM_OCxPreloadConfig     TIM_OC3PreloadConfig // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_GREEN_CCRx ，可访问该通道的比较寄存器
#define  COLOR_GREEN_CCRx                     CCR3

// 蓝灯
#define   COLOR_BLUE_TIM_LED_PORT             GPIOB
#define   COLOR_BLUE_TIM_LED_PIN              GPIO_Pin_1
#define   COLOR_BLUE_TIM_OCxInit              TIM_OC4Init          // 通道初始化函数
#define   COLOR_BLUE_TIM_OCxPreloadConfig     TIM_OC4PreloadConfig  // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_BLUE_CCRx ，可访问该通道的比较寄存器
#define   COLOR_BLUE_CCRx                      CCR4
```

- TIM GPIO初始化

```c
// 配置COLOR_TIMx复用输出PWM时用到的I/O
static void COLOR_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(COLOR_TIM_GPIO_CLK, ENABLE); // 使能GPIO时钟
  COLOR_GPIO_REMAP_FUN(); // 颜色LED引脚复用设置
  
  // 配置LED灯用到的引脚
  //红
  GPIO_InitStructure.GPIO_Pin =  COLOR_RED_TIM_LED_PIN ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COLOR_RED_TIM_LED_PORT, &GPIO_InitStructure);
	
  // 绿
  GPIO_InitStructure.GPIO_Pin =  COLOR_GREEN_TIM_LED_PIN ;
  GPIO_Init(COLOR_GREEN_TIM_LED_PORT, &GPIO_InitStructure);
	
  // 蓝
  GPIO_InitStructure.GPIO_Pin =  COLOR_BLUE_TIM_LED_PIN ;
  GPIO_Init(COLOR_BLUE_TIM_LED_PORT, &GPIO_InitStructure);
}
```

- 配置PWM信号和定时器模式

```c
// 配置COLOR_TIMx输出的PWM信号的模式，如周期、极性
static void COLOR_TIMx_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;																				
  COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK, ENABLE); // 使能COLOR_TIMx时钟

  // 基本定时器配置		 
  TIM_TimeBaseStructure.TIM_Period = 255; // 当定时器从0计数到255，即为256次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 4000;	// 设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseInit(COLOR_TIMx, &TIM_TimeBaseStructure); // 初始化基本定时器

  // PWM模式配置
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				// 配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	// 使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;										  			// 设置初始PWM脉冲宽度为0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  // 当定时器计数值小于CCR_Val时为低电平

	// 使能通道和预装载
  COLOR_RED_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);	 							
  COLOR_RED_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);						
  
	// 使能通道和预装载
  COLOR_GREEN_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);	 									
  COLOR_GREEN_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);						
  
	// 使能通道和预装载
  COLOR_BLUE_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);	 									
  COLOR_BLUE_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);						

  TIM_ARRPreloadConfig(COLOR_TIMx, ENABLE); // 使能COLOR_TIMx重载寄存器ARR
  TIM_Cmd(COLOR_TIMx, ENABLE); // 使能定时器		
```

- 呼吸灯初始化

```c
// COLOR_TIMx 呼吸灯初始化
void COLOR_TIMx_LED_Init(void)
{
	COLOR_TIMx_GPIO_Config();
	COLOR_TIMx_Mode_Config();	
}
```

- 设置彩灯颜色

```c
// 设置RGB LED的颜色
void SetRGBColor(uint32_t rgb)
{
	// 根据颜色值修改定时器的比较寄存器值
	COLOR_TIMx->COLOR_RED_CCRx = (uint8_t)(rgb>>16);	//R
	COLOR_TIMx->COLOR_GREEN_CCRx = (uint8_t)(rgb>>8);	//G     
	COLOR_TIMx->COLOR_BLUE_CCRx = (uint8_t)rgb;				//B
}


// 设置RGB LED的颜色
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	//根据颜色值修改定时器的比较寄存器值
	COLOR_TIMx->COLOR_RED_CCRx = r;	
	COLOR_TIMx->COLOR_GREEN_CCRx = g;	        
	COLOR_TIMx->COLOR_BLUE_CCRx = b;	
}
```

- 停止PWM输出

```c
//停止pwm输出
void COLOR_TIMx_LED_Close(void)
{
	SetColorValue(0,0,0); // 关闭LED灯
	TIM_Cmd(COLOR_TIMx, DISABLE); // 失能定时器						
	COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK, DISABLE); // 失能定时器时钟
	//关闭LED灯
	GPIO_SetBits(COLOR_RED_TIM_LED_PORT,COLOR_RED_TIM_LED_PIN);
	GPIO_SetBits(COLOR_GREEN_TIM_LED_PORT,COLOR_GREEN_TIM_LED_PIN);
	GPIO_SetBits(COLOR_BLUE_TIM_LED_PORT,COLOR_BLUE_TIM_LED_PIN);
}
```

- 主函数

```c
int main(void)
{			
	COLOR_TIMx_LED_Init();

	while(1)
	{
		// 显示各种颜色
		SetRGBColor(0x8080ff);
		SOFT_DELAY();
		
		SetRGBColor(0xff8000);
		SOFT_DELAY();
		
		SetRGBColor(0xffc90e);
		SOFT_DELAY();
		
		SetColorValue(181,230,29);
		SOFT_DELAY();

		SetColorValue(255,128,64);
		SOFT_DELAY();		

	}		
}
```


