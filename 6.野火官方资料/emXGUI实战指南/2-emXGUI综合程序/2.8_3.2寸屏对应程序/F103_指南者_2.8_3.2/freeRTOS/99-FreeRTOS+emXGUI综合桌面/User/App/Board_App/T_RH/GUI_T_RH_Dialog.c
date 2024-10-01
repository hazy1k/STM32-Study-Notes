#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./T_RH/dht11/bsp_dht11.h"

/* ͼƬ��Դ */

/* ���� ID */
#define ID_TEXTBOX_Title    0x00     // ������
#define ID_TEXTBOX_T        0x01     // �¶���ʾ
#define ID_TEXTBOX_RH       0x02     // ʪ����ʾ

/* ��ť ID */
#define eID_T_RH_EXIT    3

#define TitleHeight    70    // �������ĸ߶�

DHT11_Data_TypeDef DHT11_Data;

HWND T_Handle;
HWND RH_Handle;

uint8_t Pointerstyle = 0;

//�˳���ť�ػ���
static void T_RH_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
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

/*
 * @brief  �ػ���ʾ���ȵ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
//static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
//{
//	HWND hwnd;
//	HDC hdc;
//	RECT rc, rc_tmp;
//	WCHAR wbuf[128];

//	hwnd = ds->hwnd; //button�Ĵ��ھ��.
//	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
//  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
//  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);
//  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
//  rc.w -= 45;
//  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
//  SetFont(hdc, controlFont_32);
//  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
//}

/*
 * @brief  ������������Ϣ����Ļص�����
 * @param  ds:	
 * @retval NONE
*/
static LRESULT	Collect_DTH11_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 

      /* DHT11��ʼ�� */
	    DHT11_Init();
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  286, 10, 23, 23, hwnd, eID_T_RH_EXIT, NULL, NULL); 

      SetTimer(hwnd, 0, 2000, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
			/* ��F429�ж�ȡ�¶Ⱥ�����Ϊ:Read_DHT11,��������F103�Ե���Ϊ:DHT11_Read_TempAndHumidity */
      if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
      {
//				GUI_DEBUG("SUCCESS\r\n");
        InvalidateRect(hwnd, NULL, FALSE);    // �ػ洰��
      }
      else
      {
				GUI_ERROR("ERROR\r\n");
				//printf("��ȡ���ݳ���");
        /* ������ */
      } 

      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      WCHAR wbuf[128];
      RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
      RECT rc_title_grad = {0, 35, GUI_XSIZE, 5};

      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc);

      rc.h = 35;
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillRect(hdc, &rc);//, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
      
      GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 240, 240, 240), TRUE);
      
      SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
      DrawText(hdc, L"��ʪ�Ȳɼ�", -1, &rc, DT_VCENTER|DT_CENTER);
      
      /* �¶���ֵ��ʾ */
      rc.w = 64;
      rc.h = 34;
      rc.x = 160;
      rc.y = 73;
      //x_wsprintf(wbuf, L"%d.%d",99,9);
      x_wsprintf(wbuf, L"%d.%d", DHT11_Data.temp_int,DHT11_Data.temp_deci);
      SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
      HFONT controlFont_32;
			controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
      SetFont(hdc, controlFont_32);
      
			rc.w -= 13;
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)
      SetFont(hdc, defaultFont);
      rc.w += 13;rc.x = 80;rc.y += 8;
      DrawText(hdc, L"�¶�:", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
      rc.x = 180;
      DrawText(hdc, L"��", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)
      
      rc.y += 40;
      /* ��ʾʪ����ֵ */
      rc.x = 180;
      rc.w -= 35;      
      SetFont(hdc, controlFont_32);
      rc.w -= 9;
      x_wsprintf(wbuf, L"%d", DHT11_Data.humi_int);//.%d//,DHT11_Data.humi_deci
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)
      SetFont(hdc, defaultFont);
      rc.w += 9;
			
			rc.x = 80;
      DrawText(hdc, L"ʪ��:", -1, &rc, DT_VCENTER|DT_LEFT);//��������(���ж��뷽ʽ)
//      rc.y += 8;
		
			rc.x = 215;
      DrawText(hdc, L"%", -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)
      
      DeleteFont(controlFont_32); 
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_T_RH_EXIT:
          {
            T_RH_ExitButton_OwnerDraw(ds);
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
      if(code == BN_CLICKED && id == eID_T_RH_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_DESTROY:
    {
      memset(&DHT11_Data, 0, sizeof(DHT11_Data));

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_T_RH_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Collect_DTH11_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
  
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_T_RH_Dialog",
                              WS_CLIPCHILDREN,
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


