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
#include  "./ds18b20/bsp_ds18b20.h"


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
	
	/* ϵͳʱ�ӳ�ʼ����236MHz */
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

