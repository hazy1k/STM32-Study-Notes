
/*
 *GUI_DEMO_CustomWidget.c
 *2018年9月7日上午10:53:01
 *
 */

#include "emXGUI.h"
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

#define ID_MYBTN1   0x1001
#define ID_MYBTN2   0x1002


/*============================================================================*/


//定义控件的通知码
#define MYBTN_UP   0
#define MYBTN_DOWN 1

//定义控件的私有消息(所有控件的私有消息要从 WM_WIDGET 开始定义)
#define MSG_MYBTN_SET_STATE WM_WIDGET+0 //设置按扭状态
#define	MSG_MYBTN_GET_STATE WM_WIDGET+1 //获得按扭状态


//定义控件的状态
#define BTN_STATE_PRESSED (1<<0)

//定义控件的扩展数据结构.
typedef struct
{
	u32 state; //用于记录按钮状态.

}my_btn_ext;

/*============================================================================*/

//控件的窗口过程函数
static LRESULT	my_btn_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	my_btn_ext *btn_ex;

	btn_ex =(my_btn_ext*)GetWindowLong(hwnd,GWL_EXDATA_ADDR); //获得窗口扩展数据区。

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			btn_ex->state =0;
		}
		return TRUE;
		////

		case WM_LBUTTONDOWN: //鼠标左键/触摸在窗口内按下了。
		{
			btn_ex->state |= BTN_STATE_PRESSED; //设置 “按下”状态.

			NotifyParent(hwnd,MYBTN_DOWN); //发通知消息给父窗口
			//使用窗口重绘.
			InvalidateRect(hwnd,NULL,FALSE);
		}
		break;
		////
		case WM_LBUTTONUP:  //鼠标左键/触摸在窗口内“抬起”了。
		{
			btn_ex->state  &= ~BTN_STATE_PRESSED; //清除“按下”状态.
			NotifyParent(hwnd,MYBTN_UP); //发通知消息给父窗口
			//使用窗口重绘.
			InvalidateRect(hwnd,NULL,FALSE);
		}
		break;
		////

		case MSG_MYBTN_SET_STATE: //设置按扭状态
		{
			btn_ex->state = wParam;

			InvalidateRect(hwnd,NULL,FALSE); //改变状态后应该要重绘制一次...
		}
		break;
		////

		case MSG_MYBTN_GET_STATE: //获得按扭状态
		{
			return btn_ex->state;
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			LONG sta;
			RECT rc;
			WCHAR wbuf[64];

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			GetWindowText(hwnd,wbuf,64); //获得窗口文字.
			GetClientRect(hwnd,&rc);

			////用户的绘制内容...

			if(btn_ex->state&BTN_STATE_PRESSED)
			{ // “按下”状态
				SetTextColor(hdc,MapRGB(hdc,250,10,10));
				SetPenColor(hdc,MapRGB(hdc,100,0,0));
				SetBrushColor(hdc,MapRGB(hdc,250,200,200));

			}
			else
			{	// 非“按下”状态

				SetTextColor(hdc,MapRGB(hdc,0,30,30));
				SetPenColor(hdc,MapRGB(hdc,0,100,100));
				SetBrushColor(hdc,MapRGB(hdc,250,250,250));
			}

			DrawRect(hdc,&rc); //绘制“按钮”框.

			InflateRect(&rc,-1,-1); //矩形的水平，垂直缩小一个像素.
			FillRect(hdc,&rc); //填充“按钮”背景.

			DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER); //绘制“按钮”文字.

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口.
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

//定义一个控件类结构实例.
const WNDCLASS my_button=
{
    .Tag         =WNDCLASS_TAG,
    .Style       =0,
    .lpfnWndProc =my_btn_proc,
    .cbClsExtra  =0,
    .cbWndExtra  =sizeof(my_btn_ext),
    .hInstance   =NULL,
    .hIcon       =NULL,
    .hCursor	 =NULL,
};

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			HWND wnd;

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).

			//创建自定义的控件.
			CreateWindow(&my_button,L"MyBtn1",WS_VISIBLE,10,40,100,50,hwnd,ID_MYBTN1,NULL,NULL);
			CreateWindow(&my_button,L"MyBtn2",WS_VISIBLE,10,100,100,80,hwnd,ID_MYBTN2,NULL,NULL);

			//获得MYBTN2(按ID获得窗口句柄)
			wnd =GetDlgItem(hwnd,ID_MYBTN2);
			//发消息给MYBTN2,设置成"按下"状态。
			SendMessage(wnd,MSG_MYBTN_SET_STATE,MYBTN_DOWN,0);

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

			if(id==ID_MYBTN1)
			{
				if(code==MYBTN_UP)
				{

					//printf("MYBTN1 Up.\r\n");
				}

				if(code==MYBTN_DOWN)
				{
					//printf("MYBTN1 Down.\r\n");
				}

			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);

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

void	GUI_DEMO_CustomWidget(void)
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
								_T("emXGUI Demo: Custom Widget"), //窗口名称
								WS_CAPTION|WS_CLIPCHILDREN,
								10,20,320,240,    //窗口位置和大小
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
