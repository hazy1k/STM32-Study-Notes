/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   指纹识别模块实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103 MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/rx_data_queue.h"
#include "./SysTick/bsp_SysTick.h"
#include "./led/bsp_led.h"  
#include "./as608/as608_test.h"
#include "./as608/bsp_as608.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{		
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  USART_Config();
  
  LED_GPIO_Config();	
  
  /*初始化环形缓冲区*/
  rx_queue_init();
   
  /*初始化指纹模块配置*/
	AS608_Config();
  
	/*测试STM32与指纹模块的通信*/
  Connect_Test();
  
  while(1)
  {
    FR_Task();
  } 
}

/*********************************************END OF FILE**********************/
