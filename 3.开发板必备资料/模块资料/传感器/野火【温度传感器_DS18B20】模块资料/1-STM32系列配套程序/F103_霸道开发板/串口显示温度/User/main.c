/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   DS18B20�¶ȴ�������ȡ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 �Ե� ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./ds18b20/bsp_ds18b20.h"



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	uint8_t uc, ucDs18b20Id [ 8 ];
	
	
	/* ����SysTick Ϊ1us�ж�һ�� */
	SysTick_Init();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	USART_Config();	//��ʼ������1
	
	
	printf("\r\n this is a ds18b20 test demo \r\n");
	
	while( DS18B20_Init() )	
		printf("\r\n no ds18b20 exit \r\n");
	
	printf("\r\n ds18b20 exit \r\n");
	
	
	DS18B20_ReadId ( ucDs18b20Id  );           // ��ȡ DS18B20 �����к�
	
	printf("\r\nDS18B20�����к��ǣ� 0x");

	for ( uc = 0; uc < 8; uc ++ )             // ��ӡ DS18B20 �����к�
	 printf ( "%.2x", ucDs18b20Id [ uc ] );
	
	
	for(;;)
	{	
		printf ( "\r\n��ȡ�����к��������¶ȣ� %.1f\r\n", DS18B20_GetTemp_MatchRom ( ucDs18b20Id ) );		// ��ӡͨ�� DS18B20 ���кŻ�ȡ���¶�ֵ	
		
		Delay_ms(1000);		/* 1s ��ȡһ���¶�ֵ */
		
		LED2_TOGGLE;
		
	}	 
	
	
}
/*********************************************END OF FILE**********************/
