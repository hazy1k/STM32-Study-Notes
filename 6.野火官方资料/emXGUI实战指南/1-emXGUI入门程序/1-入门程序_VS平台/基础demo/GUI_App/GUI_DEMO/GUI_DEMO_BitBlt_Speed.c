


#include "emXGUI.h"
#include "x_libc.h"
/*============================================================================*/

//extern U32 GUI_GetTickCount(void);
//extern void GUI_msleep(int ms);

extern const unsigned char acbmp_rgb888[];

//����ؼ�ID
#define	ID_OK		0x1000

/*============================================================================*/
static int step=0;
static int t0,t;
static long NumPixels;
static HDC hdc_mem;
static RECT r;

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //����һ����ť(ʾ��).
			r.x =0;
			r.y =0;
			r.w =128;
			r.h =80;

			hdc_mem =CreateMemoryDC(SURF_SCREEN,r.w,r.h);

			SetTextColor(hdc_mem,MapRGB(hdc_mem,255,0,0));
			SetPenColor(hdc_mem,MapRGB(hdc_mem,0,0,255));
			SetBrushColor(hdc_mem,MapRGB(hdc_mem,255,255,255));

			DrawText(hdc_mem,L"BitBlt",-1,&r,DT_VCENTER|DT_CENTER|DT_BORDER|DT_BKGND);

			step=0;
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_OK && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
		}
		break;
		////


		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			int XSize,YSize;
			WCHAR wbuf[128];


			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			GetClientRect(hwnd,&rc);
			XSize =rc.w;
			YSize =rc.h;

			if(step==0)
			{
				step=1;
				NumPixels =0;

				SetBrushColor(hdc,MapRGB(hdc,0,0,0));
				GetClientRect(hwnd,&rc);
				FillRect(hdc,&rc);

				t0 =GUI_GetTickCount();
			}

			if(step==1)
			{
				r.x =x_rand()%XSize;
				r.y =x_rand()%YSize;

				r.x =MIN(r.x,XSize-r.w);
				r.y =MIN(r.y,YSize-r.h);

				BitBlt(hdc,r.x,r.y,r.w,r.h,hdc_mem,0,0,SRCCOPY);

				NumPixels += (r.w) * (r.h);
				t =GUI_GetTickCount();

				if((t-t0) > 5000)
				{
					t =(t-t0)/100;
					step =2;

				}
			}

			if(step==2)
			{

				SetTextColor(hdc,MapRGB(hdc,255,255,255));
				SetBrushColor(hdc,MapRGB(hdc,10,10,100));

				GetClientRect(hwnd,&rc);
				FillRect(hdc,&rc);

				x_wsprintf(wbuf,L"BitBlt Speed\r\n"
						        L"Pixels/Sec: %d",10 * (NumPixels / t));
				DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);

			}

			EndPaint(hwnd,&ps); //������ͼ

			if(step < 2)
			{//
				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{

			DeleteDC(hdc_mem);
			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
		break;
		////

		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_BitBlt_Speed(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//����������
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
								&wcex,
								_T("emXGUI DEMO - BitBlt Speed"), //��������
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //����λ�úʹ�С
								NULL,NULL,NULL,NULL);

	//��ʾ������
	ShowWindow(hwnd,SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
#if 0
void GUI_DEMO_BMP_Speed_OnDesktop(void)
{

  WCHAR buf[128];
  HDC hdc;
  HWND hwnd;
  int t,t1;
  int i;
  int XSize;
  int YSize;
  long NumPixels;
  RECT rc;
  BITMAPINFO bm_info;

  hwnd	=GetDesktopWindow();
  hdc	=GetDC(hwnd);

  XSize =GUI_XSIZE;
  YSize =GUI_YSIZE;
  NumPixels=0;


	//GUI_Printf("GUIDEMO_RectSpeed\r\n");

    BMP_GetInfo(&bm_info,acbmp_rgb888);
	t = GUI_GetTickCount();
	i = 0;
	while(1)
	{
		RECT r;

		t1 =GUI_GetTickCount();
		if((t1-t) >= 5000)
		{
			break;
		}

		r.x =x_rand()%XSize;
		r.y =x_rand()%YSize;
		r.w =bm_info.Width;
		r.h =bm_info.Height;

		r.x =MIN(r.x,XSize-r.w);
		r.y =MIN(r.y,YSize-r.h);

		BMP_Draw(hdc,r.x,r.y,acbmp_rgb888,NULL);

		NumPixels += (r.w) * (r.h);
		i++;

	}


	t = (t1 - t) / 100;
	ClrDisplay(hdc,NULL,MapRGB(hdc,0,100,200));
	GUI_msleep(200);
	if(t==0)
	{
		t++;
	}

	SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetClientRect(hwnd,&rc);

	x_wsprintf(buf,L"BMP-24bit Speed\r\n"
			       L"%d Pixels/Sec",10 * (NumPixels / t));
	DrawText(hdc,buf,-1,&rc,DT_VCENTER|DT_CENTER);
	ReleaseDC(hwnd,hdc);

    GUI_msleep(3000);
	InvalidateRect(hwnd,NULL,TRUE);
	GUI_msleep(100);
}
#endif
