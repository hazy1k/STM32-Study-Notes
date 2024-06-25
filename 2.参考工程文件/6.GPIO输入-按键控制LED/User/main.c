 #include "stm32f10x.h"
#include "bsp_led.h"  
#include "bsp_key.h" 

int main(void)
{	
	/* LED�˿ڳ�ʼ�� */
	LED_GPIO_Config();
	LED1_ON; // ��LED1

	/* �����˿ڳ�ʼ�� */
	Key_GPIO_Config();
	
	/* ��ѯ����״̬��������������תLED */
	while(1)                            
	{	   
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			/*LED1��ת*/
			LED1_TOGGLE;
		} 
		if( Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
		{
			/*LED2��ת*/
			LED2_TOGGLE;
		}		
	}
}
