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
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "usart/bsp_debug_usart.h"
#include "systick/bsp_SysTick.h"
#include "DS18B20/bsp_ds18b20.h"

float temperature;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	uint8_t uc,DS18B20Id[8];
	
  /*��ʼ��USART������ģʽΪ 115200 8-N-1*/
  Debug_USART_Config();
  printf("\r\n***Ұ��F407���� DS18B20 �¶ȴ�����ʵ��***\n");
	
	/* ϵͳ��ʱ����ʼ�� */
	SysTick_Init();
	
	if(DS18B20_Init()==0)
	{
		printf("DS18B20��ʼ���ɹ�\n");
	}
	else
	{
		printf("DS18B20��ʼ��ʧ��\n");
		printf("�뽫��������ȷ���뵽�����\n");
		/* ͣ�� */
		while(1)
		{}			
	}		
	DS18B20_ReadId ( DS18B20Id  );           // ��ȡ DS18B20 �����к�
	
	printf("\r\nDS18B20�����к��ǣ� 0x");

	for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
	 printf ( "%.2x", DS18B20Id[uc]);
	printf("\n");
	
  while(1)
	{
		temperature=DS18B20_Get_Temp();
		printf("DS18B20��ȡ�����¶�Ϊ��%0.3f\n",temperature);
    Delay_ms(1000);
	} 

}



/*********************************************END OF FILE**********************/

