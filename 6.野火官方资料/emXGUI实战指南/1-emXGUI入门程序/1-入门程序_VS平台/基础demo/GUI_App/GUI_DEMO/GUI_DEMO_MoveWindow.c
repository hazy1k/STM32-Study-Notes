


#include <stdlib.h>
#include "emXGUI.h"

/*============================================================================*/

/*============================================================================*/
//定义控件ID
enum eID
{
	eID_EXIT,
	eID_OK,

	eID_BTN1,
	eID_BTN2,
	eID_BTN3,
	eID_BTN4,

	eID_TEXTBOX1,
	eID_TEXTBOX2,
	eID_TEXTBOX3,
	eID_TEXTBOX4,

	eID_WIN1,
	eID_WIN2,
	eID_WIN3,
	eID_WIN4,

	ID_LISTBOX1,

};

enum eMSG
{
	MSG_MY_MOVE_UP =WM_USER,
	MSG_MY_MOVE_DOWN,
	MSG_SET_MOVE,

};

/*============================================================================*/

/*============================================================================*/

typedef	struct
{
	S16 move_dx; //X方向需要的移动距离.
	S16 move_dy; //Y方向需要的移动距离.

}MYWIN_DATA;

static MYWIN_DATA win1_data,win2_data,win3_data,win4_data;

static LRESULT	DlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0,m_rc[4];
	MYWIN_DATA *win_data;

	win_data =(MYWIN_DATA*)GetWindowLong(hwnd,GWL_USERDATA);

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{

			win_data =(MYWIN_DATA*)lParam;
			win_data->move_dx =0;
			win_data->move_dy =0;
			SetWindowLong(hwnd,GWL_USERDATA,lParam);//设置窗口的私有数据结构.

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			MakeMatrixRect(m_rc,&rc0,2,2,4,1);

			rc =m_rc[0];
			CreateWindow(BUTTON,L"Get Up",BS_FLAT|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN1,NULL,NULL);


			rc =m_rc[1];
			CreateWindow(BUTTON,L"Get Down",BS_FLAT|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN2,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[2];
			CreateWindow(BUTTON,L"Test",BS_FLAT|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN3,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[3];
			CreateWindow(BUTTON,L"+65",BS_FLAT|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN4,NULL,NULL);

			SetTimer(hwnd,1,20,TMR_START,NULL);
		}
		return TRUE;
		////

		case WM_TIMER:
		{
			if(win_data->move_dy > 0)
			{
				int off;

				if(abs(win_data->move_dy) > 10)
				{
					off=10;
				}
				else
				{
					off=1;
				}

				GetWindowRect(hwnd,&rc);
				OffsetRect(&rc,0,off);
				ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
				MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
				win_data->move_dy -= off;
			}
			else if(win_data->move_dy < 0)
			{
				int off;

				if(abs(win_data->move_dy) > 10)
				{
					off=10;
				}
				else
				{
					off=1;
				}

				GetWindowRect(hwnd,&rc);
				OffsetRect(&rc,0,-off);
				ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
				MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
				win_data->move_dy += off;
			}
			else
			{

			}
		}
		break;

		case MSG_SET_MOVE: //设置移动距离(只处理了Y轴的...).
		{
			win_data->move_dy =(S16)lParam;
			ResetTimer(hwnd,1,20,TMR_START,NULL);
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==eID_BTN1 && code==BN_CLICKED) // 按钮“单击”了.
			{

				PostMessage(GetParent(hwnd),MSG_MY_MOVE_UP,eID_BTN1,(LPARAM)hwnd);
			}
			if(id==eID_BTN2 && code==BN_CLICKED) // 按钮“单击”了.
			{

				PostMessage(GetParent(hwnd),MSG_MY_MOVE_DOWN,eID_BTN1,(LPARAM)hwnd);
			}
		}
		break;
		////

		case WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			RECT rc =*(RECT*)lParam;
			SetBrushColor(hdc,MapRGB(hdc,100,130,200));
			FillRect(hdc,&rc);
		}
		return TRUE;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			GetClientRect(hwnd,&rc);
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,10,100,200));

			//DrawText(hdc,L"Text",-1,&rc,DT_BKGND);
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

static WNDCLASS	wcex_dlg;


