/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : WM_Redraw.c
Purpose     : Demonstrates the redrawing mechanism of the window manager
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )

----------------------------------------------------------------------
*/
#include "WM.h"
#include "GUI.h"
#include "DIALOG.h"
#include "MainTask.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/*******************************************************************
*
*       static code
*
********************************************************************
*/
/**
  * @brief �������ڻص�����
  * @note pMsg����Ϣָ��
  * @param ��
  * @retval ��
  */
static void _cbBkWindow(WM_MESSAGE* pMsg)
{
  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_ClearRect(0, 50, 319, 239);
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief ���ڻص�����
  * @note pMsg����Ϣָ��
  * @param ��
  * @retval ��
  */
static void _cbWindow(WM_MESSAGE* pMsg)
{
	GUI_RECT Rect;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
      /* ���ش��ڿͻ������� */
			WM_GetInsideRect(&Rect);
      /* ���ô��ڱ�����ɫ */
			GUI_SetBkColor(GUI_RED);
      /* ����ǰ����ɫ */
			GUI_SetColor(GUI_YELLOW);
      /* ���ƴ��� */    
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
      /* �����ı���ɫ */
			GUI_SetColor(GUI_BLACK);
      /* �����ı���ʽ */
			GUI_SetFont(&GUI_Font8x16);
      /* ��ʾ��ʾ��Ϣ */
			GUI_DispStringHCenterAt("Foreground window", 75, 40);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief �����ƶ�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _MoveWindow(const char* pText)
{
  WM_HWIN hWnd;
  int     i;

  /* ����ǰ������ */
  hWnd = WM_CreateWindow(10, 50, 150, 100, WM_CF_SHOW, _cbWindow, 0);
  GUI_Delay(500);
  /* �ƶ�ǰ������ */
  for (i = 0; i < 40; i++)
	{
    WM_MoveWindow(hWnd, 2, 2);
    GUI_Delay(10);
  }
  /* �ƶ���������ʾ��ʾ���� */
  if (pText)
	{
    GUI_DispStringAt(pText, 5, 50);
    GUI_Delay(2500);
  }
  /* ɾ��ǰ������ */
  WM_DeleteWindow(hWnd);
  WM_Invalidate(WM_HBKWIN);
  GUI_Exec();
}

/**
  * @brief �����ػ�DEMO
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoRedraw(void)
{
  WM_CALLBACK * _cbOldBk;

  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font20_ASCII);
  GUI_DispStringHCenterAt("WM_Redraw - Sample", 120, 5);
  GUI_SetFont(&GUI_Font8x16);
  while(1)
	{
    /* �ڱ������ƶ����� */
    _MoveWindow("Background has not been redrawn");
    /* ������� */
    GUI_ClearRect(0, 50, 319, 239);
    GUI_Delay(1000);
    /* �ض��򱳾����ڵĻص����� */
    _cbOldBk = WM_SetCallback(WM_HBKWIN, _cbBkWindow);
    /* �ڱ������ƶ����� */
    _MoveWindow("Background has been redrawn");
    /* ��ԭ�������ڵĻص����� */
    WM_SetCallback(WM_HBKWIN, _cbOldBk);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/**
  * @brief GUI������
  * @note ��
  * @param ��
  * @retval ��
  */
void MainTask(void)
{
  _DemoRedraw();
}

/*************************** End of file ****************************/
