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
#include "MainTask.h"

#include <stdlib.h>

#include "./usart/bsp_usart.h"

/*******************************************************************************
 * ȫ�ֱ���
 ******************************************************************************/
GUI_RECT BasicRect = {10, 10, 50, 50};
static const unsigned aValues[] = {100, 135, 190, 240, 340, 360};
static const GUI_COLOR aColor[] = {GUI_BLUE, GUI_GREEN, GUI_RED,
                                   GUI_CYAN, GUI_MAGENTA, GUI_YELLOW};
static const char QR_TEXT[] = "http://www.firebbs.cn";
static const GUI_POINT _aPointArrow[] = {
  {  0,   0 },
  {-20, -15 },
  {-5 , -10 },
  {-5 , -35 },
  { 5 , -35 },
  { 5 , -10 },
  { 20, -15 },
};
static const GUI_POINT DashCube_BackPoint[] = {
		{ 76 /2, 104/2 },
		{ 176/2, 104/2 },
		{ 176/2,   4  /2},
		{  76/2,   4  /2}
};
static const GUI_POINT DashCube_LeftPoint[] = {
		{ 40/2, 140 /2},
		{ 76/2, 104 /2},
		{ 76/2,   4 /2},
		{ 40/2,  40 /2}
};
static const GUI_POINT DashCube_BottonPoint[] = {
		{  40/2, 140 /2},
		{ 140/2, 140 /2},
		{ 176/2, 104 /2},
		{  76/2, 104 /2}
};
static const GUI_POINT DashCube_TopPoint[] = {
		{  40/2, 40 /2},
		{ 140/2, 40 /2},
		{ 176/2,  4 /2},
		{  76/2,  4 /2},
};
static const GUI_POINT DashCube_RightPoint[] = {
		{ 140/2, 140/2 },
		{ 176/2, 104/2 },
		{ 176/2,   4/2 },
		{ 140/2,  40/2 },
};
static const 	GUI_POINT DashCube_FrontPoint[] = {
		{  40/2, 140/2},
		{ 140/2, 140/2},
		{ 140/2,  40/2},
		{  40/2,  40/2},
};

/*******************************************************************************
 * ����
 ******************************************************************************/
/**
  * @brief ��ͼ��ͼ����
  * @note ��
  * @param x0����ͼԲ�ĵ�x����
  *        y0����ͼԲ�ĵ�y����
  *        r����ͼ�뾶
  * @retval ��
  */
static void Pie_Chart_Drawing(int x0, int y0, int r)
{
	int i, a0 = 0, a1 = 0;
	
	for(i = 0; i < GUI_COUNTOF(aValues); i++)
	{
		if(i == 0) a0 = 0;
		else a0 = aValues[i - 1];
		a1 = aValues[i];	
		GUI_SetColor(aColor[i]);
		GUI_DrawPie(x0, y0, r, a0, a1, 0);
	}
}

/**
  * @brief ��ά������
  * @note ��
  * @param pText����ά������
  *        PixelSize����ά������ɫ��Ĵ�С����λ������
  *        EccLevel��������뼶��
  *        x0����ά��ͼ����LCD������x
  *        y0����ά��ͼ����LCD������y
  * @retval ��
  */
static void QR_Code_Drawing(const char *pText, int PixelSize, int EccLevel, int x0, int y0)
{
	GUI_HMEM hQR;
	
	/* ������ά����� */
	hQR = GUI_QR_Create(pText, PixelSize, EccLevel, 0);
	/* ���ƶ�ά�뵽LCD */
	GUI_QR_Draw(hQR, x0, y0);
	/* ɾ����ά����� */
	GUI_QR_Delete(hQR);
}

/**
  * @brief 2D��ͼ����
  * @note ��
  * @param ��
  * @retval ��
  */
