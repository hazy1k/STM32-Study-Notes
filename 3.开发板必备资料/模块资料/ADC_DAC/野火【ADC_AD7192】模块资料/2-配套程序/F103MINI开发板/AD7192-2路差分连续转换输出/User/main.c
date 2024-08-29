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
  * 实验平台:野火 F103-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./key/bsp_key.h"  
#include "./ad7192/bsp_ad7192.h"
#include "./ad7192_test/ad7192_test.h"

static void SystemClock_Config(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /* 系统时钟初始化成72MHz */
  SystemClock_Config();
  
  /* HAL 库初始化 */
  HAL_Init();

  /* LED 端口初始化 */
  LED_GPIO_Config();

  /* 配置串口1为：115200 8-N-1 */
  DEBUG_USART_Config();
  
  /* 按键初始化 */
  Key_GPIO_Config();
  
  /* AD7192初始化 */
  AD7192_Init();
  
  /* 软件复位 AD7192 */
  AD7192SoftwareReset();
  
  HAL_Delay(10);
  ReadFromAD7192ViaSPI(REG_ID, 1, AD7192Registers, REG_ID);
  
  if ((AD7192Registers[REG_ID] & 0x0F) != 0)
  {
    printf("AD7192 初始化失败请检查连接！\r\n");
    while(1);
  }
  printf("AD7192_ID = 0x%X\r\n", AD7192Registers[REG_ID]);

  /* 两路差分连续读实验 */
  difference_continuous_conversion_voltage();

  while (1)
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
