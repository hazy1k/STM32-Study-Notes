#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h" 
#include "./i2c/bsp_i2c_ee.h"

int main(void)
{
  
    LED_GPIO_Config();  
    LED_BLUE; // 初始状态蓝灯亮
		USART_Config();

		printf("eeprom 软件GPIO模拟i2c测试例程 \r\n");		
  
    if(ee_Test() == 1)
  	{
			LED_GREEN; // 成功绿灯亮
    }
    else
    {
        LED_RED;
    }
    while(1);
}