/**
  *********************************************************************
  * @file    MainTask.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/*******************************************************************************
 * ������ͷ�ļ�
 ******************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
/* STemWINͷ�ļ� */
#include "ScreenShot.h"
#include "MainTask.h"
#include "DIALOG.h"

/*******************************************************************************
 * ȫ�ֱ���
 ******************************************************************************/
extern GUI_CONST_STORAGE GUI_BITMAP bmngc7293;

UINT    f_num;
extern FATFS   fs;								/* FatFs�ļ�ϵͳ���� */
extern FIL     file;							/* file objects */
extern FRESULT result; 
extern DIR     dir;
/*******************************************************************************
 * ����
 ******************************************************************************/
/**
  * @brief ���ڲ��洢���ж�ȡ������BMPͼƬ����
  * @note ��
  * @param ��
  * @retval ��
  */
static void ShowBitmap(void)
{
  for(int y = 0; y < 240; y += bmngc7293.YSize)
  {
    for(int x = 0; x < 320; x += bmngc7293.XSize)
    {
       GUI_DrawBitmap(&bmngc7293, x, y);
    }
  }
}

/**
  * @brief ���ⲿ�洢���ж�ȡ������BMPͼƬ����
  * @note ��
  * @param sFilename��Ҫ��ȡ���ļ���
  *        x��Ҫ��ʾ��x������
  *        y��Ҫ��ʾ��y������
  * @retval ��
  */
static void ShowStreamedBitmap(const char *sFilename, int x, int y)
{
 	WM_HMEM hMem;
  char *_acbuffer = NULL;

	/* �����ٽ�� */
	taskENTER_CRITICAL();
	/* ��ͼƬ */
	result = f_open(&file, sFilename, FA_READ);
	if ((result != FR_OK))
	{
		printf("�ļ���ʧ�ܣ�\r\n");
		_acbuffer[0]='\0';
	}
	
	/* ����һ�鶯̬�ڴ�ռ� */
	hMem = GUI_ALLOC_AllocZero(file.fsize);
	/* ת����̬�ڴ�ľ��Ϊָ�� */
	_acbuffer = GUI_ALLOC_h2p(hMem);

	/* ��ȡͼƬ���ݵ���̬�ڴ��� */
	result = f_read(&file, _acbuffer, file.fsize, &f_num);
	if(result != FR_OK)
	{
		printf("�ļ���ȡʧ�ܣ�\r\n");
	}
	/* ��ȡ��Ϲر��ļ� */
	f_close(&file);
	/* �˳��ٽ�� */
	taskEXIT_CRITICAL();
	/* ������λͼ */
	GUI_DrawStreamedBitmapAuto(_acbuffer, x, y);
  
	/* �ͷ��ڴ� */
	GUI_ALLOC_Free(hMem);
}

/**
  * @brief GUI������
  * @note ��
  * @param ��
  * @retval ��
  */
void MainTask(void)
{	
  ShowBitmap();
  
  GUI_Delay(2000);
//  GUI_Clear();
  /* ����F103ָ���ߵ��ڴ��С���޷���ʾ�ϴ�ֱ��ʵ���λͼ��
	�ͷֱ���ͼƬ��ʾЧ�����䲻�ѣ��ʲ�������λͼ��ʾ*/
//  ShowStreamedBitmap("0:/image/illustration.dta",
//                     0,
//                     0);
  
	while(1)
	{
    GUI_Delay(2000);
	}
}
