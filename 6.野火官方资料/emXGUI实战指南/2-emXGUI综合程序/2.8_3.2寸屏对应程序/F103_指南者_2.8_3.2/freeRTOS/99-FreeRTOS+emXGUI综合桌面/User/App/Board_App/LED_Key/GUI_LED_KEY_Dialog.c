#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./led/bsp_led.h"  
#include "./key/bsp_key.h" 
 
/* ��ť ID */
enum
{
  eID_LED_KEY_EXIT  = 0x1001,
  eID_LED_USER,
  eID_LED_ONOFF,
  eID_LED_KEY
};

static uint8_t LED_ENTER_TOG=0;//��RBG���Զ���˸��־
static uint8_t LED1_ON_FLAG=0,LED2_ON_FLAG=0,LED3_ON_FLAG=0;//��ͬLED��ˢͼ��־

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

static void LED_KEY_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if ( ds->State & BST_PUSHED )
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

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      Key_GPIO_Config();//��ʼ������
      RECT rc;
      GetClientRect(hwnd, &rc); 

			CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									286, 10, 23, 23, hwnd , eID_LED_KEY_EXIT, NULL, NULL); 
			CreateWindow(BUTTON, L"USER", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									26,  198, 71, 30, hwnd, eID_LED_USER, NULL, NULL); 
			CreateWindow(BUTTON, L"KEY1", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									123, 197, 71, 30, hwnd, eID_LED_ONOFF, NULL, NULL); 
			CreateWindow(BUTTON, L"KEY2", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									221, 198, 71, 30, hwnd, eID_LED_KEY, NULL, NULL);  

      SetTimer(hwnd, 0, 20, TMR_START, NULL);
      SetTimer(hwnd, 1, 300, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      uint16_t timer_id;
      timer_id = wParam;
			
      if(timer_id == 0)
      {
        if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN))
        {
					LED2_ON_FLAG++;
					LED2_TOGGLE;
					InvalidateRect(hwnd,NULL,FALSE);
        }
				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN))
        {
					LED3_ON_FLAG++;
          LED3_TOGGLE;
					InvalidateRect(hwnd,NULL,FALSE);
        }
      }
      else if (timer_id == 1)
      {
        LED_ENTER_TOG++;
        switch (LED_ENTER_TOG)
        {
        case 1:
        {
					LED1_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED1_TOGGLE;
        }break;

        case 2:
        {
					LED1_ON_FLAG++;
					LED2_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED1_TOGGLE;
          LED2_TOGGLE;
        }break;

        case 3:
        {
					LED2_ON_FLAG++;
					LED3_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED2_TOGGLE;
          LED3_TOGGLE;
        }break;

        case 4:
        {
					LED3_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED3_TOGGLE;
        }break;
        
        default:
          KillTimer(hwnd,timer_id);
          LED_ENTER_TOG = 0;
        }break;
      }
      
      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      RECT rc = {50, 0, 220, 40};
      RECT rc_red   = {38, 96, 48, 48};
      RECT rc_green = {135, 96, 48, 48};
      RECT rc_blue  = {233, 96, 48, 48};
      PAINTSTRUCT ps;
      hdc = BeginPaint(hwnd, &ps);
      
      RECT rc_title = {0, 0, GUI_XSIZE, 40};
      RECT rc_title_grad = {0, 40, GUI_XSIZE,3};
      RECT rc_lyric = {0, 40, GUI_XSIZE, 200};
//      RECT rc_control = {0, 396, GUI_XSIZE, 84};
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillRect(hdc, &rc_title);
//         GradientFillRect(hdc, &rc_title, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);

      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
      DrawText(hdc, L"LED&KEY����", -1, &rc_title, DT_VCENTER|DT_CENTER);

      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc_lyric);
      GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 220, 220, 220), TRUE);

//      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
//      FillRect(hdc, &rc_control);
      
			if(LED1_ON_FLAG >= 2)
			{
				LED1_ON_FLAG =0;
			}
			if(LED2_ON_FLAG >= 2)
			{
				LED2_ON_FLAG =0;
			}
			if(LED3_ON_FLAG >= 2)
			{
				LED3_ON_FLAG =0;
			}
      
      HFONT controlFont_48;
      controlFont_48 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_48);
      SetFont(hdc, controlFont_48);
			
			if(LED1_ON_FLAG ==1)
			{
				SetTextColor(hdc, MapRGB(hdc, 255, 0, 0));
			}
      else
			{
				SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
			}
      
      DrawText(hdc, L"I", -1, &rc_red, NULL);//��������(���ж��뷽ʽ)
			
			if(LED2_ON_FLAG ==1)
			{
				SetTextColor(hdc, MapRGB(hdc, 0, 255, 0));
			}
      else
			{
				SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
			}
      
      DrawText(hdc, L"I", -1, &rc_green, NULL);//��������(���ж��뷽ʽ)
			
			if(LED3_ON_FLAG ==1)
			{
				SetTextColor(hdc, MapRGB(hdc, 0, 0, 255));
			}
      else
			{
				SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
			}
      
      DrawText(hdc, L"I", -1, &rc_blue, NULL);//��������(���ж��뷽ʽ)
      
			DeleteFont(controlFont_48);
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_LED_KEY_EXIT:
          {
            LED_KEY_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  
          
          case eID_LED_USER:
          case eID_LED_KEY:
          case eID_LED_ONOFF:
          {
            btn_owner_draw(ds);
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
      if(code == BN_CLICKED && id == eID_LED_KEY_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }
     if(code == BN_CLICKED && id == eID_LED_USER)
      {
				LED1_ON_FLAG++;
        LED1_TOGGLE;//��ɫ�Ʒ�ת
				InvalidateRect(hwnd,NULL,FALSE);
        break;
      }
      if(code == BN_CLICKED && id == eID_LED_ONOFF)
      {
				LED2_ON_FLAG++;
        LED2_TOGGLE;//��ɫ�Ʒ�ת
				InvalidateRect(hwnd,NULL,FALSE);
        break;
      }
			
			if(code == BN_CLICKED && id == eID_LED_KEY)
      {
				LED3_ON_FLAG++;
				LED3_TOGGLE;
				InvalidateRect(hwnd,NULL,FALSE);
        break;
      }
      break;
    } 

    case WM_DESTROY:
    {
			LED_RGBOFF;
			LED1_ON_FLAG=0;
			LED2_ON_FLAG=0;
			LED3_ON_FLAG=0;
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_LED_KEY_Dialog(void)
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
                              L"GUI_LED_KEY_Dialog",
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

