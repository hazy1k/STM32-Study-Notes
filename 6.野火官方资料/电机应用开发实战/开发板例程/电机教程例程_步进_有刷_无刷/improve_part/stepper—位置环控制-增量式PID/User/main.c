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

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
#include "./led/bsp_led.h"
#include "./Encoder/bsp_encoder.h"
#include "./pid/bsp_pid.h"
#include "./tim/bsp_basic_tim.h"
#include "./stepper/bsp_stepper_ctrl.h"

extern _pid pid;
extern int pid_status;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{
	/* ��ʼ��ϵͳʱ��Ϊ72MHz */
	SystemClock_Config();
	/*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
	DEBUG_USART_Config();
	printf("��ӭʹ��Ұ�� ��������� ������� �ٶȱջ����� λ��ʽPID����\r\n");
	printf("���°���1����Ŀ��ֵ������2����Ŀ��ֵ\r\n");	
  printf("����������ʹ��PID��������\r\n");	
  /* ��ʼ��ʱ��� */
  HAL_InitTick(5);
	/*�����жϳ�ʼ��*/
	Key_GPIO_Config();	
	/*led��ʼ��*/
	LED_GPIO_Config();
  /* ��ʼ��������ʱ����ʱ��20ms����һ���ж� */
	TIMx_Configuration();
  /* �������ӿڳ�ʼ�� */
	Encoder_Init();
	/*���������ʼ��*/
	stepper_Init();
  /* �ϵ�Ĭ��ֹͣ��� */
  Set_Stepper_Stop();
  /* PID�㷨������ʼ�� */
  PID_param_init();	

  /* Ŀ��λ��ת��Ϊ����������������ΪpidĿ��ֵ */
  pid.target_val = TARGET_DISP * ENCODER_TOTAL_RESOLUTION;
	Set_Stepper_Start();    
	
#if PID_ASSISTANT_EN
  int Temp = pid.target_val;    // ��λ����Ҫ����������ת��һ��
	set_computer_value(SEED_START_CMD, CURVES_CH1, NULL, 0);// ͬ����λ����������ť״̬
  set_computer_value(SEED_TARGET_CMD, CURVES_CH1, &Temp, 1);// ��ͨ�� 1 ����Ŀ��ֵ
#endif

	while(1)
	{
    /* ɨ��KEY1������Ŀ��λ�� */
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/* λ������2Ȧ */
      pid.target_val += 8000;
      
    #if PID_ASSISTANT_EN
      int temp = pid.target_val;
      set_computer_value(SEED_TARGET_CMD, CURVES_CH1, &temp, 1);// ��ͨ�� 1 ����Ŀ��ֵ
    #endif
		}
    /* ɨ��KEY2����СĿ��λ�� */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			/* λ�ü�С2Ȧ */
      pid.target_val -= 8000;
      
    #if PID_ASSISTANT_EN
      int temp = pid.target_val;
      set_computer_value(SEED_TARGET_CMD, CURVES_CH1, &temp, 1);// ��ͨ�� 1 ����Ŀ��ֵ
    #endif
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
