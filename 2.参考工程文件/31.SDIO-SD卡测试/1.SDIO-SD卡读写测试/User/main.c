#include "stm32f10x.h"
#include "sdio_sdcard.h"
#include "sdio_test.h"
#include "usart.h"	
#include "led.h"
#include "key.h" 

int main(void)
{
	LED_Init();	
	KEY_GPIO_Init();
	USART_Config();
	printf("����KEY1����ʼ����\r\n");
	while(1)
	{
		if(KEY_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("��ʼ����SDIO-SD����д\r\n");
			SD_Test();
		}
	}							   
}
