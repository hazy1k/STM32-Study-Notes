#include "stm32f10x.h"
#include "led.h"
#include "key.h" 
#include "usart.h"
#include "pvd.h"

int main(void)
{	
	LED_Init();	
	LED_GREEN(); 
	// ����PVD������ѹ����ʱ��������жϷ������������
	PVD_Config();
	while(1)
	{					
	}
}
