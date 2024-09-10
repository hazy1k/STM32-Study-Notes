/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   RFID-RC522模块实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>
#include "bsp_lcd.h"
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
          ILI9341_DispString_EN ( 0, 0, cStr, macBACKGROUND, macYELLOW );
					
					printf ("余额为：%d\r\n",readValue);
					sprintf ( cStr, "TThe residual amount: %d", readValue);				 										 	         
					ILI9341_DispString_EN ( 0, 16, cStr, macBACKGROUND, macYELLOW );
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
int main ( void )
{
  /*滴答时钟初始化*/
	SysTick_Init ();   
	
  /*USART1 配置模式为 115200 8-N-1，中断接收*/
	USART1_Config ();   
	
  /* Key 初始化 */
  Key_GPIO_Config();
  
  /*RC522模块所需外设的初始化配置*/
	RC522_Init ();     
	
   /*LCD 初始化*/
	LCD_Init ();      
		
	printf ( "WF-RC522 Test\n" );
		
	ILI9341_Clear ( 0,   0, 240, 320, macBACKGROUND);	
	ILI9341_Clear ( 0, 272, 240,   2, macGREEN);
	ILI9341_DispString_EN ( 0, 280, 
      "Please put the IC card on WF-RC522 antenna area ...",
                  macBACKGROUND,
                   macRED );
	PcdReset ();
  
  /*设置工作方式*/
	M500PcdConfigISOType ( 'A' );
	
  while ( 1 )
  {
    /*IC卡检测*/
    IC_test ();                	
  }	    
}
/****************************END OF FILE**********************/

