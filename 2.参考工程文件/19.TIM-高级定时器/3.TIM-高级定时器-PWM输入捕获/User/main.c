// TIM—高级定时器-PWM输入捕获应用，通用定时器产生PWM波，高级定时器则捕获这个PWM，并测量周期和占空比
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_AdvanceTim.h" 
#include "bsp_GeneralTim.h" 
#include "bsp_usart.h"

int main(void)
{	
	/* 串口初始化 */
	USART_Config();
	
	/* 通用定时器初始化，用于生成PWM信号 */
	GENERAL_TIM_Init();
	
	/* 高级定时器初始化 ，用户捕获PWM信号*/
	ADVANCE_TIM_Init();
	
  while(1)
  {      
  }
}