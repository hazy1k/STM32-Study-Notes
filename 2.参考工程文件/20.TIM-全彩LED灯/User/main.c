#include "stm32f10x.h"
#include "./color_led/bsp_color_led.h"

#define SOFT_DELAY() Delay(0xFFFFFF);

void Delay(__IO u32 nCount); 

int main(void)
{			
	COLOR_TIMx_LED_Init();

	while(1)
	{
		// ��ʾ������ɫ
		SetRGBColor(0x8080ff);
		SOFT_DELAY();
		
		SetRGBColor(0xff8000);
		SOFT_DELAY();
		
		SetRGBColor(0xffc90e);
		SOFT_DELAY();
		
		SetColorValue(181,230,29);
		SOFT_DELAY();

		SetColorValue(255,128,64);
		SOFT_DELAY();		

	}		
}


void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
