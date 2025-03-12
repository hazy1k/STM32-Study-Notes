#include "stm32f10x.h"
#include "usart.h"
#include "led.h"
#include "internal_flash.h"   

void Delay(__IO uint32_t nCount);

int main(void)
{ 	
  USART_Config();
	LED_Init();
	LED_BLUE();
	if(InternalFlash_Test()== PASSED)
	{
		LED_GREEN();
		printf("��д�ڲ�FLASH���Գɹ�\r\n");
	}
	else
	{
		printf("��д�ڲ�FLASH����ʧ��\r\n");
		LED_RED();
	}	
  while(1); 
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
