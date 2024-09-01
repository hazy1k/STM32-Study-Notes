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
	DHT11_Init ();                                                        //��ʼ��DHT11  
	
	
  printf ( "\r\nҰ�� WF-ESP8266 WiFiģ���������\r\n" );                 //��ӡ����������ʾ��Ϣ

  ESP8266_StaTcpClient_Unvarnish_ConfigTest();                          //��ESP8266��������
  
  
	SysTick_Init ();                                                      //���� SysTick Ϊ 10ms �ж�һ�Σ����ж����ȡ����������
  
  while ( 1 )
  {
    
    if( read_dht11_finish ) // read_dht11_finish == 1 or read_dht11_finish == -1
    {
      
      ESP8266_SendDHT11DataTest(); // ESP8266 ����һ����ʪ������
      //printf("read_dht11_finish=%d\r\n", read_dht11_finish);    //Debug
      read_dht11_finish = 0;       // �����־λ
      
      LED1_TOGGLE;
    }
    
  }
	
}


/*********************************************END OF FILE**********************/
