/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDCҺ����ʾ����(�ֿ����ⲿFLASH)
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
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "DHT11/bsp_dht11.h"
#include <string.h>

/* ��ʾ������ */
uint8_t dis_buf[1024];


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  DHT11_Data_TypeDef DHT11_Data;
  
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
	/*���ڳ�ʼ��*/
	Debug_USART_Config();
	
  /*��ʼ��Һ����*/
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
	
	/*�ѱ�����ˢ��ɫ*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*��ʼ����Ĭ��ʹ��ǰ����*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*Ĭ�����ò�͸��	���ú�������Ϊ��͸���ȣ���Χ 0-0xff ��0Ϊȫ͸����0xffΪ��͸��*/
  LCD_SetTransparency(0xFF);
  LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);

	LCD_Clear(LCD_COLOR_BLACK); 

	/*����LCD_SetLayer(LCD_FOREGROUND_LAYER)������
	����Һ����������ǰ����ˢ�£��������µ��ù�LCD_SetLayer�������ñ�����*/
		
	/* ��ʼ��ϵͳ��ʱ�� */
	SysTick_Init(); 
	/* DHT11��ʼ�� */
	DHT11_GPIO_Config();

 	LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t* )"DHT11 temperature & humidity detect demo");
	    
  LED_BLUE;    

  while(1)
	{
		/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
		if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d %%RH ���¶�Ϊ %d.%d�� \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
      
      LCD_ClearLine(LINE(2));
      LCD_ClearLine(LINE(3));
//      LCD_ClearLine(LINE(4));
//      LCD_ClearLine(LINE(5));
      
      sprintf((char*)dis_buf,"humidity: %2d.%2d %%RH ",DHT11_Data.humi_int,DHT11_Data.humi_deci);
      LCD_DisplayStringLine_EN_CH(LINE(4),dis_buf);
      
      sprintf((char*)dis_buf,"temperature: %2d.%2d degree Celsius ",DHT11_Data.temp_int,DHT11_Data.temp_deci);
      LCD_DisplayStringLine_EN_CH(LINE(5),dis_buf);
		}		
		else
		{
		  printf("Read DHT11 ERROR!\r\n");
      
      LCD_SetTextColor(LCD_COLOR_RED);
      LCD_DisplayStringLine_EN_CH(LINE(2),(uint8_t* )"DHT11 initialization failed!");
      LCD_DisplayStringLine_EN_CH(LINE(3),(uint8_t* )"Please check the connection!");	
		}
    Delay_ms(2000);
	} 

}



/*********************************************END OF FILE**********************/

