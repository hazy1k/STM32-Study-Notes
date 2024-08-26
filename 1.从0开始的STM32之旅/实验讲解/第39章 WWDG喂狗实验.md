# 第三十九章 WWDG喂狗实验

## 1. 硬件设计

1. WWDG一个

2. LED两个

WWDG属于单片机内部资源，不需要外部电路，需要两个LED来指示程序的运行状态。

## 2. 软件设计

- WWDG配置函数

```c
void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{
    // 开启 WWDG 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    // 设置递减计数器的值
    WWDG_SetCounter( tr );

    // 设置预分频器的值
    WWDG_SetPrescaler( prv );

    // 设置上窗口值
    WWDG_SetWindowValue( wr );

    // 设置计数器的值，使能WWDG
    WWDG_Enable(WWDG_CNT);

    // 清除提前唤醒中断标志位
    WWDG_ClearFlag();
    // 配置WWDG中断优先级
    WWDG_NVIC_Config();
    // 开WWDG 中断
    WWDG_EnableIT();
}
```

WWDG配置函数有三个形参，tr是计数器的值，一般我们设置成最大0X7F，wr是上窗口的值，这个我们要根据监控的程序的运行时间来设置， 但是值必须在0X40和计数器的值之间，prv用来设置预分频的值，取值可以是：

```c
/*
*     @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
*     @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
*     @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
*     @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
*/
```

这些宏在stm32f10x_wwdg.h中定义，宏展开是32位的16进制数，具体作用是设置配置寄存器CFR的位8:7  WDGTB[1:0]，获得各种分频系数。

- WWDG中断优先级函数

```c
// WWDG 中断优先级初始化
static void WWDG_NVIC_Config(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

在递减计数器减到0X40的时候，我们开启了提前唤醒中断，这个中断我们称它为死前中断或者叫遗嘱中断， 在中断函数里面我们应该出来最重要的事情，而且必须得快，因为递减计数器再减一次，就会产生系统复位。

- 中断服务函数

```c
// WWDG 中断复服务程序，如果发生了此中断，表示程序已经出现了故障，
// 这是一个死前中断。在此中断服务程序中应该干最重要的事，
// 比如保存重要的数据等，这个时间具体有多长，要
// 由WDGTB的值决定：
// WDGTB:0   113us
// WDGTB:1   227us
// WDGTB:2   455us
// WDGTB:3   910us
void WWDG_IRQHandler(void)
{
    // 清除中断标志位
    WWDG_ClearFlag();

    //LED2亮，点亮LED只是示意性的操作，
    //真正使用的时候，这里应该是做最重要的事情
    LED2(ON);
}
```

- 喂狗函数

```c
// 喂狗
void WWDG_Feed(void)
{
    // 喂狗，刷新递减计数器的值，设置成最大WDG_CNT=0X7F
    WWDG_SetCounter( WWDG_CNT );
}
```

喂狗就是重新刷新递减计数器的值防止系统复位，喂狗一般是在主函数中喂。

- 主函数

```c
int main(void)
{	
	uint8_t wwdg_tr, wwdg_wr; 
	
	// 配置LED GPIO，并关闭LED
	LED_GPIO_Config();

	LED1(ON) ;
	SOFT_Delay(0X00FFFFFF);	
	
	// 初始化WWDG：配置计数器初始值，配置上窗口值，启动WWDG，使能提前唤醒中断
	WWDG_Config(0X7F, 0X5F, WWDG_Prescaler_8); // 函数参数：计数器值，窗口值，预分频系数
	
	// 窗口值我们在初始化的时候设置成0X5F，这个值不会改变
	wwdg_wr = WWDG->CFR & 0X7F;

	while(1)
	{	
		LED1(OFF);
		//-----------------------------------------------------
		// 这部分应该写需要被WWDG监控的程序，这段程序运行的时间
		// 决定了窗口值应该设置成多大。
		//-----------------------------------------------------
		
		// 计时器值，初始化成最大0X7F，当开启WWDG时候，这个值会不断减小
		// 当计数器的值大于窗口值时喂狗的话，会复位，当计数器减少到0X40
		// 还没有喂狗的话就非常非常危险了，计数器再减一次到了0X3F时就复位
		// 所以要当计数器的值在窗口值和0X40之间的时候喂狗，其中0X40是固定的。
		wwdg_tr = WWDG->CR & 0X7F;
		
		if(wwdg_tr < wwdg_wr)
		{
			// 喂狗，重新设置计数器的值为最大0X7F
			WWDG_Feed();
		}
	}
}
```

主函数中我们把WWDG的计数器的值设置 为0X7F，上窗口值设置为0X5F，分频系数为8分频。在while死循环中，我们不断读取计数器的值， 当计数器的值减小到小于上窗口值的时候，我们喂狗，让计数器重新计数。

在while死循环中，一般是我们需要监控的程序，这部分代码的运行时间，决定了上窗口值应该设置为多少，当监控的程序运行完毕之后， 我们需要执行喂狗程序，比起独立看门狗，这个喂狗的窗口时间是非常短的，对时间要求很精确。如果没有在这个窗口时间内喂狗的话， 那就说明程序出故障了，会产生提前唤醒中断，最后系统复位。


