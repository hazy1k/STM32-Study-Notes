#ifndef  __TEST_H
#define	 __TEST_H



#include "stm32f10x.h"
#include "bsp_esp8266.h"


//#define   BUILTAP_TEST    //���������л���ESP8266�����ã�STAģʽ�� APģʽ



/********************************** �û���Ҫ���õĲ���**********************************/
#ifndef BUILTAP_TEST
#define   macUser_ESP8266_ApSsid              "embedfire2"         //Ҫ���ӵ��ȵ������
#define   macUser_ESP8266_ApPwd               "wildfire"           //Ҫ���ӵ��ȵ����Կ
#else
#define   macUser_ESP8266_BulitApSsid         "BinghuoLink"      //Ҫ�������ȵ������
#define   macUser_ESP8266_BulitApEcn           OPEN               //Ҫ�������ȵ�ļ��ܷ�ʽ
#define   macUser_ESP8266_BulitApPwd           "wildfire"         //Ҫ�������ȵ����Կ
#endif


#define   macUser_ESP8266_TcpServer_IP         "192.168.0.45"      //������������IP��ַ
#define   macUser_ESP8266_TcpServer_Port       "8000"             //�����������Ķ˿�   

#define   macUser_ESP8266_TcpServer_OverTime   "1800"             //��������ʱʱ�䣨��λ���룩



/********************************** ���Ժ������� ***************************************/
void ESP8266_StaTcpServer_ConfigTest(void);
void ESP8266_ApTcpServer_ConfigTest(void);
void ESP8266_CheckRecv_SendDataTest(void);

#endif

