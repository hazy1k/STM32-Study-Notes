#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "./beep/bsp_beep.h" 
#include "emxgui_png.h"

/* ��ť ID */
enum 
{
  ID_BEEPER_TICK = 0x1000,    // �������δ�ť
  ID_BEEPER_SW,               // ���������س�����ť
  ID_BEEPER_PRESS,            // ��������ס�찴ť
  ID_BEEPER_EXIT,             // �������˳���ť
};

static uint8_t sw_flag    = 0;
static uint8_t tick_flag  = 0;
static uint8_t press_flag = 3;

/********************************************************************************************
 *                                     ����������ͼƬ                                       *
 ********************************************************************************************/
#define GUI_BEEPER_PIC_EN     0    // �ڴ治����������

#if GUI_BEEPER_PIC_EN
#define GUI_BEEPER_HORN_PIC         "0:/beeper_horn.png"
#define GUI_BEEPER_HORN1_PIC        "0:/beeper_horn1.png"
#define GUI_BEEPER_HORN2_PIC        "0:/beeper_horn2.png"
#define GUI_BEEPER_HORN3_PIC        "0:/beeper_horn3.png"

typedef enum
{
  hdc_beeper_horn = 0,
	hdc_beeper_horn1,
	hdc_beeper_horn2,
	hdc_beeper_horn3,
  
	hdc_beeper_end,
}hdc_beep_png_t;

typedef struct{
	char *pic_name;      // ͼƬ��
	int w;               // ͼƬ��
	int h;               // ͼƬ��
	hdc_beep_png_t id;   // hdc ���
}beeper_hdc_t;

const beeper_hdc_t beeper_png_info[hdc_beeper_end] = 
{
  {GUI_BEEPER_HORN_PIC,     59, 101,   hdc_beeper_horn},
  {GUI_BEEPER_HORN1_PIC,    13,  40,   hdc_beeper_horn1},
  {GUI_BEEPER_HORN2_PIC,    21,  79,   hdc_beeper_horn2},
  {GUI_BEEPER_HORN3_PIC,    29, 116,   hdc_beeper_horn3},
  {GUI_BEEPER_BUTTON_PIC,   38,  38,   hdc_beeper_button},
};

HDC hdc_beeper_png[hdc_beeper_end];
#endif
/********************************************************************************************
*                             ����������ͼƬ END                                            *
********************************************************************************************/

//�˳���ť�ػ���
static void ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;

  hdc = ds->hDC;   
  rc = ds->rc; 

  if (ds->State & BST_PUSHED)
  { //��ť�ǰ���״̬
    SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //��������ɫ
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

/*
 * @brief  ѡ��ť
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void sw_button_OwnerDraw(DRAWITEM_HDR *ds)
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
  
  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 50, 240, 240));
  FillRoundRect(hdc, &rc, MIN(rc.w, rc.h)/2);

  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  
  if (ds->ID == ID_BEEPER_SW)
  {
    if (sw_flag == 0)
    {
      SetBrushColor(hdc, MapRGB(hdc, 230, 230, 230));
      InflateRect(&rc, -5, -5);
      FillRoundRect(hdc, &rc, MIN(rc.w, rc.h)/2);
      
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
      FillCircle(hdc, rc.w/2 + 5, rc.w/2 + 5, MIN(rc.w, rc.h)/2);

      rc.h = rc.w;
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
    else 
    {
      SetBrushColor(hdc, MapRGB(hdc, 50, 50, 240));
      InflateRect(&rc, -5, -5);
      FillRoundRect(hdc, &rc, MIN(rc.w, rc.h)/2);
      
      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillCircle(hdc, rc.w/2 + 5, rc.h - rc.w/2 + 5, MIN(rc.w, rc.h)/2);
      
      rc.y = rc.h - rc.w;
      rc.h = rc.w;
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
  }
  else if (ds->ID == ID_BEEPER_TICK)
  {
    if (tick_flag == 0)
    {
      SetBrushColor(hdc, MapRGB(hdc, 230, 230, 230));
      InflateRect(&rc, -5, -5);
      FillRoundRect(hdc, &rc, MIN(rc.w, rc.h)/2);
      
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
      FillCircle(hdc, rc.w/2 + 5, rc.w/2 + 5, MIN(rc.w, rc.h)/2);
      
      rc.h = rc.w;
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
    else 
    {
      SetBrushColor(hdc, MapRGB(hdc, 50, 50, 240));
      InflateRect(&rc, -5, -5);
      FillRoundRect(hdc, &rc, MIN(rc.w, rc.h)/2);
      
      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillCircle(hdc, rc.w/2 + 5, rc.h - rc.w/2 + 5, MIN(rc.w, rc.h)/2);
      
      rc.y = rc.h - rc.w;
      rc.h = rc.w;
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
    }
  }
  
  EnableAntiAlias(hdc, FALSE);
}

/*
 * @brief  �м䰴ס��ť
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void press_button_OwnerDraw(DRAWITEM_HDR *ds)
{
	HDC hdc;
  RECT rc;

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

	/* ���� */
  SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
  FillRect(hdc, &rc);
  
  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 50, 240, 240));
  FillCircle(hdc, rc.w/2, rc.h/2, MIN(rc.w, rc.h)/2);
  
