/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GSMģ��UDP���Գ���
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
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
//#define		SERVERPORT	"5000"
#define		SERVERPORT	"4000"

const char *TESTBUFF1="\r\n1.Ұ��GSMģ��UDP�����ϴ����ܲ���";
const char *TESTBUFF2="\r\n2.Ұ��GSMģ��UDP�����ϴ����ܲ���";


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
	
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
 
  SCB_EnableICache();    // ʹ��ָ�� Cache
  SCB_EnableDCache();    // ʹ������ Cache
    
	SysTick_Init();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
  Key_GPIO_Config();
    /* ���ô���1Ϊ��115200 8-N-1 */
	UARTx_Config();
  
	 printf("\r\nҰ��WF-GSMģ��UDP�շ�����\r\n");
    
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
	
	printf("\n���Խ���UDP���ӣ������ĵȴ�...\n");

	if(gsm_gprs_udp_link(LOCALPORT,SERVERIP,SERVERPORT)!=GSM_TRUE)
	{
		printf("\r\nUDP����ʧ�ܣ�������ȷ���ø���ģ��\r\n");
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
	printf("\r\nע�⣺�����ԣ�ʹ��UDP��������ʱ�ӵڶ������ݿ�ʼ���ܱ����յ�\n");	
//	GSM_DELAY(1000);	
	if(gsm_gprs_send(TESTBUFF1)!=GSM_TRUE)
	{
		printf("\r\nUDP��������ʧ�ܣ�������ȷ���ø���ģ��\r\n");
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
		printf("\r\nUDP��������ʧ�ܣ�������ȷ���ø���ģ��\r\n");
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
  *         system Clock ��������: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*ʹ�ܹ������ø��� */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* ��������ʱ��Ƶ�ʵ������ϵͳƵ��ʱ����ѹ���ڿ����Ż����ģ�
		 ����ϵͳƵ�ʵĵ�ѹ����ֵ�ĸ��¿��Բο���Ʒ�����ֲᡣ  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* ����HSE������ʹ��HSE��ΪԴ����PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
//  if(ret != HAL_OK)
//  {

//    while(1) { ; }
//  }
  
	/* ѡ��PLL��Ϊϵͳʱ��Դ����������ʱ�ӷ�Ƶ�� */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
/****************************END OF FILE***************************/
