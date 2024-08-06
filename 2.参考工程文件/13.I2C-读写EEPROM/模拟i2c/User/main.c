#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h" 
#include "./i2c/bsp_i2c_ee.h"

int main(void)
{
  
    LED_GPIO_Config();  
    LED_BLUE; // ��ʼ״̬������
		USART_Config();

		printf("eeprom ���GPIOģ��i2c�������� \r\n");		
  
    if(ee_Test() == 1)
  	{
			LED_GREEN; // �ɹ��̵���
    }
    else
    {
        LED_RED;
    }
    while(1);
}