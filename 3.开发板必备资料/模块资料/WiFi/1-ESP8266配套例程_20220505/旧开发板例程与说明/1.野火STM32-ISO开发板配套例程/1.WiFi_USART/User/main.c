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
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "bsp_SysTick.h"
#include <string.h>
 
 
void ( * pNet_Test ) ( void );


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  char cCh;	

	/* ��ʼ�� */
	WiFi_Config();                                                                  //��ʼ��WiFiģ��ʹ�õĽӿں�����
	SysTick_Init();                                                                 //���� SysTick Ϊ 1ms �ж�һ�� 

	PC_Usart("\r\nҰ�� WF-ESP8266 WiFiģ���������\r\n");                            //��ӡ����������ʾ��Ϣ

  PC_Usart ( "\r\n������ģ��Ĳ���ģʽ�ı�ţ�������£�\
              \r\n1.STAģʽ�� TCP Client\
              \r\n2.APģʽ�� AP Server\
              \r\n3.STA+APģʽ�� (STA)TCP Client + (AP)TCP Server\
              \r\n�����ʽΪ�����+�ո񣬵������\r\n" );
	
  scanf ( "%c",  & cCh );

  switch ( cCh )
  {
		case '1':
			pNet_Test = ESP8266_STA_TCP_Client;
		  PC_Usart("\r\n�������еĲ����ǣ�1.STAģʽ�� TCP Client\r\n");
		  break;
		
		case '2':
			pNet_Test = ESP8266_AP_TCP_Server;
		  PC_Usart("\r\n�������еĲ����ǣ�2.APģʽ�� TCP Server\r\n");
		  break;

		case '3':
			pNet_Test = ESP8266_StaTcpClient_ApTcpServer;
		  PC_Usart("\r\n�������еĲ����ǣ�3.STA+APģʽ�� (STA)TCP Client + (AP)TCP Server\r\n");
		  break;

		default:
			pNet_Test = NULL;
		  PC_Usart("\r\n������󣬲����в��ԣ�\r\n");
		  break;		
			
	}

	if ( pNet_Test )
		  pNet_Test ();

  while(1);
	
}


/*********************************************END OF FILE**********************/
