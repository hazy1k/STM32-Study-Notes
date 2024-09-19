/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./key/bsp_exti.h"
  
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  /* ϵͳʱ�ӳ�ʼ����72 MHz */
  SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
  
  /* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
  DEBUG_USART_Config();	
  /*��ʼ������*/
  EXTI_Key_Config();
  
	printf("\r\n 1.�������У��̵Ʊ�ʾSTM32�������У���Ʊ�ʾ˯��״̬�����Ʊ�ʾ�մ�˯��״̬������\r\n");
	printf("\r\n 2.��������һ��ʱ����Զ�����˯��״̬����˯��״̬�£���ʹ��KEY1��KEY2����\r\n");
	printf("\r\n 3.��ʵ��ִ������һ��ѭ����\r\n ------�����̵�(��������)->�����(˯��ģʽ)->��KEY1��KEY2����->������(�ձ�����)-----��\r\n");
	printf("\r\n 4.��˯��״̬�£�DAP�������޷���STM32���س���\r\n �ɰ�KEY1��KEY2���Ѻ����أ�\r\n �򰴸�λ��ʹоƬ���ڸ�λ״̬��Ȼ���ڵ����ϵ�����ذ�ť�����ͷŸ�λ��������������\r\n");
  while (1)
  {
    printf("\r\n STM32�������У����̵�\r\n");
	
		LED_GREEN;	
		HAL_Delay(2000);		
		/*****����ִ����ϣ�����˯�߽��͹���***********/
		
		printf("\r\n ����˯��ģʽ�������,��KEY1��KEY2�����ɻ���\r\n");

		//ʹ�ú��ָʾ������˯��״̬
		LED_RED;
		//��ͣ�δ�ʱ�ӣ���ֹͨ���δ�ʱ���жϻ���
		HAL_SuspendTick();
		//����˯��ģʽ
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
		//�ȴ��жϻ���  K1��K2�����ж�	
		/***�����ѣ�������ָʾ***/
		LED_BLUE;
		//�����Ѻ󣬻ָ��δ�ʱ��	  
		HAL_ResumeTick();
		HAL_Delay(2000);	
			
		printf("\r\n ���˳�˯��ģʽ\r\n");
		//����ִ��whileѭ��

  } 
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
