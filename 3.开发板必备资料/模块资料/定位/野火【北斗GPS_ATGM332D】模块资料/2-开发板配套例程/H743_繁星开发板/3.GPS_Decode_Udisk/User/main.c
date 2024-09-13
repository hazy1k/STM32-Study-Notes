/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GPS����
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./lcd/bsp_nt35510_lcd.h"
#include "string.h"
#include "./delay/core_delay.h"   
#include "./gps/gps_config.h"
#include "usb_host.h"


extern DMA_HandleTypeDef DMA_Handle;
extern UART_HandleTypeDef GPS_UartHandle;
extern void nmea_decode_test(void);



static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

  //��Cache����write-through��ʽ
  SCB->CACR|=1<<2;
}
extern uint8_t USBDevice_Ready;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	
	/* ϵͳʱ�ӳ�ʼ����480MHz */
  SystemClock_Config();
	
	CPU_CACHE_Enable();
  
	SCB_EnableICache();    // ʹ��ָ�� Cache
	SCB_EnableDCache();    // ʹ������ Cache
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
  
  LED1_ON;

	MX_USB_HOST_Init();
	
	/*LCD�˿ڳ�ʼ��*/
	NT35510_Init();

	/*���ڳ�ʼ��*/
	UARTx_Config();
  
  GPS_Config();

  printf("\r\nҰ�� GPSģ���������\r\n"); 
  
	printf("\r\n�������SD���ڵ�gpslog.txt�ļ���λ��־���ݽ��н���\r\n"); 
	printf("\r\n����Ҫ��GPSģ�鴮�ڴ��ص����ݽ��룬");
	printf("��ע�͵�gps_config.h�ļ��ĺ�__GPS_LOG_FILE \r\n"); 
  LCD_SetColors(WHITE,BLACK);
  
  while(1)
	{
		MX_USB_HOST_Process();
		if(USBDevice_Ready == 1)
		{
			/* GPS������� */
			HAL_UART_Receive_DMA (&GPS_UartHandle,(uint8_t *)gps_rbuff,GPS_RBUFF_SIZE);
			nmea_decode_test();
			USBDevice_Ready++;
		}
	}


}

/**
  * @brief  System Clock ����
  *         system Clock ��������: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*ʹ�ܹ������ø��� */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* ��������ʱ��Ƶ�ʵ������ϵͳƵ��ʱ����ѹ���ڿ����Ż����ģ�
		 ����ϵͳƵ�ʵĵ�ѹ����ֵ�ĸ��¿��Բο���Ʒ�����ֲᡣ  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* ����HSE������ʹ��HSE��ΪԴ����PLL */
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
  RCC_OscInitStruct.PLL.PLLQ = 20;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
	/* ѡ��PLL��Ϊϵͳʱ��Դ����������ʱ�ӷ�Ƶ�� */
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USB|RCC_PERIPHCLK_SPI1;
  PeriphClkInitStruct.PLL3.PLL3M = 5;
  PeriphClkInitStruct.PLL3.PLL3N = 120;
  PeriphClkInitStruct.PLL3.PLL3P = 3;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL3;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    while(1) { ; }
  }
  /** Enable USB Voltage detector 
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/****************************END OF FILE***************************/
