
#include	"emXGUI.h"

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/

//定义控件ID
#define	ID_EXIT		0x1000
#define	ID_BTN1		0x1101
#define	ID_BTN2		0x1102
#define	ID_BTN3		0x1103
#define	ID_BTN4		0x1104
#define	ID_BTN5		0x1105
#define	ID_BTN6		0x1106
#define	ID_BTN7		0x1107
/*************************************/
#define ID_MYBTN1    0x1001
#define ID_MYBTN2    0x1002

//定义控件的通知码
#define MYBTN_UP   0
#define MYBTN_DOWN 1

//定义控件的私有消息(所有控件的私有消息要从 WM_WIDGET 开始定义)
#define MSG_MYBTN_SET_STATE WM_WIDGET+0 //设置按扭状态
#define MSG_MYBTN_GET_STATE WM_WIDGET+1 //获得按扭状态
//定义控件的状态
#define BTN_STATE_PRESSED (1<<0)
//定义控件的扩展数据结构.
typedef struct
{
	u32 state; //用于记录按钮状态.
}my_btn_ext;
//控件的窗口过程函数
static LRESULT	my_btn_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	my_btn_ext *btn_ex;
	btn_ex = (my_btn_ext*)GetWindowLong(hwnd, GWL_EXDATA_ADDR); //获得窗口扩展数据区。
	switch (msg)
	{
	case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
	{
		btn_ex->state = 0;
	}
	return TRUE;
	case WM_LBUTTONDOWN: //鼠标左键/触摸在窗口内按下了。
	{
		btn_ex->state = ~btn_ex->state; //设置 “按下”状态.
		NotifyParent(hwnd, MYBTN_DOWN); //发通知消息给父窗口
		//使用窗口重绘.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	//		case WM_LBUTTONUP:  //鼠标左键/触摸在窗口内“抬起”了。
	//		{
	//			btn_ex->state  &= ~BTN_STATE_PRESSED; //清除“按下”状态.
	//			NotifyParent(hwnd,MYBTN_UP); //发通知消息给父窗口
	//			//使用窗口重绘.
	//			InvalidateRect(hwnd,NULL,FALSE);
	//		}
	case MSG_MYBTN_SET_STATE: //设置按扭状态
	{
		btn_ex->state = wParam;

		InvalidateRect(hwnd, NULL, FALSE); //改变状态后应该要重绘制一次...
	}
	break;
	case MSG_MYBTN_GET_STATE: //获得按扭状态
	{
		return btn_ex->state;
	}
	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc;
		WCHAR wbuf[64];
		hdc = BeginPaint(hwnd, &ps); //开始绘图
		GetWindowText(hwnd, wbuf, 64); //获得窗口文字.
		GetClientRect(hwnd, &rc);
		if (btn_ex->state)
		{ // “按下”状态
			SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
			SetPenColor(hdc, MapRGB(hdc, 100, 0, 0));
			SetBrushColor(hdc, MapRGB(hdc, 135, 206, 250));
		}
		else
		{	// 非“按下”状态
			SetTextColor(hdc, MapRGB(hdc, 0, 30, 30));
			SetPenColor(hdc, MapRGB(hdc, 0, 100, 100));
			SetBrushColor(hdc, MapRGB(hdc, 220, 220, 220));
		}
		DrawCircle(hdc, rc.x + rc.w / 4, rc.y + rc.w / 4, rc.w / 4 - 2);
		FillCircle(hdc, rc.x + rc.w / 4, rc.x + rc.w / 4, rc.w / 4);

		EndPaint(hwnd, &ps); //结束绘图
	}
	break;
	//		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
	//		{
	//			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口.
	//		}
	default: //用户不关心的消息,由系统处理.
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}
//控件的窗口过程函数
static LRESULT	my_rectbtn_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	my_btn_ext *btn_ex;
	btn_ex = (my_btn_ext*)GetWindowLong(hwnd, GWL_EXDATA_ADDR); //获得窗口扩展数据区。
	switch (msg)
	{
	case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
	{
		btn_ex->state = 0;
	}
	return TRUE;
	case WM_LBUTTONDOWN: //鼠标左键/触摸在窗口内按下了。
	{
		btn_ex->state |= BTN_STATE_PRESSED; //设置 “按下”状态.
		NotifyParent(hwnd, MYBTN_DOWN); //发通知消息给父窗口
		//使用窗口重绘.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	case WM_LBUTTONUP:  //鼠标左键/触摸在窗口内“抬起”了。
	{
		btn_ex->state &= ~BTN_STATE_PRESSED; //清除“按下”状态.
		NotifyParent(hwnd, MYBTN_UP); //发通知消息给父窗口
		//使用窗口重绘.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	case MSG_MYBTN_SET_STATE: //设置按扭状态
	{
		btn_ex->state = wParam;

		InvalidateRect(hwnd, NULL, FALSE); //改变状态后应该要重绘制一次...
	}
	break;
	case MSG_MYBTN_GET_STATE: //获得按扭状态
	{
		return btn_ex->state;
	}
	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc;
		WCHAR wbuf[64];
		hdc = BeginPaint(hwnd, &ps); //开始绘图
		GetWindowText(hwnd, wbuf, 64); //获得窗口文字.
		GetClientRect(hwnd, &rc);
		if (btn_ex->state&BTN_STATE_PRESSED)
		{ // “按下”状态
			SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
			SetPenColor(hdc, MapRGB(hdc, 100, 0, 0));
			SetBrushColor(hdc, MapRGB(hdc, 135, 206, 250));
		}
		else
		{
			// 非“按下”状态
			SetTextColor(hdc, MapRGB(hdc, 0, 30, 30));
			SetPenColor(hdc, MapRGB(hdc, 0, 100, 100));
			SetBrushColor(hdc, MapRGB(hdc, 220, 220, 220));
		}
		DrawRoundRect(hdc, &rc, 20);
		InflateRect(&rc, -1, -1); //矩形的水平，垂直缩小一个像素.
		FillRoundRect(hdc, &rc, 20);
		EndPaint(hwnd, &ps); //结束绘图
	}
	break;
	case WM_CLOSE: //窗口关闭时，会自动产生该消息.
	{
		return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口.
	}
	default: //用户不关心的消息,由系统处理.
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}
//定义一个控件类结构实例.
const WNDCLASS my_roundbutton =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = my_btn_proc,
	.cbClsExtra = 0,
	.cbWndExtra = sizeof(my_btn_ext),
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};
//定义一个控件类结构实例.
const WNDCLASS my_rectbutton =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = my_rectbtn_proc,
	.cbClsExtra = 0,
	.cbWndExtra = sizeof(my_btn_ext),
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};
/*===================================================================================*/

