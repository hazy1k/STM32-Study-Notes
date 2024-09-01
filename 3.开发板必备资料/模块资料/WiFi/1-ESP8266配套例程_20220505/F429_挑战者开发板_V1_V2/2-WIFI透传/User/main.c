/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   DHT11温湿度传感器读取。
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
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./dwt_delay/core_delay.h"
#include "./DHT11/bsp_dht11.h"
#include "./esp8266/bsp_esp8266.h"
#include "./esp8266/bsp_esp8266_test.h"
#include "./led/bsp_led.h"


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
//	DHT11_GPIO_Config ();                                                        //初始化DHT11
  
  
  
	printf ( "\r\n野火 WF-ESP8266 WiFi模块测试例程\r\n" );                          //打印测试例程提示信息
	printf ( "\r\n在网络调试助手或者串口调试助手上发送以下命令可以控制板载RGB灯\r\n" );    //打印测试例程提示信息
  printf ( "\r\nLED_RED\r\nLED_GREEN\r\nLED_BLUE\r\nLED_YELLOW\r\nLED_PURPLE\r\nLED_CYAN\r\nLED_WHITE\r\nLED_RGBOFF\r\n" );  

	  
  ESP8266_StaTcpClient_Unvarnish_ConfigTest();                          //对ESP8266进行配置
  
  printf ( "\r\n在网络调试助手或者串口调试助手  发送以下命令控制板载RGB灯：\r\n" );    //打印测试例程提示信息
  printf ( "\r\nLED_RED\r\nLED_GREEN\r\nLED_BLUE\r\nLED_YELLOW\r\nLED_PURPLE\r\nLED_CYAN\r\nLED_WHITE\r\nLED_RGBOFF\r\n" );
  printf ( "\r\n观察RGB灯的状态变化\r\n" );
  
  while ( 1 )
  {
    
    ESP8266_CheckRecvDataTest(); // ESP8266 检查一次是否接收到了数据
    
  }

}



/*********************************************END OF FILE**********************/

