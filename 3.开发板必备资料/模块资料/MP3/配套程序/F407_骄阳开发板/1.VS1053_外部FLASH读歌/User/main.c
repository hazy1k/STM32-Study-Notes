/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   15-串行FLASH文件系统FatFs
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./flash/bsp_spi_flash.h" 
#include "ff.h"
#include  "VS1053.h"
#include "string.h"
#include "flac.h"
/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
FATFS fs;													/* FatFs文件系统对象 */
FIL file;													/* 文件对象 */
FRESULT res_flash;                /* 文件操作结果 */
UINT bw;                    
uint8_t  buffer[BUFSIZE];
/**
  ******************************************************************************
  *                                任务函数
  ******************************************************************************
  */
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  SystemClock_Config();
	/* 初始化LED */
	LED_GPIO_Config();	
	LED1_ON;
	
	/* 初始化调试串口，一般为串口1 */
	DEBUG_USART_Config();	
  
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_flash = f_mount(&fs,"1:",1);
	
  VS_Init(); 
  printf("vs1053:%4X\n",VS_Ram_Test());
  HAL_Delay(100);
  VS_Sine_Test();	
  VS_HD_Reset();
  VS_Soft_Reset();
	
	while(1)
	{
		// 测试歌曲放在外部FLASH W25Q64中
		vs1053_player_song("1:TestFile.mp3");
		printf("MusicPlay End\n");
	}		

	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"1:",1);

}


void vs1053_player_song(uint8_t *filepath)
{
	uint16_t i=0;
	
	VS_Restart_Play();  					
	VS_Set_All();        							 
	VS_Reset_DecodeTime();
	
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
		VS_Load_Patch((uint16_t*)vs1053b_patch,VS1053B_PATCHLEN);
	
	res_flash=f_open(&file,(const TCHAR*)filepath,FA_READ);

	if(res_flash==0)
	{ 
		VS_SPI_SpeedHigh();				   
		while(1)
		{
			i=0;	
			res_flash=f_read(&file,buffer,BUFSIZE,(UINT*)&bw);		
			do
			{  	
				if(VS_Send_MusicData(buffer+i)==0)
				{
					i+=32;
				}
			}while(i<bw);
			
			if(bw!=BUFSIZE||res_flash!=0)
			{
				break;	  
			}
			LED2_TOGGLE;
		}
		f_close(&file);
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
