
/*
 *GUI_DEMO_LogoIconFont.c
 *2018年3月17日下午7:41:26
 *
 */

#include "emXGUI.h"

 /*============================================================================*/
 //定义控件ID
#define	ID_OK		0x1000
/*============================================================================*/

/*============================================================================*/

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	switch (msg)
	{
	case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
	{
		GetClientRect(hwnd, &rc); //获得窗口的客户区矩形

		CreateWindow(BUTTON, L"OK", WS_VISIBLE, rc.w - 70, rc.h - 40, 68, 32, hwnd, ID_OK, NULL, NULL);
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
	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;

		GetClientRect(hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 215, 61, 50));
		FillRect(hdc, &rc);
		return TRUE;
	}
	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		//			WCHAR wbuf[128];
		RECT rc0;

		hdc = BeginPaint(hwnd, &ps);

		////用户的绘制内容...
		GetClientRect(hwnd, &rc0);

		SetPenColor(hdc, MapRGB(hdc, 200, 200, 220));

		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));

		/* 使用图标字体*/
		SetFont(hdc, logoFont);
		DrawText(hdc, L" A B C ", -1, &rc0, DT_LEFT);

		/* 使用控制图标字体*/
		SetFont(hdc, controlFont);
		SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
		rc0.y += 80;
		DrawText(hdc, L" A B C D E F G H \r\n I J K L M N O P", -1, &rc0, DT_LEFT);

		/* 使用图标字体*/
		SetFont(hdc, iconFont);
		SetTextColor(hdc, MapRGB(hdc, 238, 142, 0));
		rc0.y += 120;
		DrawText(hdc, L" A B C D E ", -1, &rc0, DT_LEFT);

		SetTextColor(hdc, MapRGB(hdc, 100, 149, 237));
		rc0.y += 120;
		DrawText(hdc, L" F G H I J", -1, &rc0, DT_LEFT);

		/* 恢复默认字体 */
		SetFont(hdc, defaultFont);
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));

		EndPaint(hwnd, &ps);
		break;
	}

	case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
	{
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

void	GUI_DEMO_LogoIconFont(void)
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
//	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	//创建主窗口
	hwnd = CreateWindowEx(NULL,
		&wcex,
		_T("LogoIconFont"),
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
