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
#include "systick/bsp_SysTick.h"
#include "DS18B20/bsp_ds18b20.h"
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
  float temperature;

  uint8_t uc,DS18B20Id[8];
  uint8_t DS18B20Id_str[20];
  
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
		
	/* 系统定时器初始化 */
	SysTick_Init();  

 	LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t* )"DS18B20 temperature detect demo");
	
	if(DS18B20_Init()==0)
	{
		printf("DS18B20初始化成功\n");
	}
	else
	{    
		printf("DS18B20初始化失败\n");
		printf("请将传感器正确插入到插槽内\n");
    
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine_EN_CH(LINE(2),(uint8_t* )"DS18B20 initialization failed!");
    LCD_DisplayStringLine_EN_CH(LINE(3),(uint8_t* )"Please check the connection!");
		/* 停机 */
		while(1)
		{}			
	}		
    
  DS18B20_ReadId ( DS18B20Id  );           // 读取 DS18B20 的序列号	
    
	for ( uc = 0; uc < 8; uc++ )             // 打印 DS18B20 的序列号
  {    
    sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",DS18B20Id[uc]);  
    
    if(uc == 7)
      DS18B20Id_str[17] = '\0';        
  }
  
  printf("\r\nDS18B20的序列号是： 0x%s\r\n",DS18B20Id_str);

  sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
  LCD_DisplayStringLine_EN_CH(LINE(4),dis_buf);

  LED_BLUE;    

  while(1)
	{
		temperature=DS18B20_Get_Temp();
		printf("DS18B20读取到的温度为：%0.3f\n",temperature);
        
    sprintf((char*)dis_buf,"Temperature:   %0.3f   degree Celsius",temperature);
    LCD_DisplayStringLine_EN_CH(LINE(5),dis_buf);

    Delay_ms(1000);
	}  
}



/*********************************************END OF FILE**********************/

