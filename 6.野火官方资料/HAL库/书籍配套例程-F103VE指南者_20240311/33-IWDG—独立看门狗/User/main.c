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
#include "./iwdg/bsp_iwdg.h"  
#include ".\key\bsp_key.h" 

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
  
  /*��ʼ������*/
  Key_GPIO_Config();
	
  // IWDG 1s ��ʱ���
	IWDG_Config(IWDG_PRESCALER_64 ,625);
  
  LED_RED;

  //while��������������Ŀ�о�����Ҫд�Ĵ��룬�ⲿ�ֵĳ�������ö������Ź������
    //�������֪���ⲿ�ִ����ִ��ʱ�䣬������500ms����ô���ǿ������ö������Ź���
	//���ʱ����600ms����500ms��һ�㣬���Ҫ����صĳ���û���ܷ�����ִ�еĻ�����ô
	//ִ�����֮��ͻ�ִ��ι���ĳ�����������ܷ����ǳ���ͻᳬʱ�����ﲻ��ι��
	//�ĳ���,��ʱ�ͻ����ϵͳ��λ������Ҳ���ų������ܷ������ܻ����ˣ��պ�ι���ˣ�
	//������š�����Ҫ�����ȷ�ļ�س��򣬿���ʹ�ô��ڿ��Ź������ڿ��Ź��涨������
	//�涨�Ĵ���ʱ����ι����
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
		{
			/*ι���������ι����ϵͳ��Ḵλ��LD1�����һ�Σ������1s
			  ʱ����׼ʱι���Ļ�����LED1�᳣�� */
			HAL_IWDG_Refresh(&IWDG_Handle);
      
      LED_GREEN;
      
		}   
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
