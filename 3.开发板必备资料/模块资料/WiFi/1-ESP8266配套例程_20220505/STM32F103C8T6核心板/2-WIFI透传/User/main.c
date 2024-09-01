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
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
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
  USART1_Config ();                                                       //��ʼ������1
  CPU_TS_TmrInit();                                                       //��ʼ��DWT��������������ʱ����
  LED_GPIO_Config();                                                    //��ʼ�� LED ��
	ESP8266_Init ();                                                        //��ʼ��WiFiģ��ʹ�õĽӿں�����
//	DHT11_Init ();                                                        //��ʼ��DHT11
//	SysTick_Init ();                                                      //���� SysTick Ϊ 10ms �ж�һ�Σ����ж����ȡ����������
  
	
	printf ( "\r\nҰ�� WF-ESP8266 WiFiģ���������\r\n" );                          //��ӡ����������ʾ��Ϣ
	printf ( "\r\n������������ֻ��ߴ��ڵ��������Ϸ�������������Կ��ư���LED��\r\n" );    //��ӡ����������ʾ��Ϣ
  printf ( "\r\nLED1_ON\r\nLED2_ON\r\nLED3_ON\r\nLED_ALLOFF\r\n" );

	  
  ESP8266_StaTcpClient_Unvarnish_ConfigTest();                          //��ESP8266��������
  
  printf ( "\r\n������������ֻ��ߴ��ڵ�������  ��������������ư���LED�ƣ�\r\n" );    //��ӡ����������ʾ��Ϣ
  printf ( "\r\nLED1_ON\r\nLED2_ON\r\nLED3_ON\r\nLED_ALLOFF\r\n" );
  printf ( "\r\n�۲�LED�Ƶ�״̬�仯\r\n" );
  
  while ( 1 )
  {
    
    ESP8266_CheckRecvDataTest(); // ESP8266 ���һ���Ƿ���յ�������
    
  }
	
}


/*********************************************END OF FILE**********************/
