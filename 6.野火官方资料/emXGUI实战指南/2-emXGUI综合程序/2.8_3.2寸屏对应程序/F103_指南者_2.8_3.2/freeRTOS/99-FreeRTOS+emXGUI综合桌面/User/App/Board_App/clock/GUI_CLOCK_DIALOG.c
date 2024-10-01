#include "emXGUI.h"
#include "./clock/GUI_CLOCK_DIALOG.h"
#include "x_libc.h"
#include <stdlib.h>
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./clock/rtc/bsp_rtc.h"

#define ICON_BTN_NUM     3     // ��ť����
#define ICON_TEXT_NUM   (1 + ICON_BTN_NUM)    // �ı�����

//#define CLOCK_BACK_COLOR     230, 230, 230
#define CLOCK_BACK_COLOR     208, 208, 240//188, 213, 253

struct
{
	uint8_t page;    // ��ǰ����ҳ��
	uint8_t dial;    // ѡ�еı���
}Set_Start;

uint8_t clock_dial = 0;    // ����
static HWND clock_hwnd;

const WCHAR Week_List[][4] = {{L"������"}, {L"����һ"}, {L"���ڶ�"}, {L"������"}, {L"������"}, {L"������"}, {L"������"}};
const uint8_t month_list[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    // ��¼ÿ�µ��������

//ͼ���������
const clock_icon_t clock_icon[] = {

  /* ��ť */
  {L"O",              {286, 10,  23,  23},  ID_CLOCK_EXIT},            // 0. �˳�
  {L"2019��",         {182, 82, 115, 115},  ID_CLOCK_DATE},            // 1. ���ڣ��������ܣ�
  {L" ",              {45, 82, 115, 115},  ID_CLOCK_TIME},            // 2. ������ʾ����
  
  /* �ı� */
  {L"ʱ��&����",      {50,   0, 220,  40},  ID_CLOCK_TITLE},           // 3. ����
  
  /************ ���ô��ڿؼ� **************/
  /* ��ť */
  {L"F",              {  0,  5,  24,  24},  ID_CLOCK_WAIVE},           // 4. ��������
  {L"���",           {123, 197, 71,  30},  ID_CLOCK_OK},              // 5. �������

  /* �ı� */
  {L"����ʱ��",       {50,  0,  220,  40},  ID_CLOCK_SETTITLE},        // 6. ���ô��ڱ���
  {L"00:00",          {106, 46, 132,  22},  ID_CLOCK_SETTIME},         // 7. ���ô��ڱ���
  {L"2000��01��01��", {106, 46, 132,  22},  ID_CLOCK_SETDATE},         // 8. ���ô��ڱ���

  /* ʱ��&����ѡ���б� */
  {L" ",              {85, 68, 82, 124},  ID_CLOCK_SetHour},         // 9.  ����Сʱ���б�
  {L" ",              {166, 68, 82, 124},  ID_CLOCK_SetMinute},       // 10. ���÷��ӵ��б�

  {L" ",              {85, 68, 55, 124},  ID_CLOCK_SetYear},         // 11. ��������б�
  {L" ",              {139, 68, 55, 124},  ID_CLOCK_SetMonth},        // 12. �����µ��б�
  {L" ",              {194, 68, 55, 124},  ID_CLOCK_SetDate},         // 13. �����յ��б�
 
};

static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

static void waive_btn_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HDC hdc;
	RECT rc;
  WCHAR wbuf[128];

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
  { //��ť�ǰ���״̬
    SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));
  }
  else
  { //��ť�ǵ���״̬
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  
  GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
  SetFont(hdc, controlFont_24);
  /* ��ʾ�ı� */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
  
  DeleteFont(controlFont_24);
}

static void btn_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HDC hdc;
	RECT rc, rc_tmp;
  WCHAR wbuf[128];
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 66, 254, 255));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

  if (ds->State & BST_PUSHED)
  { //��ť�ǰ���״̬
    OffsetRect(&rc, 1, 1);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  { //��ť�ǵ���״̬
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  InflateRect(&rc, -5, -5);
  SetBrushColor(hdc, MapRGB(hdc, 13, 148, 214));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));
  
  EnableAntiAlias(hdc, FALSE);
  
  GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
  
  /* ��ʾ�ı� */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}

