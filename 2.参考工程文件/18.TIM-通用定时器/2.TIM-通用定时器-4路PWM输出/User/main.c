// TIM��ͨ�ö�ʱ��-4·PWM���Ӧ��
#include "stm32f10x.h"
#include "led.h"
#include "GeneralTim.h"  

int main(void)
{
	LED_Init();
	LED_BLUE();
	GeneralTim_Init();
	while(1)
	{      
	}
}
