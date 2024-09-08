/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   dht11温湿度传感器测试实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "./usart/bsp_usart.h"
#include "./lcd/bsp_ili9341_lcd.h"

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
  char dispBuff[100];
  
	DHT11_Data_TypeDef DHT11_Data;

	/* 初始化系统定时器 */
	SysTick_Init();
  
  //LCD 初始化
	ILI9341_Init (); 
 //其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9341_GramScan ( 6 );

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  
	/*初始化DTT11的引脚*/
	DHT11_Init ();
	
	while(1)
	{	
			/*调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息*/
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
        ILI9341_DispStringLine_EN(LINE(0),"YH DHT11 test");
        
        /* 显示温度 */
        sprintf(dispBuff,"Temperature : %d.%d ",DHT11_Data.temp_int, DHT11_Data.temp_deci);
        LCD_ClearLine(LINE(1));	/* 清除单行文字 */
        ILI9341_DispStringLine_EN(LINE(1),dispBuff);
        
        /* 显示湿度 */
        sprintf(dispBuff,"Humidity : %d.%d%% ",DHT11_Data.humi_int, DHT11_Data.humi_deci);
        LCD_ClearLine(LINE(2));	/* 清除单行文字 */
        ILI9341_DispStringLine_EN(LINE(2),dispBuff);
			}			
			else
			{
        LCD_ClearLine(LINE(1));	/* 清除单行文字 */
        LCD_ClearLine(LINE(2));	/* 清除单行文字 */
				ILI9341_DispStringLine_EN(LINE(1),"Read DHT11 ERROR");
        ILI9341_DispStringLine_EN(LINE(2),"Read DHT11 ERROR");
			}
			
		 Delay_ms(2000);
	}

	
}
/*********************************************END OF FILE**********************/
