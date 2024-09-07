/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   DS18B20�¶ȴ�������ȡ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ָ���� ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./ds18b20/bsp_ds18b20.h"
#include "./lcd/bsp_ili9341_lcd.h"

/* ��ʾ������ */
char dis_buf[1024];

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	float temperature;
  
	uint8_t uc, ucDs18b20Id [ 8 ];
  uint8_t DS18B20Id_str[20];
	
	/* ����SysTick Ϊ1us�ж�һ�� */
	SysTick_Init();
	
  //LCD ��ʼ��
	ILI9341_Init (); 
 //����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	ILI9341_GramScan ( 6 );

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	USART_Config();	//��ʼ������1
	
	
	ILI9341_DispStringLine_EN(LINE(0),"this is a ds18b20 test demo");
	
	while( DS18B20_Init() )	
		ILI9341_DispStringLine_EN(LINE(1),"no ds18b20 exit");
	
	ILI9341_DispStringLine_EN(LINE(1),"ds18b20 ok");

	DS18B20_ReadId ( ucDs18b20Id  );           // ��ȡ DS18B20 �����к�
	
	for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
  {    
    sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",ucDs18b20Id[uc]);  
    
    if(uc == 7)
      DS18B20Id_str[17] = '\0';        
  }

  sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
  ILI9341_DispStringLine_EN(LINE(2),dis_buf);
	
	for(;;)
	{	
		temperature=DS18B20_GetTemp_MatchRom(ucDs18b20Id);
        
    sprintf((char*)dis_buf,"T:%0.3f degree Celsius",temperature);
    ILI9341_DispStringLine_EN(LINE(5),dis_buf);

    Delay_ms(1000);
		
	}	 
	
	
}
/*********************************************END OF FILE**********************/
