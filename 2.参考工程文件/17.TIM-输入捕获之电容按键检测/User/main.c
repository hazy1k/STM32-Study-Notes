// TIM—通用-捕获-电容按键检测 应用
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_tpad.h"  
#include "bsp_SysTick.h"
#include "bsp_beep.h"

int main(void)
{
	//uint16_t temp;
	// led 端口配置
	LED_GPIO_Config();
	// 蜂鸣器初始化	
	Beep_Init();
	// 串口初始化
	USART_Config();
	printf ( "\r\n野火STM32 输入捕获电容按键检测实验\r\n" );
	printf ( "\r\n触摸电容按键，蜂鸣器则会响\r\n" );
	
//	TPAD_TIM_Init();
//	while(1)
//  {
//		temp = TPAD_Get_Val();
//		printf("电容按键默认充电时间为: %d us\n",temp);
//		SysTick_Delay_Ms(100);
//	}
	
	// 初始化电容按键
	while(TPAD_Init());

	while(1)
  	{
		if(TPAD_Scan() == TPAD_ON )
    	{
      		BEEP_ON();
			SysTick_Delay_Ms(25); // 系统点滴延时，防止蜂鸣器响太快
			BEEP_OFF();
		}
	}
}
