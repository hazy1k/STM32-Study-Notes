// TIM��ͨ��-����-���ݰ������ Ӧ��
#include "stm32f10x.h"
#include "usart.h" 
#include "SysTick.h"
#include "beep.h"
#include "tpad.h"

int main(void)
{
	SysTick_Init();
	USART_Config();
	BEEP_GPIO_Config();
	TPAD_Init(); 
	while(1)
	{
		if(TPAD_Scan() == TPAD_ON)
		{
			BEEP(ON);
			printf("���°���\r\n");
			Delay_ms(100);
			BEEP(OFF);
		}
	}
}
