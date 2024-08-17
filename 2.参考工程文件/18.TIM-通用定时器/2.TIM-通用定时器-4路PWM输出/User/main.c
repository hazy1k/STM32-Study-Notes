// TIM—通用定时器-4路PWM输出应用
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_GeneralTim.h"  

int main(void)
{
	// led 端口配置 
	LED_GPIO_Config();
	// 定时器初始化
	GENERAL_TIM_Init();
	
  while(1)
  {      
    
  }
}