/*
 * @brief  �ػ��б�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void _draw_listbox(HDC hdc,HWND hwnd,COLOR_RGB32 text_c,COLOR_RGB32 back_c,COLOR_RGB32 sel_c, int fontsize)
{

	RECT rc,rc_cli;
	int i,count;
	WCHAR wbuf[128];


	GetClientRect(hwnd,&rc_cli);

	SetBrushColor(hdc,MapRGB888(hdc,back_c));
	FillRect(hdc,&rc_cli);

	SetTextColor(hdc,MapRGB888(hdc,text_c));

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
  
  HFONT controlFont_16;
  controlFont_16 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_16);

  if (fontsize == 16)
  {
    SetFont(hdc,controlFont_16);
  }
  else
  {
    SetFont(hdc,defaultFont);
  }

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > rc_cli.h)
		{
			break;
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}
  
  DeleteFont(controlFont_16);
}

static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc_m;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//����һ���м�ľ��Σ�
	rc_m.w =ds->rc.w;
	rc_m.h =25;
	rc_m.x =0;
	rc_m.y =(ds->rc.h-rc_m.h)>>1;

	//һ��listbox�浽hdc0�У�
	_draw_listbox(hdc,hwnd,RGB888(168,168,168),RGB888(250,250,250),RGB888(10,100,100), 12);

	//һ��listbox�浽hdc1�У�
//	_draw_listbox(hdc1,hwnd,RGB888(236,126,65),RGB888(232,232,232),RGB888(150,200,220), 16);

	SetPenColor(hdc,MapRGB(hdc,0,0,0));
  HLine(hdc, rc_m.x, rc_m.y, rc_m.x + rc_m.w);
  HLine(hdc, rc_m.x, rc_m.y+rc_m.h-1, rc_m.x + rc_m.w);
}



static void Dial_OwnerDraw(DRAWITEM_HDR *ds)  // ���Ʊ���
{
  HDC hdc;
	RECT rc;
  WCHAR wbuf[128];
  uint8_t Sec = 0;
  uint8_t Min = 0;
  uint8_t Hour = 0;

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
  FillRect(hdc, &rc);

  struct rtc_time systmtime;
  RTC_TimeCovr(&systmtime);
  Hour = systmtime.tm_hour;
  Min  = systmtime.tm_min;
  Sec  = systmtime.tm_sec;

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);

  SetPenSize(hdc, 2);
  SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
  SetBrushColor(hdc, MapRGB(hdc, 128, 128, 255));
  FillArc(hdc, rc.w/2, rc.h/2, 0, MIN(rc.w, rc.h)/2 - 1, 90, Sec / 60.0 * 360 + 90);
  
  x_wsprintf(wbuf, L"%02d %02d", Hour, Min);
  
  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  
  SetFont(hdc, controlFont_24);
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);
  
  /* ������Բ�� */
  SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
  FillCircle(hdc, rc.w/2, rc.h/2 - 10, 2);
  FillCircle(hdc, rc.w/2, rc.h/2 + 10, 2);

  DeleteFont(controlFont_24);
  EnableAntiAlias(hdc, FALSE);
}

