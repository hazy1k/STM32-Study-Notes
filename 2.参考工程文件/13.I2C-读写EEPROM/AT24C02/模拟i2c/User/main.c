/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   AT24C02���ԣ����ģ��I2C��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h" 
#include "./i2c/bsp_i2c_ee.h"

 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  
    LED_GPIO_Config();
  
    LED_BLUE;

    /* ���ڳ�ʼ�� */
		USART_Config();

		printf("eeprom ���ģ��i2c�������� \r\n");		
  
    if(ee_Test() == 1)
  	{
			LED_GREEN;
    }
    else
    {
        LED_RED;
    }
    while(1);
}
/*********************************************END OF FILE**********************/
