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
#include "./sdio/bsp_sdio_sdcard.h"
#include "./sdio/sdio_test.h"
#include "./led/bsp_led.h"
#include "./FatFS/ff.h"
#include "./FatFS/ff_gen_drv.h"
#include "./FatFS/drivers/sd_diskio.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./font/fonts.h"	
#include "./bmp/bsp_bmp.h"
#include <stdio.h>
#include ".\key\bsp_key.h" 

 void printf_fatfs_error(FRESULT fresult);
/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
char SDPath[4];                   /* SD���߼��豸·�� */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL file,bmpfdst;									/* �ļ����� */
FRESULT f_res;                    /* �ļ�������� */

BYTE pColorData[960];			/* һ�����ɫ���ݻ��� 320 * 3 = 960 */
#define BMP_DEBUG_PRINTF(FORMAT,...)    printf(FORMAT,##__VA_ARGS__)	 
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{ 
  int i, j, k;
	int width, height, l_width;

	BYTE red,green,blue;
	BITMAPFILEHEADER bitHead;
	BITMAPINFOHEADER bitInfoHead;
	WORD fileType;

	unsigned int read_num;
  
  /* ����ϵͳʱ��Ϊ72 MHz */ 
  SystemClock_Config();
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  
	 /* ��ʼ��LED�� */
  LED_GPIO_Config();

	Key_GPIO_Config();
  
	ILI9341_Init ();         //LCD ��ʼ��
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
  ILI9341_GramScan ( 6 );

	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	DEBUG_USART_Config();	
  
  
  printf("\r\n ********** ��ʾBMP�ļ�����ͼ*********** \r\n"); 
	printf("\r\n ʵ��ǰ��ȷ��SD����Ŀ¼�¾���ʵ��ʹ�õ�BMPͼƬ�ļ�, \r\n"); 
	printf("\r\n ���°��ӵ�KEY1��������������Ļ��ͼ \r\n"); 
	 /* ע��һ��FatFS�豸��SD�� */
  FATFS_LinkDriver(&SD_Driver, SDPath); 

  //��SD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
  f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
  printf_fatfs_error(f_res);
  
  ILI9341_GramScan ( 6 );
  
  f_res = f_open( &file , "wildfire_1.bmp", FA_OPEN_EXISTING | FA_READ);	
  
  printf_fatfs_error(f_res);

 /*-------------------------------------------------------------------------------------------------------*/
	if(f_res == FR_OK)
	{
		BMP_DEBUG_PRINTF("���ļ��ɹ�\r\n");

		/* ��ȡ�ļ�ͷ��Ϣ  �����ֽ�*/         
		f_read(&file,&fileType,sizeof(WORD),&read_num);     

		/* �ж��ǲ���bmp�ļ� "BM"*/
		if(fileType != 0x4d42)
		{
			BMP_DEBUG_PRINTF("�ⲻ��һ�� .bmp �ļ�!\r\n");
//			return;
		}
		else
		{
			BMP_DEBUG_PRINTF("����һ�� .bmp �ļ�\r\n");	
		}        

		/* ��ȡBMP�ļ�ͷ��Ϣ*/
		f_read(&file,&bitHead,sizeof(tagBITMAPFILEHEADER),&read_num);        
		showBmpHead(&bitHead);

		/* ��ȡλͼ��Ϣͷ��Ϣ */
		f_read(&file,&bitInfoHead,sizeof(BITMAPINFOHEADER),&read_num);        
		showBmpInforHead(&bitInfoHead);
	}    
	else
	{
		BMP_DEBUG_PRINTF("���ļ�ʧ��!������룺f_res = %d \r\n",f_res);
	}    
/*-------------------------------------------------------------------------------------------------------*/
	width = bitInfoHead.biWidth;
	height = bitInfoHead.biHeight;

	/* ����λͼ��ʵ�ʿ�Ȳ�ȷ����Ϊ32�ı���	*/
	l_width = WIDTHBYTES(width* bitInfoHead.biBitCount);	

	if(l_width > 960)
	{
		BMP_DEBUG_PRINTF("\n ��ͼƬ̫���޷���Һ��������ʾ (<=320)\n");
	}
	
	
	/* ��һ��ͼƬ��С�Ĵ���*/
	ILI9341_OpenWindow(0, 0, width, height);
	ILI9341_Write_Cmd (CMD_SetPixel ); 

	
	/* �ж��Ƿ���24bit���ɫͼ */
	if( bitInfoHead.biBitCount >= 24 )
	{
		for ( i = 0; i < height; i ++ )
		{
			/*���ļ��ĺ������BMP�ļ���ԭʼͼ����Ϊ���½ǵ����Ͻ�*/
      f_lseek ( & file, bitHead .bfOffBits + ( height - i - 1 ) * l_width );	
			
			/* ��ȡһ��bmp�����ݵ�����pColorData���� */
			f_read ( & file, pColorData, l_width, & read_num );				

			for(j=0; j<width; j++) 											   //һ����Ч��Ϣ
			{
				k = j*3;																	 //һ���е�K�����ص����
				red = pColorData[k+2];
				green = pColorData[k+1];
				blue = 	pColorData[k];
				ILI9341_Write_Data ( RGB24TORGB16 ( red, green, blue ) ); //д��LCD-GRAM
			}            			
		}        		
	}    	
	else 
	{        
		BMP_DEBUG_PRINTF("�ⲻ��һ��24λ���ɫBMP�ļ���");
	}
   /* ����ʹ�ã�ȡ������ */
   f_mount(NULL,(TCHAR const*)SDPath,1);	
}

  
  
