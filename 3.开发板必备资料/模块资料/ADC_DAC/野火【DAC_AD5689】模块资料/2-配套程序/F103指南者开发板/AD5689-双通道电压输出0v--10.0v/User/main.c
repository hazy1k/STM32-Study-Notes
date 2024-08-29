/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   AD7192
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h"  
#include "./ad5689/bsp_ad5689.h"

static void SystemClock_Config(void);
/*  ����ñ���ӣ�J3(RST-GND) J4(A_IN-GND) J6(GAIN-GND) J7(A_IN-GND ) */
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	float temp1,temp2;
	uint16_t vol =0;
  /* ϵͳʱ�ӳ�ʼ����72MHz */
  SystemClock_Config();
  
  /* HAL ���ʼ�� */
  HAL_Init();

  /* ���ô���1Ϊ��115200 8-N-1 */
  DEBUG_USART_Config();
  printf("��ӭʹ��Ұ�� F103 AD5689��ѹ���-��ͨ�� ����\r\n");
  printf("����KEY1���������ѹ������KEY2��С�����ѹ\r\n");   
  /* ������ʼ�� */
  Key_GPIO_Config();
  
  /* AD5689��ʼ�� */
  AD5689_Init();

  AD5689_WriteUpdate_DACREG(DAC_A,vol); 
	AD5689_WriteUpdate_DACREG(DAC_B,0xFF-vol); 
  while (1)
  {
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
			{
				vol += 0x200;
				AD5689_WriteUpdate_DACREG(DAC_A,vol); 
				AD5689_WriteUpdate_DACREG(DAC_B,0xFFFF-vol); 
				temp1=(float)vol*1000/0xFFFF;
				temp2=(float)(0xFFFF-vol)*1000/0xFFFF;
				printf("Voltage channel1 = %0.2fV\n",temp1/100);
				printf("Voltage channel2 = %0.2fV\n",temp2/100);
			}
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
			{
				vol -= 0x200;
				AD5689_WriteUpdate_DACREG(DAC_A,vol); 
				AD5689_WriteUpdate_DACREG(DAC_B,0xFFFF-vol); 
				temp1=(float)vol*1000/0xFFFF;
				temp2=(float)(0xFFFF-vol)*1000/0xFFFF;
				printf("Voltage channel1 = %0.2fV\n",temp1/100);
				printf("Voltage channel2 = %0.2fV\n",temp2/100);
			}
			HAL_Delay(20);
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
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
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
