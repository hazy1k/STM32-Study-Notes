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
#include "./adc/bsp_adc.h"

float ADC_ConvertedValueLocal[2];
// ADת�����ֵ
uint32_t ADC_ConvertedValue;
uint32_t DMA_Transfer_Complete_Count=0;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{   
  /* ����ϵͳʱ��Ϊ72 MHz */
  SystemClock_Config();

  /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
  DEBUG_USART_Config();

  ADCx_Init();
  HAL_ADCEx_Calibration_Start(&hadcx1);
  HAL_ADCEx_Calibration_Start(&hadcx2);
  
  /* ����ADת����ʹ��DMA������ж� */
  HAL_ADC_Start(&hadcx2);
  HAL_ADCEx_MultiModeStart_DMA(&hadcx1,&ADC_ConvertedValue,sizeof(ADC_ConvertedValue));
  
  /* ����ѭ�� */
  while (1)
  {
    HAL_Delay(1000);
    /* 3.3ΪADת���Ĳο���ѹֵ��stm32��ADת��Ϊ12bit��2^12=4096��
       ��������Ϊ3.3Vʱ��ADת�����Ϊ4096 */
    ADC_ConvertedValueLocal[0] =(float)(ADC_ConvertedValue&0xFFF)*3.3/4096; 
	  ADC_ConvertedValueLocal[1] =(float)((ADC_ConvertedValue>>16)&0xFFF)*3.3/4096; 
    
		printf("ADC1ת��ԭʼֵ = 0x%04X --> ��ѹֵ = %f V \n", ADC_ConvertedValue&0xFFFF,ADC_ConvertedValueLocal[0]);    
    printf("ADC2ת��ԭʼֵ = 0x%04X --> ��ѹֵ = %f V \n", (ADC_ConvertedValue>>16)&0xFFFF,ADC_ConvertedValueLocal[1]);      
    
    printf("\r\n");    
    DMA_Transfer_Complete_Count=0;
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
