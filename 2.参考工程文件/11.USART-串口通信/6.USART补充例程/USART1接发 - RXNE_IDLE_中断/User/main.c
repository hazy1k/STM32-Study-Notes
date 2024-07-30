/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����жϽ��ղ���
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
#include "bsp_usart.h"
#include "bsp_led.h"
#include "test.h"


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  USART_Config();
	LED_GPIO_Config();
	
	printf("��ӭʹ��Ұ��STM32������\n\n\n\n");
	printf("ʹ�ÿ����жϽ��ղ������ַ���������ʾ\n\n\n\n");
	
  while(1)
	{	
		
		if(test_data.flag == 1)
		{			
			do_process(&test_data);							
		}
		
	}	
}
/*********************************************END OF FILE**********************/
