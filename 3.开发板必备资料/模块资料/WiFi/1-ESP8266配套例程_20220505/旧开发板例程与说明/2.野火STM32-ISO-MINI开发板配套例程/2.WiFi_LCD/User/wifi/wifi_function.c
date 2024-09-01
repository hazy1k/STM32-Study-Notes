#include "wifi_function.h"
#include "wifi_config.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_SysTick.h"
#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>

#include "bsp_ili9341_lcd.h"
#include "even_process.h"
#include <stdlib.h>


/*
 * 函数名：ESP8266_Choose
 * 描述  ：使能/禁用WF-ESP8266模块
 * 输入  ：enumChoose = ENABLE，使能模块
 *         enumChoose = DISABLE，禁用模块
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_Choose ( FunctionalState enumChoose )
{
	if ( enumChoose == ENABLE )
		ESP8266_CH_HIGH_LEVEL();
	
	else
		ESP8266_CH_LOW_LEVEL();
	
}


/*
 * 函数名：ESP8266_Rst
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被ESP8266_AT_Test调用
 */
void ESP8266_Rst ( void )
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
	#else
	 ESP8266_RST_LOW_LEVEL();
	 Delay_ms ( 500 ); 
	 ESP8266_RST_HIGH_LEVEL();
	 
	#endif

}


/*
 * 函数名：ESP8266_AT_Test
 * 描述  ：对WF-ESP8266模块进行AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AT_Test ( void )
{
	ESP8266_RST_HIGH_LEVEL();
	
	Delay_ms ( 1000 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 200 ) ) ESP8266_Rst ();  	

}


/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	ESP8266_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	Delay_ms ( waittime );                 //延时
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}


/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}


/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}


/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：WF-ESP8266模块创建WiFi热点
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 *       ：enunPsdMode，WiFi加密方式代号字符串
 * 返回  : 1，创建成功
 *         0，创建失败
 * 调用  ：被外部调用
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );
	
}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
	
}


/*
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
	
}


/*
 * 函数名：ESP8266_UnvarnishSend
 * 描述  ：配置WF-ESP8266模块进入透传发送
 * 输入  ：无
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_UnvarnishSend ( void )
{
	return (
	  ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) &&
	  ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 ) );
	
}


/*
 * 函数名：ESP8266_SendString
 * 描述  ：WF-ESP8266模块发送字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 *       ：pStr，要发送的字符串
 *       ：ulStrLength，要发送的字符串的字节数
 *       ：ucId，哪个ID发送的字符串
 * 返回  : 1，发送成功
 *         0，发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
		
	if ( enumEnUnvarnishTx )
		ESP8266_Usart ( "%s", pStr );

	
	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


/*
 * 函数名：ESP8266_ReceiveString
 * 描述  ：WF-ESP8266模块接收字符串
 * 输入  ：enumEnUnvarnishTx，声明是否已使能了透传模式
 * 返回  : 接收到的字符串首地址
 * 调用  ：被外部调用
 */
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	if ( enumEnUnvarnishTx )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, ">" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	
	else 
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}

	return pRecStr;
	
}


