/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   wifi ����͸������
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
#include "./dwt_delay/core_delay.h"
#include "./DHT11/bsp_dht11.h"
#include "./esp8266/bsp_esp8266.h"
#include "./esp8266/bsp_esp8266_test.h"
#include "./led/bsp_led.h"
#include "./beep/bsp_beep.h"   

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  /* ��ʼ�� */
  Debug_USART_Config ();                                                      //��ʼ������1
  CPU_TS_TmrInit();                                                     //��ʼ��DWT��������������ʱ����
  LED_GPIO_Config();                                                    //��ʼ��RGB�ʵ�
	ESP8266_Init ();                                                      //��ʼ��WiFiģ��ʹ�õĽӿں�����
	DHT11_GPIO_Config ();                                                        //��ʼ��DHT11
	BEEP_GPIO_Config ();
	
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

