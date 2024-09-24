/**
  ***********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STemWIN 5.44a
  ***********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ***********************************************************************
  */
/*******************************************************************************
 * ������ͷ�ļ�
 ******************************************************************************/
#include "MainTask.h"

/*
*************************************************************************
*                                �궨��
*************************************************************************
*/
/* ��ʼ���� */
#define X_START 60
#define Y_START 40

/*******************************************************************************
 * ȫ�ֱ���
 ******************************************************************************/
/* */
typedef struct {
  int NumBars;

  GUI_COLOR Color;
  const char * s;
} BAR_DATA;

static const BAR_DATA _aBarData[] = {
  { 2, GUI_RED    , "Red" },
  { 2, GUI_GREEN  , "Green" },
  { 2, GUI_BLUE   , "Blue" },
  { 1, GUI_WHITE  , "Grey" },
  { 2, GUI_YELLOW , "Yellow" },
  { 2, GUI_CYAN   , "Cyan" },
  { 2, GUI_MAGENTA, "Magenta" },
};

static const GUI_COLOR _aColorStart[] = { GUI_BLACK, GUI_WHITE };

/*
*************************************************************************
*                                 ����
*************************************************************************
*/
/**
  * @brief ɫ����ʾ����
  * @note ��
  * @param ��
  * @retval ��
  */
static void _DemoShowColorBar(void) 
{
	GUI_RECT Rect;
	int      yStep;
	int      i;
	int      j;
	int      xSize;
	int      ySize;
	int      NumBars;
	int      NumColors;

	xSize = LCD_GetXSize();
	ySize = LCD_GetYSize();
	
	/* ������ʾ��ɫ���� */
	NumColors = GUI_COUNTOF(_aBarData);
	for (i = NumBars = 0, NumBars = 0; i < NumColors; i++) 
	{
		NumBars += _aBarData[i].NumBars;
	}
	yStep = (ySize - Y_START) / NumBars;
	
	/* ��ʾ�ı� */
	Rect.x0 = 0;
	Rect.x1 = X_START - 1;
	Rect.y0 = Y_START;
	GUI_SetFont(&GUI_Font16B_ASCII);
	for (i = 0; i < NumColors; i++) 
	{
		Rect.y1 = Rect.y0 + yStep * _aBarData[i].NumBars - 1;
		GUI_DispStringInRect(_aBarData[i].s, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER);
		Rect.y0 = Rect.y1 + 1;
	}
	
  /* ����ɫ�� */
	Rect.x0 = X_START;
	Rect.x1 = xSize - 1;
	Rect.y0 = Y_START;
	for (i = 0; i < NumColors; i++) 
	{
		for (j = 0; j < _aBarData[i].NumBars; j++) 
		{
			Rect.y1 = Rect.y0 + yStep - 1;
			GUI_DrawGradientH(Rect.x0, Rect.y0, Rect.x1, Rect.y1, _aColorStart[j], _aBarData[i].Color);
			Rect.y0 = Rect.y1 + 1;
		}
	}
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
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	
	/* ����ǰ��ɫ�������С */
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font24B_ASCII);
	
	/* ��ʾɫ�� */
	_DemoShowColorBar();
	
	while(1) 
	{
		GUI_Delay(10);
	}
}
