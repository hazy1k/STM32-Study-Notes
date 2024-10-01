#include "emXGUI.h"
#include "x_libc.h"

#include "./led/bsp_led.h"  
#include "GUI_AppDef.h"
#include "./rgb_led/GUI_RGBLED_DIALOG.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

/* Ӳ�����ƽӿ� */
extern void TIM_GPIO_Config(void);
extern void TIM_Mode_Config(void);
extern void TIM_RGBLED_Close(void);
extern void SetRGBColor(uint32_t rgb);
extern void SetColorValue(uint8_t r,uint8_t g,uint8_t b);
extern const unsigned char RGBdesktop[];
static COLORREF color_bg;       // ͸���ؼ��ı�����ɫ
#define RGBLED_BACK_COLOR     228, 228, 240
//static BITMAP RGBdesktop_0;
/**********************�ֽ���*********************/

uint8_t SELECT_RGB = 0;

struct leddlg
{
	char col_R;  //R����ֵ
	char col_G;  //G����ֵ
	char col_B;  //B����ֵ
   char led_R;//Ӳ��RGB����ɫ����ֵ
   char led_G;//Ӳ��RGB����ɫ����ֵ
   char led_B;//Ӳ��RGB����ɫ����ֵ
   int colR_ctr;//Ӳ��RGB�ƿ���λ
   int colG_ctr;//Ӳ��RGB�ƿ���λ
   int colB_ctr;//Ӳ��RGB�ƿ���λ
}leddlg_S={0, 0, 0, 0, 0, 0, 1, 1, 1};

icon_S GUI_RGBLED_Icon[18] = 
{
      {"tuichu",           {286, 10, 23, 23},   FALSE},    // �˳�����
      {"biaotilan",        {50,0,220,36},       FALSE},    // APP������
      {"APPHouse",         {425,80,275,275},    FALSE},    // APP����ͼ��
      {"hongdeng",         {34, 184,  36, 50},  FALSE},    // ���ͼ��
      {"lvdeng",           {143, 184, 36, 50},  FALSE},    // �̵�ͼ��
      {"landeng",          {251, 184, 36, 50},  FALSE},    // ����ͼ��
      {"hongdengscrollbar",{40, 120, 240, 30},  FALSE},    // ��ɫ������
      {"lvdengscrollbar",  {40, 120, 240, 30},  FALSE},    // ��ɫ������
      {"landengscrollbar", {40, 120, 240, 30},  FALSE},    // ��ɫ������ 
      
      {"100",      {123, 68, 77, 41}, FALSE},        // ����-�ٷֱ�
      {"I",        {44,  70, 24, 24}, FALSE},       // ����-С��
      {"I",        {241, 66, 32, 32}, FALSE},       // ����-���
};

RGBLED_DIALOG_s RGBLED_DIALOG =
{
   .RGBLED_Hwnd = NULL,
   .State = TRUE,
   .col_R = 0,
   .col_G = 0,
   .col_B = 0,
   
};

static void Delete_DlALOG()
{
  RGBLED_DIALOG.col_R = 0;
  RGBLED_DIALOG.col_G = 0;
  RGBLED_DIALOG.col_B = 0;
  RGBLED_DIALOG.State = 1;
  leddlg_S.colB_ctr = 1;
  leddlg_S.colG_ctr = 1;
  leddlg_S.colR_ctr = 1;
  leddlg_S.led_R = 0;
  leddlg_S.led_G = 0;
  leddlg_S.led_B = 0;
  leddlg_S.col_R = 0;
  leddlg_S.col_G = 0;
  leddlg_S.col_B = 0;
  TIM_RGBLED_Close();
}

/*
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  hdc:    ��ͼ������
 * @param  back_c��������ɫ
 * @param  Page_c: ������Page������ɫ
 * @param  fore_c���������������ɫ
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc,rc_tmp;
  RECT rc_scrollbar;

	/* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc_tmp);    // �õ���ť��λ��

  rc_scrollbar.x = rc_tmp.x;
  rc_scrollbar.y = rc.h/2;
  rc_scrollbar.w = rc.w - rc_tmp.x - 10;
  rc_scrollbar.h = 2;
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
  FillRoundRect(hdc, &rc_scrollbar, 5);
}

/*
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  hdc:    ��ͼ������
 * @param  back_c��������ɫ
 * @param  Page_c: ������Page������ɫ
 * @param  fore_c���������������ɫ
 * @retval NONE
*/
static void draw_gradient_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc,rc_tmp;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc_tmp);    // �õ���ť��λ��

  rc_scrollbar.x = rc.x+10;
  rc_scrollbar.y = rc.h/2;
  rc_scrollbar.w = rc_tmp.x;
  rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillRoundRect(hdc, &rc_scrollbar, 5);
}

