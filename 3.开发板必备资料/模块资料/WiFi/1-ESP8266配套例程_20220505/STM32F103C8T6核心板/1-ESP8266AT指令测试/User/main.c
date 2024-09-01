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
  
	
	printf("\r\n��ӭʹ��Ұ��STM32������\r\n");
	printf("����һ��ESP8266ATָ�����ʵ��\r\n");
	printf("��ʹ�ô��ڵ������ַ���\"AT+���лس�\"����ESP8266�Ƿ�׼����\r\n");
	printf("����ATָ����ο�ģ������\r\n");
	printf("������ESP8266�ϵ��ʼ����ӡ����Ϣ\r\n");
  
  macESP8266_CH_ENABLE();   //ʹ�� ESP8266
  LED1_ON;
  LED2_ON;
  LED3_ON;
  while ( 1 )
  {
    
    if(strUSART_Fram_Record .InfBit .FramFinishFlag == 1)  //������յ��˴��ڵ������ֵ�����
		{
			strUSART_Fram_Record .Data_RX_BUF[strUSART_Fram_Record .InfBit .FramLength] = '\0';
			Usart_SendString(macESP8266_USARTx ,strUSART_Fram_Record .Data_RX_BUF);      //���ݴӴ��ڵ�������ת����ESP8266
			strUSART_Fram_Record .InfBit .FramLength = 0;                                //�������ݳ�������
			strUSART_Fram_Record .InfBit .FramFinishFlag = 0;                            //���ձ�־����
	  }
		if(strEsp8266_Fram_Record .InfBit .FramFinishFlag)                             //������յ���ESP8266������
		{                                                      
			 strEsp8266_Fram_Record .Data_RX_BUF[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
			 Usart_SendString(DEBUG_USARTx ,strEsp8266_Fram_Record .Data_RX_BUF);        //���ݴ�ESP8266ת�������ڵ�������
			 strEsp8266_Fram_Record .InfBit .FramLength = 0;                             //�������ݳ�������
			 strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;                           //���ձ�־����
		}
    
  }
	
}


/*********************************************END OF FILE**********************/
