/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ֱ����ˢ���ٵ��-������ѹ��ȡ-MOS�ܴ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include ".\motor_control\bsp_motor_control.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"

// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_Vol; 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{
  __IO uint16_t ChannelPulse = PWM_MAX_PERIOD_COUNT*0.5;
  uint8_t flag = 0;

  HAL_Init();
  
	/* ��ʼ��ϵͳʱ��Ϊ168MHz */
	SystemClock_Config();

	/* �������üĴ���ʱ�� */
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	
	/* ��ʼ������GPIO */
	Key_GPIO_Config();
  
  /* ��ʼ�� LED */
  LED_GPIO_Config();

  /* �����ʼ�� */
  motor_init();
  
  /* ���ڳ�ʼ�� */
  DEBUG_USART_Config();
  
  /* ADC ʼ�� */
  ADC_Init();
  
  set_motor_speed(ChannelPulse);
  set_motor_disable();    // ���õ��
  
  printf("Ұ��ֱ����ˢ���������ȡʵ��\r\n");
	
	while(1)
	{
    /* ɨ��KEY1 */
    if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
    {
      /* ʹ�ܵ�� */
      set_motor_enable(); 
			
			while(1){

				/* ɨ��KEY1 */
				if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)
				{
					/* ����ռ�ձ� */
					ChannelPulse += PWM_MAX_PERIOD_COUNT/10;
					
					if(ChannelPulse > PWM_MAX_PERIOD_COUNT)
						ChannelPulse = PWM_MAX_PERIOD_COUNT;
					
					set_motor_speed(ChannelPulse);
				}
				
				/* ɨ��KEY2 */
				if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
				{
					if(ChannelPulse < PWM_MAX_PERIOD_COUNT/10)
						ChannelPulse = 0;
					else
						ChannelPulse -= PWM_MAX_PERIOD_COUNT/10;
					
					set_motor_speed(ChannelPulse);
				}

				if (HAL_GetTick()%50 == 0 && flag == 0)    // ÿ50�����ȡһ�ε�������ѹ
				{
					flag = 1;
					int32_t current = get_curr_val();
					
				#if 0//defined(PID_ASSISTANT_EN)
					set_computer_value(SEED_FACT_CMD, CURVES_CH1, &current, 1);
				#else
					printf("��Դ��ѹ��%.2fV��������%dmA\r\n", get_vbus_val(), current); 
				#endif
					
				}
				else if (HAL_GetTick()%50 != 0 && flag == 1)
				{
					flag = 0;
				}
			}
    }

#if 0//����������,����\���õ�����ܲ�����
    /* ɨ��KEY2 */
    if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)
    {
      /* ���õ�� */
      set_motor_disable();
    }
		
    /* ɨ��KEY5 */
    if( Key_Scan(KEY5_GPIO_PORT, KEY5_PIN) == KEY_ON)
    {
      /* ת������ */
      set_motor_direction( (++i % 2) ? MOTOR_FWD : MOTOR_REV);
    }
#endif
    if (HAL_GetTick()%50 == 0 && flag == 0)    // ÿ50�����ȡһ�ε�������ѹ
    {
      flag = 1;
      int32_t current = get_curr_val();
      
    #if 0//defined(PID_ASSISTANT_EN)
      set_computer_value(SEED_FACT_CMD, CURVES_CH1, &current, 1);
    #else
      printf("��Դ��ѹ��%.2fV��������%dmA\r\n", get_vbus_val(), current); 
    #endif
      
    }
    else if (HAL_GetTick()%50 != 0 && flag == 1)
    {
      flag = 0;
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