/*
 * @brief  �ػ�������ʾ����
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void date_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
	RECT rc;
  WCHAR wbuf[20];
  
  RECT rc1 = {0, 10, 110, 72};
  RECT rc3 = {40, 161, 36, 18};

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
  FillRect(hdc, &rc);
  
  struct rtc_time systmtime;
  RTC_TimeCovr(&systmtime);

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);

  SetPenSize(hdc, 2);
  SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
  EnableAntiAlias(hdc, FALSE);
  
  /* �������� */
  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  
  /* ��ʾ�� */
  x_wsprintf(wbuf, L"%d", systmtime.tm_year);
  
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	
  SetFont(hdc, controlFont_24);
  DrawText(hdc, wbuf, -1, &rc1, DT_VCENTER|DT_CENTER);
	
  SetFont(hdc, defaultFont);
  rc1.y += 3;
	rc1.x += 35;
  DrawText(hdc, L"��", -1, &rc1, DT_VCENTER|DT_CENTER);
  
  /* ��ʾ���� */
  RECT rc_mont    = {20, 60, 25, 25};
  RECT rc_mont_zh = {42, 67, 15, 15};
  RECT rc_mday    = {62, 60, 25, 25};
  RECT rc_mday_zh = {84, 67, 15, 15};
  
  x_wsprintf(wbuf, L"%02d", systmtime.tm_mon);
  
  SetFont(hdc, controlFont_24);
  DrawText(hdc, wbuf, -1, &rc_mont, DT_RIGHT | DT_BOTTOM);    // ��ʾ�·�
  
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"��", -1, &rc_mont_zh, DT_RIGHT | DT_BOTTOM);    // ��ʾ���¡�
  
  x_wsprintf(wbuf, L"%02d", systmtime.tm_mday);
  
  SetFont(hdc, controlFont_24);
  DrawText(hdc, wbuf, -1, &rc_mday, DT_RIGHT | DT_BOTTOM);     // ��ʾ����
  
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"��", -1, &rc_mday_zh, DT_RIGHT | DT_BOTTOM);     // ��ʾ���ա�
  
  /* ��ʾ�� */
  SetFont(hdc, defaultFont);
  DrawText(hdc, Week_List[systmtime.tm_wday], -1, &rc3, DT_VCENTER|DT_CENTER);

  DeleteFont(controlFont_24);
}

/*
 * @brief  �ػ����͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Title_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

  /* ���� */
  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	GetWindowText(hwnd, wbuf, 128);                        // ��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
}

/*
 * @brief  �ػ��ɫ�����ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void WhiteBK_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

  /* ���� */
  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	GetWindowText(hwnd, wbuf, 128);                        // ��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
}

/*
 * @brief  ����б�ѡ�������ֵ���ַ���ת���ͣ�
 * @param  hwnd:	���ھ��
 * @param  ID��   �б� ID
 * @retval NONE
*/
static uint16_t GetListCurselVal(HWND hwnd, uint32_t ID)
{
  HWND wnd;
  uint8_t csl;    // ѡ����
  WCHAR wbuf[10];
  char cbuf[10];

  wnd = GetDlgItem(hwnd, ID);
  csl = SendMessage(wnd, LB_GETCURSEL, 0, 0);    // ���ѡ����
  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // ���ѡ������ı�
  x_wcstombs(cbuf, wbuf, 9);

  return x_atoi(cbuf);                           // ����ѡ�����ֵ
}

/*
 * @brief  ����б�ָ�������ֵ���ַ���ת���ͣ�
 * @param  hwnd:	���ھ��
 * @param  ID��   �б� ID
 * @param  csl:   �б����
 * @retval NONE
*/
//static uint16_t GetListItemVal(HWND hwnd, uint32_t ID, uint32_t csl)
//{
//  HWND wnd;
//  WCHAR wbuf[10];
//  char cbuf[10];

//  wnd = GetDlgItem(hwnd, ID);
//  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // ���ѡ������ı�
//  wcstombs(cbuf, wbuf, 9);

//  return x_atoi(cbuf);                           // ����ѡ�����ֵ
//}

