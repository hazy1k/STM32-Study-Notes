/**
  *********************************************************************
  * @file    GUI_AppDef.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   Ӧ��ʾ��ͷ�ļ�
  **********************************************************************
  */ 
#ifndef	__GUI_APPDEF_H__
#define	__GUI_APPDEF_H__

#include "emXGUI.h"

/* �������� */
/* Ҫ�����ⲿ������ļ���Ŀ */
#define FONT_NUM          1

/* �ⲿ��Դ������ɱ�־ */
extern BOOL Load_state;

/* ��������Ľ�������� */
extern HWND Boot_progbar;

/* ����ͼƬ���� */
extern HDC hdc_home_bk;
/* ��ҳ�洰�ھ�� */
extern HWND	hwnd_home;

/* �����־ */
extern uint8_t Theme_Flag;

#define ID_SLIDE_WINDOW   1200

#define HEAD_INFO_HEIGHT   36
#define WM_MSG_FRAME_DOWN  (WM_USER+50)

#define COLOR_DESKTOP_BACK_GROUND         215,61,50
#define COLOR_DESKTOP_BACK_GROUND2         236,100,141
#define COLOR_DESKTOP_BACK_GROUND_HEX     0xd73d32
#define COLOR_DESKTOP_BACK_GROUND_HEX2			0xEC648D
#define COLOR_INVALID                       165,160,160
#define RGB888_COLOR_DESKTOP_BACK_GROUND   RGB888(215,61,50)
#define RGB888_COLOR_DESKTOP_BACK_GROUND2   RGB888(236,100,141)
#define RGB888_COLOR_INVALID                 RGB888(165,160,160)
//#define COLOR_BACK_GRUND RGB888(215,61,50)

#define ICON_VIEWER_ID_PREV   0x1003
#define ICON_VIEWER_ID_NEXT		0x1004
#define ICON_VIEWER_ID_LIST   0x1005

extern int thread_ctrl;


#endif	/*__GUI_APPDEF_H__*/
