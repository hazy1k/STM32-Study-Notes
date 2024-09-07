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
#include "systick/bsp_SysTick.h"
#include "DS18B20/bsp_ds18b20.h"
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
  float temperature;

  uint8_t uc,DS18B20Id[8];
  uint8_t DS18B20Id_str[20];
  
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
		
	/* ϵͳ��ʱ����ʼ�� */
	SysTick_Init();  

 	LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t* )"DS18B20 temperature detect demo");
	
	if(DS18B20_Init()==0)
	{
		printf("DS18B20��ʼ���ɹ�\n");
	}
	else
	{    
		printf("DS18B20��ʼ��ʧ��\n");
		printf("�뽫��������ȷ���뵽�����\n");
    
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine_EN_CH(LINE(2),(uint8_t* )"DS18B20 initialization failed!");
    LCD_DisplayStringLine_EN_CH(LINE(3),(uint8_t* )"Please check the connection!");
		/* ͣ�� */
		while(1)
		{}			
	}		
    
  DS18B20_ReadId ( DS18B20Id  );           // ��ȡ DS18B20 �����к�	
    
	for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
  {    
    sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",DS18B20Id[uc]);  
    
    if(uc == 7)
      DS18B20Id_str[17] = '\0';        
  }
  
  printf("\r\nDS18B20�����к��ǣ� 0x%s\r\n",DS18B20Id_str);

  sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
  LCD_DisplayStringLine_EN_CH(LINE(4),dis_buf);

  LED_BLUE;    

  while(1)
	{
		temperature=DS18B20_Get_Temp();
		printf("DS18B20��ȡ�����¶�Ϊ��%0.3f\n",temperature);
        
    sprintf((char*)dis_buf,"Temperature:   %0.3f   degree Celsius",temperature);
    LCD_DisplayStringLine_EN_CH(LINE(5),dis_buf);

    Delay_ms(1000);
	}  
}



/*********************************************END OF FILE**********************/

