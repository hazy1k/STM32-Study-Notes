#include "stm32f10x.h"
#include "led.h"

#define SOFT_DELAY Delay(0x0FFFFF); // 延时

void Delay(__IO u32 nCount); // 延时函数声明 

int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	 

	while (1)
	{
		LED1_ON;    // LED1开
		SOFT_DELAY; // 延迟
		LED1_OFF;	// LED1关	 

		LED2_ON;	// LED2开		
		SOFT_DELAY; // 延迟
		LED2_OFF;	// LED2关	  

		LED3_ON;	// LED3开		
		SOFT_DELAY; // 延迟
		LED3_OFF;   // LED3关
		
		/*轮流显示 红绿蓝黄紫青白 颜色*
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
		*/
	}
}

// 延时函数
void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
