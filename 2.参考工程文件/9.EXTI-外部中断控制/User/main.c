#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_exti.h" 

int main(void)
{
	LED_GPIO_Config();
	EXTI_Key_Config(); 
	
	while(1)                            
	{

	}
}
