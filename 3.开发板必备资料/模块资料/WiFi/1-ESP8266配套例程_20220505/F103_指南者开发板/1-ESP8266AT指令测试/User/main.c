/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����жϽ��ղ���
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
#include "bsp_usart.h"
#include "bsp_esp8266.h"
#include "core_delay.h" 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  USART_Config();
	ESP8266_Init();
  
	printf("��ӭʹ��Ұ��STM32������\n\n");
	printf("����һ��ESP8266ATָ�����ʵ��\n\n");
	printf("��ʹ�ô��ڵ������ַ���\"AT+���лس�\"����ESP8266�Ƿ�׼����\n\n");
	printf("����ATָ����ο�ģ������\n\n");
	printf("������ESP8266�ϵ��ʼ����ӡ����Ϣ\n\n");
  
  while(1)
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