/*
 * 函数名：ESP8266_STA_TCP_Client
 * 描述  ：WF-ESP8266模块进行STA TCP Clien测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_STA_TCP_Client ( void )
{
	char cStr1 [55], cStr2 [55];
	
	char * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

	macClear_DisplayFrame();
	
	LCD_DispEnCh ( 0,  0, "正在使能WF-ESP8266模块......", BLUE );
	ESP8266_Choose ( ENABLE );
	
	LCD_DispEnCh ( 0,  18, "正在测试AT启动......", BLUE );
	ESP8266_AT_Test ();
	
	LCD_DispEnCh ( 0,  36, "正在设置网络模式......", BLUE );
	ESP8266_Net_Mode_Choose ( STA );
  
	LCD_DispEnCh ( 0,  54, "正在扫描无线网络......", BLUE );
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "请输入要连接的WiFi的名称,", BLUE );
		LCD_DispEnCh ( 0, 16, "按\"Enter\"键确认.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );

//		macClear_DisplayFrame();//
		LCD_DispEnCh ( 0, 34, "请输入要连接的WiFi的密钥,", BLUE );
		LCD_DispEnCh ( 0, 50, "按\"Enter\"键确认.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "正在连入热点......",       BLUE );
		
//		strcpy ( cStr1, "wildfire" );//
//		strcpy ( cStr2, "wildfire" );//
	
  } while ( ! ESP8266_JoinAP ( cStr1, cStr2 ) );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "正在启动多路连接模式......", BLUE );
	ESP8266_Enable_MultipleId ( ENABLE );
	
	
	do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "请在电脑将网络调试助手以TCP Server连接网络", BLUE );
		LCD_DispEnCh ( 0, 16, "然后在LCD输入电脑的IP,按\"Enter\"键确认.",  BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );
		
//		macClear_DisplayFrame();//
		LCD_DispEnCh ( 0, 34, "请输入网络调试助手连接网络的端口号,", BLUE );
		LCD_DispEnCh ( 0, 50, "按\"Enter\"键确认.",                BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "正在建立TCP连接......",                BLUE );
		
//		strcpy ( cStr1, "192.168.1.121" );//
//		strcpy ( cStr2, "8080" );//
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_2 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_3 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_4 ) ) );
	

  for ( uc = 0; uc < 5; uc ++ )
	{
		macClear_DisplayFrame();
		sprintf ( cStr1, "请输入端口ID%d要发送的字符串,", uc );
		LCD_DispEnCh ( 0,  0, ( const uint8_t * ) cStr1, BLUE );
		LCD_DispEnCh ( 0, 16, "按\"Enter\"键确认.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
//		pStr = "456";//
		ul = strlen ( pStr );
		
		LCD_DispEnCh ( 0, 34, "数据发送中......",       BLUE );
		
		while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
		
	}
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请在网络调试助手发送字符串.",         BLUE );
	LCD_DispEnCh ( 0, 18, "字符串长度控制在43个英文字符.",       BLUE );
	LCD_DispEnCh ( 0, 36, "1个中文字符=2.7个英文字符",           BLUE );
	LCD_DispEnCh ( 0, 54, "输入字符不包括功能字符和中文标点符号", BLUE );

  uc = 0;
	
	while ( 1 )
	{
		pStr = ESP8266_ReceiveString ( DISABLE );
		
		PC_Usart ( "%s", pStr );//
				
		pBuf = pStr;

		while ( ( pStr = strtok ( pBuf, "\r\n" ) ) != NULL )
		{
			if ( strcmp ( pStr, "OK" ) != 0 )
			{
		  if ( uc == 7 ) 
			  uc = 0;
		
		  if ( uc == 0)
		    macClear_DisplayFrame();
			
		  LCD_DispEnCh ( 0,  uc * 16, ( const uint8_t * ) pStr, BLUE );
			uc ++;
			
			}

			pBuf = NULL;
			
		} 

	}

}


/*
 * 函数名：ESP8266_AP_TCP_Server
 * 描述  ：WF-ESP8266模块进行AP TCP Server测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AP_TCP_Server ( void )
{
	char * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

  char cStr1 [55], cStr2 [55], cStr3 [55];


  macClear_DisplayFrame();
	
	LCD_DispEnCh ( 0,  0, "正在使能WF-ESP8266模块......", BLUE );
	ESP8266_Choose ( ENABLE );	
	
	LCD_DispEnCh ( 0,  18, "正在测试AT启动......", BLUE );
	ESP8266_AT_Test ();
	
  LCD_DispEnCh ( 0,  36, "正在设置网络模式......", BLUE );
	ESP8266_Net_Mode_Choose ( AP );


  macClear_DisplayFrame();
	LCD_DispEnCh ( 0, 0, "请输入要创建的热点的名称,按\"Enter\"键确认.", BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh (   0, 20, "请输入热点的加密方式的编号,按\"Enter\"键确认.", BLUE );
	LCD_DispEnCh (   0, 36, "0.OPEN",                                      BLUE );
	LCD_DispEnCh (  47, 36, "1.WEP",                                       BLUE );
	LCD_DispEnCh (  88, 36, "2.WPA_PSK",                                   BLUE );
	LCD_DispEnCh ( 153, 36, "3.WPA2_PSK",                                  BLUE );
	LCD_DispEnCh ( 224, 36, "4.WPA_WPA2_PSK",                              BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 52, "请输入要创建的热点的密钥,按\"Enter\"键确认.", BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr3, ( const char * ) pStr );

  LCD_DispEnCh ( 0, 74, "正在创建热点......",       BLUE );
	ESP8266_BuildAP ( cStr1, cStr3, cStr2 );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*


  macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "正在启动多路连接模式......", BLUE );
	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请输入服务器要开启的端口号,", BLUE );
	LCD_DispEnCh ( 0, 18, "按\"Enter\"键确认.",         BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 38, "请输入服务器的超时时间(0~28800,单位:秒),", BLUE );
	LCD_DispEnCh ( 0, 56, "按\"Enter\"键确认.",                      BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 77, "正在开启server模式......",       BLUE );
	ESP8266_StartOrShutServer ( ENABLE, cStr1, cStr2 );
	

  do {
		macClear_DisplayFrame();
	  LCD_DispEnCh ( 0,  0, "正查询本模块IP......", BLUE );
		ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		pStr = strtok ( strEsp8266_Fram_Record .Data_RX_BUF, "\r\nAT+CIFSROKLink" );
		LCD_DispStr ( 0,  16, ( uint8_t * ) pStr, BLUE );

		LCD_DispEnCh ( 0, 30, "请用手机连接刚才创建的热点.",               BLUE );
		LCD_DispEnCh ( 0, 46, "这里只连接一个手机,作为ID0.",               BLUE );
		LCD_DispEnCh ( 0, 62, "手机调试助手以TCP Client连接刚开启的服务器", BLUE );
		LCD_DispEnCh ( 0, 78, "连接好后按\"Enter\"键确认......",           BLUE );
		
//		macClr_LcdDis_StrInput();
    inWaiteInput ();
		
	} while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:0", 0, 500 ) );
	

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "正查询本模块IP......", BLUE );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请输入要向端口手机(ID0)发送的字符串,", BLUE );
	LCD_DispEnCh ( 0, 16, "按\"Enter\"键确认.",                 BLUE );
	
	macClr_LcdDis_StrInput();
	pStr = inWaiteInput ();
//	pStr = "456";//
	ul = strlen ( pStr );
	
	LCD_DispEnCh ( 0, 34, "数据发送中......",       BLUE );
	
	while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请在网络调试助手发送字符串.",         BLUE );
	LCD_DispEnCh ( 0, 18, "字符串长度控制在43个英文字符.",       BLUE );
	LCD_DispEnCh ( 0, 36, "1个中文字符=2.7个英文字符",           BLUE );
	LCD_DispEnCh ( 0, 54, "输入字符不包括功能字符和中文标点符号", BLUE );
	
  uc = 0;
	
	while ( 1 )
	{
		pStr = ESP8266_ReceiveString ( DISABLE );
		
		PC_Usart ( "%s", pStr );//
				
		pBuf = pStr;

		while ( ( pStr = strtok ( pBuf, "\r\n" ) ) != NULL )
		{
			if ( strcmp ( pStr, "OK" ) != 0 )
			{
		  if ( uc == 7 ) 
			  uc = 0;
		
		  if ( uc == 0)
		    macClear_DisplayFrame();
			
		  LCD_DispEnCh ( 0,  uc * 16, ( const uint8_t * ) pStr, BLUE );
			uc ++;
			
			}

			pBuf = NULL;
			
		} 

	}
	

}


/*
 * 函数名：ESP8266_StaTcpClient_ApTcpServer
 * 描述  ：WF-ESP8266模块进行STA(TCP Client)+AP(TCP Server)测试
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_StaTcpClient_ApTcpServer ( void )
{
	char * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

  char cStr1 [55], cStr2 [55], cStr3 [55];


  macClear_DisplayFrame();
	
	LCD_DispEnCh ( 0,  0, "正在使能WF-ESP8266模块......", BLUE );
	ESP8266_Choose ( ENABLE );	
	
	LCD_DispEnCh ( 0,  18, "正在测试AT启动......", BLUE );
	ESP8266_AT_Test ();
	
  LCD_DispEnCh ( 0,  36, "正在设置网络模式......", BLUE );
	ESP8266_Net_Mode_Choose ( STA_AP );


  macClear_DisplayFrame();
	LCD_DispEnCh ( 0, 0, "请输入要创建的热点的名称,按\"Enter\"键确认.", BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh (   0, 20, "请输入热点的加密方式的编号,按\"Enter\"键确认.", BLUE );
	LCD_DispEnCh (   0, 36, "0.OPEN",                              BLUE );
	LCD_DispEnCh (  47, 36, "1.WEP",                               BLUE );
	LCD_DispEnCh (  88, 36, "2.WPA_PSK",                           BLUE );
	LCD_DispEnCh ( 153, 36, "3.WPA2_PSK",                          BLUE );
	LCD_DispEnCh ( 224, 36, "4.WPA_WPA2_PSK",                      BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 52, "请输入要创建的热点的密钥,按\"Enter\"键确认.", BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr3, ( const char * ) pStr );

  LCD_DispEnCh ( 0, 74, "正在创建热点......",       BLUE );
	ESP8266_BuildAP ( cStr1, cStr3, cStr2 );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*
	
	
	macClear_DisplayFrame();
	
  LCD_DispEnCh ( 0,  36, "正在扫描无线网络......", BLUE );
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "请输入要连接的WiFi的名称,", BLUE );
		LCD_DispEnCh ( 0, 16, "按\"Enter\"键确认.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );

		LCD_DispEnCh ( 0, 34, "请输入要连接的WiFi的密钥,", BLUE );
		LCD_DispEnCh ( 0, 50, "按\"Enter\"键确认.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "正在连入热点......",       BLUE );
		
//		strcpy ( cStr1, "wildfire" );//
//		strcpy ( cStr2, "wildfire" );//
	
  } while ( ! ESP8266_JoinAP ( cStr1, cStr2 ) );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "正在启动多路连接模式......", BLUE );
	ESP8266_Enable_MultipleId ( ENABLE );
		

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请输入服务器要开启的端口号,", BLUE );
	LCD_DispEnCh ( 0, 18, "按\"Enter\"键确认.",         BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 38, "请输入服务器的超时时间(0~28800,单位:秒),", BLUE );
	LCD_DispEnCh ( 0, 56, "按\"Enter\"键确认.",                      BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 77, "正在开启server模式......",       BLUE );
	ESP8266_StartOrShutServer ( ENABLE, cStr1, cStr2 );


	do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "请在电脑将网络调试助手以TCP Server连接网络", BLUE );
		LCD_DispEnCh ( 0, 16, "然后在LCD输入电脑的IP,按\"Enter\"键确认.",  BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 34, "请输入网络调试助手连接网络的端口号,", BLUE );
		LCD_DispEnCh ( 0, 50, "按\"Enter\"键确认.",                BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "正在建立TCP连接......",                BLUE );
		
//		strcpy ( cStr1, "192.168.1.121" );//
//		strcpy ( cStr2, "8080" );//
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_2 ) ) );
	

  do {
		macClear_DisplayFrame();
	  LCD_DispEnCh ( 0,  0, "正查询本模块IP......", BLUE );
		ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		pStr = strtok ( strEsp8266_Fram_Record .Data_RX_BUF, "\r\nAT+CIFSROKLink" );
		LCD_DispStr ( 0,  16, ( uint8_t * ) pStr, BLUE );

		LCD_DispEnCh ( 0, 30, "请用手机连接刚才创建的热点.",               BLUE );
		LCD_DispEnCh ( 0, 46, "这里只连接一个手机,作为ID3.",               BLUE );
		LCD_DispEnCh ( 0, 62, "手机调试助手以TCP Client连接刚开启的服务器", BLUE );
		LCD_DispEnCh ( 0, 78, "连接好后按\"Enter\"键确认......",           BLUE );
		
//		macClr_LcdDis_StrInput();
    inWaiteInput ();
		
	} while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500 ) );
	
	
  for ( uc = 0; uc < 3; uc ++ )
	{
		macClear_DisplayFrame();
		sprintf ( cStr1, "请输入端口ID%d要发送的字符串,", uc );
		LCD_DispEnCh ( 0,  0, ( const uint8_t * ) cStr1, BLUE );
		LCD_DispEnCh ( 0, 16, "按\"Enter\"键确认.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
//		pStr = "456";//
		ul = strlen ( pStr );
		
		LCD_DispEnCh ( 0, 34, "数据发送中......",       BLUE );
		
		while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
		
	}
		

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请输入要向端口手机(ID3)发送的字符串,", BLUE );
	LCD_DispEnCh ( 0, 16, "按\"Enter\"键确认.",                 BLUE );
	
	macClr_LcdDis_StrInput();
	pStr = inWaiteInput ();
//	pStr = "456";//
	ul = strlen ( pStr );
	
	LCD_DispEnCh ( 0, 34, "数据发送中......",       BLUE );
	
	while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
	

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "请在网络调试助手发送字符串.",         BLUE );
	LCD_DispEnCh ( 0, 18, "字符串长度控制在43个英文字符.",       BLUE );
	LCD_DispEnCh ( 0, 36, "1个中文字符=2.7个英文字符",           BLUE );
	LCD_DispEnCh ( 0, 54, "输入字符不包括功能字符和中文标点符号", BLUE );
	
  uc = 0;
	
	while ( 1 )
	{
		pStr = ESP8266_ReceiveString ( DISABLE );
		
		PC_Usart ( "%s", pStr );//
				
		pBuf = pStr;

		while ( ( pStr = strtok ( pBuf, "\r\n" ) ) != NULL )
		{
			if ( strcmp ( pStr, "OK" ) != 0 )
			{
		  if ( uc == 7 ) 
			  uc = 0;
		
		  if ( uc == 0)
		    macClear_DisplayFrame();
			
		  LCD_DispEnCh ( 0,  uc * 16, ( const uint8_t * ) pStr, BLUE );
			uc ++;
			
			}

			pBuf = NULL;
			
		} 

	}
	
	
}



