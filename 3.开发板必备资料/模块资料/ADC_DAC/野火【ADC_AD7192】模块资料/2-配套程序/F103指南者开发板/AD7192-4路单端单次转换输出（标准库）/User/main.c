/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   AD7192
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"  
#include "./ad7192/bsp_ad7192.h"
#include "./ad7192_test/ad7192_test.h"
#include "bsp_SysTick.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /* LED 端口初始化 */
  LED_GPIO_Config();

  /* 配置串口1为：115200 8-N-1 */
  USART_Config();
  
  /* AD7192初始化 */
  AD7192_Init();
  
  /* 软件复位 AD7192 */
  AD7192SoftwareReset();
  
  SysTick_Delay_Ms(10);
  ReadFromAD7192ViaSPI(REG_ID, 1, AD7192Registers, REG_ID);
  
  if ((AD7192Registers[REG_ID] & 0x0F) != 0)
  {
    printf("AD7192 初始化失败请检查连接！\r\n");
    while(1);
  }
  printf("AD7192_ID = 0x%X\r\n", AD7192Registers[REG_ID]);

  /* 4路ADC单次转换读实验 */
  single_conversion_voltage();
	
	while (1)
  {
  }
}

