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
//#define DEMO_BMP_NAME  "0:srcdata/timg.bmp"
/* Ϊ1ʱ��ʾRES FLASH��Դ�ļ���Ϊ0ʱ��ʾSD�����ļ� */
#define RES_PIC_DEMO    1

#if(RES_PIC_DEMO)
  /* FLASH��Դ�ļ����� */
#define DEMO_MBP_FILE_NAME  "tiger.bmp"

#else
  /* SD�ļ�ϵͳ���� */
#define DEMO_MBP_FILE_NAME  "../GUI_App/GUI_DEMO/res/tiger.bmp"

#endif

/*============================================================================*/


/*============================================================================*/

static LRESULT	WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	static HDC hdc_mem = NULL;
	switch (msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���
		{
			HWND wnd;
			GetClientRect(hwnd, &rc); //��ô��ڵĿͻ�������	
			wnd = CreateWindow(BUTTON, L"OK", WS_VISIBLE,
								rc.w - 70, rc.h - 40, 68, 32, hwnd, ID_OK, NULL, NULL);
#if(RES_PIC_DEMO)
			/* ��ȡ�ļ�ϵͳ�е�ͼƬ��Ϣ*/
			PIC_BMP_GetInfo_Res(&bm_0, DEMO_MBP_FILE_NAME);

			/* �жϱ߽� */
			bm_0.Width = bm_0.Width >= GUI_XSIZE ? GUI_XSIZE : bm_0.Width;
			bm_0.Height = bm_0.Height >= GUI_YSIZE ? GUI_YSIZE : bm_0.Height;

			/* �����ڴ���� */
			hdc_mem = CreateMemoryDC(SURF_SCREEN, bm_0.Width, bm_0.Height);
			/* �������ڴ���� */
			PIC_BMP_Draw_Res(hdc_mem, 0, 0, DEMO_MBP_FILE_NAME, NULL);
#else
			/* ��ȡ�ļ�ϵͳ�е�ͼƬ��Ϣ*/
			PIC_BMP_GetInfo_FS(&bm_0, DEMO_MBP_FILE_NAME);

			/* �жϱ߽� */
			bm_0.Width = bm_0.Width >= GUI_XSIZE ? GUI_XSIZE : bm_0.Width;
			bm_0.Height = bm_0.Height >= GUI_YSIZE ? GUI_YSIZE : bm_0.Height;

			/* �����ڴ���� */
			hdc_mem = CreateMemoryDC(SURF_SCREEN, bm_0.Width, bm_0.Height);
			/* �������ڴ���� */
			PIC_BMP_Draw_FS(hdc_mem, 0, 0, DEMO_MBP_FILE_NAME, NULL);

#endif
			return TRUE;
		}

	case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
	{
		u16 code, id;
		code = HIWORD(wParam); //���֪ͨ������.
		id = LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.
		if (id == ID_OK && code == BN_CLICKED)
		{
			PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�رմ���
		}
		break;
	}
	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		GetClientRect(hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 0, 30, 130));
		FillRect(hdc, &rc);
		return TRUE;
	}
	case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc0;
		int x, y;
		hdc = BeginPaint(hwnd, &ps);//��ʼ����
		////�û��Ļ�������...
		GetClientRect(hwnd, &rc0);
		SetPenColor(hdc, MapRGB(hdc, 200, 200, 220));
		for (y = 0; y < rc0.h; y += bm_0.Height)
		{
			for (x = 0; x < rc0.w; x += bm_0.Width)
			{
				/* ��ʾ�ļ�ϵͳ�е�ͼƬ�ļ� */
				BitBlt(hdc, x, y, bm_0.Width, bm_0.Height, hdc_mem, 0, 0, SRCCOPY); //��MEMDC����������С�
				rc.x = x;
				rc.y = y;
				rc.w = bm_0.Width;
				rc.h = bm_0.Height;
				DrawRect(hdc, &rc);
			}
		}
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
	{
		DeleteDC(hdc_mem);

		return PostQuitMessage(hwnd); //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
	}

	default:
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_Drawbitmap_Extern(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	hwnd = CreateWindowEx(NULL,
		&wcex,
		_T("DrawBitmap_Extern"),
		/*WS_MEMSURFACE|*/WS_CAPTION | WS_BORDER | WS_CLIPCHILDREN,
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

/*============================================================================*/
