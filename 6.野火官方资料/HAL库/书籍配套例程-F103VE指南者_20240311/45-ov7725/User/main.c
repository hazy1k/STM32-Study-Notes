/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   Һ����ʾ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./lcd/bsp_ili9341_lcd.h"
#include "./ov7725/bsp_ov7725.h"
#include "./systick/bsp_SysTick.h"
#include "./key/bsp_key.h"  

extern uint8_t Ov7725_vsync;
unsigned int Task_Delay[NumOfTask]; 
extern OV7725_MODE_PARAM cam_mode;



//static void LCD_Test(void);	
//static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	float frame_count = 0;
	uint8_t retry = 0;
  /* ϵͳʱ�ӳ�ʼ����72MHz */
  SystemClock_Config();
  ILI9341_Init ();             //LCD ��ʼ��
  ILI9341_GramScan( 3 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	/********��ʾ�ַ���ʾ��*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH OV7725 Test Demo");
  /* LED �˿ڳ�ʼ�� */
  LED_GPIO_Config();	 
  /* ��ʼ������ */
  DEBUG_USART_Config();
	Key_GPIO_Config();
	SysTick_Init();

	printf("\r\n ** OV7725����ͷʵʱҺ����ʾ����** \r\n"); 
	
	/* ov7725 gpio ��ʼ�� */
	OV7725_GPIO_Config();
	
	LED_BLUE;
	/* ov7725 �Ĵ���Ĭ�����ó�ʼ�� */
	while(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			printf("\r\nû�м�⵽OV7725����ͷ\r\n");
			ILI9341_DispStringLine_EN(LINE(2),"No OV7725 module detected!");
			while(1);
		}
	}
	/*��������ͷ����������ģʽ*/
	OV7725_Special_Effect(cam_mode.effect);
	/*����ģʽ*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*���Ͷ�*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*���ն�*/
	OV7725_Brightness(cam_mode.brightness);
	/*�Աȶ�*/
	OV7725_Contrast(cam_mode.contrast);
	/*����Ч��*/
	OV7725_Special_Effect(cam_mode.effect);
	
	/*����ͼ�������ģʽ��С*/
	OV7725_Window_Set(cam_mode.cam_sx,
										cam_mode.cam_sy,
										cam_mode.cam_width,
										cam_mode.cam_height,
										cam_mode.QVGA_VGA);

	/* ����Һ��ɨ��ģʽ */
	ILI9341_GramScan( cam_mode.lcd_scan );
	ILI9341_DispStringLine_EN(LINE(2),"OV7725 initialize success!");
	printf("\r\nOV7725����ͷ��ʼ�����\r\n");
	
	Ov7725_vsync = 0;
	
	while(1)
	{
		/*���յ���ͼ�������ʾ*/
		if( Ov7725_vsync == 2 )
		{
			frame_count++;
			FIFO_PREPARE;  			/*FIFO׼��*/					
			ImagDisp(cam_mode.lcd_sx,
								cam_mode.lcd_sy,
								cam_mode.cam_width,
								cam_mode.cam_height);			/*�ɼ�����ʾ*/
			
			Ov7725_vsync = 0;			
			LED1_TOGGLE;

		}
		
		/*��ⰴ��*/
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/*LED��ת*/
			LED2_TOGGLE;

		} 
		/*��ⰴ��*/
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			/*LED��ת*/
			LED3_TOGGLE;			
			
		/*��̬��������ͷ��ģʽ��
			����Ҫ�������ʹ�ô��ڡ��û���������ѡ���ȷ�ʽ�޸���Щ������
			�ﵽ��������ʱ��������ͷģʽ��Ŀ��*/
	
		cam_mode.QVGA_VGA = 0,	//QVGAģʽ
		cam_mode.cam_sx = 0,
		cam_mode.cam_sy = 0,	

		cam_mode.cam_width = 320,
		cam_mode.cam_height = 240,

		cam_mode.lcd_sx = 0,
		cam_mode.lcd_sy = 0,
		cam_mode.lcd_scan = 3,
	//LCDɨ��ģʽ�����������ÿ���1��3��5��7ģʽ

		//���¿ɸ����Լ�����Ҫ������������Χ���ṹ�����Ͷ���	
		cam_mode.light_mode = 0,//�Զ�����ģʽ
		cam_mode.saturation = 0,	
		cam_mode.brightness = 0,
		cam_mode.contrast = 0,
		cam_mode.effect = 1,		//�ڰ�ģʽ
		
		/*��������ͷ����д������*/
		OV7725_Special_Effect(cam_mode.effect);
		/*����ģʽ*/
		OV7725_Light_Mode(cam_mode.light_mode);
		/*���Ͷ�*/
		OV7725_Color_Saturation(cam_mode.saturation);
		/*���ն�*/
		OV7725_Brightness(cam_mode.brightness);
		/*�Աȶ�*/
		OV7725_Contrast(cam_mode.contrast);
		/*����Ч��*/
		OV7725_Special_Effect(cam_mode.effect);
		
		/*����ͼ�������ģʽ��С*/
		OV7725_Window_Set(cam_mode.cam_sx,
											cam_mode.cam_sy,
											cam_mode.cam_width,
											cam_mode.cam_height,
											cam_mode.QVGA_VGA);

		/* ����Һ��ɨ��ģʽ */
		ILI9341_GramScan( cam_mode.lcd_scan );
		}
		
		/*ÿ��һ��ʱ�����һ��֡��*/
		if(Task_Delay[0] == 0)  
		{			
			printf("\r\nframe_ate = %.2f fps\r\n",frame_count/10);
			frame_count = 0;
			Task_Delay[0] = 10000;
		}
		
	}
}



/**
  * @brief  ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
//static void Delay ( __IO uint32_t nCount )
//{
//  for ( ; nCount != 0; nCount -- );
//	
//}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
