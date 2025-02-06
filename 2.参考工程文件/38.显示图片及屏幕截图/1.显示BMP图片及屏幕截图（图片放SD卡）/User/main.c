/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   LCD��ʾBMP�ļ�����ͼ
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
#include "./lcd/bsp_ili9341_lcd.h"
#include "./usart/bsp_usart.h" 
#include "./key/bsp_key.h"  
#include "./led/bsp_led.h"   
#include "./bmp/bsp_bmp.h"
#include "ff.h"
#include <stdio.h>


FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT res_sd;                /* �ļ�������� */

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{

	ILI9341_Init ();         //LCD ��ʼ��
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  ILI9341_GramScan ( 6 );
	
	USART_Config();		
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	printf("\r\n ********** ��ʾBMP�ļ�����ͼ*********** \r\n"); 
	printf("\r\n ʵ��ǰ��ȷ��SD����Ŀ¼�¾���ʵ��ʹ�õ�BMPͼƬ�ļ�, \r\n"); 
	printf("\r\n ���°��ӵ�KEY1��������������Ļ��ͼ \r\n"); 
	
	/*����sd�ļ�ϵͳ*/
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd != FR_OK)
	{
		printf("\r\n�������������Ѹ�ʽ����fat��ʽ��SD����\r\n");
	}
	
	ILI9341_GramScan ( 6 );
	/*��ʾSD���ڵ�BMP�ļ�����ȷ��SD����·�����и��ļ���
	���ڹ���Ŀ¼�µġ���Ҫ������SD�������ļ����и��Ƶ�SD���ĸ�Ŀ¼*/
	LCD_Show_BMP(0,0,"0:wildfire_1.bmp");  
	

	while ( 1 )
	{
		/*��ⰴ��*/
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{		
			static uint8_t name_count = 0;
			char name[40];
			
			//�������ý�ͼ���֣���ֹ�ظ���ʵ��Ӧ���п���ʹ��ϵͳʱ����������
			name_count++; 
			sprintf(name,"0:screen_shot_%d.bmp",name_count);

			LED_BLUE;
			printf("\r\n���ڽ�ͼ...");
			
			/*��ͼ�������ú�Һ����ʾ����ͽ�ͼ����*/
			ILI9341_GramScan ( 6 );			
			
			if(Screen_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name) == 0)
			{
				printf("\r\n��ͼ�ɹ���");
				LED_GREEN;
			}
			else
			{
				printf("\r\n��ͼʧ�ܣ�");
				LED_RED;
			}
		}
	}	
}




/* ------------------------------------------end of file---------------------------------------- */

