#include "bsp_esp8266_test.h"
#include "bsp_esp8266.h"
#include "./dwt_delay/core_delay.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "bsp_usart.h"

#define LED_CMD_NUMBER   8
char *ledCmd[8] = { "LED_RED","LED_GREEN","LED_BLUE","LED_YELLOW","LED_PURPLE","LED_CYAN","LED_WHITE","LED_RGBOFF" };

DHT11_Data_TypeDef DHT11_Data;

volatile uint8_t ucTcpClosedFlag = 0;


/**
  * @brief  获取网络调试助手和串口调试助手发来的信息
  * @param  无
  * @retval 无
  */
void Get_ESP82666_Cmd( char * cmd)
{
	uint8_t i;
	for(i = 0;i < LED_CMD_NUMBER; i++)
	{
     if(( bool ) strstr ( cmd, ledCmd[i] ))
		 break;
	}
	switch(i)
    {
      case 0:
        LED_RED;
      break;
      case 1:
        LED_GREEN;
      break;
      case 2:
        LED_BLUE;
      break;
      case 3:
        LED_YELLOW;
      break;
      case 4:
        LED_PURPLE;
      break;
      case 5:
        LED_CYAN;
      break;
      case 6:
        LED_WHITE;
      break;
      case 7:
        LED_RGBOFF;
      break;
      default:
        
        break;      
    }   
}


/**
  * @brief  ESP8266 StaTcpClient Unvarnish 配置测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_Unvarnish_ConfigTest(void)
{
  printf( "\r\n正在配置 ESP8266 ......\r\n" );
  printf( "\r\n使能 ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();
	while( ! ESP8266_AT_Test() );
  while( ! ESP8266_DHCP_CUR () );  
  printf( "\r\n正在配置工作模式 STA ......\r\n" );
	while( ! ESP8266_Net_Mode_Choose ( STA ) );

  printf( "\r\n正在连接 WiFi ......\r\n" );
  while( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
	
  printf( "\r\n禁止多连接 ......\r\n" );
	while( ! ESP8266_Enable_MultipleId ( DISABLE ) );
	
  printf( "\r\n正在连接 Server ......\r\n" );
	while( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
	
  printf( "\r\n进入透传发送模式 ......\r\n" );
	while( ! ESP8266_UnvarnishSend () );
	
	printf( "\r\n配置 ESP8266 完毕\r\n" );
  
}


/**
  * @brief  ESP8266 发送DHT11数据测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_SendDHT11DataTest(void)
{
  char cStr [ 100 ] = { 0 };
  uint8_t ucStatus;
  
  if( 1 == read_dht11_finish )
    sprintf ( cStr, "\r\n\r\n读取DHT11成功!\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n", 
              DHT11_Data.humi_int, DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci );
  else
		sprintf ( cStr, "Read DHT11 ERROR!\r\n" );
  
  printf ( "%s", cStr );                                             //打印读取 DHT11 温湿度信息

  ESP8266_SendString ( ENABLE, cStr, 0, Single_ID_0 );               //发送 DHT11 温湿度信息到网络调试助手
  
  
  if ( ucTcpClosedFlag )                                             //检测是否失去连接
  {
    ESP8266_ExitUnvarnishSend ();                                    //退出透传模式
    
    do ucStatus = ESP8266_Get_LinkStatus ();                         //获取连接状态
    while ( ! ucStatus );
    
    if ( ucStatus == 4 )                                             //确认失去连接后重连
    {
      printf ( "\r\n正在重连热点和服务器 ......\r\n" );
      
      while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
      
      while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
      
      printf ( "\r\n重连热点和服务器成功\r\n" );

    }
    
    while ( ! ESP8266_UnvarnishSend () );		
    
  }
}



