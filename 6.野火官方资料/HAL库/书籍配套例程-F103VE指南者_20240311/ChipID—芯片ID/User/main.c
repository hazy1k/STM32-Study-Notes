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
#include "./usart/bsp_debug_usart.h"

uint32_t ChipUniqueID[3];

/*
 * ��������Get_ChipID
 * ����  ����ȡоƬID
 * ����  ����
 * ���  ����
 */
void Get_ChipID(void)
{
	ChipUniqueID[0] = *(__IO uint32_t *)(0X1FFFF7F0); // ���ֽ�
	ChipUniqueID[1] = *(__IO uint32_t *)(0X1FFFF7EC); // 
	ChipUniqueID[2] = *(__IO uint32_t *)(0X1FFFF7E8); // ���ֽ�
}

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
  DEBUG_USART_Config();

  /* to get the chipid and put it in ChipUniqueID[3] */
	Get_ChipID();	
	
	/* printf the chipid */
	printf("\r\nоƬ��ΨһIDΪ: 0x%08X-%08X-%08X\r\n",
	        ChipUniqueID[0],ChipUniqueID[1],ChipUniqueID[2]);
	
	/* printf the flash memory amount */
	printf("\r\nоƬflash������Ϊ: %dK \r\n", *(__IO uint16_t *)(0X1FFFF7E0));

	for(;;)
	{
		
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
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV2;
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
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_0)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
