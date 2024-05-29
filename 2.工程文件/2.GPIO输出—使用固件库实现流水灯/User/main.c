#include "stm32f10x.h"
#include "bsp_led.h"

#define SOFT_DELAY Delay(0x0FFFFF);

void Delay(__IO u32 nCount); 

int main(void)
{	
	/* LED ¶Ë¿Ú³õÊ¼»¯ */
	LED_GPIO_Config();	 

	while (1)
	{
		LED1_ON;			  // ÁÁ
		SOFT_DELAY;
		LED1_OFF;		   // Ãð

		LED2_ON;			 // ÁÁ
		SOFT_DELAY;
		LED2_OFF;		   // Ãð

		LED3_ON;			 // ÁÁ
		SOFT_DELAY;
		LED3_OFF;		   // Ãð	 

		/*ÂÖÁ÷ÏÔÊ¾ ºìÂÌÀ¶»Æ×ÏÇà°× ÑÕÉ«*/
		LED_RED;
		SOFT_DELAY;
		
		LED_GREEN;
		SOFT_DELAY;
		
		LED_BLUE;
		SOFT_DELAY;
		
		LED_YELLOW;
		SOFT_DELAY;
		
		LED_PURPLE;
		SOFT_DELAY;
				
		LED_CYAN;
		SOFT_DELAY;
		
		LED_WHITE;
		SOFT_DELAY;
		
		LED_RGBOFF;
		SOFT_DELAY;		
	}
}

void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
