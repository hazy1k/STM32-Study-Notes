/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   WF-ESP8266 WiFi模块测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
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
三个坐标系统的转换：


触摸屏：
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
				
液晶显示字体及清屏等函数：
				
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
			 
液晶显示图像：
				
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

       

转换关系：
Y字 = 320 - X触摸
X字 = Y触摸

X图像 = X触摸
Y图像 = 240-Y触摸

Y图像 = 240 - X字
X图像 = 320 - Y字


*/




/**
  * @brief  LCD触摸控制及显示例程，
	*					示范如何使用触摸进行人机交互以及使用LCD显示变量
  * @param  无
  * @retval 无
 */
int main(void)
{
		uint8_t k;
	  char * pStr;
	
		/* 初始化LCD */
		LCD_Init();	
		LCD_Clear( 0, 0, 240, 320, BLACK );		
		
		/* 初始化sd卡文件系统，因为汉字的字库放在了sd卡里面 */
		Sd_fs_init();	
	
		/* 初始化串口 */
	  USART1_Config();
	
		/* 系统定时器 1us 定时初始化 */
	 	SysTick_Init();				

		/* 初始化外部FLASH */
		SPI_FLASH_Init();
		//SPI_FLASH_SectorErase(0);
  
		/* 触摸屏IO和中断初始化 */
		Touch_Init();			
		
		/* WiFi模块有关初始化 */
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
			/* 等待触摸屏校正完毕 */
			while(Touch_Calibrate() !=0);
		}
		#elif 0
		/* 等待触摸屏校正完毕 */
		while(Touch_Calibrate() !=0);
		#endif
		

		/* 显示LCD控制界面图片 */
		LCD_Clear ( 0, 0, 320, 240, BLACK );
		Lcd_show_bmp( 0, 0, ( unsigned char * ) "/Keyboard/InitScreen.bmp" );
		Lcd_GramScan ( 2 );
		
		
		macClear_DisplayFrame();
		LCD_DispEnCh (  48,  38, "欢迎使用野火WF-ESP8266 WiFi模块!", BLUE );
		LCD_DispEnCh ( 168,  90, "按\"Enter\"键进入测试.",           BLUE );
		inWaiteInput ();
		
		
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "请输入模块的测试模式的编号,编号如下:"            , BLUE );
		LCD_DispEnCh ( 0, 18, "1.STA模式: TCP Client"                         , BLUE );
		LCD_DispEnCh ( 0, 36, "2.AP模式: TCP Server"                          , BLUE );
		LCD_DispEnCh ( 0, 54, "3.STA+AP模式: (STA)TCP Client + (AP)TCP Server", BLUE );
		LCD_DispEnCh ( 0, 72, "按\"Enter\"键确认.",                             BLUE );
    
    macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
//		pStr = "1";//
		
		if ( strcmp ( ( const char * ) pStr, "1" ) == 0 )
		{
			pNet_Test = ESP8266_STA_TCP_Client;
			macClear_DisplayFrame();
		  LCD_DispEnCh (  0,  48, "即将进行的测试是:",      BLUE );
			LCD_DispEnCh ( 32,  66, "1.STA模式: TCP Client", BLUE );
		}
		
		else if ( strcmp ( ( const char * ) pStr, "2" ) == 0 )
		{
			pNet_Test = ESP8266_AP_TCP_Server;
			macClear_DisplayFrame();
		  LCD_DispEnCh (  0,  48, "即将进行的测试是:2.AP模式: TCP Server", BLUE );
			LCD_DispEnCh ( 32,  66, "2.AP模式: TCP Server",                 BLUE );
		}
		
		else if ( strcmp ( ( const char * ) pStr, "3" ) == 0 )
		{
			pNet_Test = ESP8266_StaTcpClient_ApTcpServer;
			macClear_DisplayFrame();
		  LCD_DispEnCh (  0,  48, "即将进行的测试是:",                               BLUE );
			LCD_DispEnCh ( 32,  66, "3.STA+AP模式: (STA)TCP Client + (AP)TCP Server", BLUE );
		}
		
		else
		{
			pNet_Test = NULL;
			macClear_DisplayFrame();
		  LCD_DispEnCh ( 0,  56, "输入错误,不进行测试!", BLUE );
		}
		
		Delay_ms ( 3000 );
		
		if ( pNet_Test )
		  pNet_Test ();
	
		
		while ( 1 );

}


/******************* end of file**************************/

