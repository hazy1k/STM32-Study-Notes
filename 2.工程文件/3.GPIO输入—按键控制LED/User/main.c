/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ���԰���
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
#include "bsp_led.h"  
#include "bsp_key.h" 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{	
	/* LED�˿ڳ�ʼ�� */
	LED_GPIO_Config();
	LED1_ON;

	/* �����˿ڳ�ʼ�� */
	Key_GPIO_Config();
	
	/* ��ѯ����״̬��������������תLED */
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			/*LED1��ת*/
			LED1_TOGGLE;
		} 

		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			/*LED2��ת*/
			LED2_TOGGLE;
		}		
	}
}
/*********************************************END OF FILE**********************/
