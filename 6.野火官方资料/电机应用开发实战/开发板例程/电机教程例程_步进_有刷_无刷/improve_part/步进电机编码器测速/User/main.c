/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   �����������������
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
#include <stdio.h>
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./stepper/bsp_stepper_init.h"
#include "./key/bsp_key.h"
#include "./led/bsp_led.h"
#include "./Encoder/bsp_encoder.h"

/* �����ת���� */
__IO int8_t motor_direction = 0;
/* ��ǰʱ���ܼ���ֵ */
__IO int32_t capture_count = 0;
/* ��һʱ���ܼ���ֵ */
__IO int32_t last_count = 0;
/* ��λʱ�����ܼ���ֵ */
__IO int32_t count_per_unit = 0;
/* ���ת��ת�� */
__IO float shaft_speed = 0.0f;
/* �ۻ�Ȧ�� */
__IO float number_of_rotations = 0.0f;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{
  int i = 0;
  
	/* ��ʼ��ϵͳʱ��Ϊ72MHz */
	SystemClock_Config();
	/*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
	DEBUG_USART_Config();
	printf("��ӭʹ��Ұ�� ��������� ������� ���������� ����\r\n");
	printf("���°���1��ͣ���������2�ı䷽��\r\n");	
  /* ��ʼ��ʱ��� */
  HAL_InitTick(5);
	/*������ʼ��*/
	Key_GPIO_Config();	
	/*led��ʼ��*/
	LED_GPIO_Config();
	/*���������ʼ��*/
	stepper_Init();
  /* �ϵ�Ĭ��ֹͣ���������1���� */
  MOTOR_EN(LOW);
  /* �������ӿڳ�ʼ�� */
	Encoder_Init();
  
	while(1)
	{
    /* ɨ��KEY1��������� */
    if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
    {
      MOTOR_EN_TOGGLE;
    }
    /* ɨ��KEY2���ı䷽�� */
    if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON)
    {
      static int j = 0;
      j > 0 ? MOTOR_DIR(CCW) : MOTOR_DIR(CW);
      j=!j;
    }
    
    /* 20ms����һ�� */
    /* �����ת���� = �������������� */
    motor_direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&TIM_EncoderHandle);
    
    /* ��ǰʱ���ܼ���ֵ = ������ֵ + ����������� * ENCODER_TIM_PERIOD  */
    capture_count =__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    
    /* ��λʱ�����ܼ���ֵ = ��ǰʱ���ܼ���ֵ - ��һʱ���ܼ���ֵ */
    count_per_unit = capture_count - last_count;
    
    /* ת��ת�� = ��λʱ���ڵļ���ֵ / �������ֱܷ��� * ʱ��ϵ��  */
    shaft_speed = (float)count_per_unit / ENCODER_TOTAL_RESOLUTION * 50 ;
    
    /* �ۻ�Ȧ�� = ��ǰʱ���ܼ���ֵ / �������ֱܷ���  */
    number_of_rotations = (float)capture_count / ENCODER_TOTAL_RESOLUTION;

    /* ��¼��ǰ�ܼ���ֵ������һʱ�̼���ʹ�� */
    last_count = capture_count;
    
    if(i == 50)/* 1s����һ�� */
    {
      printf("\r\n�������%d\r\n", motor_direction);
      printf("��λʱ������Ч����ֵ��%d\r\n", (count_per_unit<0 ? abs(count_per_unit) : count_per_unit));
      printf("�������ת�٣�%.2f ת/��\r\n", shaft_speed);
      printf("�ۼ�Ȧ����%.2f Ȧ\r\n", number_of_rotations);
      i = 0;
    }
    HAL_Delay(20);
    i++;
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
