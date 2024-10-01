/**
  *********************************************************************
  * @file    GUI_DEMO_Drawbitmap.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��ʾ�ⲿBMPͼ��
  * @note    ʹ��ʱ��Ҫ��FLASH��SD�����ͼƬ�ļ�
  **********************************************************************
  */ 
#include <emXGUI.h>

/*============================================================================*/
//����ؼ�ID
#define	ID_OK		0x1000
/*============================================================================*/

extern const unsigned char gImage_0[];
/*============================================================================*/
static BITMAPINFO bm_0;

#define DEMO_BMP_NAME  "0:srcdata/tiger.bmp"

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������

			//����λͼ�ṹ����

      /* ��ȡFLASH�е�ͼƬ�ļ���Ϣ */
//      PIC_BMP_GetInfo_rawFlash(&bm_0, DEMO_BMP_NAME);
      /* ��ȡ�ļ�ϵͳ�е�ͼƬ��Ϣ*/
      PIC_BMP_GetInfo_FS(&bm_0, DEMO_BMP_NAME);
      
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-70,rc.h-40,68,32,hwnd,ID_OK,NULL,NULL);
			//SetTimer(hwnd,0,50,TMR_START,NULL);

		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_OK && code==BN_CLICKED)
			{
				PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�رմ���
			}
		}
		break;
		////
		case WM_TIMER:

			InvalidateRect(hwnd,&rc,FALSE);
			break;

		case WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0,30,130));
			FillRect(hdc,&rc);
		}
		return TRUE;
		/////

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
//			WCHAR wbuf[128];
			RECT rc0;
			int x,y;


			hdc =BeginPaint(hwnd,&ps);

			////�û��Ļ�������...
			GetClientRect(hwnd,&rc0);

			SetPenColor(hdc,MapRGB(hdc,200,200,220));
			for(y=0; y<rc0.h; y+=bm_0.Height)
			{
				for(x=0; x<rc0.w; x+=bm_0.Width)
				{

          /* ��ʾFLASH�е�ͼƬ�ļ� */
//          PIC_BMP_Draw_rawFlash(hdc,x,y,DEMO_BMP_NAME,NULL);
          /* ��ʾ�ļ�ϵͳ�е�ͼƬ�ļ� */
					PIC_BMP_Draw_FS(hdc,x,y,DEMO_BMP_NAME,NULL);

          rc.x=x;
					rc.y=y;
					rc.w=bm_0.Width;
					rc.h=bm_0.Height;
					DrawRect(hdc,&rc);
				}
			}



			EndPaint(hwnd,&ps);
			//////////
/*
			rc.x=10;
			rc.y=10;
			rc.w=100;
			rc.h=200;
			InvalidateRect(hwnd,&rc,FALSE);
*/
		}
		break;
		////

		case WM_DESTROY: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
		{
			return PostQuitMessage(hwnd); //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
		}
//		break;
		////

		default:
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_Drawbitmap_extern(void)
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
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
//	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	//����������
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("DrawBitmap_Extern"),
								/*WS_MEMSURFACE|*/WS_CAPTION|WS_BORDER|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,
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
