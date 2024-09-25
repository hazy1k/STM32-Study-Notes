/**
  ******************************************************************************
  * @file    GUIDEMO_Fatfs.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����ļ�ϵͳ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "stm32f10x.h"
#include "..\User\usart\bsp_usart1.h"
#include "..\User\lcd\bsp_ili9341_lcd.h"
#include "ff.h"
#include "GUI.h"

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[4096]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "Welcome to use Wildfire ISO-V2 stm32 Development Board,\
this is a demo about enWin && Flash FatFS.\n\
today is a good day";

void Fatfs_MainTask(void)
{
	  /* ������ʾ�ļ����ݵĴ��ڴ�С */ 
		GUI_RECT file_content={5,120,235,250};
	
		/* ����Ļˢ�ϱ�����ɫ */
		GUI_ClearRect(0,0,240,320);	
	  
		/* ����������ɫ */
		GUI_SetFont(&GUI_Font8x16);

		GUI_DispStringAt ("f_mount...",5,20);
		
		GUI_Delay(500);		
		LCD_BK_EN;//��Һ��������		
		
		/* Register work area for each volume (Always succeeds regardless of disk status) */
		f_mount(&fs,"0:",1);	
//		f_mount(&fs,"1:",1);
		/* function disk_initialize() has been called in f_open */
		
	    GUI_DispStringAt ("f_open :newfile.txt ...",5,40);
	
		/* Create new file on the drive 0 */
		res = f_open(&fnew, "0:newfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
		 
		if ( res == FR_OK )
		{
			res = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}

		GUI_DispStringAt ("f_read :newfile.txt ...",5,60);
		
		res = f_open(&fnew, "0:newfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
		res = f_read(&fnew, buffer, sizeof(buffer), &br); 

		printf("\r\n %s ", buffer);
		
		GUI_DispStringAt ("file content:",5,100);
		
		/* ����ǰ����ɫ��������ɫ��*/
		GUI_SetColor(GUI_RED);
		
		/* �������� */
		GUI_SetFont(GUI_FONT_COMIC18B_ASCII);
		
		/* ��ʾ�ı�����Ļ�� */
		GUI_DispStringInRectWrap((const char *)buffer,&file_content,GUI_TA_LEFT,GUI_WRAPMODE_WORD) ;
		
		/* Close open files */
		f_close(&fnew);	                                      
		 
    /* Unregister work area prior to discard it */
		f_mount(&fs,"0:",0);	
		
		while(1);
}

	  

