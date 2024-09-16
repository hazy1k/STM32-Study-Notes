/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief    RTC ������ʵ��
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
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./key/bsp_key.h"  


// N = 2^32/365/24/60/60 = 136 ��

/*ʱ��ṹ�壬Ĭ��ʱ��2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

extern __IO uint32_t TimeDisplay ;



//��*��ע�����
//��bsp_rtc.h�ļ��У�

//1.�����ú�USE_LCD_DISPLAY�����Ƿ�ʹ��LCD��ʾ
//2.�����ú�RTC_CLOCK_SOURCE_LSI��RTC_CLOCK_SOURCE_LSE����ʹ��LSE������LSI����

//3.STM32��LSE����Ҫ��ǳ��ϸ�ͬ���ĵ�·������������Ʒʱ����Щ��������⡣
//  ��ʵ����Ĭ��ʹ��LSI����
//  
//4.��������ϣ��RTC������Դ�������Ȼ���У���Ҫ��������ĵ�ز۰�װť�۵�أ�
//  �������Ҹĳ�ʹ���ⲿ����ģʽRTC_CLOCK_SOURCE_LSE
//  ť�۵���ͺţ�CR1220
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main()
{		
	
//��ʹ�øú������Ƿ�ʹ��Һ����ʾ
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();         //LCD ��ʼ��
		LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

		ILI9341_DispStringLine_EN(LINE(0),"        BH RTC demo");
#endif
	
	  USART_Config();		
	
		Key_GPIO_Config();

		/* ����RTC���ж����ȼ� */
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	
	  while (1)
	  {
	    /* ÿ��1s ����һ��ʱ��*/
	    if (TimeDisplay == 1)
	    {
				/* ��ǰʱ�� */
	      Time_Display( RTC_GetCounter(),&systmtime); 		  
	      TimeDisplay = 0;
	    }
			
			//���°�����ͨ�������޸�ʱ��
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;

				/*ʹ�ô��ڽ������õ�ʱ�䣬��������ʱע��ĩβҪ�ӻس�*/
				Time_Regulate_Get(&set_time);
				/*�ý��յ���ʱ������RTC*/
				Time_Adjust(&set_time);
				
				//�򱸷ݼĴ���д���־
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

			} 			
	  }
}

/***********************************END OF FILE*********************************/

