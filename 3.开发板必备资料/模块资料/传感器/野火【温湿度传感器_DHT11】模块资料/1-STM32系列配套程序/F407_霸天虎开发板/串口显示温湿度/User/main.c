/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   DHT11��ʪ�ȴ�������ȡ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "DHT11/bsp_dht11.h"

DHT11_Data_TypeDef DHT11_Data;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  /*��ʼ��USART������ģʽΪ 115200 8-N-1*/
  Debug_USART_Config();
 
	/* ��ʼ��ϵͳ��ʱ�� */
	SysTick_Init();
	
	/* DHT11��ʼ�� */
	DHT11_GPIO_Config();
	
	printf("\r\n***Ұ��F407������ dht11 ��ʪ�ȴ�����ʵ��***\r\n");
	
  while(1)
	{
		/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
		if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
		}		
		else
		{
		  printf("Read DHT11 ERROR!\r\n");
		}
    Delay_ms(2000);
	} 

}



/*********************************************END OF FILE**********************/

