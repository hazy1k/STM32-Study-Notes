#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"

/*
t : ��ʱʱ�� 
Ticks : ���ٸ�ʱ�����ڲ���һ���ж� 
f : ʱ��Ƶ�� 72000000
t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
*/ 
int main(void)
{	
	// 1.GPIO��ʼ��
	LED_GPIO_Config();

	// 2.����SysTick Ϊ10us�ж�һ��
	SysTick_Init();

	// 1000ms = 1s
	// ��ѭ��
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