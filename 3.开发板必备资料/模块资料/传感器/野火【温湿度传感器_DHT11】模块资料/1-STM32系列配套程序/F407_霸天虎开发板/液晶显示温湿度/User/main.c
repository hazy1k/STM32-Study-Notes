/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   Һ����ʾ����
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
#include "./led/bsp_led.h"  
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./dwt_delay/core_delay.h" 
#include "DHT11/bsp_dht11.h"


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{
  char dispBuff[100];
  DHT11_Data_TypeDef DHT11_Data;
  
  LED_GPIO_Config();
	
	ILI9806G_Init ();         //LCD ��ʼ��

	Debug_USART_Config();		
	
	printf("\r\n ********** Һ����Ӣ����ʾ����*********** \r\n"); 
	printf("\r\n ������֧�����ģ���ʾ���ĵĳ�����ѧϰ��һ�� \r\n"); 

	//����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  ILI9806G_GramScan ( 6 );

  /* DHT11 ��ʼ�� */
  DHT11_GPIO_Config();
  
  ILI9806G_DispStringLine_EN(LINE(0),"YH DHT11 test");
	    
		while(1)
	{	
			/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
			if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
			{
        /* ��ʾ�¶� */
        sprintf(dispBuff,"Temperature : %d.%d ",DHT11_Data.temp_int, DHT11_Data.temp_deci);
        ILI9806G_ClearLine(LINE(1));	/* ����������� */
        ILI9806G_DispStringLine_EN(LINE(1),dispBuff);
        
        /* ��ʾʪ�� */
        sprintf(dispBuff,"Humidity : %d.%d%% ",DHT11_Data.humi_int, DHT11_Data.humi_deci);
        ILI9806G_ClearLine(LINE(2));	/* ����������� */
        ILI9806G_DispStringLine_EN(LINE(2),dispBuff);
			}			
			else
			{
        LCD_ClearLine(LINE(1));	/* ����������� */
        LCD_ClearLine(LINE(2));	/* ����������� */
				ILI9806G_DispStringLine_EN(LINE(1),"Read DHT11 ERROR");
        ILI9806G_DispStringLine_EN(LINE(2),"Read DHT11 ERROR");
			}
			
		 CPU_TS_Tmr_Delay_MS(2000);
	}
	
	
}

/*********************************************END OF FILE**********************/

