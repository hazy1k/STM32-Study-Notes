#include "stm32f10x.h"
#include "led.h"

#define SOFT_DELAY Delay(0x0FFFFF); // ��ʱ

void Delay(__IO u32 nCount); // ��ʱ�������� 

int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 

	while (1)
	{
		LED1_ON;    // LED1��
		SOFT_DELAY; // �ӳ�
		LED1_OFF;	// LED1��	 

		LED2_ON;	// LED2��		
		SOFT_DELAY; // �ӳ�
		LED2_OFF;	// LED2��	  

		LED3_ON;	// LED3��		
		SOFT_DELAY; // �ӳ�
		LED3_OFF;   // LED3��
		
		/*������ʾ ������������� ��ɫ*
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

// ��ʱ����
void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
