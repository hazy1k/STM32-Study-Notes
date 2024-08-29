/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   AD7192
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h"
#include "./ad5689/bsp_ad5689.h"
void SystemClock_Config(void);
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/* 跳线帽连接：J3(RST-GND) J4(A_IN-GND) J6(GAIN-GND) J7(A_IN-GND ) */
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint32_t vol=0;
	float temp1,temp2;
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();
  /* HAL 库初始化 */
  HAL_Init();
  /* 配置串口1为：115200 8-N-1 */
  DEBUG_USART_Config();
  printf("欢迎使用野火 H743 AD5689双通道输出 例程\r\n");
	
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
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*使能供电配置更新 */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* 当器件的时钟频率低于最大系统频率时，电压调节可以优化功耗，
		 关于系统频率的电压调节值的更新可以参考产品数据手册。  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* 启用HSE振荡器并使用HSE作为源激活PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
	/* 选择PLL作为系统时钟源并配置总线时钟分频器 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 72;
  PeriphClkInitStruct.PLL2.PLL2P = 36;
  PeriphClkInitStruct.PLL2.PLL2Q = 72;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    while(1) { ; }
  }
}
/****************************END OF FILE***************************/
