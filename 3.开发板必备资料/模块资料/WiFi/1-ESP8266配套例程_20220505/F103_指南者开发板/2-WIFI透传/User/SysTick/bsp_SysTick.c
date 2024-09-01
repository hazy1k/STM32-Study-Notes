/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
  *          常用的有 1us 10us 1ms 中断。     
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 F103-霸道 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
	
#include "bsp_SysTick.h"




/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init( void )
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if ( SysTick_Config(SystemCoreClock / 100) )	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while (1);
	}
  
  // 关闭滴答定时器  
	//SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	
  // 使能滴答定时器  10ms中断一次
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
  
}







/*********************************************END OF FILE**********************/
