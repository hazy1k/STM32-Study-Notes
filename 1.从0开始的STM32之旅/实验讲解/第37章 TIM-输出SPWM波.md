# 第三十七章 TIM-输出SPWM波

## 1. 硬件设计

与全彩呼吸灯一样，只是使用的PWM表不同

## 2. 软件设计

- 硬件相关宏定义

```c
/*电压幅值等级数*/
#define AMPLITUDE_CLASS 256

//控制输出波形的频率
extern __IO uint16_t period_class ;

/*PWM表中的点数*/
extern uint16_t POINT_NUM;
/********************定时器通道**************************/

#define   BRE_TIMx                      TIM3

#define   BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd
#define   BRE_TIM_CLK                   RCC_APB1Periph_TIM3
#define   BRE_TIM_GPIO_CLK              (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)

//红灯的引脚需要重映射
#define   BRE_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				

/************红灯***************/
#define  BRE_RED_TIM_LED_PORT               GPIOB
#define  BRE_RED_TIM_LED_PIN                GPIO_Pin_5

#define  BRE_RED_TIM_OCxInit                TIM_OC2Init            //通道初始化函数
#define  BRE_RED_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig //通道重载配置函数

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIMx->BRE_RED_CCRx ，可访问该通道的比较寄存器
#define  BRE_RED_CCRx                       	CCR2		

/************绿灯***************/

#define  BRE_GREEN_TIM_LED_PORT               GPIOB
#define  BRE_GREEN_TIM_LED_PIN                GPIO_Pin_0

#define  BRE_GREEN_TIM_OCxInit                TIM_OC3Init            //通道初始化函数
#define  BRE_GREEN_TIM_OCxPreloadConfig       TIM_OC3PreloadConfig //通道重载配置函数

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIMx->BRE_GREEN_CCRx ，可访问该通道的比较寄存器
#define  BRE_GREEN_CCRx                       CCR3

/************蓝灯***************/
#define   BRE_BLUE_TIM_LED_PORT             GPIOB
#define   BRE_BLUE_TIM_LED_PIN              GPIO_Pin_1

#define   BRE_BLUE_TIM_OCxInit              TIM_OC4Init            //通道初始化函数
#define   BRE_BLUE_TIM_OCxPreloadConfig    TIM_OC4PreloadConfig  //通道重载配置函数

//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：BRE_TIMx->BRE_BLUE_CCRx ，可访问该通道的比较寄存器
#define   BRE_BLUE_CCRx                      CCR4

#define   BRE_TIMx_IRQn                TIM3_IRQn              //中断
#define   BRE_TIMx_IRQHandler          TIM3_IRQHandler
```

除了PWM表不同，其余均与TIM-全彩呼吸灯一样

```c
/* SPWM表,正弦曲线，此表使用工程目录下的python脚本sin_wave.py生成*/
const uint16_t indexWave[] = {
0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 89, 98,
	107, 116, 125, 133, 142, 151, 159, 168, 176,
	184, 193, 201, 209, 218, 226, 234, 242, 249,
	257, 265, 273, 280, 288, 295, 302, 310, 317, 
	324, 331, 337, 344, 351, 357, 364, 370, 376, 
	382, 388, 394, 399, 405, 410, 416, 421, 426, 
	431, 436, 440, 445, 449, 454, 458, 462, 465, 
	469, 473, 476, 479, 482, 485, 488, 491, 493, 
	496, 498, 500, 502, 503, 505, 506, 508, 509, 
	510, 510, 511, 512, 512, 512, 512, 512, 512,
	511, 510, 510, 509, 508, 506, 505, 503, 502,
	500, 498, 496, 493, 491, 488, 485, 482, 479,
	476, 473, 469, 465, 462, 458, 454, 449, 445, 
	440, 436, 431, 426, 421, 416, 410, 405, 399, 
	394, 388, 382, 376, 370, 364, 357, 351, 344, 
	337, 331, 324, 	317, 310, 302, 295, 288, 280, 
	273, 265, 257, 249, 242, 234, 226, 218, 209, 
	201, 193, 184, 176, 168, 159, 151, 142, 133, 
125, 116, 107, 98, 89, 81, 72, 63, 54, 45, 36,
27, 18, 9, 0
};
```

- TIM配置

```c
static void TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	
	/* 设置TIMCLK 时钟 */
	BRE_TIM_APBxClock_FUN ( BRE_TIM_CLK, ENABLE ); 

  /* 基本定时器配置 ,配合PWM表点数、中断服务函数中的PERIOD_CLASS、AMPLITUDE_CLASS循环次数设置*/	
	
	/* 设置使得整个呼吸过程为3秒左右即可达到很好的效果 */	
	
	//要求：
	//TIM_Period：与PWM表中数值范围一致
	//TIM_Prescaler：越小越好，可减轻闪烁现象
	//PERIOD_CLASS：中断服务函数中控制单个点循环的次数，调整它可控制拟合曲线的周期

	//AMPLITUDE_CLASS：中断服务函数中控制单个点循环的次数，调整它可控制幅值，
	//						在本实验中它为LED通道的亮度值，分256个等级，对应RGB888格式各通道的颜色等级
	//POINT_NUM：PWM表的元素，它是PWM拟合曲线的采样点数

	/*************本实验中的配置***************/	
	/***********************************************
	#python计算脚本	count.py
	#PWM点数
	POINT_NUM = 180

	#幅值（颜色）等级
	AMPLITUDE_CLASS = 256
	
	#周期倍数
	PERIOD_CLASS = 1

	#定时器定时周期
	TIMER_TIM_Period = 2**9
	#定时器分频
	TIMER_TIM_Prescaler = 10

	#STM32系统时钟频率和周期
	f_pclk = 72000000
	t_pclk = 1/f_pclk

	#定时器update事件周期
	t_timer = t_pclk*TIMER_TIM_Prescaler*TIMER_TIM_Period

	#每个PWM点的时间
	T_Point = t_timer * RGB_CLASS

	#整个呼吸周期
	T_Up_Down_Cycle = T_Point * POINT_NUM*PERIOD_CLASS

	print ("呼吸周期：",T_Up_Down_Cycle)
	
	#运行结果：
	
	呼吸周期：3.27679
	************************************************************/
  TIM_TimeBaseStructure.TIM_Period = (512-1);       							  //当定时器从0计数到 TIM_Period+1 ，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = (10-1);	    							//设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
  TIM_TimeBaseInit(BRE_TIMx, &TIM_TimeBaseStructure);

  /* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;				 						  			//设置初始PWM脉冲宽度为0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //当定时器计数值小于CCR_Val时为低电平

  BRE_RED_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	
  BRE_RED_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );						//使能预装载	

  BRE_GREEN_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	
  BRE_GREEN_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );						//使能预装载	

  BRE_BLUE_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	
  BRE_BLUE_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );						//使能预装载	

  TIM_ARRPreloadConfig(BRE_TIMx, ENABLE);			 										//使能TIM重载寄存器ARR

  /* TIM3 enable counter */
  TIM_Cmd(BRE_TIMx, ENABLE);                   										//使能定时器	
	
	TIM_ITConfig(BRE_TIMx, TIM_IT_Update, ENABLE);										//使能update中断
		
	NVIC_Config_PWM();																					//配置中断优先级		
	
}
```


