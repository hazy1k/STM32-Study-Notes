/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   dht11��ʪ�ȴ���������ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "./usart/bsp_usart.h"
#include "./lcd/bsp_ili9341_lcd.h"

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
  char dispBuff[100];
  
	DHT11_Data_TypeDef DHT11_Data;

	/* ��ʼ��ϵͳ��ʱ�� */
	SysTick_Init();
  
  //LCD ��ʼ��
	ILI9341_Init (); 
 //����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	ILI9341_GramScan ( 6 );

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  
	/*��ʼ��DTT11������*/
	DHT11_Init ();
	
	while(1)
	{	
			/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
        ILI9341_DispStringLine_EN(LINE(0),"YH DHT11 test");
        
        /* ��ʾ�¶� */
        sprintf(dispBuff,"Temperature : %d.%d ",DHT11_Data.temp_int, DHT11_Data.temp_deci);
        LCD_ClearLine(LINE(1));	/* ����������� */
        ILI9341_DispStringLine_EN(LINE(1),dispBuff);
        
        /* ��ʾʪ�� */
        sprintf(dispBuff,"Humidity : %d.%d%% ",DHT11_Data.humi_int, DHT11_Data.humi_deci);
        LCD_ClearLine(LINE(2));	/* ����������� */
        ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			}			
			else
			{
        LCD_ClearLine(LINE(1));	/* ����������� */
        LCD_ClearLine(LINE(2));	/* ����������� */
				ILI9341_DispStringLine_EN(LINE(1),"Read DHT11 ERROR");
        ILI9341_DispStringLine_EN(LINE(2),"Read DHT11 ERROR");
			}
			
		 Delay_ms(2000);
	}

	
}
/*********************************************END OF FILE**********************/
