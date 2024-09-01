#include "./esp8266/bsp_esp8266.h"
#include "./esp8266/bsp_esp8266_test.h"
#include "./dwt_delay/core_delay.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "./DHT11/bsp_dht11.h"
#include "./led/bsp_led.h"  
#include "./beep/bsp_beep.h"   
#include "./usart/bsp_debug_usart.h"

// 测试函数用全局变量
uint8_t ucId, ucLen;
uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;
char cStr [ 100 ] = { 0 }, cCh;
char * pCh, * pCh1;
DHT11_Data_TypeDef DHT11_Data; 


#ifndef BUILTAP_TEST

/**
  * @brief  ESP8266 StaTcpServer Unvarnish 配置测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpServer_ConfigTest(void)
{
  printf( "\r\n正在配置 ESP8266 ......\r\n" );
  printf( "\r\n使能 ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();
	while( ! ESP8266_AT_Test() );
  while( ! ESP8266_DHCP_CUR () );
  printf( "\r\n正在配置工作模式 STA ......\r\n" );
	while( ! ESP8266_Net_Mode_Choose ( STA ) );

  printf( "\r\n正在连接 WiFi ......\r\n" );
  while ( ! ESP8266_CIPSTA ( macUser_ESP8266_TcpServer_IP ) ); //设置模块的 AP IP
  while( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	

  printf( "\r\n允许多连接 ......\r\n" );
	while( ! ESP8266_Enable_MultipleId ( ENABLE ) );
  
  printf( "\r\n开启服务器模式 ......\r\n" );
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_TcpServer_Port, macUser_ESP8266_TcpServer_OverTime ) );

  ESP8266_Inquire_StaIp ( cStr, 20 );
	printf ( "\n本模块已连接WIFI为 %s，\nSTA IP 为：%s，开启的端口为：%s\n手机网络助手连接该 IP 和端口，最多可连接5个客户端\n",
           macUser_ESP8266_ApSsid, cStr, macUser_ESP8266_TcpServer_Port );
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	printf( "\r\n配置 ESP8266 完毕\r\n" );
  
}


#else


/**
  * @brief  ESP8266 ApTcpServer 配置测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_ApTcpServer_ConfigTest(void)
{  
  printf( "\r\n正在配置 ESP8266 ......\r\n" );
  printf( "\r\n使能 ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();
	while( ! ESP8266_AT_Test() );
  
  printf( "\r\n正在配置工作模式为 AP ......\r\n" );
	while( ! ESP8266_Net_Mode_Choose ( AP ) );

  printf( "\r\n正在创建WiFi热点 ......\r\n" );
  while ( ! ESP8266_CIPAP ( macUser_ESP8266_TcpServer_IP ) ); //设置模块的 AP IP
  while ( ! ESP8266_BuildAP ( macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, macUser_ESP8266_BulitApEcn ) );
	
  printf( "\r\n允许多连接 ......\r\n" );
	while( ! ESP8266_Enable_MultipleId ( ENABLE ) );
  
  printf( "\r\n开启服务器模式 ......\r\n" );
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_TcpServer_Port, macUser_ESP8266_TcpServer_OverTime ) );

	
  ESP8266_Inquire_ApIp ( cStr, 20 );
	printf ( "\n本模块WIFI为 %s，密码开放\nAP IP 为：%s，开启的端口为：%s\n手机网络助手连接该 IP 和端口，最多可连接5个客户端\n",
           macUser_ESP8266_BulitApSsid, cStr, macUser_ESP8266_TcpServer_Port );
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	printf( "\r\n配置 ESP8266 完毕\r\n" );
  
}


#endif



/**
  * @brief  ESP8266 检查接收信息并发送数据测试函数
  * @param  无
  * @retval 无
  */
void ESP8266_CheckRecv_SendDataTest(void)
{
  
  if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
  {
    USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //禁用串口接收中断
    
    strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
    printf("ucCh =%s\n",strEsp8266_Fram_Record .Data_RX_BUF);
//    printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );//
    
    if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LED_" ) ) != 0 )    //LED控制
    {
      cCh = * ( pCh + 8 );
      printf("%c\n",cCh);
      switch ( cCh )
      {
        case '1':
          cCh = * ( pCh + 10 );
          switch ( cCh )
          {
            case '0':
              LED1_OFF;
              ucLed1Status = 0;
              break;
            case '1':
              LED1_ON;
              ucLed1Status = 1;
              break;
            default :
              break;
          }
          break;
          
        case '2':
          cCh = * ( pCh + 10 );
          switch ( cCh )
          {
            case '0':
              LED2_OFF;
              ucLed2Status = 0;
              break;
            case '1':
              LED2_ON;
              ucLed2Status = 1;
              break;
            default :
              break;
          }
          break;

        case '3':
          cCh = * ( pCh + 10 );
          switch ( cCh )
          {
            case '0':
              LED3_OFF;
              ucLed3Status = 0;
              break;
            case '1':
              LED3_ON;
              ucLed3Status = 1;
              break;
            default :
              break;
          }
          break;
          
        default :
          break;					
          
      }
      
      sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_END", ucLed1Status, ucLed2Status, ucLed3Status );
      
    }
    
    else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_BUZZER_" ) ) != 0 )  //蜂鸣器控制
    {
      cCh = * ( pCh + 11 );
      
      switch ( cCh )
      {
        case '0':
          BEEP_OFF;
          ucBuzzerStatus = 0;
          break;
        case '1':
          BEEP_ON;
          ucBuzzerStatus = 1;
          break;
        default:
          break;
      }
      
      sprintf ( cStr, "CMD_BUZZER_%d_ENDBUZZER_END", ucBuzzerStatus );
      
    }
      
    else if ( ( ( pCh  = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_UART_" ) ) != 0 ) && 
              ( ( pCh1 = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "_ENDUART_END" ) )  != 0 ) ) 
    {
      if ( pCh < pCh1)
      {
        ucLen = pCh1 - pCh + 12;
        memcpy ( cStr, pCh, ucLen );
        cStr [ ucLen ] = '\0';
      }
    }

    else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_READ_ALL_END" ) != 0 )     //读取状态数据
    {
      Read_DHT11 ( & DHT11_Data );
      sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_DHT11_%d.%d_%d.%d_ENDDHT11_BUZZER_%d_ENDBUZZER_END", 
                ucLed1Status, ucLed2Status, ucLed3Status, DHT11_Data .temp_int, 
                DHT11_Data .temp_deci, DHT11_Data .humi_int, DHT11_Data .humi_deci,
                ucBuzzerStatus );
    }
    
      
    if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD," ) ) != 0 ) 
    {
      ucId = * ( pCh + strlen ( "+IPD," ) ) - '0';
      ESP8266_SendString ( DISABLE, cStr, strlen ( cStr ), ( ENUM_ID_NO_TypeDef ) ucId );
    }
    
    strEsp8266_Fram_Record .InfBit .FramLength = 0;
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
    
    USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断
    
  }
  
}