static LRESULT setting_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg){
      case WM_CREATE:
      {
        
        for (uint8_t xC=4; xC<6; xC++)     //  ��ť
        {
          /* ѭ��������ť */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }
        
        /* �������ñ����ı��� */
        CreateWindow(TEXTBOX, clock_icon[6].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                      clock_icon[6].rc.x, clock_icon[6].rc.y,
                      clock_icon[6].rc.w,clock_icon[6].rc.h,
                      hwnd, clock_icon[6].id, NULL, NULL);
        
        /* ��������ʱ�������������������ͬ�Ŀؼ� */
        if (Set_Start.page == 0)
        {
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"����ʱ��");
          
          /* ѭ��ʱ����ʾ7�ı��� */
          CreateWindow(TEXTBOX, clock_icon[7].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                        clock_icon[7].rc.x, clock_icon[7].rc.y,
                        clock_icon[7].rc.w,clock_icon[7].rc.h,
                          hwnd, clock_icon[7].id, NULL, NULL);

          for (uint8_t xC=9; xC<11; xC++)
          {
            /* ���������б�����Ϊ�������ɼ��� */
            CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY | WS_OVERLAPPED | WS_VISIBLE,
                              clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                              clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
          }

          for (uint8_t xC=0; xC<24; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetHour);

            /* ���б�������Сʱ */
            x_wsprintf(wbuf, L"%02d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
          }
          
          for (uint8_t xC=0; xC<60; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMinute);

            /* ���б������ӷ��� */
            x_wsprintf(wbuf, L"%02d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
          }
        }
        else
        {
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"��������");
          
          /* ѭ��ʱ����ʾ7�ı��� */
          CreateWindow(TEXTBOX, clock_icon[8].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_OVERLAPPED,
                        clock_icon[8].rc.x, clock_icon[8].rc.y,
                        clock_icon[8].rc.w,clock_icon[8].rc.h,
                          hwnd, clock_icon[8].id, NULL, NULL);

          for (uint8_t xC=11; xC<14; xC++)
          {
            /* ���������б� */
            CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY | WS_OVERLAPPED | WS_VISIBLE,
                              clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                              clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
          }
          
          uint8_t i=0;
          for (uint16_t xC=2000; xC<2050; xC++)
          {
            WCHAR wbuf[10];
            
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetYear);

            /* ���б��������� */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }

          i=0;
          for (uint8_t xC=1; xC<13; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMonth);

            /* ���б��������� */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }

          i=0;
          for (uint8_t xC=1; xC<32; xC++)
          {
            WCHAR wbuf[10];
            HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

            /* ���б����������� */
            x_wsprintf(wbuf, L"%d", xC);
            SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
          }
        }
        SetTimer(hwnd, 1, 50, TMR_START, NULL);
        
        break;
      }

      case WM_TIMER:
      {
        int cur_sel, i, y;
        HWND wnd;
        RECT rc0, rc;

        if(GetKeyState(VK_LBUTTON)==0) //����������������û�а���ʱ.
        {
          for (uint32_t xC=ID_CLOCK_SetYear; xC<ID_CLOCK_SetMinute+1; xC++)
          {
            wnd =GetDlgItem(hwnd, xC); //���LISTBOX ����ʱ��HWND.

            cur_sel=SendMessage(wnd,LB_GETCURSEL,0,0); //���LISTBOX����.

            GetClientRect(wnd,&rc0);
            SendMessage(wnd,LB_GETITEMRECT,cur_sel,(LPARAM)&rc); //��ö���ľ���λ��.

            y = rc.y+(rc.h/2);

            if(y < (rc0.h/2)) //���ѡ�е�����Listbox�����ߵ�����,�������ƶ�.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//�����߸���Զ����ÿ�ζ���һ�����(����ȥ�ƶ��ٶȿ�)��
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//�����߸��úܽ��ˣ���ֻ��1��offset(�����ƶ�)��
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,i);
            }

            if(y > (rc0.h/2)) //���ѡ�е�����Listbox�����ߵ�����,�������ƶ�.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//�����߸���Զ����ÿ�ζ���һ�����(����ȥ�ƶ��ٶȿ�)��
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//�����߸��úܽ��ˣ���ֻ��1��offset(�����ƶ�)��
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,-i);
            }
          }
        }
        
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code, id;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������
         
         NMHDR *nr;
         nr =(NMHDR*)lParam;      

         //���͵���
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* �˳���ť���� */
            case ID_CLOCK_WAIVE:
            {
              PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
            }
            break;

            /* ������ð�ť���� */
            case ID_CLOCK_OK:
            {
//              if (Set_Start.page == 0)    // ֻ���ñ���
//              {
//                // clock_dial = Set_Start.dial;    // ���ñ���
//              }
//              else 
              if (Set_Start.page == 0)    // �޸ı��̺�ʱ��
              {
                struct rtc_time systmtime;
                RTC_TimeCovr(&systmtime);    // ���޸����ڣ���������

                // clock_dial = Set_Start.dial;    // ���ñ���
                systmtime.tm_hour = GetListCurselVal(hwnd, ID_CLOCK_SetHour);        // ��ȡ�б��ʱ
                systmtime.tm_min = GetListCurselVal(hwnd, ID_CLOCK_SetMinute);       // ��ȡ�б�ķ�
                systmtime.tm_sec = 0;

                Time_Adjust(&systmtime);    // ����ʱ��
              }
              else if (Set_Start.page == 1)    // �޸ı��̡�ʱ�������
              {
                struct rtc_time systmtime;
                RTC_TimeCovr(&systmtime);    // ���޸�ʱ�䣬����ʱ��

                systmtime.tm_year = GetListCurselVal(hwnd, ID_CLOCK_SetYear);    // ��ȡ�б����
                systmtime.tm_mon  = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);          // ��ȡ�б����
                systmtime.tm_mday = GetListCurselVal(hwnd, ID_CLOCK_SetDate);           // ��ȡ�б����

                /* ��ķ����ɭ�ܼ��㹫ʽ (Time_Adjust()�����ڲ����㣬�����������) */
                //  systmtime.tm_wday = (systmtime.tm_mday + 2 * systmtime.tm_mon + 3     \
                //                          * systmtime.tm_mon + 1) / 5 + systmtime.tm_year \
                //                          + systmtime.tm_year / 4 - systmtime.tm_year / 100  \
                //                          + systmtime.tm_year / 400) % 7 + 1;

                Time_Adjust(&systmtime);    // ����ʱ��
              }

              PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
            }
            break;

