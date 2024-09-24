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
File        : WM_Sample.c
Purpose     : Demonstrates the window manager
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

#include <string.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/* �Զ�����ϢID */
#define MSG_CHANGE_TEXT (WM_USER + 0)
/* �ƶ��ٶ� */
#define SPEED           1500

/*******************************************************************
*
*       Static variables
*
********************************************************************
*/
/* �ı�����ȥ */
static char _acInfoText[40];

/* ��ɫ */
static GUI_COLOR _WindowColor1 = GUI_GREEN;
static GUI_COLOR _FrameColor1  = GUI_BLUE;
static GUI_COLOR _WindowColor2 = GUI_RED;
static GUI_COLOR _FrameColor2  = GUI_YELLOW;
static GUI_COLOR _ChildColor   = GUI_YELLOW;
static GUI_COLOR _ChildFrame   = GUI_BLACK;

/* �ص����� */
static WM_CALLBACK * _cbBkWindowOld;

/* ���*/
static WM_HWIN _hWindow1;
static WM_HWIN _hWindow2;
static WM_HWIN _hChild;

/*******************************************************************
*
*       Static code, helper functions
*
********************************************************************
*/
/**
  * @brief ���Զ�����Ϣ���͵���̨���ڲ�ʹ����Ч����˺�̨���ڵĻص���ʾ���ı���
  * @note ��
  * @param ��
  * @retval ��
  */
static void _ChangeInfoText(char * pStr)
{
  WM_MESSAGE Message;

  Message.MsgId  = MSG_CHANGE_TEXT;
  Message.Data.p = pStr;
  WM_SendMessage(WM_HBKWIN, &Message);
  WM_InvalidateWindow(WM_HBKWIN);
}

/**
  * @brief ֱ������ʾ���ϻ�����Ϣ�ı�
  * @note �˹���������δ���ûص�������ʱ��
  * @param ��
  * @retval ��
  */
static void _DrawInfoText(char * pStr)
{
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font16_ASCII);
  GUI_DispStringHCenterAt("WindowManager - Sample", 120, 5);
  GUI_SetFont(&GUI_Font8x16);
  GUI_DispStringAtCEOL(pStr, 5, 40);
}

/*******************************************************************
*
*       _LiftUp
*/
static void _LiftUp(int dy)
{
  int i;
  int tm;

  for (i = 0; i < (dy/4); i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, 0, -4);
    WM_MoveWindow(_hWindow2, 0, -4);
    while ((GUI_GetTime() - tm) < 20)
		{
      WM_Exec();
    }
  }
}

/*******************************************************************
*
*       _LiftDown
*/
static void _LiftDown(int dy)
{
  int i;
  int tm;

  for (i = 0; i < (dy/4); i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, 0, 4);
    WM_MoveWindow(_hWindow2, 0, 4);
    while ((GUI_GetTime() - tm) < 20)
		{
      WM_Exec();
    }
  }
}

/*******************************************************************
*
*       Static code, callbacks for windows
*
********************************************************************
*/
/**
  * @brief �������ڻص�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _cbBkWindow(WM_MESSAGE * pMsg)
{
  switch (pMsg->MsgId)
	{
		case MSG_CHANGE_TEXT:
			strcpy(_acInfoText, (char const *)pMsg->Data.p);
		case WM_PAINT:
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font20_ASCII);
			GUI_DispStringHCenterAt("WindowManager - Sample", 120, 5);
			GUI_SetFont(&GUI_Font8x16);
			GUI_DispStringAt(_acInfoText, 5, 40);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief ����1�ص�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _cbWindow1(WM_MESSAGE * pMsg)
{
  GUI_RECT Rect;
  int      x;
  int      y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_WindowColor1);
			GUI_SetColor(_FrameColor1);
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 1", x / 2, (y / 2) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief ����2�ص�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _cbWindow2(WM_MESSAGE * pMsg)
{
  GUI_RECT Rect;
  int      x;
  int      y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_WindowColor2);
			GUI_SetColor(_FrameColor2);
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 2", x / 2, (y / 4) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief �Ӵ��ڻص�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _cbChild(WM_MESSAGE * pMsg)
{
  GUI_RECT Rect;
  int      x;
  int      y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_ChildColor);
			GUI_SetColor(_ChildFrame);
			GUI_ClearRectEx(&Rect);
			GUI_DrawRectEx(&Rect);
			GUI_SetColor(GUI_RED);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Child window", x / 2, (y / 2) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief ����1����һ���ص�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _cbDemoCallback1(WM_MESSAGE * pMsg)
{
  int x;
  int y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_GREEN);
			GUI_Clear();
			GUI_SetColor(GUI_RED);
			GUI_SetFont(&GUI_FontComic18B_1);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 1\nanother Callback", x / 2, (y / 2) - 18);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/**
  * @brief ����2����һ���ص�����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _cbDemoCallback2(WM_MESSAGE * pMsg)
{
  int x;
  int y;

  switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_MAGENTA);
			GUI_Clear();
			GUI_SetColor(GUI_YELLOW);
			GUI_SetFont(&GUI_FontComic18B_1);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 2\nanother Callback", x / 2, (y / 4) - 18);
			break;
		default:
			WM_DefaultProc(pMsg);
  }
}

/*******************************************************************
*
*       Static code, functions for demo
*
********************************************************************
*/
/**
  * @brief ��ʾWM_SetDesktopColor��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoSetDesktopColor(void)
{
  GUI_SetBkColor(GUI_BLUE);
  GUI_Clear();
  _DrawInfoText("WM_SetDesktopColor()");
  GUI_Delay(SPEED*3/2);
  WM_SetDesktopColor(GUI_BLACK);
  GUI_Delay(SPEED/2);
  /* ���ñ�����ɫ��ʹ������ɫ��Ч�����Ǻ����ػ���ʾ������� */
  GUI_SetBkColor(GUI_BLACK);
  WM_SetDesktopColor(GUI_INVALID_COLOR);
}

