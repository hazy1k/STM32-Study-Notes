/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   WF-ESP8266 WiFiģ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103-�Ե� ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_SysTick.h"
#include "stm32f10x_it.h"
#include "bsp_esp8266.h"
#include "bsp_esp8266_test.h"
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "./dwt_delay/core_delay.h"


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main ( void )
{
	/* ��ʼ�� */
  USART_Config ();                                                      //��ʼ������1
  CPU_TS_TmrInit();                                                     //��ʼ��DWT��������������ʱ����
  LED_GPIO_Config();                                                    //��ʼ��RGB�ʵ�
	ESP8266_Init ();                                                      //��ʼ��WiFiģ��ʹ�õĽӿں�����
//	DHT11_Init ();                                                        //��ʼ��DHT11
//	SysTick_Init ();                                                      //���� SysTick Ϊ 10ms �ж�һ�Σ����ж����ȡ����������

	
	printf ( "\r\nҰ�� WF-ESP8266 WiFiģ���������\r\n" );                          //��ӡ����������ʾ��Ϣ
	printf ( "\r\n������������ֻ��ߴ��ڵ��������Ϸ�������������Կ��ư���RGB��\r\n" );    //��ӡ����������ʾ��Ϣ
  printf ( "\r\nLED_RED\r\nLED_GREEN\r\nLED_BLUE\r\nLED_YELLOW\r\nLED_PURPLE\r\nLED_CYAN\r\nLED_WHITE\r\nLED_RGBOFF\r\n" );  

	  
  ESP8266_StaTcpClient_Unvarnish_ConfigTest();                          //��ESP8266��������
  
  printf ( "\r\n������������ֻ��ߴ��ڵ�������  ��������������ư���RGB�ƣ�\r\n" );    //��ӡ����������ʾ��Ϣ
  printf ( "\r\nLED_RED\r\nLED_GREEN\r\nLED_BLUE\r\nLED_YELLOW\r\nLED_PURPLE\r\nLED_CYAN\r\nLED_WHITE\r\nLED_RGBOFF\r\n" );
  printf ( "\r\n�۲�RGB�Ƶ�״̬�仯\r\n" );
  
  while ( 1 )
  {
    
    ESP8266_CheckRecvDataTest(); // ESP8266 ���һ���Ƿ���յ�������
    
  }
	
}


/*********************************************END OF FILE**********************/
