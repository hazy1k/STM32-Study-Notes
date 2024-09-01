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
#include "bsp_beep.h"
#include "./dwt_delay/core_delay.h"




/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main ( void )
{
	/* ��ʼ�� */
  USARTx_Config ();                                                              //��ʼ������1
  CPU_TS_TmrInit();
	ESP8266_Init ();                                                               //��ʼ��WiFiģ��ʹ�õĽӿں�����
	DHT11_Init ();
  LED_Init ();
	Beep_Init ();
	
	
	printf ( "\r\nҰ�� WF-ESP8266 WiFiģ���������\r\n" );                          //��ӡ����������ʾ��Ϣ
	
  
  #ifndef BUILTAP_TEST
  ESP8266_StaTcpServer_ConfigTest();                                             //��ESP8266�������� STAģʽ
  #else
  ESP8266_ApTcpServer_ConfigTest();                                              //��ESP8266�������� APģʽ
  #endif
  
  while ( 1 )
  {
    
    ESP8266_CheckRecv_SendDataTest(); // ESP8266 ������������
    
  }
	
	
}


/*********************************************END OF FILE**********************/