static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

	if (ds->State & BST_PUSHED)
	{ //按钮是按下状态

		SetBrushColor(hdc, MapRGB(hdc, 150, 200, 250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		SetPenColor(hdc, MapRGB(hdc, 250, 0, 0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 250, 0, 0));      //设置文字色

	}
	else
	{ //按钮是弹起状态
		SetBrushColor(hdc, MapRGB(hdc, 30, 150, 30));
		SetPenColor(hdc, MapRGB(hdc, 0, 250, 0));
		SetTextColor(hdc, MapRGB(hdc, 0, 50, 100));
	}

	FillRect(hdc, &rc); //用矩形填充背景
	DrawRect(hdc, &rc); //画矩形外框
	if (ds->hwnd == GetFocus())
	{
		RECT rc;
		GetClientRect(ds->hwnd, &rc);
		CommFace_DrawFocusRect(hdc, &rc);
	}
	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)

}


static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rc;

	switch (msg)
	{
	case	WM_CREATE:

		GetClientRect(hwnd, &rc); //获得窗口的客户区矩形

		CreateWindow(BUTTON, L"EXIT", WS_VISIBLE, rc.w - 100, 8, 80, 48, hwnd, ID_EXIT, NULL, NULL);

		//设置矩形参数，用于创建按钮
		rc.x = 20;
		rc.y = 50;
		rc.w = 100;
		rc.h = 40;
		CreateWindow(BUTTON, L"Button1", WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN1, NULL, NULL);
		//往下移动矩形位置(X轴不变,Y轴位置增加rc.h+10个像素)
		OffsetRect(&rc, 0, rc.h + 10);
		//创建立体风格的按钮(BS_3D)
		CreateWindow(BUTTON, L"Button2", BS_3D | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN2, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//创建平面风格的按钮(BS_FLAT)
		CreateWindow(BUTTON, L"Button3", BS_FLAT | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN3, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//创建圆角风格的按钮 (BS_ROUND )
		CreateWindow(BUTTON, L"Button4", BS_ROUND | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN4, NULL, NULL);
		rc.x = 150;
		rc.y = 30;
		rc.w = 120;
		rc.h = 60;
		//创建圆角风格的按钮(BS_ROUND+BS_NOTIFY)
		CreateWindow(BUTTON, L"Button5", BS_ROUND | BS_NOTIFY | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN5, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//创建自绘制按钮(WS_OWNERDRAW)
		CreateWindow(BUTTON, L"Button6\r\nOwnerDraw", WS_OWNERDRAW | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN6, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//创建圆角风格的按钮(BS_ROUND+WS_DISABLE)
		CreateWindow(BUTTON, L"Button7", BS_ROUND | WS_DISABLED | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN7, NULL, NULL);

		rc.y = 30;
		OffsetRect(&rc, rc.w + 30, 0);
		CreateWindow(&my_roundbutton, L"Button8", WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_MYBTN1, NULL, NULL);
		OffsetRect(&rc, 0, rc.h + 10);
		CreateWindow(&my_rectbutton, L"Button9", WS_VISIBLE,
			rc.x, rc.y, rc.w / 2, rc.h, hwnd, ID_MYBTN2, NULL, NULL);
		return TRUE;
		////////
	case	WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
	{
		u16 code, id;
		//			NMHDR *nr=(NMHDR*)lParam;
		id = LOWORD(wParam);
		code = HIWORD(wParam);
		if (id >= ID_BTN1 && id <= ID_BTN6)
		{
			if (code == BN_PUSHED)
			{ //按钮被按下了.
				GUI_Printf("Button PUSHED: ID:%04XH\r\n", id);
			}

			if (code == BN_CLICKED)
			{ //按钮弹起了.
				GUI_Printf("Button UNPUSHED: ID:%04XH\r\n", id);
			}
		}
		if (id == ID_EXIT && code == BN_CLICKED)
		{ // EXIT按钮弹起
			PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭主窗口
		}
	}
	break;
	////
	case	WM_CTLCOLOR:
	{
		/* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
		 * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
		 * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
		 * 将忽略本次操作，继续使用默认的颜色进行绘制.
		 *
		 */
		u16 id;

		id = LOWORD(wParam);

		if (id == ID_BTN5) //只改变 BTN5的颜色.
		{
			CTLCOLOR *cr;
			cr = (CTLCOLOR*)lParam;

			if (SendMessage(GetDlgItem(hwnd, id), BM_GETSTATE, 0, 0)&BST_PUSHED)
			{
				cr->TextColor = RGB888(50, 220, 50);  //文字颜色（RGB32颜色格式)
				cr->BackColor = RGB888(20, 100, 20);  //背景颜色（RGB32颜色格式)
				cr->BorderColor = RGB888(30, 30, 30); //边框颜色（RGB32颜色格式)
			}
			else
			{
				cr->TextColor = RGB888(250, 250, 250);
				cr->BackColor = RGB888(200, 0, 0);
				cr->BorderColor = RGB888(50, 50, 50);
			}
			return TRUE;
		}
		else
		{
			//其它按钮使用系统默认的颜色进行绘制，所以直接返回FALSE.
			return FALSE;
		}
	}
	case	WM_DRAWITEM:
	{
		/*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
		 *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
		 *  该指针成员包含了一些控件绘制相关的参数.
		 */
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if (ds->ID >= ID_BTN1 && ds->ID <= ID_BTN6)
		{
			button_owner_draw(ds); //执行自绘制按钮
		}
		return TRUE;
	}
	case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
	{
		PAINTSTRUCT ps;

		hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, MapRGB(hdc, 0, 0, 255));
		TextOut(hdc, 8, 4, L"Button Test:", -1);


		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return	WM_NULL;
}

void	GUI_DEMO_Button(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;
	//第1部分：配置wcex参数。
	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//第2部分：创建主窗口
	hwnd = CreateWindowEx(NULL,
		&wcex,
		_T("GUI Demo - Button"),
		WS_CAPTION | WS_DLGFRAME | WS_BORDER | WS_CLIPCHILDREN,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//第3部分：显示主窗口
	ShowWindow(hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
