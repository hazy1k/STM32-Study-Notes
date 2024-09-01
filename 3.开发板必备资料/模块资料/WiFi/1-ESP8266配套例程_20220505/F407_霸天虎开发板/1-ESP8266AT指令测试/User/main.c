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
#include "./DHT11/bsp_dht11.h"
#include "./esp8266/bsp_esp8266.h"
#include "./dwt_delay/core_delay.h"   


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  /* 初始化 */
  Debug_USART_Config ();                                                      /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  CPU_TS_TmrInit();                                                     //初始化DWT计数器 延时函数用到
	ESP8266_Init ();                                                      //初始化WiFi模块使用的接口和外设
	
	printf("欢迎使用野火STM32开发板\n\n");
	printf("这是一个ESP8266AT指令测试实验\n\n");
	printf("请使用串口调试助手发送\"AT+换行回车\"测试ESP8266是否准备好\n\n");
	printf("更多AT指令请参考模块资料\n\n");
	printf("以下是ESP8266上电初始化打印的信息\n\n");
  
  macESP8266_CH_ENABLE();     //记得一定要先使能
  
  while(1)
	{	
		if(strUSART_Fram_Record .InfBit .FramFinishFlag == 1)  //如果接收到了串口调试助手的数据
		{
			strUSART_Fram_Record .Data_RX_BUF[strUSART_Fram_Record .InfBit .FramLength] = '\0';
			Usart_SendString(macESP8266_USARTx ,strUSART_Fram_Record .Data_RX_BUF);      //数据从串口调试助手转发到ESP8266
			strUSART_Fram_Record .InfBit .FramLength = 0;                                //接收数据长度置零
			strUSART_Fram_Record .InfBit .FramFinishFlag = 0;                            //接收标志置零
	  }
		if(strEsp8266_Fram_Record .InfBit .FramFinishFlag)                             //如果接收到了ESP8266的数据
		{                                                      
			 strEsp8266_Fram_Record .Data_RX_BUF[strEsp8266_Fram_Record .InfBit .FramLength] = '\0';
			 Usart_SendString(DEBUG_USART ,strEsp8266_Fram_Record .Data_RX_BUF);        //数据从ESP8266转发到串口调试助手
			 strEsp8266_Fram_Record .InfBit .FramLength = 0;                             //接收数据长度置零
			 strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;                           //接收标志置零
		}
  }	

}



/*********************************************END OF FILE**********************/

