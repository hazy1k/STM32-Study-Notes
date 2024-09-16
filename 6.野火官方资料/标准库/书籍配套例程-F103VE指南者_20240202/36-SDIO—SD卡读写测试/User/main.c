/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SDIO sd������ʵ�飨�����ļ�ϵͳ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103-ָ���� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "./sdio/bsp_sdio_sdcard.h"
#include "./sdio/sdio_test.h"
#include "./usart/bsp_usart.h"	
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{									   
	/* ��ʼ��LED�� */
  LED_GPIO_Config();
	LED_BLUE;	
	/* ��ʼ���������� */
	Key_GPIO_Config();
  
  /*��ʼ��USART1*/
  USART_Config();
  
	printf("\r\n��ӭʹ��Ұ��  STM32 �����塣\r\n");	
	
	printf("�ڿ�ʼ����SD����������ǰ��������������32G���ڵ�SD��\r\n");			
	printf("��������SD������ ���ļ�ϵͳ ��ʽ��д����ɾ��SD�����ļ�ϵͳ\r\n");		
	printf("ʵ����ͨ�����Ը�ʽ����ʹ��SD���ļ�ϵͳ�����ָ̻�SD���ļ�ϵͳ\r\n");		
	printf("\r\n ��sd���ڵ�ԭ�ļ����ɻָ���ʵ��ǰ��ر���SD���ڵ�ԭ�ļ�������\r\n");		
	
	printf("\r\n ����ȷ�ϣ��밴�������KEY1��������ʼSD������ʵ��....\r\n");	
  
  /* Infinite loop */
  while (1)
  {	
		/*���°�����ʼ����SD����дʵ�飬����SD��ԭ�ļ�*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("\r\n��ʼ����SD����дʵ��\r\n");	
		  SD_Test();			
		}
	} 
}



/***************************END OF FILE*******************************/
