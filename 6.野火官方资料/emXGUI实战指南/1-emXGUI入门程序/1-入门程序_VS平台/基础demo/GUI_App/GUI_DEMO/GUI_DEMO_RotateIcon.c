
/*
 *GUI_DEMO_MoveRect.c
 *2018年12月22日下午3:34:49
 *
 */

#include "emXGUI.h"
#include "x_libc.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

typedef struct
{
	RECT rc;
	COLOR_RGB32 cr;
}__obj_t;

static __obj_t objs[20];


static int cx=200;

/*============================================================================*/
static void _init(void)
{
	int i;
	RECT rc;

	rc.x =0;
	rc.y =0;
	rc.w =80;
	rc.h =60;

	for(i=0;i<20;i++)
	{
		objs[i].rc =rc;
		objs[i].cr =RGB888(x_rand()&0xFF,x_rand()&0xFF,x_rand()&0xFF);

		OffsetRect(&rc,rc.w+1,0);

	}
}

static void _move(int dx,int dy)
{
	int i,k;
	RECT rc;

	for(i=0;i<20;i++)
	{
		OffsetRect(&objs[i].rc,dx,dy);
	}
}

static void _draw(HDC hdc,HWND hwnd)
{
	int i;
	RECT rc,rc0;
	HDC hdc_win;
	int k;

//	hdc_win =GetWindowDC(hwnd);
//	GetWindowRect(hwnd,&rc0);

	for(i=0;i<8;i++)
	{
		rc =objs[i].rc;

		rc.x += 10;
		rc.y += 50;

		k =(rc.x+rc.w/2)-cx;

		if(k<0)
		{
			k=0-k;
		}
		k /= 6;

		InflateRect(&rc,-k,-k);
		if(rc.w<0) rc.w =0;
		if(rc.h<0) rc.h =0;

		SetBrushColor(hdc,MapRGB888(hdc,objs[i].cr));
		FillRect(hdc,&rc);

		//把窗口贴到矩形里。
//		InflateRect(&rc,-4,-4);
//		StretchBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_win,0,0,rc0.w,rc0.h,SRCCOPY);

	}

//	ReleaseDC(hwnd,hdc_win);

}

/*============================================================================*/
static int temp_x0,temp_y0;
static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			_init();

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////
		case WM_LBUTTONDOWN:
		{
			temp_x0 =GET_LPARAM_X(lParam);
			temp_y0 =GET_LPARAM_Y(lParam);
		}
		break;
		////

		case WM_MOUSEMOVE:
		{
			int key;
			s16 x,y;

			key = wParam;

			if(key&MK_LBUTTON)
			{
				x =GET_LPARAM_X(lParam);
				y =GET_LPARAM_Y(lParam);
				_move(x-temp_x0,0);

				temp_x0 =x;
				temp_y0 =y;
				InvalidateRect(hwnd,NULL,FALSE);
			}

		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			HDC hdc_mem;

			GetClientRect(hwnd,&rc);

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);
			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc,0,0,0));
			_draw(hdc_mem,hwnd);
			BitBlt(hdc,0,0,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			DeleteDC(hdc_mem);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_RotateIcon(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("emXGUI Window"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								10,20,400,300,    //窗口位置和大小
								NULL,0,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