/* ���ڴ�Ŷ������ת��ĵ��б� */ 
GUI_POINT aArrowRotatedPoints[GUI_COUNTOF(_aPointArrow)];
static void _2D_Graph_Drawing(void)
{
	I16 aY[90] = {0};
	int i;
  float pi = 3.1415926L;
  float angle = 0.0f;
	
	/* ���Ƹ��־��� */
	GUI_SetColor(GUI_GREEN);
	GUI_DrawRectEx(&BasicRect);
	BasicRect.x0 += 45;
	BasicRect.x1 += 45;
	GUI_FillRectEx(&BasicRect);
  GUI_SetColor(GUI_RED);
	GUI_DrawRoundedRect(100, 10, 140, 50, 5);
	GUI_DrawRoundedFrame(145, 10, 185, 50, 5, 5);
	GUI_FillRoundedRect(190, 10, 230, 50, 5);
	GUI_DrawGradientRoundedH(10, 55, 50, 95, 5, GUI_LIGHTMAGENTA, GUI_LIGHTCYAN);
	GUI_DrawGradientRoundedV(55, 55, 95, 95, 5, GUI_LIGHTMAGENTA, GUI_LIGHTCYAN);
	
	/* �������� */
	GUI_SetPenSize(5);
  GUI_SetColor(GUI_YELLOW);
	GUI_DrawLine(100, 55, 180, 95);
	
	/* ���ƶ���� */
	GUI_SetColor(GUI_RED);
	GUI_FillPolygon(_aPointArrow, 7, 210, 100);
  /* ��ת����� */
	angle = pi / 2;
	GUI_RotatePolygon(aArrowRotatedPoints,
	                  _aPointArrow, 
                    (sizeof(_aPointArrow) / sizeof(_aPointArrow[0])),
										angle);
	GUI_FillPolygon(&aArrowRotatedPoints[0], 7, 230, 120);
  
  /* �����߿������� */
  GUI_SetPenSize(1);
	GUI_SetColor(0x4a51cc);
	GUI_SetLineStyle(GUI_LS_DOT);
	GUI_DrawPolygon(DashCube_BackPoint, 4, 0, 105);
  GUI_DrawPolygon(DashCube_LeftPoint, 4, 0, 105);
  GUI_DrawPolygon(DashCube_BottonPoint, 4, 0, 105);
  GUI_SetPenSize(2);
  GUI_SetLineStyle(GUI_LS_SOLID);
  GUI_DrawPolygon(DashCube_TopPoint, 4, 0, 105);
  GUI_DrawPolygon(DashCube_RightPoint, 4, 0, 105);
  GUI_DrawPolygon(DashCube_FrontPoint, 4, 0, 105);
                    
	/* ����Բ */
	GUI_SetColor(GUI_LIGHTMAGENTA);
	for(i = 10; i <= 40; i += 10)
	{
		GUI_DrawCircle(140, 135, i);
	}
	GUI_SetColor(GUI_LIGHTCYAN);
	GUI_FillCircle(210, 155, 20);
	
	/* ������Բ */
	GUI_SetColor(GUI_BLUE);
	GUI_FillEllipse(30, 210, 20, 30);
	GUI_SetPenSize(2);
	GUI_SetColor(GUI_WHITE);
	GUI_DrawEllipse(30, 210, 20, 10);
	
	/* ����Բ�� */
	GUI_SetPenSize(4);
	GUI_SetColor(GUI_GRAY_3F);
	GUI_DrawArc(100, 215, 30, 30, -30, 210);
	
	/* ��������ͼ */
	for(i = 0; i< GUI_COUNTOF(aY); i++)
	{
		aY[i] = rand() % 50;
	}
	GUI_SetColor(GUI_BLACK);
	GUI_DrawGraph(aY, GUI_COUNTOF(aY), 140, 180);
	
	/* ���Ʊ�ͼ */
	Pie_Chart_Drawing(60, 280, 35);
	
	/* ���ƶ�ά�� */
	QR_Code_Drawing(QR_TEXT, 3, GUI_QR_ECLEVEL_L, 140, 240);
}

/**
  * @brief Alpha���
  * @note ��
  * @param ��
  * @retval ��
  */
static void Alpha_Blending(void)
{
  /* ��ʾ�ַ� */
	GUI_SetColor(GUI_BLACK);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_SetFont(GUI_FONT_13B_ASCII);
	GUI_DispStringHCenterAt("Alpha blending", 55, 275);
	GUI_DispStringHCenterAt("Not Used", 185, 275);
  /* �����Զ�Alpha��� */
  GUI_EnableAlpha(1);
	/* ��Alpha��ֵ��ӵ���ɫ�в���ʾ */
	GUI_SetColor((0xC0uL << 24) | 0xFF0000);
	GUI_FillRect(10, 10, 100, 90);
	GUI_SetColor((0x80uL << 24) | 0x00FF00);
	GUI_FillRect(10, 100, 100, 180);
	GUI_SetColor((0x40uL << 24) | 0x0000FF);
	GUI_FillRect(10, 190, 100, 270);
  /* �ر��Զ�Alpha��� */
  GUI_EnableAlpha(0);
	GUI_SetColor((0xC0uL << 24) | 0xFF0000);
	GUI_FillRect(140, 10, 230, 90);
	GUI_SetColor((0x80uL << 24) | 0x00FF00);
	GUI_FillRect(140, 100, 230, 180);
	GUI_SetColor((0x40uL << 24) | 0x0000FF);
	GUI_FillRect(140, 190, 230, 270);
}

/**
  * @brief GUI������
  * @note ��
  * @param ��
  * @retval ��
  */
void MainTask(void)
{
	/* ���ñ���ɫ */
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	
	/* 2D��ͼ */
	_2D_Graph_Drawing();
	
	GUI_Delay(5000);
	GUI_Clear();

	/* Alpha��� */
	Alpha_Blending();
	
	while(1)
	{
		GUI_Delay(100);
	}
}
