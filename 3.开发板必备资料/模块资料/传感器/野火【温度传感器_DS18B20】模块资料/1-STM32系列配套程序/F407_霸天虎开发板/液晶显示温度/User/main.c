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
#include "DS18B20/bsp_ds18b20.h"
#include "systick/bsp_SysTick.h"

/* ��ʾ������ */
char dis_buf[1024];

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{
  float temperature;
	uint8_t uc, ucDs18b20Id [ 8 ];
  uint8_t DS18B20Id_str[20];
	
	/* ����SysTick Ϊ1us�ж�һ�� */
	SysTick_Init();
	
  ILI9806G_Init ();         //LCD ��ʼ��

	Debug_USART_Config();		
	
	printf("\r\n ********** Һ����Ӣ����ʾ����*********** \r\n"); 
	printf("\r\n ������֧�����ģ���ʾ���ĵĳ�����ѧϰ��һ�� \r\n"); 

	//����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  ILI9806G_GramScan ( 6 );

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	ILI9806G_DispStringLine_EN(LINE(0),"this is a ds18b20 test demo");
	
	while( DS18B20_Init() )	
		ILI9806G_DispStringLine_EN(LINE(1),"no ds18b20 exit");
	
	ILI9806G_DispStringLine_EN(LINE(1),"ds18b20 ok");

	DS18B20_ReadId ( ucDs18b20Id  );           // ��ȡ DS18B20 �����к�
	
	for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
  {    
    sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",ucDs18b20Id[uc]);  
    
    if(uc == 7)
      DS18B20Id_str[17] = '\0';        
  }

  sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
  ILI9806G_DispStringLine_EN(LINE(2),dis_buf);
	
	for(;;)
	{	
		temperature=DS18B20_Get_Temp();
        
    sprintf((char*)dis_buf,"T:%0.3f degree Celsius",temperature);
    ILI9806G_DispStringLine_EN(LINE(5),dis_buf);
		
    CPU_TS_Tmr_Delay_MS(1000);
	}	 
}

/*********************************************END OF FILE**********************/

