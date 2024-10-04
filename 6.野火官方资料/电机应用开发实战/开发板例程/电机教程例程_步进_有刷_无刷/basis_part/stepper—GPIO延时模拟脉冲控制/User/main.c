/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   �������-GPIO��ʱģ���������
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
#include "./stepper/bsp_stepper_init.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{
  int key_val=0;
  int i=0;
  int dir_val=0;
  int angle_val=90;
  
  /* ��ʼ��ϵͳʱ��Ϊ72MHz */
  SystemClock_Config();
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  printf("��ӭʹ��Ұ�� ��������� ������� IO��ģ����� ����\r\n");
  printf("���°���1���޸���ת���򡢰��°���2���޸���ת�Ƕ�\r\n");
  /*������ʼ��*/
  Key_GPIO_Config();
  /*���������ʼ��*/
  stepper_Init();
  /*�����������ʹ��*/
  while(1)
  {     
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
    {
      /*�ı䷽��*/
      dir_val=(++i % 2) ? CW : CCW;
      MOTOR_DIR(dir_val);
      key_val = ON;
    }
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
    {
      /*�ı�Ƕ�*/
      angle_val=angle_val+90;
      key_val = ON;
    }
    if( key_val == ON  )
    {
      /*��ӡ״̬*/
      if(dir_val)
        printf("˳ʱ����ת %d ��,",angle_val);
      else
        printf("��ʱ����ת %d ��,",angle_val);
      
      printf("����������......\r\n");
      stepper_turn(100,angle_val,32,dir_val);
      key_val = OFF;
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
