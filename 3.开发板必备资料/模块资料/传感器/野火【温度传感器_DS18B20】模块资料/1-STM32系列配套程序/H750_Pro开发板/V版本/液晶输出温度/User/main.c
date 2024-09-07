/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   LTDC��Һ����ʾ�ɼ����¶�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H750 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
#include "./ds18b20/bsp_ds18b20.h"

/* ��ʾ������ */
uint8_t dis_buf[1024];
float temperature;

__IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{   
	uint8_t uc,DS18B20Id[8];
	uint8_t DS18B20Id_str[20];
	
	/* ϵͳʱ�ӳ�ʼ����480MHz */
	SystemClock_Config();
	
	/* ����I-Cache */
	SCB_EnableICache();
	
	/* ����D-Cache */
	SCB_EnableDCache();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	/* ���ô���1Ϊ��115200 8-N-1 */
	UARTx_Config();
	
	/* LCD �˿ڳ�ʼ�� */ 
	LCD_Init();
	
	/* LCD ��һ���ʼ�� */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,RGB888);

	/* ʹ��LCD������������ */ 
	LCD_DisplayOn(); 

	/* ѡ��LCD��һ�� */
	LCD_SelectLayer(0);

	/* ��һ����������ʾȫ�� */ 
	LCD_Clear(LCD_COLOR_BLACK);  

	/* �����������ɫ�Լ����屳����ɫ(�˴��ı�������ָLCD�ı�����,ע������) */
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	
	/* ѡ������ */
	LCD_SetFont(&LCD_DEFAULT_FONT);
	
	LCD_DisplayStringLine_EN_CH(1,(uint8_t* )"DS18B20 temperature detect demo");
	/* �¶ȴ�������ʼ�� */
		if(DS18B20_Init()==0)
	{
		printf("DS18B20��ʼ���ɹ�\n");
	}
	else
	{
		printf("DS18B20��ʼ��ʧ��\n");
		printf("�뽫��������ȷ���뵽�����\n");
		LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine_EN_CH(2,(uint8_t* )"DS18B20 initialization failed!");
    LCD_DisplayStringLine_EN_CH(3,(uint8_t* )"Please check the connection!");
		/* ͣ�� */
		while(1)
		{}			
	}		
  DS18B20_ReadId ( DS18B20Id  );           // ��ȡ DS18B20 �����к�	 
	for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
  {    
    sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",DS18B20Id[uc]);
    if(uc == 7)
      DS18B20Id_str[17] = '\0';         
  }
  printf("\r\nDS18B20�����к��ǣ� 0x%s\r\n",DS18B20Id_str);

  sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
  LCD_DisplayStringLine_EN_CH(4,dis_buf);
  
	while(1)
	{		
		temperature=DS18B20_Get_Temp();
		printf("DS18B20��ȡ�����¶�Ϊ��%0.3f\n",temperature);   
    sprintf((char*)dis_buf,"Temperature:   %0.3f   degree Celsius",temperature);
    LCD_DisplayStringLine_EN_CH(7,(uint8_t* )dis_buf);
    HAL_Delay(1000); 
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
