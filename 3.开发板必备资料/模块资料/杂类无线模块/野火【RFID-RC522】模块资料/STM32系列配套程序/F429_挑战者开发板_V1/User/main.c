/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RFID-R522模块
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>
#include "./lcd/bsp_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "./font/fonts.h"
#include "./sdram/bsp_sdram.h"
#include "./key/bsp_key.h" 


uint8_t KeyValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF};   // 卡A密钥
//uint8_t KeyValue[]={'1' ,'2', '3', '4', '5', '6'};   // 卡A密钥


/**
  * @brief  IC测试函数
  * @param  无
  * @retval 无
  */
void IC_test ( void )
{
	uint32_t writeValue = 100;
	uint32_t readValue;
	char cStr [ 30 ];
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
	uint8_t ucStatusReturn;      /*返回状态*/                                                                                           
  while ( 1 )
  {    
    /*寻卡*/
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )
    {
      /*若失败再次寻卡*/
      printf ( "寻卡失败\n" );
			ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ); //PICC_REQALL   PICC_REQIDL
    }		                                                

		if ( ucStatusReturn == MI_OK  )
		{
      //printf ( "寻卡成功\n" );
      /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
			if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
			{
				PcdSelect(ucArray_ID);			
		
				PcdAuthState( PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID );//校验密码 
        WriteAmount(0x11,writeValue); //写入金额
        if(ReadAmount(0x11,&readValue) == MI_OK)	//读取金额
				{		
					//writeValue +=100;
				  sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",ucArray_ID [0], ucArray_ID [1], ucArray_ID [2],ucArray_ID [3] );
					printf ( "%s\r\n",cStr );  //打印卡片ID
          LCD_DisplayStringLine(LINE(0), (uint8_t*)cStr);
					
					printf ("余额为：%d\r\n",readValue);
					sprintf ( cStr, "TThe residual amount: %d", readValue);				 										 	         
					LCD_DisplayStringLine(LINE(1) , (uint8_t*)cStr);
          PcdHalt();
				}
        
        /* 按键1检测 */
        if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
        {
          printf("金额 +100\r\n");
          writeValue += 100;
        }
        /* 按键2检测 */
        if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
        {
          printf("金额 -100\r\n");
          writeValue -= 100;
        }
			}				
		}		
		    
  }	
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
  /*滴答时钟初始化*/
  SysTick_Init();   

  /*USART1 配置模式为 115200 8-N-1，中断接收*/
  Debug_USART_Config();   

  /* Key 初始化 */
  Key_GPIO_Config();

  /*RC522模块所需外设的初始化配置*/
  RC522_Init ();     

  /*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();

  printf ( "WF-RC522 Test\n" );

  LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
  
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
  LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
  
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/		
  LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
  
  LCD_SetTextColor(LCD_COLOR_BLUE);

  LCD_DisplayStringLine(LINE(14),
               (uint8_t*)"Please put the IC card on WF-RC522 antenna area ..." );

  PcdReset ();
  M500PcdConfigISOType ( 'A' );/*设置工作方式*/

  while ( 1 )
  {
    IC_test ();/*IC卡检测*/	
  }	
}



/*********************************************END OF FILE**********************/

