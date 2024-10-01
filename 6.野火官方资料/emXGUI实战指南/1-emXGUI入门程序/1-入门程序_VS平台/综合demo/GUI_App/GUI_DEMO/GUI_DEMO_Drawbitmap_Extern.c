/**
  *********************************************************************
  * @file    GUI_DEMO_Drawbitmap.c
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

extern const unsigned char gImage_0[];
/*============================================================================*/
static BITMAPINFO bm_0;
//#define DEMO_BMP_NAME  "0:srcdata/timg.bmp"
/* 为1时显示RES FLASH资源文件，为0时显示SD卡的文件 */
#define RES_PIC_DEMO    1

#if(RES_PIC_DEMO)
  /* FLASH资源文件加载 */
#define DEMO_MBP_FILE_NAME  "tiger.bmp"

#else
  /* SD文件系统加载 */
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
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
		{
			HWND wnd;
			GetClientRect(hwnd, &rc); //获得窗口的客户区矩形	
			wnd = CreateWindow(BUTTON, L"OK", WS_VISIBLE,
								rc.w - 70, rc.h - 40, 68, 32, hwnd, ID_OK, NULL, NULL);
#if(RES_PIC_DEMO)
			/* 读取文件系统中的图片信息*/
			PIC_BMP_GetInfo_Res(&bm_0, DEMO_MBP_FILE_NAME);

			/* 判断边界 */
			bm_0.Width = bm_0.Width >= GUI_XSIZE ? GUI_XSIZE : bm_0.Width;
			bm_0.Height = bm_0.Height >= GUI_YSIZE ? GUI_YSIZE : bm_0.Height;

			/* 创建内存对象 */
			hdc_mem = CreateMemoryDC(SURF_SCREEN, bm_0.Width, bm_0.Height);
			/* 绘制至内存对象 */
			PIC_BMP_Draw_Res(hdc_mem, 0, 0, DEMO_MBP_FILE_NAME, NULL);
#else
			/* 读取文件系统中的图片信息*/
			PIC_BMP_GetInfo_FS(&bm_0, DEMO_MBP_FILE_NAME);

			/* 判断边界 */
			bm_0.Width = bm_0.Width >= GUI_XSIZE ? GUI_XSIZE : bm_0.Width;
			bm_0.Height = bm_0.Height >= GUI_YSIZE ? GUI_YSIZE : bm_0.Height;

			/* 创建内存对象 */
			hdc_mem = CreateMemoryDC(SURF_SCREEN, bm_0.Width, bm_0.Height);
			/* 绘制至内存对象 */
			PIC_BMP_Draw_FS(hdc_mem, 0, 0, DEMO_MBP_FILE_NAME, NULL);

#endif
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
		SetBrushColor(hdc, MapRGB(hdc, 0, 30, 130));
		FillRect(hdc, &rc);
		return TRUE;
	}
	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc0;
		int x, y;
		hdc = BeginPaint(hwnd, &ps);//开始绘制
		////用户的绘制内容...
		GetClientRect(hwnd, &rc0);
		SetPenColor(hdc, MapRGB(hdc, 200, 200, 220));
		for (y = 0; y < rc0.h; y += bm_0.Height)
		{
			for (x = 0; x < rc0.w; x += bm_0.Width)
			{
				/* 显示文件系统中的图片文件 */
				BitBlt(hdc, x, y, bm_0.Width, bm_0.Height, hdc_mem, 0, 0, SRCCOPY); //将MEMDC输出到窗口中。
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
	case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
	{
		DeleteDC(hdc_mem);

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

void	GUI_DEMO_Drawbitmap_Extern(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

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
		_T("DrawBitmap_Extern"),
		/*WS_MEMSURFACE|*/WS_CAPTION | WS_BORDER | WS_CLIPCHILDREN,
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
