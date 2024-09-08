/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   液晶显示例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./dwt_delay/core_delay.h" 
#include "DHT11/bsp_dht11.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{
  char dispBuff[100];
  DHT11_Data_TypeDef DHT11_Data;
  
  LED_GPIO_Config();
	
	ILI9806G_Init ();         //LCD 初始化

	Debug_USART_Config();		
	
	printf("\r\n ********** 液晶屏英文显示程序*********** \r\n"); 
	printf("\r\n 本程序不支持中文，显示中文的程序请学习下一章 \r\n"); 

	//其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
  ILI9806G_GramScan ( 6 );

  /* DHT11 初始化 */
  DHT11_GPIO_Config();
  
  ILI9806G_DispStringLine_EN(LINE(0),"YH DHT11 test");
	    
		while(1)
	{	
			/*调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息*/
			if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
			{
        /* 显示温度 */
        sprintf(dispBuff,"Temperature : %d.%d ",DHT11_Data.temp_int, DHT11_Data.temp_deci);
        ILI9806G_ClearLine(LINE(1));	/* 清除单行文字 */
        ILI9806G_DispStringLine_EN(LINE(1),dispBuff);
        
        /* 显示湿度 */
        sprintf(dispBuff,"Humidity : %d.%d%% ",DHT11_Data.humi_int, DHT11_Data.humi_deci);
        ILI9806G_ClearLine(LINE(2));	/* 清除单行文字 */
        ILI9806G_DispStringLine_EN(LINE(2),dispBuff);
			}			
			else
			{
        LCD_ClearLine(LINE(1));	/* 清除单行文字 */
        LCD_ClearLine(LINE(2));	/* 清除单行文字 */
				ILI9806G_DispStringLine_EN(LINE(1),"Read DHT11 ERROR");
        ILI9806G_DispStringLine_EN(LINE(2),"Read DHT11 ERROR");
			}
			
		 CPU_TS_Tmr_Delay_MS(2000);
	}
	
	
}

/*********************************************END OF FILE**********************/

