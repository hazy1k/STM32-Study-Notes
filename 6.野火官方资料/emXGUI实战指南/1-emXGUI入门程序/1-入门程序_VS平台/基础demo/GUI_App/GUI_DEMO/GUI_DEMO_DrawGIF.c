/**
  *********************************************************************
  * @file    GUI_DEMO_DrawJPEG.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   显示外部BMP图像，
  * @note    使用时需要在FLASH或SD卡存放图片文件
  **********************************************************************
  */
#include <emXGUI.h>

  /*============================================================================*/
  //定义控件ID
#define	ID_OK		0x1000
/*============================================================================*/

/*============================================================================*/
/* 外部图片数据 */
extern char minion_gif[];
extern char king[];
/* 外部图片数据大小 */
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
	case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
	{
		GetClientRect(hwnd, &rc);
		/* 获取HGIF句柄 */
		hgif = GIF_Open(king);
		/* 获取GIF的图片信息 */
		GIF_GetInfo(hgif, &img_info);
		/* 获取GIF的帧数 */
		frame_num = GIF_GetFrameCount(hgif);
		CreateWindow(BUTTON, L"OK", WS_VISIBLE, rc.w - 70, rc.h - 40, 68, 32, hwnd, ID_OK, NULL, NULL);
		SetTimer(hwnd, 0, 0, TMR_SINGLE, NULL);
		return TRUE;
	}

	case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
	{
		u16 code, id;
		code = HIWORD(wParam); //获得通知码类型.
		id = LOWORD(wParam); //获得产生该消息的控件ID.
		if (id == ID_OK && code == BN_CLICKED)
		{
			PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭窗口
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
	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		if (hgif)
		{
			/* 创建MemoryDC */
			hdc_mem = CreateMemoryDC(SURF_SCREEN, img_info.Width, img_info.Height);
			/* 清除屏幕显示 */
			ClrDisplay(hdc_mem, NULL, MapRGB(hdc_mem, 255, 255, 255));
			if (i >= frame_num)
			{
				i = 0;
			}
			/* 绘制图片至MemoryDC，并获取延时值 */
			delay = GIF_DrawFrame(hdc_mem, 0, 0, MapRGB(hdc_mem, 255, 255, 255), hgif, i);
			BitBlt(hdc, rc.x, rc.y, img_info.Width, img_info.Height, hdc_mem, 0, 0, SRCCOPY); //将MEMDC输出到窗口中。
				/* 获取当前帧的延时值 */
			//delay = GIF_GetFrameDelay(hgif, i);
			i++;
		}
		/* 释放MemoryDC */
		DeleteDC(hdc_mem);

		ResetTimer(hwnd, 0, delay, TMR_SINGLE | TMR_START, NULL);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
	{
		GIF_Close(hgif);
		return PostQuitMessage(hwnd); //调用PostQuitMessage，使用主窗口结束并退出消息循环.
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
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);


	//创建主窗口
	hwnd = CreateWindowEx(NULL,
		&wcex,
		_T("DrawGIF"),
		/*WS_MEMSURFACE|*/WS_CAPTION | WS_CLIPCHILDREN,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
