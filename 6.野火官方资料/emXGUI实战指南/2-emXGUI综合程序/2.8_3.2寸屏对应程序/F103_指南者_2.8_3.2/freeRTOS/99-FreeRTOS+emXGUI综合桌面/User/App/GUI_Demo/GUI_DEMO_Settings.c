#include "emXGUI.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include <stdlib.h>
#include "CListMenu.h"

/* ͼƬ��Դ�� */
//#define GUI_SETTINGS_BACKGROUNG_PIC      "settingsdesktop.jpg"        // 800*480

typedef enum 
{ 
  /****************** ��ť�ؼ� ID ֵ *******************/
  ID_SETTINGS_EXIT = 0x1000,      // �˳���ť
  ID_SETTINGS_DET,                // ���ڿ�����
	ID_SETTINGS_THEME,              // ����ѡ��
  ID_DET_EXIT,                // ���ڿ�����
  /***************** �ı��ؼ� ID ֵ *********************/
  ID_SETTINGS_TITLE,              // ����
  ID_SETTINGS_THEMEINFO,          // ������ʾ
  /***************** �Ӵ��� ID ֵ *********************/
  ID_DET_WIN,
}set_id_t;

typedef struct{
	WCHAR *icon_name;    // ͼ����
	RECT rc;             // λ����Ϣ
	set_id_t id;         // ��ťID
}set_icon_t;

#define ICON_BTN_NUM     3     // ��ť����

//ͼ���������
const set_icon_t set_icon[] = {

  /* ��ť */
  {L"-",           {286, 12, 23, 23}, ID_SETTINGS_EXIT},      // 0. �˳���ť
  {L"���ڿ�����",  { 10,  43, 310,  23}, ID_SETTINGS_DET},       // 1. ���ڿ�����
  {L"1",           {268, 69,  42,  20}, ID_SETTINGS_THEME},     // 2. ����ѡ��
  {L"����",        {50, 0,  220,  35}, ID_SETTINGS_TITLE},      // 3. 
  {L"����",        {10, 70, 100, 23}, ID_SETTINGS_THEMEINFO},   // 4. 
};

extern uint8_t Theme_Flag;   // �����־
extern HWND	hwnd_home;
extern HWND	app_hwnd_desktop;

/*
 * @brief  �ػ�����ѡ��ť
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void theme_button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc, rc_tmp;
  RECT rc_tmp1 = {0, 0, ds->rc.h-2, ds->rc.h-2};
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  SetBrushColor(hdc, MapRGB(hdc, 1, 153, 255));

  EnableAntiAlias(hdc, ENABLE);
  FillRoundRect(hdc, &rc, MIN(rc.w/2, rc.h/2));    // ����Բ�Ǿ���
  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
  if (wbuf[0] == L'1')
  {
		EnableAntiAlias(hdc, ENABLE);
		FillCircle(hdc, rc.x+9, rc.y+10, rc_tmp1.h/2);    // ����Բ
		EnableAntiAlias(hdc, DISABLE);
		rc.x =5;
		DrawText(hdc, wbuf, -1, &rc, DT_LEFT);//��������(���ж��뷽ʽ)
  }
  else 
  {
		EnableAntiAlias(hdc, ENABLE);
		FillCircle(hdc, rc.x+33, rc.y+10, rc_tmp1.h/2);    // ����Բ
		EnableAntiAlias(hdc, DISABLE);
		rc.x =28;
		DrawText(hdc, wbuf, -1, &rc, DT_RIGHT|DT_CENTER);//��������(���ж��뷽ʽ)
  }
  EnableAntiAlias(hdc, DISABLE);
}

static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

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

static void det_exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HWND hwnd;
	HDC hdc;
  RECT rc,rc_tmp;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
  {
    SetTextColor(hdc, MapRGB(hdc, 100, 100, 100));
  }
  else
  {
    SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
  }
	HFONT controlFont_24;
	
  controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
  SetFont(hdc, controlFont_24);
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
  DeleteFont(controlFont_24);
}

/*
 * @brief  �ػ���ڿ����尴ť
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void det_button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */
  SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
  FillRect(hdc, &rc);

  SetFont(hdc, defaultFont);
  if (ds->State & BST_PUSHED)
  {
    SetTextColor(hdc, MapRGB(hdc, 100, 100, 100));
  }
  else
  {
    SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
  }
	
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
  rc.w -= 10;
  HFONT controlFont_16;
	
  controlFont_16 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_16);
  SetFont(hdc, controlFont_16);
  DrawText(hdc, L"C", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)
  SetPenColor(hdc, MapRGB(hdc, 22, 22, 22));
  HLine(hdc, rc.x, rc.y+rc.h-1, rc.x+rc.w+10);
  
  DeleteFont(controlFont_16);
}

/*
 * @brief  �ػ��ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void text_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */

  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));

  if (ds->ID == ID_SETTINGS_TITLE)
  {
    SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
    FillRect(hdc, &rc);
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
  }
  else
  {
    SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
    FillRect(hdc, &rc);
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
  }

}

