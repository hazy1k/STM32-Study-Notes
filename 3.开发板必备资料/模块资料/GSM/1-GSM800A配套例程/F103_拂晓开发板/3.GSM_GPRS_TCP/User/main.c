/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块UDP收发数据例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "./key/bsp_key.h" 
#include <string.h>

#define		LOCALPORT	"2000"

#define		SERVERIP	"163.142.57.125"
#define		SERVERPORT	"5000"
//#define		SERVERPORT	"4000"

const char *TESTBUFF1="\r\n1.野火GSM模块TCP数据上传功能测试";
const char *TESTBUFF2="\r\n2.野火GSM模块TCP数据上传功能测试";


/*
 * 系统软件复位
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* 关闭所有中断*/  
  NVIC_SystemReset();   /* 系统复位 */
}

/*
 * 测试TCP收发例程
 * 
 */
int main(void)
{
	static uint8_t timecount=1,timestop=0;

	uint8_t testCard = 0;
	/* 配置USART */
    USART_Config();
    
    /* 初始化系统定时器 */
    SysTick_Init();
    
    printf("\r\n野火WF-GSM模块TCP收发例程\r\n");
    
    //检测模块响应是否正常
    while(gsm_init()!= GSM_TRUE)
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
			GSM_DELAY(1000);          
   
		}
		
		printf("\r\n正在等待GSM模块初始化...\r\n");
		while(IsInsertCard() != GSM_TRUE)
		{
			
			if(++testCard >20)
			{
				printf("\r\n检测不到电话卡，请断电并重新接入电话卡\r\n");
			}
			GSM_DELAY(1000); 		
		}		
	
  GSM_DELAY(100); 
	
	//确认关闭之前的连接		
	gsm_gprs_link_close();	
	
	GSM_DELAY(100);
	//确认关闭之前的场境
	gsm_gprs_shut_close();
	
	printf("\n初始化GPRS工作环境...\n");
  //重新初始化gprs
	if(gsm_gprs_init()!= GSM_TRUE)//GPRS初始化环境
	{
		printf("\r初始化GPRS失败，请重新给模块上电并复位开发板\n");
		while(1);
	}
	
	printf("\n尝试建立TCP链接，请耐心等待...\n");

	if(gsm_gprs_tcp_link(LOCALPORT,SERVERIP,SERVERPORT)!=GSM_TRUE)
	{
		printf("\r\nTCP链接失败，请检测正确设置各个模块\r\n");
		GSM_DELAY(1000); 
		printf("\nIP链接断开\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n关闭场景\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		printf("\r\n5s后自动重启\r\n");
		GSM_DELAY(5000);  
		Soft_Reset();
	}
	
	printf("\r\n连接成功,尝试发送数据...\n");	
//	GSM_DELAY(1000);	
	if(gsm_gprs_send(TESTBUFF1)!=GSM_TRUE)
	{
		printf("\r\nTCP发送数据失败，请检测正确设置各个模块\r\n");
		GSM_DELAY(1000); 
		printf("\nIP链接断开\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n关闭场景\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n尝试发送第二条数据...\n");	
	if(gsm_gprs_send(TESTBUFF2)!=GSM_TRUE)
	{
		printf("\r\nTCP发送数据失败，请检测正确设置各个模块\r\n");
		GSM_DELAY(1000); 
		printf("\nIP链接断开\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n关闭场景\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	printf("\n发送第二条数据成功\n");

	printf("\n已准备好接收远端数据，可使用网络调试助手往模块发送数据\n");
	GSM_CLEAN_RX();
	
//	printf("\nIP链接断开\n");
//	GSM_DELAY(100);
//	gsm_gprs_link_close();	
//	
//	printf("\n关闭场景\n");
//	GSM_DELAY(100);
//	gsm_gprs_shut_close();
    while(1)
	{
		if((timecount>=50)&& (timestop!=0xFF))
		{
			
			if(PostGPRS()!=GSM_TRUE)
				timestop++;
			else
				timestop=0;
			timecount=0;
		}
		if(timestop==120)//60s
		{
			printf("\n长时间无通讯，即将将关闭网络\n");

			printf("\nIP链接断开\n");
			GSM_DELAY(100);
			gsm_gprs_link_close();	
			
			printf("\n关闭场景\n");
			GSM_DELAY(100);
			gsm_gprs_shut_close();
			
			GSM_DELAY(1000);
			timestop=0xFF;
		}
		timecount++;
		GSM_DELAY(10);
	}		
}



