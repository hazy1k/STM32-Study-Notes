/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   LTDC��Һ����ʾ�ɼ�����ʪ��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32F767 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
  
#include "stm32f7xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./lcd/bsp_lcd.h"
#include <stdlib.h>
#include "main.h"
#include  "./dht11/bsp_dht11.h"



/* ��ʾ������ */
uint8_t dis_buf[1024];

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{   
	DHT11_Data_TypeDef DHT11_Data;
	
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
	
	/* ����I-Cache */
	SCB_EnableICache();
	
	/* ����D-Cache */
	SCB_EnableDCache();
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	
	/* ���ô���1Ϊ��115200 8-N-1 */
	DEBUG_USART_Config();
	
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
	
	LCD_DisplayStringLine_EN_CH(1,(uint8_t* )"DHT11 temperature & humidity detect demo");
	
	/* DHT11��ʼ�� */
	DHT11_GPIO_Config();	
  
	while(1)
	{		
		/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
		if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d %%RH ���¶�Ϊ %d.%d�� \r\n",\
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
    HAL_Delay(1200); 
	}
}


/**
  * @brief  System Clock ����
  *         system Clock �������� : 
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
  * @param  ��
  * @retval ��
  */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* ʹ��HSE������HSEΪPLL��ʱ��Դ������PLL�ĸ��ַ�Ƶ����M N P Q 
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

	/* ���� OverDrive ģʽ�Դﵽ216MƵ��  */  
	ret = HAL_PWREx_EnableOverDrive();
	if(ret != HAL_OK)
	{
		while(1) { ; }
	}

	/* ѡ��PLLCLK��ΪSYSCLK�������� HCLK, PCLK1 and PCLK2 ��ʱ�ӷ�Ƶ���� 
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

/*********************************************END OF FILE**********************/