//            /* ����ѡ�� */
//            case ID_CLOCK_Background00:
//            {
//              Set_Start.dial = 0;    // ѡ����� 1
//            }
//            break;

//            /* ����ѡ�� */
//            case ID_CLOCK_Background01:
//            {
//              Set_Start.dial = 1;    // ѡ����� 2
//            }
//            break;

//            /* ����ѡ�� */
//            case ID_CLOCK_Background02:
//            {
//              Set_Start.dial = 2;    // ѡ����� 3
//            }
//            break;

            /* ��һ�� */
            case ID_CLOCK_NEXT:
            {
//              if (Set_Start.page == 0)
//              {
                Set_Start.page = 1;    // ���Ϊʱ�����ý���
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"��������");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background00), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background01), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background02), SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_SHOW);
//              }
//              else if (Set_Start.page == 1)
//              {
//                Set_Start.page = 2;    // ���Ϊ�������ý���
//                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"��������");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),   SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),      SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),   SW_HIDE);

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_SHOW);
//              }
              
            }
            break;

            /* ��һ�� */
            case ID_CLOCK_BACK:
            {
//              if (Set_Start.page == 2)
//              {
//                Set_Start.page = 1;    // ���Ϊʱ�����ý���
//                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"����ʱ��");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),   SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),      SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),  SW_SHOW);

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_HIDE);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_HIDE);
//              }
//              else 
//                if (Set_Start.page == 1)
//              {
                Set_Start.page = 0;    // ���Ϊ����ʱ�����
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"����ʱ��");

//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background00), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background01), SW_SHOW);
//                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background02), SW_SHOW);

               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),         SW_SHOW);
