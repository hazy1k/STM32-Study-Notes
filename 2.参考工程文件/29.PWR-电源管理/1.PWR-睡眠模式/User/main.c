#include "led.h"
#include "usart.h"
#include "key.h"

static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

int main(void)
{
	KEY_EXTI_Init();
	LED_Init();
	USART_Config();
	printf("PWR-Sleep Mode Test\r\n");
	while(1)
	{
		printf("�̵�����stm32��������\r\n");
		LED_GREEN();
		Delay(0xFFFFFF);
		printf("�������stm32����˯��ģʽ\r\n");
		LED_RED();
		__WFI(); // ����˯��ģʽ
		Delay(0xFFFFFF);
		printf("���˳�˯��ģʽ\r\n");
	}
}
