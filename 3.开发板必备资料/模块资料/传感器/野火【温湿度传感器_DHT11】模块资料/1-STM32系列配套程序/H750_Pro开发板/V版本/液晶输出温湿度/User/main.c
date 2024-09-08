/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   LTDC—液晶显示采集的温湿度
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
	 
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./sdram/bsp_sdram.h" 
#include "./lcd/bsp_lcd.h"
#include "string.h"
#include "./delay/core_delay.h"
#include "./dht11/bsp_dht11.h"


/* 显示缓冲区 */
uint8_t dis_buf[1024];

__IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{   
	DHT11_Data_TypeDef DHT11_Data;
	
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();
	
	/* 开启I-Cache */
	SCB_EnableICache();
	
	/* 开启D-Cache */
	SCB_EnableDCache();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	/* 配置串口1为：115200 8-N-1 */
	UARTx_Config();
	
	/* LCD 端口初始化 */ 
	LCD_Init();
	
	/* LCD 第一层初始化 */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,RGB888);

	/* 使能LCD，包括开背光 */ 
	LCD_DisplayOn(); 

	/* 选择LCD第一层 */
	LCD_SelectLayer(0);

	/* 第一层清屏，显示全黑 */ 
	LCD_Clear(LCD_COLOR_BLACK);  

	/* 设置字体的颜色以及字体背景颜色(此处的背景不是指LCD的背景层,注意区分) */
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	
	/* 选择字体 */
	LCD_SetFont(&LCD_DEFAULT_FONT);
	
	LCD_DisplayStringLine_EN_CH(1,(uint8_t* )"DHT11 temperature & humidity detect demo");
	
	/* DHT11初始化 */
	DHT11_GPIO_Config();	
  
	while(1)
	{		
		/*调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息*/
		if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d %%RH ，温度为 %d.%d℃ \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
      
      LCD_ClearLine(2);
      LCD_ClearLine(3);
  
      sprintf((char*)dis_buf,"humidity: %2d.%2d %%RH ",DHT11_Data.humi_int,DHT11_Data.humi_deci);
      LCD_DisplayStringLine_EN_CH(4,dis_buf);
      
      sprintf((char*)dis_buf,"temperature: %2d.%2d degree Celsius ",DHT11_Data.temp_int,DHT11_Data.temp_deci);
      LCD_DisplayStringLine_EN_CH(6,dis_buf);
		}		
		else
		{
		  printf("Read DHT11 ERROR!\r\n");
      
      LCD_SetTextColor(LCD_COLOR_RED);
      LCD_DisplayStringLine_EN_CH(2,(uint8_t* )"DHT11 initialization failed!");
      LCD_DisplayStringLine_EN_CH(3,(uint8_t* )"Please check the connection!");	
		}
    HAL_Delay(800); 
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
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 启用电源配置更新
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** 配置主内稳压器输出电压
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** 初始化CPU、AHB和APB总线时钟
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		while(1);
  }
  /** 初始化CPU、AHB和APB总线时钟
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
		while(1);
  }
}
/****************************END OF FILE***************************/