//              }
            }
            break;
          }
        }

        /* �б��� */
        if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetMinute)
        {
          if(nr->code == LBN_SELCHANGE)    // �б�ѡ����ı�
          {
            
          }
          
          if(nr->code == LBN_POSCHANGE)    // ���б�λ�ñ��ı�
          {
            RECT rc0,rc;
            HWND wnd;
            int i;

            wnd = GetDlgItem(hwnd, id);     // ��ñ��ı��б�� HWND

            GetClientRect(wnd,&rc0);
            /* ʹ�б�ѭ�� */
            i = 0;
            SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
            if(rc.y > 2)
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_MOVEINDEX,i,0);
            }
            else
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
              if((rc.y+rc.h) < (rc0.h-8))
              {
                SendMessage(wnd,LB_MOVEINDEX,0,0xFFFF);
              }
            }

            /* ���ÿ����м���Ϊѡ���� */
            int min = 0;    // ��¼����ģ�����ѡ���
            int len = 0xFFFFFFF;
            int count = SendMessage(wnd,LB_GETCOUNT,0,0);
            i = SendMessage(wnd, LB_GETTOPINDEX, 0, 0);            // �׸���ʾ������Ŀ
            
            while (1)
            {
              SendMessage(wnd, LB_GETITEMRECT, i, (LPARAM)&rc);    // �������Ŀ�ľ��β���

              if (rc.y >= rc0.h || i >= count - 1)          // �ж��ǲ�������ʾ������
              {
                SendMessage(wnd, LB_SETCURSEL, min, 0);     // ���õ�ǰѡ�е�����Ŀ
                break;                                      // ��������ʾ���򣬽���ѭ��
              }

              rc.y += rc.h/2;                // ����������м�λ��
              rc.y = abs(rc.y - rc0.h/2);    // ���㵱ǰ��м�ľ���

              if (len > rc.y)
              {
                len = rc.y;    // ��¼��С����
                min = i;       // ��¼��С��
              }
              i++;         // ������һ��
              if (i>=count) 
              {
                i = 0;
              }
            }

            WCHAR wbuf[5];
            if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetDate)    // ������������
            {
              x_wsprintf(wbuf, L"%d��%02d��%02d��", GetListCurselVal(hwnd, ID_CLOCK_SetYear),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetMonth),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetDate));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETDATE), wbuf);    // ��������
            }
            else    // ��������ʱ��
            {
              x_wsprintf(wbuf, L"%02d:%02d", GetListCurselVal(hwnd, ID_CLOCK_SetHour),
                                             GetListCurselVal(hwnd, ID_CLOCK_SetMinute));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTIME), wbuf);    // ��������
            }
      
            uint8_t month = 0;
            month = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);    // ���ѡ�е���
            if (id == ID_CLOCK_SetMonth || (id == ID_CLOCK_SetYear && month == 2))    // �����·ݣ���ͬ�·����������
            {
              uint8_t date_max = 0;

              date_max = month_list[month - 1];

              if (month == 2)    // ����ƽ�� ����
              {
                uint16_t year = GetListCurselVal(hwnd, ID_CLOCK_SetYear);    // ���ѡ�е���

                if( year % 400 == 0 || (year % 4 ==  0 && year % 100 != 0))
                {
                  date_max = 29;    // ����
                }
              }

              HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

              SendMessage(ListHwnd, LB_RESETCONTENT, 0, 0);

              for (uint8_t xC=1; xC<date_max+1; xC++)
              {
                WCHAR wbuf[10];

                /* ���б����������� */
                x_wsprintf(wbuf, L"%d", xC);
                SendMessage(ListHwnd, LB_ADDSTRING, xC-1, (LPARAM)wbuf);
                SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC-1, (LPARAM)25);
              }
              SendMessage(ListHwnd, LB_SETCURSEL, 2, 0);

              // uint8_t list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);
              // uint8_t DateCursel = GetListCurselVal(hwnd, ID_CLOCK_SetDate);    // ���ѡ�е�����
              // uint8_t max = 0;
              // uint8_t item = 0;

              // if (list_conut > date_max)
              // {
              //   /* �б���Ŀ��ʵ��Ҫ����Ҫɾ�� */
              //   for (uint8_t xC=0; xC<list_conut; xC++)
              //   {
              //     if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > date_max)
              //     {
              //       SendMessage(ListHwnd, LB_DELETESTRING, xC, 0);
              //     }
              //   }
              // }
              // else if (list_conut < date_max)
              // {
              //   /* �б���Ŀ��ʵ��Ҫ����Ҫ���� */
              //   for (uint8_t xC=0; xC<list_conut; xC++)    // �ҵ������Ŀ
              //   {
              //     if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > max)
              //     {
              //       item = xC;    // ��¼�ϴ���Ŀ
              //       max = GetListItemVal(hwnd, ID_CLOCK_SetDate, item);       // �õ�����������
              //     }
              //   }

                
              //   list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);    // �õ����е���Ŀ��
              //   uint8_t ii = max;    // ��Ŀ

              //   for (uint8_t xY=item; xY<list_conut+date_max-max+1; xY++)    // ��������Ŀ����������Ҫ���ӵ���Ŀ
              //   {
              //     x_wsprintf(wbuf, L"%d", ii+1);
              //     ii++;
              //     SendMessage(ListHwnd, LB_ADDSTRING, xY, (LPARAM)wbuf);      // ������Ŀ
              //     SendMessage(ListHwnd, LB_SETITEMHEIGHT, xY, (LPARAM)45);    // ������Ŀ�߶�
              //   }
              // }

              // /* �����б��ѡ����Ŀ */
              // list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);
              // max = 0;
              // uint8_t xC=0;
              // for (xC=0; xC<list_conut; xC++)    // �ҵ������Ŀ
              // {
              //   if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) == DateCursel)
              //   {
              //     SendMessage(ListHwnd, LB_SETCURSEL, xC, 0);    // ����ѡ�е���Ŀ
              //     break;
              //   }

              //   if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > max)
              //   {
              //     item = xC;    // ��¼�ϴ���Ŀ
              //     max = GetListItemVal(hwnd, ID_CLOCK_SetDate, xC);
              //   }
              // }

              // if (xC >= list_conut)    // û�ҵ�ԭ��������������Ϊѡ����
              // {
              //   SendMessage(ListHwnd, LB_SETCURSEL, item, 0);    // ����ѡ�е���Ŀ
              // }
              // printf("xC=%d,item=%d\n", xC, item);
            }
          }
        }

      break;
    }

      // �ػ��ƺ�����Ϣ
      case WM_DRAWITEM:
      {
        DRAWITEM_HDR *ds;
        ds = (DRAWITEM_HDR*)lParam;        
        if (ds->ID >= ID_CLOCK_BACK && ds->ID <= ID_CLOCK_OK)
        {
          btn_owner_draw(ds);             // �ػ水ť
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTITLE)
        {
          Title_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTIME || ds->ID == ID_CLOCK_SETDATE)
        {
          WhiteBK_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_WAIVE)
        {
          waive_btn_owner_draw(ds);
          return TRUE;
        }
//        else if (ds->ID >= ID_CLOCK_Background00 && ds->ID <= ID_CLOCK_Background02)
//        {
//          radiobox_owner_draw(ds, ds->ID);
//          return TRUE;
//        }
        else if (ds->ID >= ID_CLOCK_SetYear && ds->ID <= ID_CLOCK_SetMinute)
        {
          listbox_owner_draw(ds);
          return TRUE;
        }

        return FALSE;
      }     
      
      //���ƴ��ڽ�����Ϣ
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
//        HDC hdc;//��Ļhdc

        //��ʼ����
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
      }
      break;

      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        
        RECT rc_title = {0, 0, GUI_XSIZE, 40};
        RECT rc_title_grad = {0, 40, GUI_XSIZE, 3};
        RECT rc_lyric = {0, 40, GUI_XSIZE, 200};
        RECT rc1 = {85, 46, 164, 150};

        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc_title);

        SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
        FillRect(hdc, &rc_lyric);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, CLOCK_BACK_COLOR), TRUE);
        
        SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
        EnableAntiAlias(hdc, TRUE);
        FillRoundRect(hdc, &rc1, 5);    // ���б���
        EnableAntiAlias(hdc, FALSE);
    

        return FALSE;
      }

      //�رմ�����Ϣ����case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //�رմ�����Ϣ����case
      case WM_DESTROY:
      {
        SetTimer(clock_hwnd, 1, 400, TMR_START, NULL);
        Set_Start.page = 0;

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}

