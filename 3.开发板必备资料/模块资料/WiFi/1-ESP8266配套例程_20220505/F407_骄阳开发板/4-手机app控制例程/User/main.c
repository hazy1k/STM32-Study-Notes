/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   wifi 串口透传例程
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
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./dwt_delay/core_delay.h"
#include "./DHT11/bsp_dht11.h"
#include "./esp8266/bsp_esp8266.h"
#include "./esp8266/bsp_esp8266_test.h"
#include "./led/bsp_led.h"
#include "./beep/bsp_beep.h"   

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  /* 初始化 */
  Debug_USART_Config ();                                                      //初始化串口1
  CPU_TS_TmrInit();                                                     //初始化DWT计数器，用于延时函数
  LED_GPIO_Config();                                                    //初始化RGB彩灯
	ESP8266_Init ();                                                      //初始化WiFi模块使用的接口和外设
	DHT11_GPIO_Config ();                                                        //初始化DHT11
	BEEP_GPIO_Config ();
	
	printf ( "\r\n野火 WF-ESP8266 WiFi模块测试例程\r\n" );                          //打印测试例程提示信息
	
  
  #ifndef BUILTAP_TEST
  ESP8266_StaTcpServer_ConfigTest();                                             //对ESP8266进行配置 STA模式
  #else
  ESP8266_ApTcpServer_ConfigTest();                                              //对ESP8266进行配置 AP模式
  #endif
  
  while ( 1 )
  {
    
    ESP8266_CheckRecv_SendDataTest(); // ESP8266 处理并发送数据
    
  }

}



/*********************************************END OF FILE**********************/

