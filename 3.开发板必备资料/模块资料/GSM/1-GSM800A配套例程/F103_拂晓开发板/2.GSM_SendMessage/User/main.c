/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块发送短信测试程序，实际测试时请把电话号码修改成要接收短信的手机号
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
#include "bsp_sdfs_app.h"
#include <string.h>


/* 请修改接收短信的电话号码 */
const char num[]="10086";

char namenum[20*4]={0},str[512]={0},gbkstr[256]={0},namegbk[256]={0};

/*
 * 测试发送短信功能，实际测试时请把电话号码修改成要接收短信的手机号
 * 
 */
int main(void)
{
	uint8_t newmessadd=0,IsRead=0;
	uint8_t testCard = 0;

//	static uint8_t timecount=0;
     /* 配置USART */
    USART_Config();

    /* 配置USART */
    GSM_USART_Config();
    
    /* 初始化系统定时器 */
    SysTick_Init();
    
    printf("\r\n野火BH-GSM模块发送短信例程\r\n");
	  printf("\r\n测试前请确认GSM模块已接入电话卡\r\n");
    printf("\r\n测试前请在程序中修改接收短信的目标手机号码\r\n");

    //检测模块响应是否正常
    while(gsm_cmd("AT\r","OK",1000) != GSM_TRUE)
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
    }
    
    printf("\r\n通过了模块响应测试\r\n");

	GSM_DELAY(1000); 
	//先执行次设置文本模式
	if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE)
	{
		printf("\r\n设置文本模式错误\r\n");
	}
	GSM_DELAY(1000); 	

	printf("\r\n正在等待GSM模块初始化...\r\n");
	while(IsInsertCard() != GSM_TRUE)
	{
		
		if(++testCard >20)
		{
			printf("\r\n检测不到电话卡，请断电并重新接入电话卡\r\n");
		}
		GSM_DELAY(1000); 		
	}		
	
	printf("\r\n初始化完成，5秒后开始发送短信测试... \r\n");

    /* 延时5秒再发送命令到模块 */
    GSM_DELAY(5000);    
    
    //发送英文短信
		
	if(gsm_sms((char *)num,"hellow world") == GSM_TRUE)
		printf("\r\n英文短信已发送至：%s，为方便测试，请在程序中修改接收短信的手机号码",num);
	else
	{
		printf("\r\n短信发送失败，请确认目标号码有效");
	}
    GSM_DELAY(2000);    

	//中英文短信，实际测试时请把电话号码修改成要接收短信的手机号
	if(gsm_sms((char *)num,"野火WF-GSM模块短信测试") == GSM_TRUE)
		printf("\r\n中英文短信已发送至：%s，为方便测试，请在程序中修改接收短信的手机号码",num);
	else
	{
		printf("\r\n短信发送失败，请确认目标号码有效");
		while(1);
	}		

	printf("\r\n接收短信测试：通过串口1打印新短信信息\r\n");
		
  while(1)
	{
		GSM_DELAY(1000);
		newmessadd=IsReceiveMS();	
		if(newmessadd)
		{			
			IsRead=readmessage(newmessadd,namenum,str);				
			 
//			printf("newmessadd=%d,IsRead:%d\n",newmessadd,IsRead);
			if(IsRead)
			{				
				printf("新短信\n\r");

				hexuni2gbk(namenum,namegbk);	
				hexuni2gbk(str,gbkstr);						
				printf("新短信:\n发件人:%s\n内容:%s\n\r",namegbk,gbkstr);
			}
		}
		
	}
}



