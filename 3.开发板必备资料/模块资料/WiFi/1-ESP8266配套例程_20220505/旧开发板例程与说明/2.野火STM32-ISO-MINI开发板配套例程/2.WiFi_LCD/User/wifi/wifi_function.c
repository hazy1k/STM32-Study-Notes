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
 * ��������ESP8266_Choose
 * ����  ��ʹ��/����WF-ESP8266ģ��
 * ����  ��enumChoose = ENABLE��ʹ��ģ��
 *         enumChoose = DISABLE������ģ��
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_Choose ( FunctionalState enumChoose )
{
	if ( enumChoose == ENABLE )
		ESP8266_CH_HIGH_LEVEL();
	
	else
		ESP8266_CH_LOW_LEVEL();
	
}


/*
 * ��������ESP8266_Rst
 * ����  ������WF-ESP8266ģ��
 * ����  ����
 * ����  : ��
 * ����  ����ESP8266_AT_Test����
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
 * ��������ESP8266_AT_Test
 * ����  ����WF-ESP8266ģ�����AT��������
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_AT_Test ( void )
{
	ESP8266_RST_HIGH_LEVEL();
	
	Delay_ms ( 1000 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 200 ) ) ESP8266_Rst ();  	

}


/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

	ESP8266_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	Delay_ms ( waittime );                 //��ʱ
	
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
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
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
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}


/*
 * ��������ESP8266_BuildAP
 * ����  ��WF-ESP8266ģ�鴴��WiFi�ȵ�
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 *       ��enunPsdMode��WiFi���ܷ�ʽ�����ַ���
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );
	
}


/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}


/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
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
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
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
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_UnvarnishSend ( void )
{
	return (
	  ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) &&
	  ESP8266_Cmd ( "AT+CIPSEND", "\r\n", ">", 500 ) );
	
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
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
 * ��������ESP8266_ReceiveString
 * ����  ��WF-ESP8266ģ������ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 * ����  : ���յ����ַ����׵�ַ
 * ����  �����ⲿ����
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
 * ��������ESP8266_STA_TCP_Client
 * ����  ��WF-ESP8266ģ�����STA TCP Clien����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_STA_TCP_Client ( void )
{
	char cStr1 [55], cStr2 [55];
	
	char * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

	macClear_DisplayFrame();
	
	LCD_DispEnCh ( 0,  0, "����ʹ��WF-ESP8266ģ��......", BLUE );
	ESP8266_Choose ( ENABLE );
	
	LCD_DispEnCh ( 0,  18, "���ڲ���AT����......", BLUE );
	ESP8266_AT_Test ();
	
	LCD_DispEnCh ( 0,  36, "������������ģʽ......", BLUE );
	ESP8266_Net_Mode_Choose ( STA );
  
	LCD_DispEnCh ( 0,  54, "����ɨ����������......", BLUE );
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "������Ҫ���ӵ�WiFi������,", BLUE );
		LCD_DispEnCh ( 0, 16, "��\"Enter\"��ȷ��.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );

//		macClear_DisplayFrame();//
		LCD_DispEnCh ( 0, 34, "������Ҫ���ӵ�WiFi����Կ,", BLUE );
		LCD_DispEnCh ( 0, 50, "��\"Enter\"��ȷ��.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "���������ȵ�......",       BLUE );
		
//		strcpy ( cStr1, "wildfire" );//
//		strcpy ( cStr2, "wildfire" );//
	
  } while ( ! ESP8266_JoinAP ( cStr1, cStr2 ) );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "����������·����ģʽ......", BLUE );
	ESP8266_Enable_MultipleId ( ENABLE );
	
	
	do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "���ڵ��Խ��������������TCP Server��������", BLUE );
		LCD_DispEnCh ( 0, 16, "Ȼ����LCD������Ե�IP,��\"Enter\"��ȷ��.",  BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );
		
//		macClear_DisplayFrame();//
		LCD_DispEnCh ( 0, 34, "�������������������������Ķ˿ں�,", BLUE );
		LCD_DispEnCh ( 0, 50, "��\"Enter\"��ȷ��.",                BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "���ڽ���TCP����......",                BLUE );
		
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
		sprintf ( cStr1, "������˿�ID%dҪ���͵��ַ���,", uc );
		LCD_DispEnCh ( 0,  0, ( const uint8_t * ) cStr1, BLUE );
		LCD_DispEnCh ( 0, 16, "��\"Enter\"��ȷ��.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
//		pStr = "456";//
		ul = strlen ( pStr );
		
		LCD_DispEnCh ( 0, 34, "���ݷ�����......",       BLUE );
		
		while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
		
	}
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "��������������ַ����ַ���.",         BLUE );
	LCD_DispEnCh ( 0, 18, "�ַ������ȿ�����43��Ӣ���ַ�.",       BLUE );
	LCD_DispEnCh ( 0, 36, "1�������ַ�=2.7��Ӣ���ַ�",           BLUE );
	LCD_DispEnCh ( 0, 54, "�����ַ������������ַ������ı�����", BLUE );

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
 * ��������ESP8266_AP_TCP_Server
 * ����  ��WF-ESP8266ģ�����AP TCP Server����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_AP_TCP_Server ( void )
{
	char * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

  char cStr1 [55], cStr2 [55], cStr3 [55];


  macClear_DisplayFrame();
	
	LCD_DispEnCh ( 0,  0, "����ʹ��WF-ESP8266ģ��......", BLUE );
	ESP8266_Choose ( ENABLE );	
	
	LCD_DispEnCh ( 0,  18, "���ڲ���AT����......", BLUE );
	ESP8266_AT_Test ();
	
  LCD_DispEnCh ( 0,  36, "������������ģʽ......", BLUE );
	ESP8266_Net_Mode_Choose ( AP );


  macClear_DisplayFrame();
	LCD_DispEnCh ( 0, 0, "������Ҫ�������ȵ������,��\"Enter\"��ȷ��.", BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh (   0, 20, "�������ȵ�ļ��ܷ�ʽ�ı��,��\"Enter\"��ȷ��.", BLUE );
	LCD_DispEnCh (   0, 36, "0.OPEN",                                      BLUE );
	LCD_DispEnCh (  47, 36, "1.WEP",                                       BLUE );
	LCD_DispEnCh (  88, 36, "2.WPA_PSK",                                   BLUE );
	LCD_DispEnCh ( 153, 36, "3.WPA2_PSK",                                  BLUE );
	LCD_DispEnCh ( 224, 36, "4.WPA_WPA2_PSK",                              BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 52, "������Ҫ�������ȵ����Կ,��\"Enter\"��ȷ��.", BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr3, ( const char * ) pStr );

  LCD_DispEnCh ( 0, 74, "���ڴ����ȵ�......",       BLUE );
	ESP8266_BuildAP ( cStr1, cStr3, cStr2 );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*


  macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "����������·����ģʽ......", BLUE );
	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "�����������Ҫ�����Ķ˿ں�,", BLUE );
	LCD_DispEnCh ( 0, 18, "��\"Enter\"��ȷ��.",         BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 38, "������������ĳ�ʱʱ��(0~28800,��λ:��),", BLUE );
	LCD_DispEnCh ( 0, 56, "��\"Enter\"��ȷ��.",                      BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 77, "���ڿ���serverģʽ......",       BLUE );
	ESP8266_StartOrShutServer ( ENABLE, cStr1, cStr2 );
	

  do {
		macClear_DisplayFrame();
	  LCD_DispEnCh ( 0,  0, "����ѯ��ģ��IP......", BLUE );
		ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		pStr = strtok ( strEsp8266_Fram_Record .Data_RX_BUF, "\r\nAT+CIFSROKLink" );
		LCD_DispStr ( 0,  16, ( uint8_t * ) pStr, BLUE );

		LCD_DispEnCh ( 0, 30, "�����ֻ����ӸղŴ������ȵ�.",               BLUE );
		LCD_DispEnCh ( 0, 46, "����ֻ����һ���ֻ�,��ΪID0.",               BLUE );
		LCD_DispEnCh ( 0, 62, "�ֻ�����������TCP Client���Ӹտ����ķ�����", BLUE );
		LCD_DispEnCh ( 0, 78, "���Ӻú�\"Enter\"��ȷ��......",           BLUE );
		
//		macClr_LcdDis_StrInput();
    inWaiteInput ();
		
	} while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:0", 0, 500 ) );
	

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "����ѯ��ģ��IP......", BLUE );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "������Ҫ��˿��ֻ�(ID0)���͵��ַ���,", BLUE );
	LCD_DispEnCh ( 0, 16, "��\"Enter\"��ȷ��.",                 BLUE );
	
	macClr_LcdDis_StrInput();
	pStr = inWaiteInput ();
//	pStr = "456";//
	ul = strlen ( pStr );
	
	LCD_DispEnCh ( 0, 34, "���ݷ�����......",       BLUE );
	
	while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "��������������ַ����ַ���.",         BLUE );
	LCD_DispEnCh ( 0, 18, "�ַ������ȿ�����43��Ӣ���ַ�.",       BLUE );
	LCD_DispEnCh ( 0, 36, "1�������ַ�=2.7��Ӣ���ַ�",           BLUE );
	LCD_DispEnCh ( 0, 54, "�����ַ������������ַ������ı�����", BLUE );
	
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
 * ��������ESP8266_StaTcpClient_ApTcpServer
 * ����  ��WF-ESP8266ģ�����STA(TCP Client)+AP(TCP Server)����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_StaTcpClient_ApTcpServer ( void )
{
	char * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;

  char cStr1 [55], cStr2 [55], cStr3 [55];


  macClear_DisplayFrame();
	
	LCD_DispEnCh ( 0,  0, "����ʹ��WF-ESP8266ģ��......", BLUE );
	ESP8266_Choose ( ENABLE );	
	
	LCD_DispEnCh ( 0,  18, "���ڲ���AT����......", BLUE );
	ESP8266_AT_Test ();
	
  LCD_DispEnCh ( 0,  36, "������������ģʽ......", BLUE );
	ESP8266_Net_Mode_Choose ( STA_AP );


  macClear_DisplayFrame();
	LCD_DispEnCh ( 0, 0, "������Ҫ�������ȵ������,��\"Enter\"��ȷ��.", BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh (   0, 20, "�������ȵ�ļ��ܷ�ʽ�ı��,��\"Enter\"��ȷ��.", BLUE );
	LCD_DispEnCh (   0, 36, "0.OPEN",                              BLUE );
	LCD_DispEnCh (  47, 36, "1.WEP",                               BLUE );
	LCD_DispEnCh (  88, 36, "2.WPA_PSK",                           BLUE );
	LCD_DispEnCh ( 153, 36, "3.WPA2_PSK",                          BLUE );
	LCD_DispEnCh ( 224, 36, "4.WPA_WPA2_PSK",                      BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 52, "������Ҫ�������ȵ����Կ,��\"Enter\"��ȷ��.", BLUE );
	
	macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr3, ( const char * ) pStr );

  LCD_DispEnCh ( 0, 74, "���ڴ����ȵ�......",       BLUE );
	ESP8266_BuildAP ( cStr1, cStr3, cStr2 );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 ); //*
	
	
	macClear_DisplayFrame();
	
  LCD_DispEnCh ( 0,  36, "����ɨ����������......", BLUE );
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000 );
		
  do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "������Ҫ���ӵ�WiFi������,", BLUE );
		LCD_DispEnCh ( 0, 16, "��\"Enter\"��ȷ��.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );

		LCD_DispEnCh ( 0, 34, "������Ҫ���ӵ�WiFi����Կ,", BLUE );
		LCD_DispEnCh ( 0, 50, "��\"Enter\"��ȷ��.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "���������ȵ�......",       BLUE );
		
//		strcpy ( cStr1, "wildfire" );//
//		strcpy ( cStr2, "wildfire" );//
	
  } while ( ! ESP8266_JoinAP ( cStr1, cStr2 ) );
	
	
	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "����������·����ģʽ......", BLUE );
	ESP8266_Enable_MultipleId ( ENABLE );
		

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "�����������Ҫ�����Ķ˿ں�,", BLUE );
	LCD_DispEnCh ( 0, 18, "��\"Enter\"��ȷ��.",         BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr1, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 38, "������������ĳ�ʱʱ��(0~28800,��λ:��),", BLUE );
	LCD_DispEnCh ( 0, 56, "��\"Enter\"��ȷ��.",                      BLUE );

  macClr_LcdDis_StrInput();
  pStr = inWaiteInput ();
  strcpy ( cStr2, ( const char * ) pStr );
	
	LCD_DispEnCh ( 0, 77, "���ڿ���serverģʽ......",       BLUE );
	ESP8266_StartOrShutServer ( ENABLE, cStr1, cStr2 );


	do {
		macClear_DisplayFrame();
		LCD_DispEnCh ( 0,  0, "���ڵ��Խ��������������TCP Server��������", BLUE );
		LCD_DispEnCh ( 0, 16, "Ȼ����LCD������Ե�IP,��\"Enter\"��ȷ��.",  BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr1, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 34, "�������������������������Ķ˿ں�,", BLUE );
		LCD_DispEnCh ( 0, 50, "��\"Enter\"��ȷ��.",                BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
		strcpy ( cStr2, ( const char * ) pStr );
		
		LCD_DispEnCh ( 0, 68, "���ڽ���TCP����......",                BLUE );
		
//		strcpy ( cStr1, "192.168.1.121" );//
//		strcpy ( cStr2, "8080" );//
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, cStr1, cStr2, Multiple_ID_2 ) ) );
	

  do {
		macClear_DisplayFrame();
	  LCD_DispEnCh ( 0,  0, "����ѯ��ģ��IP......", BLUE );
		ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500 );
		pStr = strtok ( strEsp8266_Fram_Record .Data_RX_BUF, "\r\nAT+CIFSROKLink" );
		LCD_DispStr ( 0,  16, ( uint8_t * ) pStr, BLUE );

		LCD_DispEnCh ( 0, 30, "�����ֻ����ӸղŴ������ȵ�.",               BLUE );
		LCD_DispEnCh ( 0, 46, "����ֻ����һ���ֻ�,��ΪID3.",               BLUE );
		LCD_DispEnCh ( 0, 62, "�ֻ�����������TCP Client���Ӹտ����ķ�����", BLUE );
		LCD_DispEnCh ( 0, 78, "���Ӻú�\"Enter\"��ȷ��......",           BLUE );
		
//		macClr_LcdDis_StrInput();
    inWaiteInput ();
		
	} while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500 ) );
	
	
  for ( uc = 0; uc < 3; uc ++ )
	{
		macClear_DisplayFrame();
		sprintf ( cStr1, "������˿�ID%dҪ���͵��ַ���,", uc );
		LCD_DispEnCh ( 0,  0, ( const uint8_t * ) cStr1, BLUE );
		LCD_DispEnCh ( 0, 16, "��\"Enter\"��ȷ��.",       BLUE );
		
		macClr_LcdDis_StrInput();
		pStr = inWaiteInput ();
//		pStr = "456";//
		ul = strlen ( pStr );
		
		LCD_DispEnCh ( 0, 34, "���ݷ�����......",       BLUE );
		
		while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
		
	}
		

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "������Ҫ��˿��ֻ�(ID3)���͵��ַ���,", BLUE );
	LCD_DispEnCh ( 0, 16, "��\"Enter\"��ȷ��.",                 BLUE );
	
	macClr_LcdDis_StrInput();
	pStr = inWaiteInput ();
//	pStr = "456";//
	ul = strlen ( pStr );
	
	LCD_DispEnCh ( 0, 34, "���ݷ�����......",       BLUE );
	
	while ( ! ESP8266_SendString ( DISABLE, pStr, ul, ( ENUM_ID_NO_TypeDef ) uc ) );
	

	macClear_DisplayFrame();
	LCD_DispEnCh ( 0,  0, "��������������ַ����ַ���.",         BLUE );
	LCD_DispEnCh ( 0, 18, "�ַ������ȿ�����43��Ӣ���ַ�.",       BLUE );
	LCD_DispEnCh ( 0, 36, "1�������ַ�=2.7��Ӣ���ַ�",           BLUE );
	LCD_DispEnCh ( 0, 54, "�����ַ������������ַ������ı�����", BLUE );
	
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



