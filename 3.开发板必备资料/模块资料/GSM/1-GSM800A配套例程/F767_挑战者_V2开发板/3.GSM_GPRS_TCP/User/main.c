/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GSMģ��TCP���Գ���
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32F767 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32f7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
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


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{  
  static uint8_t timecount=1,timestop=0;
	uint8_t testCard = 0;
	/* ϵͳʱ�ӳ�ʼ����216MHz */
	SystemClock_Config();
 
  SCB_EnableICache();    // ʹ��ָ�� Cache
  SCB_EnableDCache();    // ʹ������ Cache
    
	SysTick_Init();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
  Key_GPIO_Config();
    /* ���ô���1Ϊ��115200 8-N-1 */
	UARTx_Config();
  
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


/**
  * @brief  System Clock ����
  *         system Clock �������� : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  ��
  * @retval ��
  */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;

	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	/* ���� OverDrive ģʽ�Դﵽ216MƵ��  */  
	ret = HAL_PWREx_EnableOverDrive();
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	/* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}  
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
