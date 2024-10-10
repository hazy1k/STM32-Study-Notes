/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   步进电机驱动例程
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
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h" 
#include "./SMD/MicroStepDriver.h" 
#include "./usart/bsp_debug_usart.h"
#include "./SMD/SMD_test.h" 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
    //初始化LED
    LED_GPIO_Config();
    //初始化按键
    EXTI_Key_Config();
    //初始化串口
    Debug_USART_Config();
	//步进电机初始化
	SMD_Init();
    //打印帮助命令
    ShowHelp();
    
    while(1) 
    {
        //处理串口数据
        DealSerialData();
    }                      
}

/*********************************************END OF FILE**********************/

