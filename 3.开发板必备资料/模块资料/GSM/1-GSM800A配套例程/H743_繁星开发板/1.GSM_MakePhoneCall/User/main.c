/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GSMģ��绰���Ų��Գ���
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "./key/bsp_key.h"
#include <string.h>

char    testbuff[100];
uint8_t len;
char *  rebuff;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{  
  static uint8_t timecount=0;
	char num[20]={0};
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
 
  SCB_EnableICache();    // ʹ��ָ�� Cache
  SCB_EnableDCache();    // ʹ������ Cache
    
	SysTick_Init();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
  Key_GPIO_Config();
    /* ���ô���1Ϊ��115200 8-N-1 */
	UARTx_Config();
  
	 printf("\r\nҰ��BH-GSMģ�鲦������\r\n");          
    
    
    //���ģ����Ӧ�Ƿ�����
    while(gsm_init()!= GSM_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
			GSM_DELAY(500);

    }
    
    printf("\r\nͨ����ģ����Ӧ���ԣ�5���ʼ���Ų���...\r\n");
    
    /* ��ʱ3���ٷ������ģ�� */
    GSM_DELAY(5000);
    
    //����绰
  
    gsm_call("17320519310");                           //����112�绰����
    rebuff = gsm_waitask(0);
    if(strstr(rebuff,"ATD") != NULL)           //��ӦOK����ʾģ���������յ�����
    {
      printf("\r\n���ں���\r\n");
      GSM_CLEAN_RX();                     //������ջ�����
      rebuff = gsm_waitask(0);            //���µȴ���Ϣ
     
      if(strstr(rebuff,"NO CARRIER") != NULL) //��ӦNO CARRIER,ͨ������
       {
         printf("\r\nͨ������\r\n");
       }
      else if(strstr(rebuff,"NO ANSWER") != NULL)   //��ӦNO ANSWER�����˽���
      {
        printf("\r\n�㲦��ĵ绰��ʱ���˽��������Ժ��ٲ�\r\n");
      }  
    }
	
	GSM_CLEAN_RX();                     //������ջ�����
	
    while(1)
	{
		if(timecount>=100)
		{
			if(IsRing(num)== GSM_TRUE)
			{
				printf("Ringing:\nFrom:%s\n�밴��KEY2�������߰���KEY1�Ҷ�\n\r",num);	
			}
			timecount=0;
		}
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			GSM_HANGON();
		}
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			GSM_HANGOFF();
		}	
		timecount++;
		GSM_DELAY(10);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