#if GUI_BEEPER_PIC_EN
  
  BitBlt(hdc, 54, 77, 103, 190, hdc_beeper_png[hdc_beeper_horn], 0, 0, SRCCOPY);
  
  switch (press_flag)
  {
    case 3:
      BitBlt(hdc, 200, 62, 74, 220, hdc_beeper_png[hdc_beeper_horn3], 0, 0, SRCCOPY);
    
    case 2:
      BitBlt(hdc, 187, 98, 53, 148, hdc_beeper_png[hdc_beeper_horn2], 0, 0, SRCCOPY);

    case 1:
      BitBlt(hdc, 174, 135, 33, 74, hdc_beeper_png[hdc_beeper_horn1], 0, 0, SRCCOPY);
  
  default:
    break;
  }

#else

  SetBrushColor(hdc, MapRGB(hdc, 50, 50, 50));
  
  switch (press_flag)
  {
    case 3:
      FillArc(hdc, rc.w/2, rc.h/2, 50, 60, 0, 360);
    
    case 2:
      FillArc(hdc, rc.w/2, rc.h/2, 30, 40, 0, 360);

    case 1:
      FillArc(hdc, rc.w/2, rc.h/2, 10, 20, 0, 360);
  
  default:
    break;
  }
  
#endif
  
  EnableAntiAlias(hdc, FALSE);
}

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
			Beep_Init();
      RECT rc;
      GetClientRect(hwnd, &rc); 
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  286, 5, 23, 23, hwnd, ID_BEEPER_EXIT, NULL, NULL); 

      CreateWindow(BUTTON, L"�δ�", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  12, 94, 38, 84, hwnd, ID_BEEPER_TICK, NULL, NULL); 

      CreateWindow(BUTTON, L"��ס��", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  69, 42, 182, 182, hwnd, ID_BEEPER_PRESS, NULL, NULL); 

      CreateWindow(BUTTON, L"����", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                  266, 94, 38, 84, hwnd, ID_BEEPER_SW, NULL, NULL); 
#if GUI_BEEPER_PIC_EN
      BOOL res;
      u8 *pic_buf;
      u32 pic_size;
      PNG_DEC *png_dec;
      BITMAP png_bm;
      
      
      for(uint8_t xC=0; xC<hdc_beeper_end; xC++)
      {
        /* ���� HDC */
        hdc_beeper_png[xC] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, beeper_png_info[xC].w, beeper_png_info[xC].h);
        ClrDisplay(hdc_beeper_png[xC], NULL, 0);
        if (strstr(beeper_png_info[xC].pic_name, "0:/") != NULL)
        {
          res = FS_Load_Content(beeper_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);    // ��Դ�� SD ��
        }
        else
        {
          res = RES_Load_Content(beeper_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);     // ��Դ���ⲿ FLASH
        }
        
        if(res)
        {
          png_dec = PNG_Open(pic_buf, pic_size);
          PNG_GetBitmap(png_dec, &png_bm);
          DrawBitmap(hdc_beeper_png[xC], 0, 0, &png_bm, NULL);
          PNG_Close(png_dec);
        }
        else
        {
          GUI_ERROR("Can not find RES:%s",beeper_png_info[xC].pic_name);
          res_not_found_flag = TRUE;    // ���û���ҵ���Դ�ļ�
        }
        
        /* �ͷ�ͼƬ���ݿռ� */
        RES_Release_Content((char **)&pic_buf);
      }
