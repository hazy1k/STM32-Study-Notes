#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"

/*
t : 定时时间 
Ticks : 多少个时钟周期产生一次中断 
f : 时钟频率 72000000
t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
*/ 
int main(void)
{	
	// 1.GPIO初始化
	LED_GPIO_Config();

	// 2.配置SysTick 为10us中断一次
	SysTick_Init();

	// 1000ms = 1s
	// 死循环
	for(;;)
	{
		LED1( ON ); 
		SysTick_Delay_Ms( 1000 );
		LED1( OFF );
	  
		LED2( ON );
		SysTick_Delay_Ms( 1000 );
		LED2( OFF );
	
		LED3( ON );
		SysTick_Delay_Ms( 1000 );
		LED3( OFF );
	} 	
}