static LRESULT DetWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
   switch(msg){
      case WM_CREATE:
      {
          CreateWindow(BUTTON, L"F", WS_OWNERDRAW | WS_VISIBLE,
                        10, 3, 30, 30, hwnd, ID_DET_EXIT, NULL, NULL); 

        break;
      }    
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������   

         //���͵���
        if(code == BN_CLICKED)
        {
          if (id == ID_DET_EXIT)
          {
            PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
          }
        }

        break;
      }

      //�ػ��ƺ�����Ϣ
      case WM_DRAWITEM:
      {
        DRAWITEM_HDR *ds;
        ds = (DRAWITEM_HDR*)lParam;        
        if(ds->ID == ID_DET_EXIT)
        {
          det_exit_owner_draw(ds);
          return TRUE;
        }

        return FALSE;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
      
        RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
        RECT rc_title_grad = {0, 35, GUI_XSIZE, 200};
        
        SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
        FillRect(hdc, &rc);

        rc.h = 35;
        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc);//, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 240, 240, 240), TRUE);

        return FALSE;
      }
      
      //���ƴ��ڽ�����Ϣ
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        RECT rc = {10, 44, 310, 23};
        RECT rc1 = {50, 0, 220, 40};
        HDC hdc;

        //��ʼ����
        hdc = BeginPaint(hwnd, &ps); 

        SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
        SetPenColor(hdc, MapRGB(hdc, 22, 22, 22));

        DrawText(hdc, L"���ڿ�����", -1, &rc1, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)

        DrawText(hdc, L"Ӳ���汾", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"V2", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"�������ͺ�", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"Ұ��-ָ����", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"����ռ�", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"512KB", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"�����ڴ�", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"64KB", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"emXGUI�汾", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"V1.0.0", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"FreeRTOS�汾", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"V9.0.0", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"������", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
        DrawText(hdc, L"STM32F103", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);
        
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+310);
        
        EndPaint(hwnd, &ps);
        break;
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

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}
HWND hwnd_d;
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
   switch(msg){
      case WM_CREATE:
      {
        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  ��ť
        {
          /* ѭ��������ť */
          CreateWindow(BUTTON, set_icon[xC].icon_name,  WS_OWNERDRAW | WS_VISIBLE,
                        set_icon[xC].rc.x, set_icon[xC].rc.y,
                        set_icon[xC].rc.w, set_icon[xC].rc.h,
                        hwnd, set_icon[xC].id, NULL, NULL); 
        }

        if (Theme_Flag == 0)
        {
          SetWindowText(GetDlgItem(hwnd, ID_SETTINGS_THEME), L"1");
        }
        else if  (Theme_Flag == 1)
        {
          SetWindowText(GetDlgItem(hwnd, ID_SETTINGS_THEME), L"2");
        }
        
        for (uint8_t xC=ICON_BTN_NUM; xC<ICON_BTN_NUM + 2; xC++)     //  ��ť
        {
          /* ѭ�������ı��� */
          CreateWindow(TEXTBOX, set_icon[xC].icon_name,  WS_OWNERDRAW | WS_VISIBLE,
                        set_icon[xC].rc.x, set_icon[xC].rc.y,
                        set_icon[xC].rc.w, set_icon[xC].rc.h,
                        hwnd, set_icon[xC].id, NULL, NULL); 
        }

        break;
      }

      case WM_TIMER:
      {


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
          if (id == ID_SETTINGS_EXIT)
          {
            PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
          }
          else if (id == ID_SETTINGS_DET)
          {
            WNDCLASS wcex;
						RECT rc;

						wcex.Tag = WNDCLASS_TAG;
						wcex.Style = CS_HREDRAW | CS_VREDRAW;
						wcex.lpfnWndProc = (WNDPROC)DetWinProc;
						wcex.cbClsExtra = 0;
						wcex.cbWndExtra = 0;
						wcex.hInstance = NULL;
						wcex.hIcon = NULL;
						wcex.hCursor = NULL;

						rc.x = 0;
						rc.y = 0;
						rc.w = GUI_XSIZE;
						rc.h = GUI_YSIZE;

						CreateWindow(&wcex, L"---", WS_VISIBLE, 
                         rc.x, rc.y, rc.w, rc.h, hwnd, ID_DET_WIN, NULL, NULL);
          }
          else if (id == ID_SETTINGS_THEME)
          {
            WCHAR wbuf[3];
            HWND  wnd = GetDlgItem(hwnd, ID_SETTINGS_THEME);

            GetWindowText(wnd, wbuf, 3);
            if (wbuf[0] == L'1')
            {
              Theme_Flag = 1;
              SendMessage(GetDlgItem(hwnd_home, 0x1000), MSG_SET_BGCOLOR, COLOR_DESKTOP_BACK_GROUND_HEX, NULL);    // 0x1000 ��home ���б�ID
              SetWindowText(wnd, L"2");
            }
            else
            {
              SendMessage(GetDlgItem(hwnd_home, 0x1000), MSG_SET_BGCOLOR, 1, NULL);           // ����Ϊ 1 ʱ ʹ��ͼƬ��Ϊ����  0x1000 ��home ���б�ID
              Theme_Flag = 0;
              SetWindowText(wnd, L"1");
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
         if(ds->ID == ID_SETTINGS_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_SETTINGS_DET)
         {
            det_button_OwnerDraw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_SETTINGS_THEME)
         {
            theme_button_OwnerDraw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_SETTINGS_TITLE || ds->ID == ID_SETTINGS_THEMEINFO)
         {
            text_OwnerDraw(ds);
            return TRUE;
         }
         

        return FALSE;
      }
      
      //���ƴ��ڽ�����Ϣ
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        RECT rc = {9,72, 312, 21};
        HDC hdc;
        //��ʼ����
        hdc = BeginPaint(hwnd, &ps); 
        SetPenColor(hdc, MapRGB(hdc, 22, 22, 22));
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+rc.w);
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
      
        RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
        RECT rc_title_grad = {0, 35, GUI_XSIZE, 5};
        
        SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
        FillRect(hdc, &rc);

        rc.h = 35;
        SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
        FillRect(hdc, &rc);//, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
        
        GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 240, 240, 240), TRUE);

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

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_Settings_DIALOG(void)
{ 	
	WNDCLASS	wcex;
  HWND hwnd;
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
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI Settings DIALOG",
                                    WS_VISIBLE,
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


