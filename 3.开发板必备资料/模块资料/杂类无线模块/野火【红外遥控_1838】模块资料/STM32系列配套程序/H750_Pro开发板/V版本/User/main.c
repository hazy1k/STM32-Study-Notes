/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����ң�����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H750������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./IrDa/bsp_irda.h"
#include "./systick/bsp_SysTick.h"

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
     uint8_t key_val;  
	 /* ϵͳʱ�ӳ�ʼ����480MHz */
	  SystemClock_Config();
	
		SCB_EnableICache();//ʹ��I-Cache
    SCB_EnableDCache();//ʹ��D-Cache  

    SysTick_Init();	
	
    HAL_NVIC_SetPriority(SysTick_IRQn, 0,0);
    /* ��ʼ��RGB�ʵ� */
	
    LED_GPIO_Config();

    /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
    UARTx_Config();
	
    printf("\r\n ����һ������ң�ط��������ʵ�� \r\n");
  
   /* ��ʼ���������ͷCP1838�õ���IO */
	 IrDa_Init();  

	for(;;)
	{	  
    if( frame_flag == 1 ) /* һ֡�������ݽ������ */
    {
      key_val = IrDa_Process();
      printf("\r\n key_val=%d \r\n",key_val);
      printf("\r\n ��������frame_cnt=%d \r\n",frame_cnt);
      printf("\r\n �жϴ���isr_cnt=%d \r\n",isr_cnt);
      
      /* ��ͬ��ң��������Ӧ��ͬ�ļ�ֵ����Ҫʵ�ʲ��� */
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
  * @brief  System Clock ����
  *         system Clock ��������: 
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

  /** ���õ�Դ���ø���
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** ����������ѹ�������ѹ
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** ��ʼ��CPU��AHB��APB����ʱ��
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
  /** ��ʼ��CPU��AHB��APB����ʱ��
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
