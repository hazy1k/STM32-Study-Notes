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

// ���Ժ�����ȫ�ֱ���
uint8_t ucId, ucLen;
uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;
char cStr [ 100 ] = { 0 }, cCh;
char * pCh, * pCh1;
DHT11_Data_TypeDef DHT11_Data; 


#ifndef BUILTAP_TEST

/**
  * @brief  ESP8266 StaTcpServer Unvarnish ���ò��Ժ���
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpServer_ConfigTest(void)
{
  printf( "\r\n�������� ESP8266 ......\r\n" );
  printf( "\r\nʹ�� ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();
	while( ! ESP8266_AT_Test() );
  while( ! ESP8266_DHCP_CUR () );
  printf( "\r\n�������ù���ģʽ STA ......\r\n" );
	while( ! ESP8266_Net_Mode_Choose ( STA ) );

  printf( "\r\n�������� WiFi ......\r\n" );
  while ( ! ESP8266_CIPSTA ( macUser_ESP8266_TcpServer_IP ) ); //����ģ��� AP IP
  while( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	

  printf( "\r\n��������� ......\r\n" );
	while( ! ESP8266_Enable_MultipleId ( ENABLE ) );
  
  printf( "\r\n����������ģʽ ......\r\n" );
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_TcpServer_Port, macUser_ESP8266_TcpServer_OverTime ) );

  ESP8266_Inquire_StaIp ( cStr, 20 );
	printf ( "\n��ģ��������WIFIΪ %s��\nSTA IP Ϊ��%s�������Ķ˿�Ϊ��%s\n�ֻ������������Ӹ� IP �Ͷ˿ڣ���������5���ͻ���\n",
           macUser_ESP8266_ApSsid, cStr, macUser_ESP8266_TcpServer_Port );
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	printf( "\r\n���� ESP8266 ���\r\n" );
  
}


#else


/**
  * @brief  ESP8266 ApTcpServer ���ò��Ժ���
  * @param  ��
  * @retval ��
  */
void ESP8266_ApTcpServer_ConfigTest(void)
{  
  printf( "\r\n�������� ESP8266 ......\r\n" );
  printf( "\r\nʹ�� ESP8266 ......\r\n" );
	macESP8266_CH_ENABLE();
	while( ! ESP8266_AT_Test() );
  
  printf( "\r\n�������ù���ģʽΪ AP ......\r\n" );
	while( ! ESP8266_Net_Mode_Choose ( AP ) );

  printf( "\r\n���ڴ���WiFi�ȵ� ......\r\n" );
  while ( ! ESP8266_CIPAP ( macUser_ESP8266_TcpServer_IP ) ); //����ģ��� AP IP
  while ( ! ESP8266_BuildAP ( macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, macUser_ESP8266_BulitApEcn ) );
	
  printf( "\r\n��������� ......\r\n" );
	while( ! ESP8266_Enable_MultipleId ( ENABLE ) );
  
  printf( "\r\n����������ģʽ ......\r\n" );
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_TcpServer_Port, macUser_ESP8266_TcpServer_OverTime ) );

	
  ESP8266_Inquire_ApIp ( cStr, 20 );
	printf ( "\n��ģ��WIFIΪ %s�����뿪��\nAP IP Ϊ��%s�������Ķ˿�Ϊ��%s\n�ֻ������������Ӹ� IP �Ͷ˿ڣ���������5���ͻ���\n",
           macUser_ESP8266_BulitApSsid, cStr, macUser_ESP8266_TcpServer_Port );
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	printf( "\r\n���� ESP8266 ���\r\n" );
  
}


#endif



/**
  * @brief  ESP8266 ��������Ϣ���������ݲ��Ժ���
  * @param  ��
  * @retval ��
  */
void ESP8266_CheckRecv_SendDataTest(void)
{
  
  if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
  {
    USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //���ô��ڽ����ж�
    
    strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
    printf("ucCh =%s\n",strEsp8266_Fram_Record .Data_RX_BUF);
//    printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );//
    
    if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LED_" ) ) != 0 )    //LED����
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
    
    else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_BUZZER_" ) ) != 0 )  //����������
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

    else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_READ_ALL_END" ) != 0 )     //��ȡ״̬����
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
    
    USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�
    
  }
  
}

