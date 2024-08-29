/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   AD5689-双通道电压输出
  ******************************************************************************
  * @attention
	* 跳线帽连接：J3(RST-GND) J4(A_IN-GND) J6(GAIN-GND) J7(A_IN-GND )
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./ad5689/bsp_ad5689.h"
#include ".\key\bsp_key.h"

static void SystemClock_Config(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	float temp1,temp2;
	uint16_t vol =0;
  /* 系统时钟初始化成168MHz */
  SystemClock_Config();
  
  /* HAL 库初始化 */
  HAL_Init();

  /* LED 端口初始化 */
  LED_GPIO_Config();

  /* 配置串口1为：115200 8-N-1 */
  DEBUG_USART_Config();
  printf("欢迎使用野火 F407 AD5689电压输出-单通道 例程\r\n");
  printf("按下KEY1增大输出电压，按下KEY2减小输出电压\r\n");   
  /* 按键初始化 */
  Key_GPIO_Config();
  
  /* AD5689初始化 */
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
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
