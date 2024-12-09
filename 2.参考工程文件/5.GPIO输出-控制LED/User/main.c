#include "stm32f10x.h"
#include "led.h"
#define SOFT_DELAY Delay(0x0FFFFF);
void Delay(__IO uint32_t nCount);

int main(void)
{
	LED_GPIO_Init();
	while(1)
	{
        LED1_ON; // ?
        SOFT_DELAY; // ??
        LED1_OFF; // ?

        LED2_ON;             
        SOFT_DELAY;
        LED2_OFF;          

        LED3_ON;             
        SOFT_DELAY;
        LED3_OFF;         

        /*???? ??????? ??*/
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
    for (; nCount != 0; nCount--);
}
