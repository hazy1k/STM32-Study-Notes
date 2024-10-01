#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"

#ifndef _GUI_CLOCK_DIALOG_H
#define _GUI_CLOCK_DIALOG_H          

typedef enum 
{ 
  /****************** ��ť�ؼ� ID ֵ *******************/
  ID_CLOCK_BACK = 0x1000,      // ��һ��
  ID_CLOCK_NEXT,               // ��һ��
  ID_CLOCK_OK,                 // �������
  ID_CLOCK_WAIVE,              // ������������
  ID_CLOCK_EXIT,               // �˳���ť
	ID_CLOCK_SET,                // ����
  /***************** �ı��ؼ� ID ֵ *********************/
  ID_CLOCK_TITLE,              // ���ڱ�����
  ID_CLOCK_MONTH,              // ��
  ID_CLOCK_WEEK,               // ��
  ID_CLOCK_DATE,                // ��
	ID_CLOCK_SETTITLE,           // ���ô��ڱ�����
	ID_CLOCK_SETTIME,            // ������ʾʱ��
	ID_CLOCK_SETDATE,            // ������ʾ����
  ID_CLOCK_TIME,               // ʱ����ʾ���򣨴���Ϊ�ؼ������ػ棩
	/****************** ��ѡ��ť ***********************/
//	ID_CLOCK_Background00,
//	ID_CLOCK_Background01,
	ID_CLOCK_Background02,
	/********************* �б� **************************/
	ID_CLOCK_SetWin,
  ID_CLOCK_SetYear,
	ID_CLOCK_SetMonth,
  ID_CLOCK_SetDate,
  ID_CLOCK_SetHour,
  ID_CLOCK_SetMinute,
}clock_id_t;

typedef struct{
	WCHAR *icon_name;    // ͼ����
	RECT rc;             // λ����Ϣ
	clock_id_t id;      // ��ťID
}clock_icon_t;

typedef enum
{
	hdc_clock_btn = 0,
	hdc_clock_btn_press,
	hdc_clock_calendar,
//	hdc_clock_back_00,
//	hdc_clock_h_00,
//	hdc_clock_m_00,
//	hdc_clock_s_00,
//	hdc_clock_back_01,
//	hdc_clock_h_01,
//	hdc_clock_m_01,
//	hdc_clock_s_01,
	hdc_clock_back_02,
	hdc_clock_h_02,
	hdc_clock_m_02,
	hdc_clock_s_02,
//	hdc_clock_chcked,

	hdc_clock_end,              // �������������
}hdc_clock_png_t;

typedef struct{
	char *pic_name;     // ͼƬ��
	int w;               // ͼƬ��
	int h;               // ͼƬ��
	hdc_clock_png_t id;        // hdc ���
}clock_hdc_t;



#endif

