/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"

static void LCD_Test(void);	
static void Delay ( __IO uint32_t nCount );

void Printf_Charater(void)   ;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  /* ϵͳʱ�ӳ�ʼ����72 MHz */
  SystemClock_Config();
  /* LED �˿ڳ�ʼ�� */
  LED_GPIO_Config();	 
  /* ��ʼ������ */
  DEBUG_USART_Config();

  ILI9341_Init ();             //LCD ��ʼ��
	
	printf("\r\n ********** Һ����������ʾ���������С��*********** \r\n"); 
	printf("\r\n ��������ʾ�����������Ķ������е�readme.txt�ļ�˵��������Ҫ���FLASH��ˢ��ģ����\r\n"); 
	
 //����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 //���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	ILI9341_GramScan ( 6 );
	
	Printf_Charater();
	
	while ( 1 )
	{
		LCD_Test();
	}
		
}



/*���ڲ��Ը���Һ���ĺ���*/
void LCD_Test(void)
{
	/*��ʾ��ʾ����*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	/********��ʾ�ַ���ʾ��*******/ 
  ILI9341_DispStringLine_EN_CH(LINE(0),"Ұ��BH");
	//��ʾָ����С���ַ� 
	ILI9341_DisplayStringEx(0,1*24,24,24,(uint8_t *)"Ұ��BH",0);
	ILI9341_DisplayStringEx(2*48,0*48,48,48,(uint8_t *)"Ұ��BH",0);

	/********��ʾ����ʾ��*******/
	LCD_SetTextColor(GREEN);

	/*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"��ʾ������ %d ",testCNT);
  LCD_ClearLine(LINE(5));	/* ����������� */
	
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	ILI9341_DispStringLine_EN_CH(LINE(5),dispBuff);

	/*******��ʾͼ��ʾ��******/
  /* ��ֱ�� */
  
  LCD_ClearLine(LINE(7));/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN_CH(LINE(7),"��ֱ�ߣ�");
  
	LCD_SetTextColor(RED);
  ILI9341_DrawLine(50,170,210,230);  
  ILI9341_DrawLine(50,200,210,240);
  
	LCD_SetTextColor(GREEN);
  ILI9341_DrawLine(100,170,200,230);  
  ILI9341_DrawLine(200,200,220,240);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawLine(110,170,110,230);  
  ILI9341_DrawLine(130,200,220,240);
  
  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */
  
  
  /*������*/

  LCD_ClearLine(LINE(7));	/* ����������� */
	LCD_SetTextColor(BLUE);

  ILI9341_DispStringLine_EN_CH(LINE(7),"�����Σ�");

	LCD_SetTextColor(RED);
  ILI9341_DrawRectangle(50,200,100,30,1);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawRectangle(160,200,20,40,0);
	
	LCD_SetTextColor(BLUE);
  ILI9341_DrawRectangle(170,200,50,20,1);
  
  Delay(0xFFFFFF);
	
	ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

  /* ��Բ */
  LCD_ClearLine(LINE(7));	/* ����������� */
	LCD_SetTextColor(BLUE);
	
  ILI9341_DispStringLine_EN_CH(LINE(7),"��Բ");
	
	LCD_SetTextColor(RED);
  ILI9341_DrawCircle(100,200,20,0);
	
	LCD_SetTextColor(GREEN);
  ILI9341_DrawCircle(100,200,10,1);
	
	LCD_SetTextColor(BLUE);
	ILI9341_DrawCircle(140,200,20,0);

  Delay(0xFFFFFF);
  
  ILI9341_Clear(0,16*8,LCD_X_LENGTH,LCD_Y_LENGTH-16*8);	/* ��������ʾȫ�� */

}


/**
  * @brief  ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}



/*"��"�ַ�����ģ16x16 */ 	  
unsigned char charater_matrix[] =    
{ /*"��",0*/
0x01,0x00,0x21,0x08,0x11,0x08,0x09,0x10,0x09,0x20,0x01,0x00,0x7F,0xF8,0x00,	0x08,
0x00,0x08,0x00,0x08,0x3F,0xF8,0x00,0x08,0x00,0x08,0x00,0x08,0x7F,0xF8,0x00,0x08,

};   
  
/**
  * @brief  ʹ�ô�������λ����ӡ��ģ
	*					��ʾ��ģ��ʾԭ��
  * @retval ��
  */
void Printf_Charater(void)   
{   
	int i,j;   
	unsigned char kk; 
  
	/*i�����м���*/
  for ( i=0;i<16;i++)   
	{   
		/*j����һ�ֽ������ݵ���λ����*/
		/*һ�����صĵ�һ���ֽ�*/
    for(j=0; j<8; j++)   
    {      
			/*һ������λһ������λ�ش���*/
      kk = charater_matrix[2*i] << j ;  //����Jλ          
			if( kk & 0x80)     
			{   
				printf("*"); //������λΪ1�����*�ţ���ʾ�ʼ�
			}   
			else  
			{   
				printf(" "); //������λΪ0������ո񣬱�ʾ�հ�  
			}   
    }   
		/*һ�����صĵڶ����ֽ�*/
    for(j=0; j<8; j++)   
		{              
      kk = charater_matrix[2*i+1] << j ;  //����Jλ   
       
			if( kk & 0x80)         
			{   
				printf("*"); //������λΪ1�����*�ţ���ʾ�ʼ�   
			}   
			else  
			{   
				printf(" "); //������λΪ0������ո񣬱�ʾ�հ� 
			}   
		}         
    printf("\n");    //�����һ�����أ����� 
	}   
	printf("\n\n"); 		//һ����������
}   

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
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
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
