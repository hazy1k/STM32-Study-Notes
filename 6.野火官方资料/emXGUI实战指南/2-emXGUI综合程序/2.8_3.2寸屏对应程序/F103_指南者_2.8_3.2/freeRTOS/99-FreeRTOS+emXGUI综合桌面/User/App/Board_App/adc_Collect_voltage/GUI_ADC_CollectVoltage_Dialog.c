#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  "GUI_ADC_CollectVoltage_Dialog.h"
#include "bsp_adc.h"

/* ���� ID */
#define ID_ADV_WIN         0x01    // �м���ʾ����ID
#define SCROLLBAR_Brigh_ID 0x02    // ��������ID
#define ID_TEXTBOX_Title   0x03    // ������
#define ID_TEXTBOX_Brigh   0x04    // ���Ȱٷֱ�

#define CircleCenter_1    (46)     // ��������ת�뾶
#define CircleCenter_2    (64)    // Բ���������뾶��С��
#define CircleCenter_3    (CircleCenter_2 + 6)    //  ������ CircleSize / 2

#define CircleSize    140    // Բ����ʾ����Ĵ�С
#define Circle_X      (160)    // Բ����ʾ�����λ��
#define Circle_Y      (130)   // Բ����ʾ�����λ��

#define TitleHeight    28    // �������ĸ߶�

#define TriangleLen    10    // �����εı߳�

uint8_t AovingDirection = 0;
double count = 0.0;
HWND MAIN_Handle;
HWND Title_Handle;
HWND Brigh_Handle;
HWND ADC_Handle;

static BITMAP bm_Triangle;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
double ADC_Vol; 
#if 0
static void	X_MeterPointer(HDC hdc, int cx, int cy, int r, u32 color, double dat_val)
{
  double angle;
  int midpoint_x,midpoint_y;
  POINT pt[4];

  angle = (dat_val * 1.427 );    // ����Ƕ� + 0.785 = 45��
  
  /* ����������ƽ�е�һ�ߵ��е����� */
  midpoint_x =cx - sin(angle) * (r - TriangleLen * 0.866);    // 0.866 = sqrt(3) / 2
  midpoint_y =cy + cos(angle) * (r - TriangleLen * 0.866);

  /* ��Զ��һ���� */
  pt[0].x = cx - r * sin(angle);
  pt[0].y = cy + r * cos(angle);

  /* ��Զһ������һ�� */
  pt[1].x = midpoint_x - (TriangleLen / 2) * sin(angle - 1.57);    // 1.57 = 3.14/2 = ��/2 = 90��
  pt[1].y = midpoint_y + (TriangleLen / 2) * cos(angle - 1.57);

  /* ��Զһ����ұ�һ�� */
  pt[2].x = midpoint_x - (TriangleLen / 2) * sin(angle + 1.57);
  pt[2].y = midpoint_y + (TriangleLen / 2) * cos(angle + 1.57);

  pt[3].x = pt[0].x;
  pt[3].y = pt[0].y;


  /* �������� */
  SetBrushColor(hdc,color);
  EnableAntiAlias(hdc, TRUE);
  FillPolygon(hdc,0,0,pt,4);
  EnableAntiAlias(hdc, FALSE);
}
#endif
//�˳���ť�ػ���
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
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