/**
  * @brief ��ʾWM_CreateWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoCreateWindow(void)
{
  /* �ض��򱳾����ڻص����� */
  _cbBkWindowOld = WM_SetCallback(WM_HBKWIN, _cbBkWindow);
  /* �������� */
  _ChangeInfoText("WM_CreateWindow()");
  GUI_Delay(SPEED);
  _hWindow1 = WM_CreateWindow( 50,  70, 120, 70, WM_CF_SHOW | WM_CF_MEMDEV, _cbWindow1, 0);
  GUI_Delay(SPEED/3);
  _hWindow2 = WM_CreateWindow(105, 125, 120, 70, WM_CF_SHOW | WM_CF_MEMDEV, _cbWindow2, 0);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_CreateWindowAsChild��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoCreateWindowAsChild(void)
{
  /* �����Ӵ��� */
  _ChangeInfoText("WM_CreateWindowAsChild()");
  GUI_Delay(SPEED);
  _hChild = WM_CreateWindowAsChild(10, 30, 100, 40, _hWindow2, WM_CF_SHOW | WM_CF_MEMDEV, _cbChild, 0);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_InvalidateWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoInvalidateWindow(void)
{
  _ChangeInfoText("WM_InvalidateWindow()");
  _WindowColor1 = GUI_BLUE;
  _FrameColor1  = GUI_GREEN;
  GUI_Delay(SPEED);
  WM_InvalidateWindow(_hWindow1);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_BringToTop��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoBringToTop(void)
{
  _ChangeInfoText("WM_BringToTop()");
  GUI_Delay(SPEED);
  WM_BringToTop(_hWindow1);    
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_MoveTo��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoMoveTo(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("WM_MoveTo()");
  GUI_Delay(SPEED);
  for (i = 1; i < 56; i++)
	{
    tm = GUI_GetTime();
    WM_MoveTo(_hWindow1,  50 + i,  70 + i);
    WM_MoveTo(_hWindow2, 105 - i, 125 - i);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 1; i < 56; i++)
	{
    tm = GUI_GetTime();
    WM_MoveTo(_hWindow1, 105 - i, 125 - i);
    WM_MoveTo(_hWindow2,  50 + i,  70 + i);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_BringToBottom��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoBringToBottom(void)
{
  _ChangeInfoText("WM_BringToBottom()");
  GUI_Delay(SPEED);
  WM_BringToBottom(_hWindow1);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_MoveWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoMoveWindow(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("WM_MoveWindow()");
  GUI_Delay(SPEED);
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1,  1,  1);
    WM_MoveWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, -1, -1);
    WM_MoveWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾ�ڸ�������WM_HideWindow��WM_ShowWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoHideShowParent(void)
{
  _ChangeInfoText("WM_HideWindow(Parent)");
  GUI_Delay(SPEED);
  WM_HideWindow(_hWindow2);
  GUI_Delay(SPEED/3);
  WM_HideWindow(_hWindow1);
  GUI_Delay(SPEED);
  _ChangeInfoText("WM_ShowWindow(Parent)");
  GUI_Delay(SPEED);
  WM_ShowWindow(_hWindow1);
  GUI_Delay(SPEED/3);
  WM_ShowWindow(_hWindow2);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾ���Ӵ�����WM_HideWindow��WM_ShowWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoHideShowChild(void)
{
  _ChangeInfoText("WM_HideWindow(Child)");
  GUI_Delay(SPEED);
  WM_HideWindow(_hChild);
  GUI_Delay(SPEED);
  _ChangeInfoText("WM_ShowWindow(Child)");
  GUI_Delay(SPEED);
  WM_ShowWindow(_hChild);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾ���߽�Ĳü�
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoClipping(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("Demonstrating clipping of child");
  GUI_Delay(SPEED);
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  1,  0);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  0,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 50; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild, -1,  0);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  0, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 25; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hChild,  1,  0);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾ�ص�������ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoRedrawing(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("Demonstrating redrawing");
  GUI_Delay(SPEED);
  _LiftUp(40);
  GUI_Delay(SPEED/3);
  _ChangeInfoText("Using a callback for redrawing");
  GUI_Delay(SPEED/3);
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1,  1,  1);
    WM_MoveWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, -1, -1);
    WM_MoveWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED/4);
  _LiftDown(30);
  GUI_Delay(SPEED/2);
  _ChangeInfoText("Without redrawing");
  GUI_Delay(SPEED);
  _LiftUp(30);
  GUI_Delay(SPEED/4);
  WM_SetCallback(WM_HBKWIN, _cbBkWindowOld);
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1,  1,  1);
    WM_MoveWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 55; i++)
	{
    tm = GUI_GetTime();
    WM_MoveWindow(_hWindow1, -1, -1);
    WM_MoveWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  GUI_Delay(SPEED/3);
  WM_SetCallback(WM_HBKWIN, _cbBkWindow);
  _LiftDown(40);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_ResizeWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoResizeWindow(void)
{
  int i;
  int tm;
  int tDiff;

  _ChangeInfoText("WM_ResizeWindow()");
  GUI_Delay(SPEED);
  _LiftUp(30);
  for (i = 0; i < 20; i++)
	{
    tm = GUI_GetTime();
    WM_ResizeWindow(_hWindow1,  1,  1);
    WM_ResizeWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 40; i++)
	{
    tm = GUI_GetTime();
    WM_ResizeWindow(_hWindow1, -1, -1);
    WM_ResizeWindow(_hWindow2,  1,  1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  for (i = 0; i < 20; i++)
	{
    tm = GUI_GetTime();
    WM_ResizeWindow(_hWindow1,  1,  1);
    WM_ResizeWindow(_hWindow2, -1, -1);
    tDiff = 15 - (GUI_GetTime() - tm);
    GUI_Delay(tDiff);
  }
  _LiftDown(30);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_SetCallback��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoSetCallback(void)
{
  _ChangeInfoText("WM_SetCallback()");
  GUI_Delay(SPEED);
  WM_SetCallback(_hWindow1, _cbDemoCallback1);
  WM_InvalidateWindow(_hWindow1);
  GUI_Delay(SPEED/2);
  WM_SetCallback(_hWindow2, _cbDemoCallback2);
  WM_InvalidateWindow(_hWindow2);
  GUI_Delay(SPEED*3);
  WM_SetCallback(_hWindow1, _cbWindow1);
  WM_InvalidateWindow(_hWindow1);
  GUI_Delay(SPEED/2);
  WM_SetCallback(_hWindow2, _cbWindow2);
  WM_InvalidateWindow(_hWindow2);
  GUI_Delay(SPEED);
}

/**
  * @brief ��ʾWM_DeleteWindow��ʹ��
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoDeleteWindow(void)
{
  _ChangeInfoText("WM_DeleteWindow()");
  GUI_Delay(SPEED);
  WM_DeleteWindow(_hWindow2);
  GUI_Delay(SPEED/3);
  WM_DeleteWindow(_hWindow1);
  GUI_Delay(SPEED);
  _ChangeInfoText("");
  GUI_Delay(SPEED);
  /* ��ԭ�������ڻص���������ɫ */
  WM_SetCallback(WM_HBKWIN, _cbBkWindowOld);
  _WindowColor1 = GUI_GREEN;
  _WindowColor2 = GUI_RED;
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
  GUI_SetBkColor(GUI_BLACK);
  WM_SetCreateFlags(WM_CF_MEMDEV);
  WM_EnableMemdev(WM_HBKWIN);
  while (1)
	{
    /*�����ڴ����ƣ�����demo���в��ȶ�*/
    _DemoSetDesktopColor();
    _DemoCreateWindow();
//  _DemoCreateWindowAsChild();
    _DemoInvalidateWindow();
    _DemoBringToTop();
//  _DemoMoveTo();
    _DemoBringToBottom();
//  _DemoMoveWindow();
    _DemoHideShowParent();
    _DemoHideShowChild();
    _DemoClipping();
    _DemoRedrawing();
    _DemoResizeWindow();
    _DemoSetCallback();
    _DemoDeleteWindow();
  }
}

/*************************** End of file ****************************/
