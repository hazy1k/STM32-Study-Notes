
/*
 *GUI_DEMO_ScaleBitmap.c
 *2018��6��30������3:58:32
 *
 */

#include <emXGUI.h>
#include "x_libc.h"
/*============================================================================*/
//����ؼ�ID
#define	ID_OK		0x1000


/*============================================================================*/

static HDC hdc_mem=NULL;
static int time=0;
static int x,y,w,h;
/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			time=0;
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.

			hdc_mem =CreateMemoryDC(SURF_XRGB8888,128,80);

			x=10;
			y=50;
			w=200;
			h=100;
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //����һ����ť(ʾ��).
			SetTimer(hwnd,1,1000,TMR_START,NULL);
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
		case WM_TIMER:
		{
			time++;
			InvalidateRect(hwnd,NULL,FALSE);
		}
		break;

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			WCHAR wbuf[48];
			BITMAP bm;

			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...
			//TextOut(hdc,10,10,L"Hello",-1);
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,210,210,210));
			rc.x=0;
			rc.y=0;
			rc.w=128;
			rc.h=80;

			SetPenColor(hdc_mem,MapRGB(hdc_mem,10,10,100));
			SetBrushColor(hdc_mem,MapRGB(hdc_mem,100,100,200));
			SetTextColor(hdc_mem,MapRGB(hdc_mem,250,0,0));
			x_wsprintf(wbuf,L"TIME\r\n[%04d]",time);
			DrawText(hdc_mem,wbuf,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

			DCtoBitmap(hdc_mem,&bm);
			ScaleBitmap(hdc,x,y,w,h,&bm);

			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
		////

		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{
			DeleteDC(hdc_mem);
			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
//		break;
		////

		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_ScaleBitmap(void)
//static void	AppMain(void)
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
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_DEMO: ScaleBitmap"), //��������
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,400,300,    //����λ�úʹ�С
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
