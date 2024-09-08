/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   红外测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 767 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "main.h"  
#include "stm32f7xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./IrDa/bsp_irda.h"
#include "./systick/bsp_SysTick.h"

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
    uint8_t key_val;  
    /* 配置系统时钟为216 MHz */
    SystemClock_Config();
	
	  SCB_EnableICache();//使能I-Cache
    SCB_EnableDCache();//使能D-Cache   

	
	  SysTick_Init();
	
	  HAL_NVIC_SetPriority(SysTick_IRQn, 0,1);
    /* 初始化RGB彩灯 */
    LED_GPIO_Config();

    /* 初始化USART1 配置模式为 115200 8-N-1 */
    UARTx_Config();
	
    printf("\r\n 这是一个红外遥控发射与接收实验 \r\n");
  
   /* 初始化红外接收头CP1838用到的IO */
	 IrDa_Init();  

	for(;;)
	{	  
    if( frame_flag == 1 ) /* 一帧红外数据接收完成 */
    {
      key_val = IrDa_Process();
      printf("\r\n key_val=%d \r\n",key_val);
      printf("\r\n 按键次数frame_cnt=%d \r\n",frame_cnt);
      printf("\r\n 中断次数isr_cnt=%d \r\n",isr_cnt);
      
      /* 不同的遥控器面板对应不同的键值，需要实际测量 */
      switch( key_val )
      {
        case 0:
        LED1_TOGGLE;  
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n Error \r\n");
        break;
        
        case 162:
        LED1_TOGGLE;  
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n POWER \r\n");
        break;
        
        case 226:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n MENU \r\n");
        break;
        
        case 34:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n TEST \r\n");
        break;
        
        case 2:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n + \r\n");
        break;
        
        case 194:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n RETURN \r\n");
        break;
        
        case 224:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n |<< \r\n");
        break;
        
        case 168:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n > \r\n");
        break;
        
        case 144:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n >>| \r\n");
        break;
        
        case 104:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 0 \r\n");
        break;
        
        case 152:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n - \r\n");
        break;
        
        case 176:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n C \r\n");
        break;
        
        case 48:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 1 \r\n");
        break;
        
        case 24:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 2 \r\n");
        break;
        
        case 122:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 3 \r\n");
        break;
        
        case 16:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 4 \r\n");
        break;
        
        case 56:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 5 \r\n");
        break;
        
        case 90:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 6 \r\n");
        break;
        
          case 66:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 7 \r\n");
        break;
        
        case 74:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 8 \r\n");
        break;
        
        case 82:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 9 \r\n");
        break;
        
        default:       
        break;
      }      
    }
	}
}



/**
  * @brief  System Clock 配置
  *         system Clock 配置如下 : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  无
  * @retval 无
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q 
	 * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
	 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 激活 OverDrive 模式以达到216M频率  */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
	 * SYSCLK = PLLCLK     = 216M
	 * HCLK   = SYSCLK / 1 = 216M
	 * PCLK2  = SYSCLK / 2 = 108M
	 * PCLK1  = SYSCLK / 4 = 54M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}
void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

