/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   LCD��ʾJPG�ļ�
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
#include "./led/bsp_led.h"   
#include "ff.h"
#include <stdio.h>


extern void jpgDisplay(char *pic_name);


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

	LED_BLUE;
	
	//�����̱���� jpg�������һЩwarning����Ӱ��ʹ�á�
	//��ֲ������ʱ����ע�������ļ��Ķ�ջ��С
	printf("\r\n ********** ��ʾJPG�ļ�*********** \r\n"); 
	printf("\r\n ʵ��ǰ��ȷ��SD����Ŀ¼�¾���ʵ��ʹ�õ�JPGͼƬ�ļ�, \r\n"); 
	
	/*����sd�ļ�ϵͳ*/
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd != FR_OK)
	{
		printf("\r\n�������������Ѹ�ʽ����fat��ʽ��SD����\r\n");
	}
	
	ILI9341_GramScan ( 6 );
	/*��ʾSD���ڵ�JPG�ļ�����ȷ��SD����·�����и��ļ���
	���ڹ���Ŀ¼�µġ���Ҫ������SD�������ļ����и��Ƶ�SD���ĸ�Ŀ¼*/
	
	//jpgDisplay("0:/Image/Logo.jpg");
	jpgDisplay("0:/one_piece.jpg");

	while ( 1 )
	{
	}
}




/* ------------------------------------------end of file---------------------------------------- */

