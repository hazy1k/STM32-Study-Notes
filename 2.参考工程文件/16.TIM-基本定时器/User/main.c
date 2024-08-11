// 基本定时器TIMx,x[6,7]定时应用
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_TiMbase.h"

volatile uint32_t time = 0; // ms 计时变量 

int main(void)
{
	LED_GPIO_Config();	
	BASIC_TIM_Init();
	
  while(1)
  {
    if (time == 1000) // 1000 * 1 ms = 1s 时间到 
    {
      time = 0;
			LED1_TOGGLE; 
    }        
  }
}
