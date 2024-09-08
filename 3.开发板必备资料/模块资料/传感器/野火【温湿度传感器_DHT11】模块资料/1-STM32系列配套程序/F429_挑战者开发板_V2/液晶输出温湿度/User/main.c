/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LTDC液晶显示汉字(字库在外部FLASH)
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./sdram/bsp_sdram.h"
#include "./lcd/bsp_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "DHT11/bsp_dht11.h"
#include <string.h>

/* 显示缓冲区 */
uint8_t dis_buf[1024];


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  DHT11_Data_TypeDef DHT11_Data;
  
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
	/*串口初始化*/
	Debug_USART_Config();
	
  /*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
  LCD_SetTransparency(0xFF);
  LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);

	LCD_Clear(LCD_COLOR_BLACK); 

	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
		
	/* 初始化系统定时器 */
	SysTick_Init(); 
	/* DHT11初始化 */
	DHT11_GPIO_Config();

 	LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t* )"DHT11 temperature & humidity detect demo");
	    
  LED_BLUE;    

  while(1)
	{
		/*调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息*/
		if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d %%RH ，温度为 %d.%d℃ \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
      
      LCD_ClearLine(LINE(2));
      LCD_ClearLine(LINE(3));
//      LCD_ClearLine(LINE(4));
//      LCD_ClearLine(LINE(5));
      
      sprintf((char*)dis_buf,"humidity: %2d.%2d %%RH ",DHT11_Data.humi_int,DHT11_Data.humi_deci);
      LCD_DisplayStringLine_EN_CH(LINE(4),dis_buf);
      
      sprintf((char*)dis_buf,"temperature: %2d.%2d degree Celsius ",DHT11_Data.temp_int,DHT11_Data.temp_deci);
      LCD_DisplayStringLine_EN_CH(LINE(5),dis_buf);
		}		
		else
		{
		  printf("Read DHT11 ERROR!\r\n");
      
      LCD_SetTextColor(LCD_COLOR_RED);
      LCD_DisplayStringLine_EN_CH(LINE(2),(uint8_t* )"DHT11 initialization failed!");
      LCD_DisplayStringLine_EN_CH(LINE(3),(uint8_t* )"Please check the connection!");	
		}
    Delay_ms(2000);
	} 

}



/*********************************************END OF FILE**********************/

