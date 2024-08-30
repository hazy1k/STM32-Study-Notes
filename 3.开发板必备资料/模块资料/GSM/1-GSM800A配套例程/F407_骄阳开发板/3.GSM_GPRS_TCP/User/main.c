/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ��UDP�շ���������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "./key/bsp_key.h" 
#include <string.h>

#define		LOCALPORT	"2000"

#define		SERVERIP	"10.92.15.252"
#define		SERVERPORT	"8080"
//#define		SERVERPORT	"4000"

const char *TESTBUFF1="\r\n1.Ұ��GSMģ��TCP�����ϴ����ܲ���";
const char *TESTBUFF2="\r\n2.Ұ��GSMģ��TCP�����ϴ����ܲ���";


/*
 * ϵͳ�����λ
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* �ر������ж�*/  
  NVIC_SystemReset();   /* ϵͳ��λ */
}

/*
 * ����TCP�շ�����
 * 
 */
int main(void)
{
	static uint8_t timecount=1,timestop=0;

	uint8_t testCard = 0;
	/* ����USART */
    Debug_USART_Config();
    
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\nҰ��WF-GSMģ��TCP�շ�����\r\n");
    
    //���ģ����Ӧ�Ƿ�����
    while(gsm_init()!= GSM_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
			GSM_DELAY(1000);          
   
		}
		
		printf("\r\n���ڵȴ�GSMģ���ʼ��...\r\n");
		while(IsInsertCard() != GSM_TRUE)
		{
			
			if(++testCard >20)
			{
				printf("\r\n��ⲻ���绰������ϵ粢���½���绰��\r\n");
			}
			GSM_DELAY(1000); 		
		}		
	
  GSM_DELAY(100); 
	
	//ȷ�Ϲر�֮ǰ������		
	gsm_gprs_link_close();	
	
	GSM_DELAY(100);
	//ȷ�Ϲر�֮ǰ�ĳ���
	gsm_gprs_shut_close();
	
	printf("\n��ʼ��GPRS��������...\n");
  //���³�ʼ��gprs
	if(gsm_gprs_init()!= GSM_TRUE)//GPRS��ʼ������
	{
		printf("\r��ʼ��GPRSʧ�ܣ������¸�ģ���ϵ粢��λ������\n");
		while(1);
	}
	
	printf("\n���Խ���TCP���ӣ������ĵȴ�...\n");

	if(gsm_gprs_tcp_link(LOCALPORT,SERVERIP,SERVERPORT)!=GSM_TRUE)
	{
		printf("\r\nTCP����ʧ�ܣ�������ȷ���ø���ģ��\r\n");
		GSM_DELAY(1000); 
		printf("\nIP���ӶϿ�\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n�رճ���\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		printf("\r\n5s���Զ�����\r\n");
		GSM_DELAY(5000);  
		Soft_Reset();
	}
	
	printf("\r\n���ӳɹ�,���Է�������...\n");	
//	GSM_DELAY(1000);	
	if(gsm_gprs_send(TESTBUFF1)!=GSM_TRUE)
	{
		printf("\r\nTCP��������ʧ�ܣ�������ȷ���ø���ģ��\r\n");
		GSM_DELAY(1000); 
		printf("\nIP���ӶϿ�\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n�رճ���\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n���Է��͵ڶ�������...\n");	
	if(gsm_gprs_send(TESTBUFF2)!=GSM_TRUE)
	{
		printf("\r\nTCP��������ʧ�ܣ�������ȷ���ø���ģ��\r\n");
		GSM_DELAY(1000); 
		printf("\nIP���ӶϿ�\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n�رճ���\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	printf("\n���͵ڶ������ݳɹ�\n");

	printf("\n��׼���ý���Զ�����ݣ���ʹ���������������ģ�鷢������\n");
	GSM_CLEAN_RX();
	
//	printf("\nIP���ӶϿ�\n");
//	GSM_DELAY(100);
//	gsm_gprs_link_close();	
//	
//	printf("\n�رճ���\n");
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
			printf("\n��ʱ����ͨѶ���������ر�����\n");

			printf("\nIP���ӶϿ�\n");
			GSM_DELAY(100);
			gsm_gprs_link_close();	
			
			printf("\n�رճ���\n");
			GSM_DELAY(100);
			gsm_gprs_shut_close();
			
			GSM_DELAY(1000);
			timestop=0xFF;
		}
		timecount++;
		GSM_DELAY(10);
	}		
}



