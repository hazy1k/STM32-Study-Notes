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
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������
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

