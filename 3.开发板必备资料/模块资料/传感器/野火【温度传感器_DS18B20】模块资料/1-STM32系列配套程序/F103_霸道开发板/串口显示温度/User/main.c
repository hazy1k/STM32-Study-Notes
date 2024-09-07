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
  * 实验平台:野火  STM32 霸道 开发板
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



/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint8_t uc, ucDs18b20Id [ 8 ];
	
	
	/* 配置SysTick 为1us中断一次 */
	SysTick_Init();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	USART_Config();	//初始化串口1
	
	
	printf("\r\n this is a ds18b20 test demo \r\n");
	
	while( DS18B20_Init() )	
		printf("\r\n no ds18b20 exit \r\n");
	
	printf("\r\n ds18b20 exit \r\n");
	
	
	DS18B20_ReadId ( ucDs18b20Id  );           // 读取 DS18B20 的序列号
	
	printf("\r\nDS18B20的序列号是： 0x");

	for ( uc = 0; uc < 8; uc ++ )             // 打印 DS18B20 的序列号
	 printf ( "%.2x", ucDs18b20Id [ uc ] );
	
	
	for(;;)
	{	
		printf ( "\r\n获取该序列号器件的温度： %.1f\r\n", DS18B20_GetTemp_MatchRom ( ucDs18b20Id ) );		// 打印通过 DS18B20 序列号获取的温度值	
		
		Delay_ms(1000);		/* 1s 读取一次温度值 */
		
		LED2_TOGGLE;
		
	}	 
	
	
}
/*********************************************END OF FILE**********************/