/*
 * @brief  �ػ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.

  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��

  SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
  FillRect(hdc, &rc);
  
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));

  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}

/* �ػ�Բ����ʾ���� */
void Circle_Paint(HWND hwnd, HDC hdc)
{
  char  cbuf[128];
  WCHAR wbuf[128];
  int vertex_x,vertex_y;
  RECT rc = {Circle_X, Circle_Y, CircleSize, CircleSize};
  int angle = ((225 - (-45))) * ADC_Vol / 3.3;

  EnableAntiAlias(hdc, TRUE);

  RotateBitmap(hdc, Circle_X, Circle_Y, &bm_Triangle, angle + 45);
  
  SetBrushColor(hdc, MapARGB(hdc, 255, 80, 80, 80));
  FillArc(hdc, Circle_X, Circle_Y, CircleCenter_2, CircleCenter_3, -45, 225);
  /* �������СԲȦ������ */
  vertex_x =  CircleSize / 2 - ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * sin(3.14/4); 
  vertex_y =  CircleSize / 2 + ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * cos(3.14/4) + 2; 
  FillCircle(hdc, vertex_x+91, vertex_y+58, (CircleCenter_3 - CircleCenter_2) / 2);     // ���СԲȦ
  
  vertex_x =  CircleSize / 2 + ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * sin(3.14/4) + 1;
  FillCircle(hdc, vertex_x+90, vertex_y+58, (CircleCenter_3 - CircleCenter_2) / 2);     // �ұ�СԲȦ
  
  if (ADC_Vol > 0.03)    // ��ѹ̫С������ɫСԲȦ
  {
    SetBrushColor(hdc, MapARGB(hdc, 255, 250, 250, 250));
    FillArc(hdc, Circle_X, Circle_Y, CircleCenter_2+1, CircleCenter_3-1, -45, angle - 45);
    /* �������СԲȦ������ */
    vertex_x =  CircleSize / 2 - ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * sin(3.14/4); 
    vertex_y =  CircleSize / 2 + ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * cos(3.14/4) + 2; 
    FillCircle(hdc, vertex_x+91, vertex_y+58, (CircleCenter_3 - CircleCenter_2) / 2 - 1);     // ���СԲȦ
  }

  EnableAntiAlias(hdc, FALSE);

  /* ʹ��Ĭ������ */
	SetFont(hdc, defaultFont);

  rc.w = 24*3;
  rc.h = 48;
  rc.x = CircleSize/2 - rc.w/2+90;
  rc.y = CircleSize/2 - rc.h/2+58;

  /* ��ʾ��ѹ�ٷֱ� */
  SetTextColor(hdc, MapARGB(hdc, 255, 0, 0, 0));
  
  HFONT controlFont_32;
	
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
  SetFont(hdc, controlFont_32);
  x_sprintf(cbuf, "%d", (int)(ADC_Vol/3.3*100));    // H -> % x_wsprintf(Backlightwbuf, L"%d", i);
  x_mbstowcs(wbuf, cbuf, 128);
  rc.w -= 20;
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);    // ��������(���ж��뷽ʽ)DT_CENTER
  rc.w += 20;
  rc.y += 4;
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"%", -1, &rc, DT_VCENTER|DT_RIGHT);    // ��������(���ж��뷽ʽ)DT_CENTER

  /* ��ʾ�ɼ����ĵ�ѹֵ */
  SetFont(hdc, defaultFont);
  rc.w = 24*4 - 10;
  rc.h = 30;rc.x -=5; 
  rc.y = CircleSize/2 - rc.h/2 + CircleCenter_3 + 48;
  x_sprintf(cbuf, "%.2fV", ADC_Vol);
  x_mbstowcs(wbuf, cbuf, 128);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
  
  DeleteFont(controlFont_32);
}
 
static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      ADCx_Init();    // ��ʼ�� ADC
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  286, 10, 25, 25, hwnd, eID_ADC_EXIT, NULL, NULL); 

      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0;

      SetTimer(hwnd, 2, 20, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      int tmr_id = wParam;

      if (tmr_id == 2)
      {
//        RECT rc;
        static double ADC_buff = 0.0;
        double vol_buff = 0.0;
        static uint8_t xC = 0;
        vol_buff =(double) ADC_ConvertedValue/4096*(double)3.3; // ��ȡת����ADֵ
//        GUI_DEBUG("��ѹֵǰΪ��%f", ADC_Vol);
        #if 1

          if (xC++ < 10)
          {
            ADC_buff += vol_buff;
            break;
          }
          else
          {
            ADC_Vol = ADC_buff / ((double)(xC-1));
            ADC_buff = 0;
            xC = 0;
          }

        #else
          
        ADC_Vol = (double)(((int)(vol_buff * 10)) / 10.0);

        #endif
//        GUI_DEBUG("��ѹֵ��Ϊ��%f", ADC_Vol);
        
//        rc.x = Circle_X;
//        rc.y = Circle_Y;
//        rc.w = CircleSize * 2;
//        rc.h = CircleSize * 2;
        InvalidateRect(hwnd, NULL, FALSE);

      }
      break;
    }
    case WM_ERASEBKGND:
    {
      
      HDC hdc =(HDC)wParam;
      
      RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
      RECT rc_title_grad = {0, 35, GUI_XSIZE, 3};
      
      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc);

      rc.h = 35;
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillRect(hdc, &rc);//, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
      
      GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 240, 240, 240), TRUE);
      
      SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
      DrawText(hdc, L"ADC ��ѹ�ɼ�", -1, &rc, DT_VCENTER|DT_CENTER);
      

      return FALSE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc = {160, 130, CircleSize, CircleSize};

      hdc = BeginPaint(hwnd, &ps);

      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillCircle(hdc, 160,130, CircleSize/2);

      Circle_Paint(hwnd, hdc);    // ���Ƶ�һ�������е�Բ����ʾ����
      
      EndPaint(hwnd, &ps);

      break;
    } 
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

          case ID_TEXTBOX_Title:
          {
            Textbox_OwnerDraw(ds);
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
      if(code == BN_CLICKED && id == eID_ADC_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_LBUTTONUP:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_MOUSEMOVE:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);
    }

    case WM_LBUTTONDOWN:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_DESTROY:
    {
      stopADC();    // ֹͣADC�Ĳɼ�
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_ADC_CollectVoltage_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CollectVoltage_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
    
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
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


