/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   MPU6050 基本数据读
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 H743 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32h7xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./delay/core_delay.h"
#include <stdlib.h>
#include "main.h"
#include "string.h"
#include "./sdram/bsp_sdram.h"  
#include "./i2c/i2c.h"
#include "./mpu6050/mpu6050.h"
#include "./lcd/bsp_lcd.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	static short Acel[3];
	static short Gyro[3];
	static float Temp;
	char cStr [ 70 ];
	
  HAL_Init();
  /* 系统时钟初始化成480 MHz */
  SystemClock_Config();
	/* 开启I-Cache */
	SCB_EnableICache();
	
	/* 开启D-Cache */
	SCB_EnableDCache();
  /* LED 端口初始化 */
  LED_GPIO_Config();
	
  /*初始化USART1*/
	UARTx_Config(); 
		
	//初始化 I2C
	I2cMaster_Init(); 

	printf("\r\n 欢迎使用野火  STM32 H743 开发板。\r\n");		 

	printf("\r\n 这是一个I2C外设(MPU6050)读写测试例程 \r\n");
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
	
	LCD_DisplayStringLine_EN_CH(1,(uint8_t* )"           This is a MPU6050 demo       ");
	LCD_DisplayStringLine_EN_CH(2,(uint8_t* )"                 (Base Version)       ");
	
 	//MPU6050初始化
	MPU6050_Init();
	
	//检测MPU6050
	if (MPU6050ReadID() == 1)
	{	
		while(1)
		{
			MPU6050ReadAcc(Acel);
			printf("加速度：%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
			sprintf ( cStr, "Acceleration:%8d%8d%8d",Acel[0],Acel[1],Acel[2] );	//加速度原始数据
			LCD_DisplayStringLine_EN_CH(7,(uint8_t* )cStr);			
			
			MPU6050ReadGyro(Gyro);
			printf("    陀螺仪%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
			sprintf ( cStr, "Gyro        :%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2] );	//角原始数据
			LCD_DisplayStringLine_EN_CH(8,(uint8_t* )cStr);		
			
			MPU6050_ReturnTemp(&Temp);
			printf("    温度%8.2f\r\n",Temp);				
      sprintf ( cStr, "Temperture  :%8.2f",Temp );	//温度值
			LCD_DisplayStringLine_EN_CH(9,(uint8_t* )cStr);					

      HAL_Delay(500);
		}
	}
	else
	{
		printf("\r\n没有检测到MPU6050传感器！\r\n");
    LED_RED; 
		while(1);	
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
}

/*********************************************END OF FILE**********************/

