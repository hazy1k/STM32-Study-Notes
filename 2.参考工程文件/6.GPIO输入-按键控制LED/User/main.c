 #include "stm32f10x.h"
#include "bsp_led.h"  
#include "bsp_key.h" 

int main(void)
{	
	/* LED端口初始化 */
	LED_GPIO_Config();
	LED1_ON; // 打开LED1

	/* 按键端口初始化 */
	Key_GPIO_Config();
	
	/* 轮询按键状态，若按键按下则反转LED */
	while(1)                            
	{	   
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			/*LED1反转*/
			LED1_TOGGLE;
		} 
		if( Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
		{
			/*LED2反转*/
			LED2_TOGGLE;
		}		
	}
}
