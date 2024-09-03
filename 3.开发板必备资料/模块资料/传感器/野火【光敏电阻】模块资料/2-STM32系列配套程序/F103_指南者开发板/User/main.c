/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   测试光敏
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
#include "bsp_led.h"  
#include "./ldr/bsp_ldr.h" 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */ 
int main(void)
{	
	/* LED端口初始化 */
	LED_GPIO_Config();
	LED2_OFF;

	/* 光敏端口初始化 */
	LDR_GPIO_Config();
	
	/* 轮询光敏状态，若有光则点亮灯 */
	while(1)
	{	   
		if (LDR_Test(LDR_GPIO_PORT,LDR_GPIO_PIN) == LDR_ON)
    {
      LED2_ON;    // 有光灯亮
    }
    else
      LED2_OFF;   // 无光灯灭
	}
}
/*********************************************END OF FILE**********************/
