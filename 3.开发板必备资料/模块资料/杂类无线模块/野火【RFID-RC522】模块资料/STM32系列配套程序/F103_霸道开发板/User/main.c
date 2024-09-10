/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   RFID-RC522模块
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./SysTick/bsp_SysTick.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>
#include "./key/bsp_key.h"  


extern uint16_t lcdid;


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
	uint32_t readValue ;
	char cStr [ 30 ];
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
	uint8_t ucStatusReturn;      /*返回状态*/
  
  /* 按照流程操作 RC522 ,否者会导致操作失败 */
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
          ILI9341_DispStringLine_EN(LINE(0) , (char* )cStr ); //在屏幕上面显示ID
          
          printf ("余额为：%d\r\n",readValue);
          sprintf ( cStr, "TThe residual amount: %d", readValue);				 										 	         
          ILI9341_DispStringLine_EN(LINE(1) , (char* )cStr ); //在屏幕上面显示金额
          PcdHalt();
        }
        
        /* 按键1检测 */
        if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
        {
          printf("金额 +100\r\n");
          writeValue += 100;
        }
        /* 按键2检测 */
        if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
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
int main(void)
{	
  /*滴答时钟初始化*/
  SysTick_Init ();
  
	/*LCD 初始化*/
	ILI9341_Init ();  
  
	/* USART config */
	USART_Config(); 
  
  /* Key 初始化 */
  Key_GPIO_Config();
  
  /*RC522模块所需外设的初始化配置*/
  RC522_Init ();     
	
  printf ( "WF-RC522 Test\n" );
	
  /*其中0、3、5、6 模式适合从左至右显示文字，*/
	ILI9341_GramScan ( 6 );
  LCD_SetFont(&Font8x16);
	LCD_SetColors(BLACK,BLACK);
  
  /* 清屏，显示全黑 */
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
  
	/********显示字符串示例*******/
  LCD_SetTextColor(RED);  

  ILI9341_DispStringLine_EN(LINE(18),
       (char* )"Please put the IC card on WF-RC522 antenna area ..."); 
  
  LCD_SetTextColor(YELLOW);
  
  PcdReset ();
   
  /*设置工作方式*/   
	M500PcdConfigISOType ( 'A' );

  while(1)
  { 
    /*IC卡检测	*/    
    IC_test ();     

  }

}


/* ------------------------------------------end of file---------------------------------------- */

