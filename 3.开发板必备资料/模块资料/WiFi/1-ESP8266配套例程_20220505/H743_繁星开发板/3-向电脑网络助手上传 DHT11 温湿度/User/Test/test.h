#ifndef  __TEST_H
#define	 __TEST_H



#include "stm32h7xx.h"



/********************************** 用户需要设置的参数**********************************/
//要连接的热点的名称，即WIFI名称
#define      macUser_ESP8266_ApSsid           "wifiname" 

//要连接的热点的密钥
#define      macUser_ESP8266_ApPwd            "password" 

//要连接的服务器的 IP，即电脑的IP
#define      macUser_ESP8266_TcpServer_IP     "192.168.0.72" 

//要连接的服务器的端口
#define      macUser_ESP8266_TcpServer_Port    "8080"         



/********************************** 外部全局变量 ***************************************/
extern volatile uint8_t ucTcpClosedFlag;



/********************************** 测试函数声明 ***************************************/
void                     ESP8266_StaTcpClient_UnvarnishTest  ( void );



#endif