/*
 * @brief  �Զ���ص�����
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void GUI_RGBLED_ScrollbarOwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
//	HDC hdc_mem;
	RECT rc;
	RECT rc_cli;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

  SetBrushColor(hdc, MapRGB(hdc, RGBLED_BACK_COLOR));
  FillRect(hdc, &rc_cli);
         
  EnableAntiAlias(hdc, TRUE);

  /* ���� */
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��

	//���ư�ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc, color_bg, RGB888(50, 50, 50), RGB888(255, 255, 255));
	//���ƽ������͵Ĺ�����
	draw_gradient_scrollbar(hwnd, hdc, color_bg, RGB888(50, 50, 50), RGB888(1, 218, 254));
  
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   
  
	//���ƻ���
  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillCircle(hdc, rc.x + rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  EnableAntiAlias(hdc, FALSE);
}

void GUI_RGBLED_HomeOwnerDraw(DRAWITEM_HDR *ds) 
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //���û���ɫ
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}
/**
  * @brief  �������ػ�
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @retval NULL
  */
static void GUI_TEXTLED_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;

  hdc = ds->hDC;   
  rc = ds->rc; 

  HFONT controlFont_24, controlFont_32;
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);

  /* ��ʾ����ͼ�� */
  if (ds->ID == ID_TEXTBOX_SMALL)
  {
    SetFont(hdc, controlFont_24);
  }
  else if (ds->ID == ID_TEXTBOX_BIG)
  {
    SetFont(hdc, controlFont_32);
  }

  SetTextColor(hdc, MapRGB(hdc, leddlg_S.col_R, leddlg_S.col_G, leddlg_S.col_B));

  DrawText(hdc, L"I", -1, &rc, NULL);//��������(���ж��뷽ʽ)
  DeleteFont(controlFont_24);
  DeleteFont(controlFont_32);
}
/**
  * @brief  �ٷֱ��ı��ػ�
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @retval NULL
  */
static void GUI_RGBPCTTEXT_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc; //�ؼ�����HDC
  HWND hwnd; //�ؼ���� 
  RECT rc_cli;//�ؼ���λ�ô�С����
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 
  //   if(ds->ID ==  ID_BUTTON_START && show_lrc == 1)
  //      return;
  //��ȡ�ؼ���λ�ô�С��Ϣ
  GetClientRect(hwnd, &rc_cli);
  rc_cli.y -=10;
  SetBrushColor(hdc, MapRGB(hdc, RGBLED_BACK_COLOR));
  FillRect(hdc, &rc_cli);

  GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������  

  //�����ı�����ɫ
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  HFONT controlFont_32;
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
  SetFont(hdc, controlFont_32);
  DrawText(hdc, L"H",-1,&rc_cli,DT_BOTTOM|DT_RIGHT);

  HFONT controlFont_24;
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  SetFont(hdc, controlFont_24);
  rc_cli.w -= 30;
  DrawText(hdc, wbuf,-1,&rc_cli,DT_BOTTOM|DT_RIGHT);
  
  DeleteFont(controlFont_24);
  DeleteFont(controlFont_32);
}

