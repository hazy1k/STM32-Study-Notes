/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GSMģ�鷢�Ͷ��Ų��Գ���
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

/* ���޸Ľ��ն��ŵĵ绰���� */
const char num[]="13926769552";
char namenum[20*4]={0},str[512]={0},gbkstr[256]={0},namegbk[256]={0};


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{  
 	uint8_t newmessadd=0,IsRead=0;
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
  
	 printf("\r\nҰ��BH-GSMģ�鷢�Ͷ�������\r\n");
	 printf("\r\n����ǰ��ȷ��GSMģ���ѽ���绰��\r\n");
   printf("\r\n����ǰ���ڳ������޸Ľ��ն��ŵ�Ŀ���ֻ�����\r\n");

		//���ģ����Ӧ�Ƿ�����
	while(gsm_init()!= GSM_TRUE)
	{
		printf("\r\nģ����Ӧ���Բ���������\r\n");
		printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
		GSM_DELAY(500);

	}
    
    
    printf("\r\nͨ����ģ����Ӧ����\r\n");

	GSM_DELAY(1000); 
	//��ִ�д������ı�ģʽ
	if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE)
	{
		printf("\r\n�����ı�ģʽ����\r\n");
	}
	GSM_DELAY(1000); 	 

	printf("\r\n���ڵȴ�GSMģ���ʼ��...\r\n");
	while(IsInsertCard() != GSM_TRUE)
	{
		
		if(++testCard >20)
		{
			printf("\r\n��ⲻ���绰������ϵ粢���½���绰��\r\n");
		}
		GSM_DELAY(1000); 		
	}		
	
	printf("\r\n��ʼ����ɣ�5���ʼ���Ͷ��Ų���... \r\n");

    /* ��ʱ5���ٷ������ģ�� */
    GSM_DELAY(5000);    
    
    //����Ӣ�Ķ���
		
	if(gsm_sms((char *)num,"hellow world") == GSM_TRUE)
		printf("\r\nӢ�Ķ����ѷ�������%s��Ϊ������ԣ����ڳ������޸Ľ��ն��ŵ��ֻ�����",num);
	else
	{
		printf("\r\n���ŷ���ʧ�ܣ���ȷ��Ŀ�������Ч");
	}
    GSM_DELAY(2000);    

	//��Ӣ�Ķ��ţ�ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
	if(gsm_sms((char *)num,"Ұ��WF-GSMģ����Ų���") == GSM_TRUE)
		printf("\r\n��Ӣ�Ķ����ѷ�������%s��Ϊ������ԣ����ڳ������޸Ľ��ն��ŵ��ֻ�����",num);
	else
	{
		printf("\r\n���ŷ���ʧ�ܣ���ȷ��Ŀ�������Ч");
		while(1);
	}		

	printf("\r\n���ն��Ų��ԣ�ͨ������1��ӡ�¶�����Ϣ\r\n");
		
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
				printf("�¶���\n\r");

				hexuni2gbk(namenum,namegbk);	
				hexuni2gbk(str,gbkstr);						
				printf("�¶���:\n������:%s\n����:%s\n\r",namegbk,gbkstr);
			}
		}
		
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