/*============================================================================*/


static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;
	HWND wnd;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			rc.w =60;
			rc.h =40;
			rc.x =rc0.w-rc.w-2;
			rc.y =2;

			CreateWindow(BUTTON,L"Exit",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_EXIT,NULL,NULL);

//			SetTimer(hwnd,1,20,TMR_START,NULL);

			////

			wcex_dlg.Tag 		    = WNDCLASS_TAG;

			wcex_dlg.Style			= CS_HREDRAW | CS_VREDRAW;
			wcex_dlg.lpfnWndProc	= DlgProc; //设置主窗口消息处理的回调函数.
			wcex_dlg.cbClsExtra		= 0;
			wcex_dlg.cbWndExtra		= 0;
			wcex_dlg.hInstance		= NULL;
			wcex_dlg.hIcon			= NULL;
			wcex_dlg.hCursor		= NULL;

			rc.w =600;
			rc.h =80;
			rc.x =10;
			rc.y =10;
			CreateWindow(&wcex_dlg,L"Win1",WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_WIN1,NULL,(LPVOID)&win1_data);

			OffsetRect(&rc,0,rc.h+8);
			CreateWindow(&wcex_dlg,L"Win2",WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_WIN2,NULL,(LPVOID)&win2_data);

			OffsetRect(&rc,0,rc.h+8);
			CreateWindow(&wcex_dlg,L"Win3",WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_WIN3,NULL,(LPVOID)&win3_data);

			OffsetRect(&rc,0,rc.h+8);
			CreateWindow(&wcex_dlg,L"Win4",WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_WIN4,NULL,(LPVOID)&win4_data);
/*
			wnd=CreateWindow(LISTBOX,_T("Listbox"),LBS_LINE|LBS_NOTIFY|WS_BORDER|WS_VISIBLE,8,20,200,150,hwnd,ID_LISTBOX1,NULL,NULL);
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-0");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-1");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-2");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-3");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-4");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-5");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-6");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-7");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-8");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-9");
			SendMessage(wnd,LB_ADDSTRING,-1,(LPARAM)L"Item-1-10");
*/
		}
		return TRUE;
		////
		case MSG_MY_MOVE_UP: //向上移动
		{
			PostMessage(GetDlgItem(hwnd,eID_WIN1),MSG_SET_MOVE,0,-50);
			PostMessage(GetDlgItem(hwnd,eID_WIN2),MSG_SET_MOVE,0,-50);
			PostMessage(GetDlgItem(hwnd,eID_WIN3),MSG_SET_MOVE,0,-50);
			PostMessage(GetDlgItem(hwnd,eID_WIN4),MSG_SET_MOVE,0,-50);
		}
		return TRUE;

		case MSG_MY_MOVE_DOWN://向下移动
		{
			PostMessage(GetDlgItem(hwnd,eID_WIN1),MSG_SET_MOVE,0,+50);
			PostMessage(GetDlgItem(hwnd,eID_WIN2),MSG_SET_MOVE,0,+50);
			PostMessage(GetDlgItem(hwnd,eID_WIN3),MSG_SET_MOVE,0,+50);
			PostMessage(GetDlgItem(hwnd,eID_WIN4),MSG_SET_MOVE,0,+50);
		}
		return TRUE;

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;
			NMHDR *nm =(NMHDR*)lParam;
			////

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.


			if(id==eID_EXIT && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case WM_TIMER:
		{


		}
		break;
		////

		case WM_ERASEBKGND:
		{
			HDC hdc;
			RECT rc;

			hdc =(HDC)wParam;
			rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapRGB(hdc,50,100,120));
			FillRect(hdc,&rc);
			return TRUE;
		}

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

void	GUI_DEMO_MoveWindow(void)
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
	hwnd	=CreateWindowEx(	WS_EX_LOCKZORDER|WS_EX_FRAMEBUFFER,
								&wcex,
								_T("GUI_DEMO: MoveWindow"), //窗口名称
								WS_CLIPCHILDREN,
								//WS_CAPTION|WS_CLIPCHILDREN,
								10,20,700,450,    //窗口位置和大小
								NULL,0,NULL,NULL);


	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);


	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	ShowWindow(hwnd,SW_SHOW);
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
