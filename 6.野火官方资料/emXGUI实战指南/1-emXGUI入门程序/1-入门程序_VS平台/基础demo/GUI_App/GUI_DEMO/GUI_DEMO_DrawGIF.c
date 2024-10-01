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

  /*============================================================================*/
  //����ؼ�ID
#define	ID_OK		0x1000
/*============================================================================*/

/*============================================================================*/
/* �ⲿͼƬ���� */
extern char minion_gif[];
extern char king[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int minion_gif_size(void);


/*============================================================================*/

static LRESULT	WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int i = 0, frame_num, delay;
	static HANDLE hgif;
	static IMAGE_INFO img_info;
	HDC hdc_mem;

	RECT rc;
	switch (msg)
	{
	case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���
	{
		GetClientRect(hwnd, &rc);
		/* ��ȡHGIF��� */
		hgif = GIF_Open(king);
		/* ��ȡGIF��ͼƬ��Ϣ */
		GIF_GetInfo(hgif, &img_info);
		/* ��ȡGIF��֡�� */
		frame_num = GIF_GetFrameCount(hgif);
		CreateWindow(BUTTON, L"OK", WS_VISIBLE, rc.w - 70, rc.h - 40, 68, 32, hwnd, ID_OK, NULL, NULL);
		SetTimer(hwnd, 0, 0, TMR_SINGLE, NULL);
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

	case WM_TIMER:
		InvalidateRect(hwnd, NULL, FALSE);
		break;

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
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		if (hgif)
		{
			/* ����MemoryDC */
			hdc_mem = CreateMemoryDC(SURF_SCREEN, img_info.Width, img_info.Height);
			/* �����Ļ��ʾ */
			ClrDisplay(hdc_mem, NULL, MapRGB(hdc_mem, 255, 255, 255));
			if (i >= frame_num)
			{
				i = 0;
			}
			/* ����ͼƬ��MemoryDC������ȡ��ʱֵ */
			delay = GIF_DrawFrame(hdc_mem, 0, 0, MapRGB(hdc_mem, 255, 255, 255), hgif, i);
			BitBlt(hdc, rc.x, rc.y, img_info.Width, img_info.Height, hdc_mem, 0, 0, SRCCOPY); //��MEMDC����������С�
				/* ��ȡ��ǰ֡����ʱֵ */
			//delay = GIF_GetFrameDelay(hgif, i);
			i++;
		}
		/* �ͷ�MemoryDC */
		DeleteDC(hdc_mem);

		ResetTimer(hwnd, 0, delay, TMR_SINGLE | TMR_START, NULL);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
	{
		GIF_Close(hgif);
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

void	GUI_DEMO_DrawGIF(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
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
		_T("DrawGIF"),
		/*WS_MEMSURFACE|*/WS_CAPTION | WS_CLIPCHILDREN,
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
