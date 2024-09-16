 /**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��STM32���ڲ�FLASHд������
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
#include "./internal_flash/bsp_internal_flash.h"   


// ����ԭ������
void Delay(__IO uint32_t nCount);
/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{ 	
	/*��ʼ��USART������ģʽΪ 115200 8-N-1*/
    USART_Config();
	LED_GPIO_Config();
 
	LED_BLUE;
	printf("\r\n ��ӭʹ��Ұ��  STM32  �����塣\r\n");	
	printf("���ڽ��ж�д�ڲ�FLASHʵ�飬�����ĵȴ�\r\n");
	
	if(InternalFlash_Test()== PASSED)
	{
		LED_GREEN;
		printf("��д�ڲ�FLASH���Գɹ�\r\n");

	}
	else
	{
		printf("��д�ڲ�FLASH����ʧ��\r\n");
		LED_RED;
	}
	
	
  while(1)
	{	} 
}


void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
