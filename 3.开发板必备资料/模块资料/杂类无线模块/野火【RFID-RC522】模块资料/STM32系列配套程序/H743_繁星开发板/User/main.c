/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RS485通信实验
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h"
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./RC522/rc522_config.h"
#include "./RC522/rc522_function.h"


static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void)   ;
void IC_test ( void );
/**
  * @brief  简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 使能指令缓存 */
	SCB_EnableICache();
  /* 使能数据缓存 */
  SCB_EnableDCache();
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();

	/* LED 端口初始化 */
	LED_GPIO_Config();	
	
	/* 配置串口1为：115200 8-N-1 */
	DEBUG_USART_Config();
	
	/*RC522模块所需外设的初始化配置*/
  RC522_Init (); 
	
	ILI9806G_Init ();         //LCD 初始化

	DEBUG_USART_Config();		
	
	printf("\r\n ********** 液晶屏英文显示程序*********** \r\n"); 
	printf("\r\n 本程序不支持中文，显示中文的程序请学习下一章 \r\n"); 

	//其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9806G_GramScan ( 6 );    
	ILI9806G_DispStringLine_EN(LINE(7),"Please put the IC card on WF-RC522 antenna area ...");
	    
	PcdReset ();
   
  /*设置工作方式*/   
	M500PcdConfigISOType ( 'A' );
	
	while ( 1 )
	{
		
		IC_test();
	}
}

/**
  * @brief  IC测试函数
  * @param  无
  * @retval 无
  */
void IC_test ( void )
{
	char cStr [ 30 ];
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
	uint8_t ucStatusReturn;      /*返回状态*/                                                                                          
  static uint8_t ucLineCount = 2; 

  while ( 1 )
  { 
		
    /*寻卡*/
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )  
       /*若失败再次寻卡*/
			ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );		                                                

		if ( ucStatusReturn == MI_OK  )
		{
      /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
			if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
			{
				sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",
                  ucArray_ID [ 0 ],
                  ucArray_ID [ 1 ],
                  ucArray_ID [ 2 ],
                  ucArray_ID [ 3 ] );
        
        	printf ( "%s\r\n",cStr ); 	
				
				ILI9806G_DispStringLine_EN(ucLineCount,(char* )cStr );
       
				
				ucLineCount ++;
				
				if ( ucLineCount == 10 ) ucLineCount = 2;				
			}		
		}		
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
	*            PLL_Q                = 24
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
  RCC_OscInitStruct.PLL.PLLQ = 24;  /* fdcan_ker_ck = 40 MHz */
 
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
/****************************END OF FILE***************************/
