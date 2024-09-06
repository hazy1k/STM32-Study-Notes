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
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "./usart/bsp_debug_usart.h"
//#include "./systick/bsp_SysTick.h" /* SysTick HAL���Ѿ��￪���ˣ��ж�����1ms */
#include "./mpu6050/mpu6050.h"
#include "./led/bsp_led.h"  
#include "./i2c/bsp_i2c.h"

/* MPU6050���� */
short Acel[3];
short Gyro[3];
float Temp;

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	SystemClock_Config();
	/* �������üĴ���ʱ�� */
	__HAL_RCC_SYSCFG_CLK_ENABLE();	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	/* ����ͨ�ų�ʼ�� */
	DEBUG_USART_Config();

	//I2C��ʼ��
	I2C_Init();
	//MPU6050��ʼ��
	MPU6050_Init();
  //���MPU6050
  if( MPU6050ReadID() == 0 )
  {
		printf("\r\nû�м�⵽MPU6050��������\r\n");
		LED_RED;
    
		while(1);	//��ⲻ��MPU6050 LED2����Ȼ����
	}

  while(1)
  {
    if( task_readdata_finish ) //task_readdata_finish = 1 ��ʾ��ȡMPU6050�������
    {
      
      printf("���ٶȣ�%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
      printf("    ������%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
      printf("    �¶�%8.2f\r\n",Temp);
      
      task_readdata_finish = 0; // �����־λ
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
