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


/*******************************************************************************
 * ȫ�ֱ���
 ******************************************************************************/

/*******************************************************************************
 * ����
 ******************************************************************************/
/**
  * @brief ʮ��������ʾ
  * @note ��
  * @param ��
  * @retval ��
  */
static void Disp_DecNum(void)
{
	GUI_SetFont(GUI_FONT_13_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_DispStringAt("GUI_DispDec():", 0, 0);
	GUI_DispNextLine();
	GUI_DispDec(12345, 6);
	GUI_GotoX(7 * 9);
	GUI_DispDec(-12345, 6);
	
	GUI_DispStringAt("GUI_DispDecAt():", 0, 13 * 2);
	GUI_DispDecAt(12345, 0, 13 * 3, 6);
	GUI_DispDecAt(-12345, 7 * 9, 13 * 3, 6);
	
	GUI_DispStringAt("GUI_DispDecMin():", 0, 13 * 4);
	GUI_DispNextLine();
	GUI_DispDecMin(12345);
	GUI_GotoX(7 * 9);
	GUI_DispDecMin(-12345);
	
	GUI_DispStringAt("GUI_DispDecShift():", 0, 13 * 6);
	GUI_DispNextLine();
	GUI_DispDecShift(12345, 7, 2);
	GUI_GotoX(7 * 9);
	GUI_DispDecShift(-12345, 7, 2);
	
	GUI_DispStringAt("GUI_DispDecSpace():", 0, 13 * 8);
	GUI_DispNextLine();
	GUI_DispDecSpace(12345, 6);
//	GUI_GotoX(12 * 9);
//	GUI_DispDecSpace(-12345, 7);

  GUI_DispStringAt("GUI_DispSDec():", 0, 13 * 10);
	GUI_DispNextLine();
	GUI_DispSDec(12345,6);
	
	GUI_DispStringAt("GUI_DispSDecShift():", 0, 13 * 12);
	GUI_DispNextLine();
	GUI_DispSDecShift(12345, 7, 2);
}

/**
  * @brief ��������ʾ
  * @note ��
  * @param ��
  * @retval ��
  */
static void Disp_FloatingNum(void)
{
	int x0 = 120;
	
	GUI_SetFont(GUI_FONT_13_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_GotoX(x0);
	GUI_DispStringAt("GUI_DispFloat():", x0, 0);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispFloat(123.4567, 9);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispFloat(-123.4567, 9);
	
	GUI_DispStringAt("GUI_DispFloatFix():", x0, 13 * 2);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispFloatFix(123.4567, 9, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispFloatFix(-123.4567, 9, 3);
	
	GUI_DispStringAt("GUI_DispFloatMin():", x0, 13 * 4);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispFloatMin(123.4567, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispFloatMin(-123.4567, 3);
	
	GUI_DispStringAt("GUI_DispSFloatFix():", x0, 13 * 6);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispSFloatFix(123.4567, 9, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispSFloatFix(-123.4567, 9, 3);
	
	GUI_DispStringAt("GUI_DispSFloatMin():", x0, 13 * 8);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispSFloatMin(123.4567, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispSFloatMin(-123.4567, 3);
}

/**
  * @brief ����������ʮ����������ʾ
  * @note ��
  * @param ��
  * @retval ��
  */
static void Disp_BinHexNum(void)
{
	int x0 = 0;
	
	GUI_SetFont(GUI_FONT_13_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_DispStringAt("GUI_DispBin():", x0, 13 * 16);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispBin(10, 8);
	
	GUI_DispStringAt("GUI_DispBinAt():", x0, 13 * 18);
	GUI_DispBinAt(10, x0, 13 * 19, 8);
	
	GUI_DispStringAt("GUI_DispHex():", x0, 13 * 20);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispHex(4779, 4);
	
	GUI_DispStringAt("GUI_DispHexAt():", 120, 13 * 16);
	GUI_DispNextLine();
	GUI_DispHexAt(4779, 120, 13 * 17, 4);
	
	GUI_DispStringAt("GUI_GetVersionString()", 120, 13 * 18);
	GUI_DispStringAt(GUI_GetVersionString(), 120, 13 * 19);
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
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	
	/* ��ʾʮ������ */
	Disp_DecNum();
	/* ��ʾ������ */
	Disp_FloatingNum();
	/* ��ʾ�����ƺ�ʮ�������� */
	Disp_BinHexNum();
	
	while(1)
	{
		GUI_Delay(100);
	}
}
