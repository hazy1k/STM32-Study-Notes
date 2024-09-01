/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   WF-ESP8266 WiFi模块测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "bsp_esp8266_test.h"
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "./dwt_delay/core_delay.h"





/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
	/* 初始化 */
  USARTx_Config ();                                                              //初始化串口1
  CPU_TS_TmrInit();
	ESP8266_Init ();                                                               //初始化WiFi模块使用的接口和外设
	DHT11_Init ();
  LED_Init ();
	Beep_Init ();
	
	
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
