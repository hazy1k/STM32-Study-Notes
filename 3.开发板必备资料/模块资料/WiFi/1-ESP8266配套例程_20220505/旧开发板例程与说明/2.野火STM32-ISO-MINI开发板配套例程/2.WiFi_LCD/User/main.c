/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   WF-ESP8266 WiFiģ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_bmp.h"
#include "bsp_SysTick.h"
#include "bsp_touch.h"
#include "bsp_spi_flash.h"
#include "even_process.h"


#include "bsp_lcd_keyboard.h"
#include <stdio.h>

#include "wifi_config.h"
#include "wifi_function.h"


void ( * pNet_Test ) ( void );


/*
��������ϵͳ��ת����


��������
				column 240
			 
			 x
        _ _ _ _ _ _
       ^           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
	(0,0) - - - - - ->  y       
				
Һ����ʾ���弰�����Ⱥ�����
				
				column 240
			 
										x
  (0,0) ----------->
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 V- - - - - - 

       y
			 
Һ����ʾͼ��
				
				column 240		 
									 X	
				-----------A
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
		 Y <- - - - - -(0,0) 

       

ת����ϵ��
Y�� = 320 - X����
X�� = Y����

Xͼ�� = X����
Yͼ�� = 240-Y����

Yͼ�� = 240 - X��
Xͼ�� = 320 - Y��


*/




/**
  * @brief  LCD�������Ƽ���ʾ���̣�
	*					ʾ�����ʹ�ô��������˻������Լ�ʹ��LCD��ʾ����
  * @param  ��
  * @retval ��
 */
int main(void)
{
		uint8_t k;
	  char * pStr;
	
		/* ��ʼ��LCD */
		LCD_Init();	
		LCD_Clear( 0, 0, 240, 320, BLACK );		
		
		/* ��ʼ��sd���ļ�ϵͳ����Ϊ���ֵ��ֿ������sd������ */
		Sd_fs_init();	
	
		/* ��ʼ������ */
	  USART1_Config();
	
		/* ϵͳ��ʱ�� 1us ��ʱ��ʼ�� */
	 	SysTick_Init();				

		/* ��ʼ���ⲿFLASH */
		SPI_FLASH_Init();
		//SPI_FLASH_SectorErase(0);
  
		/* ������IO���жϳ�ʼ�� */
		Touch_Init();			
		
		/* WiFiģ���йس�ʼ�� */
		WiFi_Config ();
		
		/*-------------------------------------------------------------------------------------------------------*/  
		#if 1
		SPI_FLASH_BufferRead(&cal_flag, 0, 1);
		if( cal_flag == 0x55 )
		{
			SPI_FLASH_BufferRead((void*)cal_p, 1, sizeof(cal_p));
			SPI_FLASH_CS_HIGH();
			for( k=0; k<6; k++ )
						printf("\r\n rx = %LF \r\n",cal_p[k]);
		}
		else
		{
			/* �ȴ�������У����� */
			while(Touch_Calibrate() !=0);
		}
		#elif 0
		/* �ȴ�������У����� */
		while(Touch_Calibrate() !=0);
		#endif
		

		/* ��ʾLCD���ƽ���ͼƬ */
		LCD_Clear ( 0, 0, 320, 240, BLACK );
		Lcd_show_bmp( 0, 0, ( unsigned char * ) "/Keyboard/InitScreen.bmp" );
		Lcd_GramScan ( 2 );
		
		
		macClear_DisplayFrame();
		LCD_DispEnCh (  48,  38, "��ӭʹ��Ұ��WF-ESP8266 WiFiģ��!", BLUE );
		LCD_DispEnCh ( 168,  90, "��\"Enter\"���������.",           BLUE );
		inWaiteInput ();
		
		
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "������ģ��Ĳ���ģʽ�ı��,�������:"            , BLUE );
		LCD_DispEnCh ( 0, 18, "1.STAģʽ: TCP Client"                         , BLUE );
		LCD_DispEnCh ( 0, 36, "2.APģʽ: TCP Server"                          , BLUE );
		LCD_DispEnCh ( 0, 54, "3.STA+APģʽ: (STA)TCP Client + (AP)TCP Server", BLUE );
		LCD_DispEnCh ( 0, 72, "��\"Enter\"��ȷ��.",                             BLUE );
    
    macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
//		pStr = "1";//
		
		if ( strcmp ( ( const char * ) pStr, "1" ) == 0 )
		{
			pNet_Test = ESP8266_STA_TCP_Client;
			macClear_DisplayFrame();
		  LCD_DispEnCh (  0,  48, "�������еĲ�����:",      BLUE );
			LCD_DispEnCh ( 32,  66, "1.STAģʽ: TCP Client", BLUE );
		}
		
		else if ( strcmp ( ( const char * ) pStr, "2" ) == 0 )
		{
			pNet_Test = ESP8266_AP_TCP_Server;
			macClear_DisplayFrame();
		  LCD_DispEnCh (  0,  48, "�������еĲ�����:2.APģʽ: TCP Server", BLUE );
			LCD_DispEnCh ( 32,  66, "2.APģʽ: TCP Server",                 BLUE );
		}
		
		else if ( strcmp ( ( const char * ) pStr, "3" ) == 0 )
		{
			pNet_Test = ESP8266_StaTcpClient_ApTcpServer;
			macClear_DisplayFrame();
		  LCD_DispEnCh (  0,  48, "�������еĲ�����:",                               BLUE );
			LCD_DispEnCh ( 32,  66, "3.STA+APģʽ: (STA)TCP Client + (AP)TCP Server", BLUE );
		}
		
		else
		{
			pNet_Test = NULL;
			macClear_DisplayFrame();
		  LCD_DispEnCh ( 0,  56, "�������,�����в���!", BLUE );
		}
		
		Delay_ms ( 3000 );
		
		if ( pNet_Test )
		  pNet_Test ();
	
		
		while ( 1 );

}


/******************* end of file**************************/

