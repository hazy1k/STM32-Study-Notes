/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   
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
#include "./usart/bsp_usart_dma.h"
#include "./led/bsp_led.h"

extern UART_HandleTypeDef UartHandle; 
extern DMA_HandleTypeDef DMA_Handle;
static void SystemClock_Config(void);
extern uint8_t SendBuff[SENDBUFF_SIZE];
static void Delay(__IO uint32_t nCount); 
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  uint16_t i;
  
  SystemClock_Config();
  /* ��ʼ��USART */
  Debug_USART_Config(); 

  /* ����ʹ��DMAģʽ */
  USART_DMA_Config();
  
  /* ����RGB��ɫ�� */
  LED_GPIO_Config();

  printf("\r\n USART1 DMA TX ���� \r\n");
  
  /*��佫Ҫ���͵�����*/
  for(i=0;i<SENDBUFF_SIZE;i++)
  {
    SendBuff[i]	 = 'A';
    
  }

  /*Ϊ��ʾDMA�������ж�CPU���ܴ����������飬����ʹ��DMA�������ݣ����ǳ���
  *��ʱ�����п��ܻᵼ�µ��Զ˴��ڵ������ֻῨ��������ҷɵ������
  *���DMA�����е�ѭ��ģʽ��Ϊ����ģʽ*/		
  
  /* USART1 �� DMA����TX���� */
   HAL_UART_Transmit_DMA(&UartHandle, (uint8_t *)SendBuff ,SENDBUFF_SIZE);

  /* ��ʱCPU�ǿ��еģ����Ը����������� */  
  //����ͬʱ����LED
  while(1)
  {
    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
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