/**
  * @brief  ��ӡ�����Ϣ
  * @param  ��
  * @retval ��
  */
 void printf_fatfs_error(FRESULT fresult)
{
  switch(fresult)
  {
    case FR_OK:                   
      printf("�������ɹ���\r\n");
    break;
    case FR_DISK_ERR:             
      printf("����Ӳ�����������������\r\n");
    break;
    case FR_INT_ERR:              
      printf("�������Դ���\r\n");
    break;
    case FR_NOT_READY:            
      printf("���������豸�޷�������\r\n");
    break;
    case FR_NO_FILE:              
      printf("�����޷��ҵ��ļ���\r\n");
    break;
    case FR_NO_PATH:             
      printf("�����޷��ҵ�·����\r\n");
    break;
    case FR_INVALID_NAME:         
      printf("������Ч��·������\r\n");
    break;
    case FR_DENIED:               
    case FR_EXIST:                
      printf("�����ܾ����ʡ�\r\n");
    break;
    case FR_INVALID_OBJECT:       
      printf("������Ч���ļ���·����\r\n");
    break;
    case FR_WRITE_PROTECTED:      
      printf("�����߼��豸д������\r\n");
    break;
    case FR_INVALID_DRIVE:        
      printf("������Ч���߼��豸��\r\n");
    break;
    case FR_NOT_ENABLED:          
      printf("������Ч�Ĺ�������\r\n");
    break;
    case FR_NO_FILESYSTEM:        
      printf("������Ч���ļ�ϵͳ��\r\n");
    break;
    case FR_MKFS_ABORTED:         
      printf("���������������⵼��f_mkfs��������ʧ�ܡ�\r\n");
    break;
    case FR_TIMEOUT:              
      printf("����������ʱ��\r\n");
    break;
    case FR_LOCKED:               
      printf("�����ļ���������\r\n");
    break;
    case FR_NOT_ENOUGH_CORE:      
      printf("�������ļ���֧�ֻ�ȡ�ѿռ�ʧ�ܡ�\r\n");
    break;
    case FR_TOO_MANY_OPEN_FILES:  
      printf("������̫���ļ���\r\n");
    break;
    case FR_INVALID_PARAMETER:    
      printf("����������Ч��\r\n");
    break;
  }
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
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV2;
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
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_0)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