static uint8_t sec_old = ~0;
static uint8_t sec_date = ~0;

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg){
      case WM_CREATE:
      {
        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  ��ť
        {
          /* ѭ��������ť */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        for (uint8_t xC=ICON_BTN_NUM; xC<ICON_TEXT_NUM; xC++)    // �ı���
        {
          /* ѭ�������ı��� */
          CreateWindow(TEXTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w,clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

//        /* ��ʼ������ */
//        WCHAR wbuf[5];
//        struct rtc_time systmtime;
//        RTC_TimeCovr(&systmtime);
//        
//        x_wsprintf(wbuf, L"%d", systmtime.tm_mday);
//        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DAY), wbuf);    // ��������
//        
//        x_wsprintf(wbuf, L"%d��", systmtime.tm_mon);
//        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // ������
//        
//        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[systmtime.tm_wday]);    // ��������

        SetTimer(hwnd, 1, 400, TMR_START, NULL);

        break;
      }

      case WM_TIMER:
      {
        int tmr_id;

				tmr_id = wParam;    // ��ʱ�� ID

				if (tmr_id == 1)
        {
          struct rtc_time systmtime;
          RTC_TimeCovr(&systmtime);
          
          if (systmtime.tm_sec != sec_old)
          {
            sec_old = systmtime.tm_sec;
            InvalidateRect(GetDlgItem(hwnd, ID_CLOCK_TIME), NULL, TRUE);
          }

          if (systmtime.tm_mday != sec_date)
          {
            sec_date = systmtime.tm_mday;
            InvalidateRect(GetDlgItem(hwnd, ID_CLOCK_DATE), NULL, TRUE);
          }
        }
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������   

         //���͵���
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* �˳���ť���� */
            case ID_CLOCK_EXIT:
            {
              PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
            }
            break;

            /* ��ʾ���������� */
            case ID_CLOCK_DATE:
            {
              KillTimer(hwnd, 1);
              /* �������ô��� */
              WNDCLASS wcex;
              RECT rc;
              
              Set_Start.page = 1;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // ����"����"����.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
            break;
            
            /* ��ʾʱ�������� */
            case ID_CLOCK_TIME:
            {
              KillTimer(hwnd, 1);
              /* �������ô��� */
              WNDCLASS wcex;
              RECT rc;
              
              Set_Start.page = 0;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // ����"����"����.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_OVERLAPPED,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
          }
        }
        break;
      }

      //�ػ��ƺ�����Ϣ
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_CLOCK_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_DATE)
         {
            date_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TITLE)
         {
            Title_Textbox_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TIME)
         {
            Dial_OwnerDraw(ds);
            return TRUE;
         }

         return FALSE;
      }     
      
      //���ƴ��ڽ�����Ϣ
      case WM_PAINT:
      {
        PAINTSTRUCT ps;

        //��ʼ����
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        
        RECT rc_title = {0, 0, GUI_XSIZE, 40};
        RECT rc_title_grad = {0, 40, GUI_XSIZE, 3};
        RECT rc_lyric = {0, 40, GUI_XSIZE, 200};

        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc_title);

        SetBrushColor(hdc, MapRGB(hdc, CLOCK_BACK_COLOR));
        FillRect(hdc, &rc_lyric);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, CLOCK_BACK_COLOR), TRUE);

        return FALSE;
      }

      //�رմ�����Ϣ����case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //�رմ�����Ϣ����case
      case WM_DESTROY:
      {        
        sec_old = ~0;
        sec_date = ~0;
        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_CLOCK_DIALOG(void)
{ 	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	clock_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI CLOCK DIALOG",
                                    WS_VISIBLE|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(clock_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, clock_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


