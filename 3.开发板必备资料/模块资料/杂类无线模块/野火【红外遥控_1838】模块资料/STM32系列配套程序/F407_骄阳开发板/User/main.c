/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   红外测试
  ******************************************************************************
  * @attention
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
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
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
  
  HAL_Init();        
  /* 配置系统时钟为168 MHz */ 
  SystemClock_Config();
	/* 初始化RGB彩灯 */ 
  LED_GPIO_Config(); 
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  
	SysTick_Init();
	
	HAL_NVIC_SetPriority(SysTick_IRQn, 0,1);
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