#endif
      SetTimer(hwnd, 5, 10, TMR_START | TMR_SINGLE, NULL);
      SetTimer(hwnd, 6, 1000, TMR_START | TMR_SINGLE, NULL);
      break;
    } 
    case WM_TIMER:
    {
      static uint8_t beep_flag = 0;

      uint16_t timer_id;
      timer_id = wParam;
      if(timer_id == 5)
      {
        BEEP_ON;
      }
      else if(timer_id == 6)
      {
        BEEP_OFF;
      }
      else if (timer_id == 1)
      {
        beep_flag = !beep_flag;

        if (beep_flag)
        {
          BEEP_ON;
        }
        else
        {
          BEEP_OFF;
        }

        press_flag++;
        press_flag %= 4;

        RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
			}
        
    }break;

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      hdc = BeginPaint(hwnd, &ps);
      
      RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
      RECT rc_title_grad = {0, 25, GUI_XSIZE, 3};
      
      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc);

      rc.h = 25;
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillRect(hdc, &rc);//, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
      
      GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 240, 240, 240), TRUE);
      
      SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
      DrawText(hdc, L"����������", -1, &rc, DT_VCENTER|DT_CENTER);

      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case ID_BEEPER_EXIT:
          {
            ExitButton_OwnerDraw(ds);
            return TRUE;             
          }

          case ID_BEEPER_SW:
          case ID_BEEPER_TICK:
          {
            sw_button_OwnerDraw(ds);
            return TRUE;   
          }

          case ID_BEEPER_PRESS:
          {
            press_button_OwnerDraw(ds);
            return TRUE;   
          }
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    
      if(code == BN_CLICKED)
      {
        if (id == ID_BEEPER_EXIT)
        {
          PostCloseMessage(hwnd);
        }
        else if (id == ID_BEEPER_TICK)
        {
          tick_flag = !tick_flag;    // ������־

          if (tick_flag)
          {
            SetTimer(hwnd, 1, 500, TMR_START, NULL);    // �����δ�ʱ��
          }
          else
          {
            KillTimer(hwnd, 1);    // �رյδ�ʱ��
            BEEP_OFF;              // �رշ�����
            press_flag =3;
          }
          
          sw_flag = 0;    // ��λ��־
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_SW), NULL, TRUE);
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
        }
        else if (id == ID_BEEPER_SW)
        {
          sw_flag = !sw_flag;    // ������־

          if (sw_flag)
          {
            BEEP_ON;    // ��������
          }
          else
          { 
            BEEP_OFF;    // �ط����� 
          }

          KillTimer(hwnd, 1);
          tick_flag = 0;
          press_flag = 3;

          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_TICK), NULL, TRUE);
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
        }
        else if (id == ID_BEEPER_PRESS)
        {
          KillTimer(hwnd, 1);
          BEEP_OFF;    // �ط����� 
          press_flag = 3;
          RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_PRESS), NULL, TRUE);
        }
      }
      
      if(code == BN_PUSHED && id == ID_BEEPER_PRESS)    // ����
      {
        tick_flag = 0;
        sw_flag = 0;

        RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_TICK), NULL, TRUE);
        RedrawWindow(GetDlgItem(hwnd, ID_BEEPER_SW), NULL, TRUE);
        SetTimer(hwnd, 1, 500, TMR_START, NULL);
      }

      break;
    } 

    case WM_DESTROY:
    {
      tick_flag = 0;
      sw_flag = 0;
      press_flag = 3;
      
      BEEP_OFF;
    #if GUI_BEEPER_PIC_EN
      for(uint8_t xC=0; xC<hdc_beeper_end; xC++)
      {
        /* �ͷ� HDC */
        DeleteDC(hdc_beeper_png[xC]);
      }
    #endif
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_Beeper_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_Beeper_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(MAIN_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


