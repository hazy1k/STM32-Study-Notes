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
#include "bsp_usart.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "bsp_esp8266_test.h"
#include "bsp_dht11.h"
#include "bsp_led.h"  
#include "./dwt_delay/core_delay.h"


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
	/* 初始化 */
  USART_Config ();                                                      //初始化串口1
  CPU_TS_TmrInit();                                                     //初始化DWT计数器，用于延时函数
  LED_GPIO_Config();                                                    //初始化RGB彩灯
	ESP8266_Init ();                                                      //初始化WiFi模块使用的接口和外设
	DHT11_Init ();                                                        //初始化DHT11

	
	printf ( "\r\n野火 WF-ESP8266 WiFi模块测试例程\r\n" );                 //打印测试例程提示信息

  ESP8266_StaTcpClient_Unvarnish_ConfigTest();                          //对ESP8266进行配置
  
  
	SysTick_Init ();                                                      //配置 SysTick 为 10ms 中断一次，在中断里读取传感器数据
  
  while ( 1 )
  {
    
    if( read_dht11_finish ) // read_dht11_finish == 1 or read_dht11_finish == -1
    {
      
      ESP8266_SendDHT11DataTest(); // ESP8266 发送一次温湿度数据
      //printf("read_dht11_finish=%d\r\n", read_dht11_finish);    //Debug
      read_dht11_finish = 0;       // 清零标志位
      
      LED1_TOGGLE;
    }
    
  }
	
	
}


/*********************************************END OF FILE**********************/
