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
  * 实验平台:野火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "usart/bsp_debug_usart.h"
#include "systick/bsp_SysTick.h"
#include "DS18B20/bsp_ds18b20.h"

float temperature;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	uint8_t uc,DS18B20Id[8];
	
  /*初始化USART，配置模式为 115200 8-N-1*/
  Debug_USART_Config();
  printf("\r\n***野火F429至尊版 DS18B20 温度传感器实验***\n");
	
	/* 系统定时器初始化 */
	SysTick_Init();
	
	if(DS18B20_Init()==0)
	{
		printf("DS18B20初始化成功\n");
	}
	else
	{
		printf("DS18B20初始化失败\n");
		printf("请将传感器正确插入到插槽内\n");
		/* 停机 */
		while(1)
		{}			
	}		
	DS18B20_ReadId ( DS18B20Id  );           // 读取 DS18B20 的序列号
	
	printf("\r\nDS18B20的序列号是： 0x");

	for ( uc = 0; uc < 8; uc++ )             // 打印 DS18B20 的序列号
    printf ( "%.2x", DS18B20Id[uc]);
	printf("\n");
	
  while(1)
	{
		temperature=DS18B20_Get_Temp();
		printf("DS18B20读取到的温度为：%0.3f\n",temperature);
    Delay_ms(1000);
	} 

}



/*********************************************END OF FILE**********************/

