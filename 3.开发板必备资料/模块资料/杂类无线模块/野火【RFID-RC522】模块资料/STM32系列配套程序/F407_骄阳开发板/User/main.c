/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   RFID RC522测试程序
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 骄阳 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./RC522/rc522_config.h"
#include "./RC522/rc522_function.h"


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
          ILI9806G_DispStringLine_EN(LINE(0) , (char* )cStr ); //在屏幕上面显示ID
          
          printf ("余额为：%d\r\n",readValue);
          sprintf ( cStr, "TThe residual amount: %d", readValue);				 										 	         
          ILI9806G_DispStringLine_EN(LINE(1) , (char* )cStr ); //在屏幕上面显示金额
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
int main(void)
{
	/* 系统时钟初始化成168MHz */
	SystemClock_Config();
   //初始化systick 
	SysTick_Init();
	/* LED 端口初始化 */	
	LED_GPIO_Config();
	/* 按键初始化 */	
	Key_GPIO_Config(); 
	/* 配置串口1为：115200 8-N-1 */
	DEBUG_USART_Config();
	/*RC522模块所需外设初始化*/
	RC522_Init ();       
  /* 屏幕初始化 */
	ILI9806G_Init();
	printf ( "WF-RC522 Test\n" );
	
	ILI9806G_GramScan ( 6 );
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);
  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	ILI9806G_DispStringLine_EN(LINE(1) ,
             "Please put the IC card on WF-RC522 antenna area ..." );
	
	PcdReset ();
	M500PcdConfigISOType ( 'A' );//设置工作方式
		
  while ( 1 )
  {
    IC_test ();//IC卡检测	
  }	
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
 void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
