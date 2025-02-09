#include "stm32f10x.h"
#include "ili9341.h"
#include "xpt2046.h"
#include "flash.h"
#include "led.h" 
#include "palette.h"
#include <string.h>

int main(void)
{		
	ILI9341_Init();  	
	XPT2046_Init();
	//��FLASH���ȡУ����������FLASH�޲�������ʹ��ģʽ3����У��
	Calibrate_or_Get_TouchParaWithFlash(3,0);
	LED_GPIO_Config();
  	ILI9341_GramScan(3);	
	Palette_Init(LCD_SCAN_MODE);
	while(1)
	{
		// ������⺯��������������10ms����һ��
		XPT2046_TouchEvenHandler();
	}
}
