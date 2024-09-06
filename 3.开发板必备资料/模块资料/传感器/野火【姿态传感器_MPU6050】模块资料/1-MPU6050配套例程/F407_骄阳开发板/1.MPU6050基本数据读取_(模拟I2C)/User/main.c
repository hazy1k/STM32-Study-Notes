/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   模拟I2C外设(MPU6050)读写测试例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 骄阳 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include <stdlib.h>
#include "main.h"
#include "./i2c/bsp_i2c.h"
#include "./mpu6050/mpu6050.h"

//设置是否使用LCD进行显示，不需要的话把这个宏注释掉即可
//#define USE_LCD_DISPLAY

#ifdef USE_LCD_DISPLAY
  #include "./lcd/bsp_ili9806g_lcd.h"
#endif


/* MPU6050数据 */
short Acel[3];
short Gyro[3];
float Temp;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 系统时钟初始化成168MHz */
	SystemClock_Config();
	/* LED 端口初始化 */	
	LED_GPIO_Config();
	/* 配置串口1为：115200 8-N-1 */
	DEBUG_USART_Config();
  //初始化 I2C
	I2C_Init();
  
#ifdef USE_LCD_DISPLAY			
  /* 屏幕初始化 */
	ILI9806G_Init();
	
	ILI9806G_GramScan ( 6 );
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);
  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
#endif

	printf("\r\n 欢迎使用野火 STM32F407 开发板。\r\n");		 
	printf("\r\n 这是一个模拟I2C外设(MPU6050)读写测试例程 \r\n");

 	//MPU6050初始化
	MPU6050_Init();
  //检测MPU6050
  if( MPU6050ReadID() == 0 )
  {
		printf("\r\n没有检测到MPU6050传感器！\r\n");
		LED2_ON;
    
    #ifdef USE_LCD_DISPLAY			
    ILI9806G_DispStringLine_EN(LINE(4),(char* )"No MPU6050 detected! ");			//野火自带的17*24显示
    ILI9806G_DispStringLine_EN(LINE(5),(char* )"Please check the hardware connection! ");//野火自带的17*24显示
    #endif
    
		while(1);	//检测不到MPU6050 LED2常亮然后卡死
	}
	
  /* 配置SysTick定时器和中断 */
  SysTick_Init(); //配置 SysTick 为 1ms 中断一次，在中断里读取传感器数据
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //启动定时器
  

  while(1)
  {
    if( task_readdata_finish ) //task_readdata_finish = 1 表示读取MPU6050数据完成
    {
      
      printf("加速度：%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
      
      printf("    陀螺仪%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
      
      printf("    温度%8.2f\r\n",Temp);
      
      #ifdef USE_LCD_DISPLAY
      {
        char cStr [ 70 ];
        sprintf ( cStr, "Acceleration:%8d%8d%8d",Acel[0],Acel[1],Acel[2] );	//加速度原始数据
        ILI9806G_DispStringLine_EN(LINE(7),(char* )cStr);			

        sprintf ( cStr, "Gyro        :%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2] );	//角原始数据
        ILI9806G_DispStringLine_EN(LINE(8),(char* )cStr);			

        sprintf ( cStr, "Temperture  :%8.2f",Temp );	//温度值
        ILI9806G_DispStringLine_EN(LINE(9),(char* )cStr);
      }
      #endif
      
      task_readdata_finish = 0; // 清零标志位
    }
    
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
 void SystemClock_Config(void)
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
