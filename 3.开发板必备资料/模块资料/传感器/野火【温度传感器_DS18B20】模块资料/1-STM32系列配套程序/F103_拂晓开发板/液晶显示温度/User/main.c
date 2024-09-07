/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   DS18B20温度传感器读取。
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 指南者 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./ds18b20/bsp_ds18b20.h"
#include "./lcd/bsp_ili9341_lcd.h"

/* 显示缓冲区 */
char dis_buf[1024];

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	float temperature;
  
	uint8_t uc, ucDs18b20Id [ 8 ];
  uint8_t DS18B20Id_str[20];
	
	/* 配置SysTick 为1us中断一次 */
	SysTick_Init();
	
  //LCD 初始化
	ILI9341_Init (); 
 //其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9341_GramScan ( 6 );

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	USART_Config();	//初始化串口1
	
	
	ILI9341_DispStringLine_EN(LINE(0),"this is a ds18b20 test demo");
	
	while( DS18B20_Init() )	
		ILI9341_DispStringLine_EN(LINE(1),"no ds18b20 exit");
	
	ILI9341_DispStringLine_EN(LINE(1),"ds18b20 ok");

	DS18B20_ReadId ( ucDs18b20Id  );           // 读取 DS18B20 的序列号
	
	for ( uc = 0; uc < 8; uc++ )             // 打印 DS18B20 的序列号
  {    
    sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",ucDs18b20Id[uc]);  
    
    if(uc == 7)
      DS18B20Id_str[17] = '\0';        
  }

  sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
  ILI9341_DispStringLine_EN(LINE(2),dis_buf);
	
	for(;;)
	{	
		temperature=DS18B20_GetTemp_MatchRom(ucDs18b20Id);
        
    sprintf((char*)dis_buf,"T:%0.3f degree Celsius",temperature);
    ILI9341_DispStringLine_EN(LINE(5),dis_buf);

    Delay_ms(1000);
		
	}	 
	
	
}
/*********************************************END OF FILE**********************/