static void radiobox_owner_draw(DRAWITEM_HDR *ds) // ��ѡ��ť���
{
	HDC hdc;
	RECT rc;
  WCHAR wbuf[128];

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
  
  SetBrushColor(hdc, MapRGB(hdc, RGBLED_BACK_COLOR));
  FillRect(hdc, &rc);
  GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ������� 

  if (ds->State & BN_CHECKED)
  { 
    // ��ť��ѡ��״̬
    SetBrushColor(hdc, MapRGB(hdc, 50, 50, 50));
  }
  else
  {
    SetBrushColor(hdc, MapRGB(hdc, 210, 210, 210));
  }
  
  SetTextColor(hdc, MapRGB(hdc, 240, 240, 240));
  
  EnableAntiAlias(hdc, TRUE);
  FillCircle(hdc, rc.w/2, rc.w/2, MIN(rc.w, rc.h)/2);
  EnableAntiAlias(hdc, FALSE);
  HFONT controlFont_32;
  
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
  SetFont(hdc, controlFont_32);
  rc.y += 0;
  rc.h -= 5;
  rc.x += 1;
  DrawText(hdc, L"I", -1, &rc, DT_TOP|DT_CENTER);//��������(���ж��뷽ʽ)
  rc.x -= 1;
  rc.y += 6;
  SetFont(hdc, defaultFont);
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  DrawText(hdc, wbuf, -1, &rc, DT_CENTER|DT_BOTTOM);//��������(���ж��뷽ʽ)
  
  DeleteFont(controlFont_32);
}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  RECT rc;
	switch (msg)
	{
      case WM_CREATE: 
      {
         GetClientRect(hwnd, &rc);

         CreateWindow(BUTTON, L"O",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                    GUI_RGBLED_Icon[0].rc.x, GUI_RGBLED_Icon[0].rc.y, 
                    GUI_RGBLED_Icon[0].rc.w, GUI_RGBLED_Icon[0].rc.h, 
                    hwnd, ID_EXIT, NULL, NULL); 

         CreateWindow(TEXTBOX, L"RGB�ƿ���", WS_TRANSPARENT|WS_VISIBLE, 
                      GUI_RGBLED_Icon[1].rc.x, GUI_RGBLED_Icon[1].rc.y, 
                      GUI_RGBLED_Icon[1].rc.w, GUI_RGBLED_Icon[1].rc.h, 
                      hwnd, ID_TEXTBOX_Title, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_Title),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER); 

         RGBLED_DIALOG.sif_R.cbSize = sizeof(RGBLED_DIALOG.sif_R);
         RGBLED_DIALOG.sif_R.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_R.nMin = 0;
         RGBLED_DIALOG.sif_R.nMax = 255;
         RGBLED_DIALOG.sif_R.nValue = RGBLED_DIALOG.col_R;
         RGBLED_DIALOG.sif_R.TrackSize = 30;
         RGBLED_DIALOG.sif_R.ArrowSize = 0;

         /*����������--R*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW |WS_TRANSPARENT| WS_VISIBLE, 
                      GUI_RGBLED_Icon[6].rc.x, GUI_RGBLED_Icon[6].rc.y, 
                      GUI_RGBLED_Icon[6].rc.w, GUI_RGBLED_Icon[6].rc.h, 
                      hwnd, ID_SCROLLBAR_R, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_R), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_R);

         RGBLED_DIALOG.sif_G.cbSize = sizeof(RGBLED_DIALOG.sif_G);
         RGBLED_DIALOG.sif_G.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_G.nMin = 0;
         RGBLED_DIALOG.sif_G.nMax = 255;
         RGBLED_DIALOG.sif_G.nValue = RGBLED_DIALOG.col_G;
         RGBLED_DIALOG.sif_G.TrackSize = 30;
         RGBLED_DIALOG.sif_G.ArrowSize = 0;
         /*����������--G*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_G", WS_OWNERDRAW |WS_TRANSPARENT, 
                      GUI_RGBLED_Icon[7].rc.x, GUI_RGBLED_Icon[7].rc.y, 
                      GUI_RGBLED_Icon[7].rc.w, GUI_RGBLED_Icon[7].rc.h, 
                      hwnd, ID_SCROLLBAR_G, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_G), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_G);
         RGBLED_DIALOG.sif_B.cbSize = sizeof(RGBLED_DIALOG.sif_B);
         RGBLED_DIALOG.sif_B.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_B.nMin = 0;
         RGBLED_DIALOG.sif_B.nMax = 255;
         RGBLED_DIALOG.sif_B.nValue = RGBLED_DIALOG.col_B;
         RGBLED_DIALOG.sif_B.TrackSize = 30;
         RGBLED_DIALOG.sif_B.ArrowSize = 0;
         /*����������--B*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_B", WS_OWNERDRAW | WS_TRANSPARENT, 
                      GUI_RGBLED_Icon[8].rc.x, GUI_RGBLED_Icon[8].rc.y, 
                      GUI_RGBLED_Icon[8].rc.w, GUI_RGBLED_Icon[8].rc.h,
                      hwnd, ID_SCROLLBAR_B, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_B), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_B);

         SetColorValue(0, 0, 0);

         /* ��ѡ��ť */
         CreateWindow(BUTTON, L"���", WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        GUI_RGBLED_Icon[3].rc.x, GUI_RGBLED_Icon[3].rc.y,
                        GUI_RGBLED_Icon[3].rc.w, GUI_RGBLED_Icon[3].rc.h,
                        hwnd, ID_RGB_SELECT_R | (1 << 16), NULL, NULL); 

         /* ��ѡ��ť */
         CreateWindow(BUTTON, L"�̵�", WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        GUI_RGBLED_Icon[4].rc.x, GUI_RGBLED_Icon[4].rc.y,
                        GUI_RGBLED_Icon[4].rc.w, GUI_RGBLED_Icon[4].rc.h,
                        hwnd, ID_RGB_SELECT_G | (1 << 16), NULL, NULL); 
         
         /* ��ѡ��ť */
         CreateWindow(BUTTON, L"����", WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        GUI_RGBLED_Icon[5].rc.x, GUI_RGBLED_Icon[5].rc.y,
                        GUI_RGBLED_Icon[5].rc.w, GUI_RGBLED_Icon[5].rc.h,
                        hwnd, ID_RGB_SELECT_B | (1 << 16), NULL, NULL); 

        SendMessage(GetDlgItem(hwnd, ID_RGB_SELECT_R), BM_SETSTATE, BST_CHECKED, 0);    // Ĭ��ѡ�е�һ��

         // CreateWindow(BUTTON,L"Checkbox5",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
         //              GUI_RGBLED_Icon[9].rc.x, GUI_RGBLED_Icon[9].rc.y, 
         //              GUI_RGBLED_Icon[9].rc.w, GUI_RGBLED_Icon[9].rc.h,
         //              hwnd,ID_CHECKBOX_SW,NULL,NULL);         
         /*�����ı���--���*/
         CreateWindow(BUTTON, L"0", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                      GUI_RGBLED_Icon[9].rc.x, GUI_RGBLED_Icon[9].rc.y, 
                      GUI_RGBLED_Icon[9].rc.w, GUI_RGBLED_Icon[9].rc.h,
                      hwnd, ID_TEXTBOX_PCT, NULL, NULL);

         CreateWindow(BUTTON, L"I", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                      GUI_RGBLED_Icon[10].rc.x, GUI_RGBLED_Icon[10].rc.y, 
                      GUI_RGBLED_Icon[10].rc.w, GUI_RGBLED_Icon[10].rc.h,
                      hwnd, ID_TEXTBOX_SMALL, NULL, NULL);

         CreateWindow(BUTTON, L"I", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                      GUI_RGBLED_Icon[11].rc.x, GUI_RGBLED_Icon[11].rc.y, 
                      GUI_RGBLED_Icon[11].rc.w, GUI_RGBLED_Icon[11].rc.h,
                      hwnd, ID_TEXTBOX_BIG, NULL, NULL);

         
         //DrawBitmap(hdc_bk,0, 0,&RGBdesktop_0,NULL);
         //StretchBlt(RGBLED_DIALOG.hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_bk, 0, 0,RGBdesktop_0.Width, RGBdesktop_0.Height, SRCCOPY);               
         SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
         //GUI_DEBUG("%x%x", leddlg_S.led_R/16, leddlg_S.led_R%16);
         break;
      }

      case WM_NOTIFY: 
      {
         NMHDR *nr;
         WCHAR wbuf[128];
         u16 ctr_id; 
         u16 code,  id;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������
         ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID.
         nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
         if(id == ID_EXIT && code == BN_CLICKED)
         {
            PostCloseMessage(hwnd);
         }

         if (ctr_id == ID_SCROLLBAR_R)
         {
            NM_SCROLLBAR *sb_nr;		
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R�����ƶ�
               {
                  leddlg_S.col_R = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_R); //����λ��ֵ
                  x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_R/255.0*100));
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_SMALL), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_BIG), NULL, TRUE);
                  //RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_PCT), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }
         
         if (ctr_id == ID_SCROLLBAR_G)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //G�����ƶ�
               {
                  leddlg_S.col_G = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_G); //����λ��ֵ
                  x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_G/255.0*100));
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_SMALL), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_BIG), NULL, TRUE);
                  //RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_PCT), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }

         if(ctr_id == ID_SCROLLBAR_B)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //B�����ƶ�
               {
                  leddlg_S.col_B = sb_nr->nTrackValue; //���B���鵱ǰλ��ֵ
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_B); //����B�����λ��
                  x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_B/255.0*100));
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_PCT), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_SMALL), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_BIG), NULL, TRUE);
                  
                  //RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_B_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }

         /* RGB���� */
         if (id >= ID_RGB_SELECT_R && id <= ID_RGB_SELECT_B && code ==  BN_CLICKED)
         {
            WCHAR wbuf[128];
            SELECT_RGB = id - ID_RGB_SELECT_R;    // ѡ��Ҫ���ڵ�RGB
            if (SELECT_RGB == 0)
            {
               x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_R/255.0*100));
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_R), SW_SHOW);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_G), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_B), SW_HIDE);
            }
            else if (SELECT_RGB == 1)
            {
               x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_G/255.0*100));
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_R), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_G), SW_SHOW);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_B), SW_HIDE);
            }
            else if (SELECT_RGB == 2)
            {
               x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_B/255.0*100));
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_R), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_G), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_B), SW_SHOW);
            }
         }

         if(RGBLED_DIALOG.State == TRUE)
         {
            leddlg_S.led_R=leddlg_S.col_R;
            leddlg_S.led_G=leddlg_S.col_G;
            leddlg_S.led_B=leddlg_S.col_B;
            SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
         }
         else
         {
            SetColorValue(0, 0, 0);
         }
         break;
      }
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;

         rc = ds->rc;

         switch(ds->ID)
         {
            case ID_EXIT:
            {              
               GUI_RGBLED_HomeOwnerDraw(ds);

               return TRUE;              
            }
            case ID_SCROLLBAR_R:
            case ID_SCROLLBAR_G:
            case ID_SCROLLBAR_B:
            {
               GUI_RGBLED_ScrollbarOwnerDraw(ds);
               return TRUE;
            }
            case ID_RGB_SELECT_R:
            case ID_RGB_SELECT_G:
            case ID_RGB_SELECT_B:
            {
               radiobox_owner_draw(ds);
               return TRUE;
            }
            
            case ID_TEXTBOX_PCT:
            {
               GUI_RGBPCTTEXT_OwnerDraw(ds);
               return TRUE;
            }
            case ID_TEXTBOX_SMALL:
            case ID_TEXTBOX_BIG:
            {
               GUI_TEXTLED_OwnerDraw(ds);
               return TRUE;
            }      
         }
         break;

      }
     case WM_PAINT:
     {
        HDC hdc;
        PAINTSTRUCT ps;

        hdc = BeginPaint(hwnd, &ps);

        RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
        RECT rc_title_grad = {0, 40, GUI_XSIZE, 3};
        
        SetBrushColor(hdc, MapRGB(hdc, RGBLED_BACK_COLOR));
        FillRect(hdc, &rc);
        

        rc.h = 40;
        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc);//, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
//      GradientFillRect(hdc, &rc, MapRGB(hdc, 64, 189, 162), MapRGB(hdc, 36, 158, 189), TRUE);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, RGBLED_BACK_COLOR), TRUE);
        
        EndPaint(hwnd, &ps);
        break;
     }      

      case WM_DESTROY:
      {        
         Delete_DlALOG();

         return PostQuitMessage(hwnd);	
      }          
      default:
         return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_LED_DIALOG(void)
{
	HWND	hwnd;
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
   //��ʼ����ʱ��
   //LED_GPIO_Config();
   TIM_GPIO_Config();
   TIM_Mode_Config();
   //SetColorValue(leddlg_S.col_R, leddlg_S.col_R, leddlg_S.col_R);
   
	//����������
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_DIALOG",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}
