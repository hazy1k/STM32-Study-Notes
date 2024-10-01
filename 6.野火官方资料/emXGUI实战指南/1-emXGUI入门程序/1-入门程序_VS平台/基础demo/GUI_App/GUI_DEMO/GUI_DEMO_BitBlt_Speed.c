


#include "emXGUI.h"
#include "x_libc.h"
/*============================================================================*/

//extern U32 GUI_GetTickCount(void);
//extern void GUI_msleep(int ms);

extern const unsigned char acbmp_rgb888[];

//定义控件ID
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
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
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


		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			int XSize,YSize;
			WCHAR wbuf[128];


			hdc =BeginPaint(hwnd,&ps); //开始绘图

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

			EndPaint(hwnd,&ps); //结束绘图

			if(step < 2)
			{//
				InvalidateRect(hwnd,NULL,FALSE);
			}
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			DeleteDC(hdc_mem);
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

void	GUI_DEMO_BitBlt_Speed(void)
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
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
								&wcex,
								_T("emXGUI DEMO - BitBlt Speed"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

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
