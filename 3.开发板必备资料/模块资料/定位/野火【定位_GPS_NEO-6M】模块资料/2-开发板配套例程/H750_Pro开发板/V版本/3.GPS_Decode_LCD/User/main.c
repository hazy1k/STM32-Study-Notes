/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����NEMA�����
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
#include "./mpu/bsp_mpu.h" 
#include "string.h"
#include "./gps/gps_config.h"

extern DMA_HandleTypeDef DMA_Handle;
extern UART_HandleTypeDef GPS_UartHandle;
extern void nmea_decode_test(void);
// WIFI��SD������SDIO���ߣ���ʹ��SD��ʱ������WIFI�ĳ�ͻ
// ��Ҫ��WIFI��ʹ������PDN���͡�
void WIFI_PDNPIN_SET(void)
{
   	GPIO_InitTypeDef  GPIO_InitStruct;
   __GPIOB_CLK_ENABLE();														   
    GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,(GPIO_PinState)0);
} 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	
	/* ϵͳʱ�ӳ�ʼ����480MHz */
    SystemClock_Config();
  
	SCB_EnableICache();    // ʹ��ָ�� Cache
	SCB_EnableDCache();    // ʹ������ Cache
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
  
    LED_BLUE;
	
	/*LCD�˿ڳ�ʼ��*/
	LCD_Init();
	
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,ARGB8888);
	
	LCD_DisplayOn(); 
	
	LCD_SelectLayer(0);
	
	LCD_Clear(LCD_COLOR_BLACK);  

	/*���ڳ�ʼ��*/
	UARTx_Config();
  
  //GPS_Config();

	// ʧ��WIFI
	WIFI_PDNPIN_SET();

  printf("\r\nҰ�� GPSģ���������\r\n");   
	printf("\r\n�������SD���ڵ�gpslog.txt�ļ���λ��־���ݽ��н���\r\n"); 
	printf("\r\n����Ҫ��GPSģ�鴮�ڴ��ص����ݽ��룬");
	printf("��ע�͵�gps_config.h�ļ��ĺ�__GPS_LOG_FILE \r\n"); 
	
  /* GPS������� */
	HAL_UART_Receive_DMA (&GPS_UartHandle,(uint8_t *)gps_rbuff,GPS_RBUFF_SIZE);
  nmea_decode_test();
  
  while(1);
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

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/****************************END OF FILE***************************/
