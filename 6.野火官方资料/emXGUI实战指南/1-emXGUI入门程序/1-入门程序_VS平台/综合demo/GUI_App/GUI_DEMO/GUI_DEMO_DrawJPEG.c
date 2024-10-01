/**
  *********************************************************************
  * @file    GUI_DEMO_DrawJPEG.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��ʾ�ⲿBMPͼ��
  * @note    ʹ��ʱ��Ҫ��FLASH��SD�����ͼƬ�ļ�
  **********************************************************************
  */ 
#include <emXGUI.h>
#include "emXGUI_JPEG.h"

/*============================================================================*/
//����ؼ�ID
#define	ID_OK		0x1000
/*============================================================================*/

/*============================================================================*/
/* �ⲿͼƬ���� */
extern char tiger_jpg[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int tiger_jpg_size(void);


/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  /* �ⲿͼƬ�ߴ��С */
  static U16 pic_width,pic_height;
  static HDC hdc_mem=NULL;
  
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���
		{
      JPG_DEC *dec;
      
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������

      /* ����ͼƬ���ݴ���JPG_DEC��� */
      dec = JPG_Open(tiger_jpg, tiger_jpg_size());
      /* ��ȡͼƬ�ļ���Ϣ */
      JPG_GetImageSize(&pic_width, &pic_height,dec);
      
      /* �жϱ߽� */
      pic_width = pic_width >= GUI_XSIZE ? GUI_XSIZE:pic_width;
      pic_height = pic_height >= GUI_YSIZE ? GUI_YSIZE:pic_height;

      /* �����ڴ���� */
      hdc_mem =CreateMemoryDC(SURF_SCREEN,pic_width,pic_height); 
      
      /* �������ڴ���� */
      JPG_Draw(hdc_mem, 0, 0, dec);
            
      /* �ر�JPG_DEC��� */
      JPG_Close(dec);
      
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
			int x=0,y=0;
      
			hdc =BeginPaint(hwnd,&ps);

			////�û��Ļ�������...
			GetClientRect(hwnd,&rc0);

			for(y=0; y<rc0.h; y+=pic_height)
			{
				for(x=0; x<rc0.w; x+=pic_width)
				{

          /* ���ڴ�����������Ļ */
          BitBlt(hdc,x,y,pic_width,pic_height,hdc_mem,0,0,SRCCOPY); //��MEMDC����������С�

          rc.x=x;
					rc.y=y;
					rc.w=pic_width;
					rc.h=pic_height;
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
      DeleteDC(hdc_mem);

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

void	GUI_DEMO_DrawJPEG(void)
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
								_T("DrawJPEG"),
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
