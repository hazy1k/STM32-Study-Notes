
/*
 *GUI_DEMO_MultiWindow.c
 *2018年10月22日下午3:51:41
 *
 */

#include "emXGUI.h"
#include "x_libc.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/

enum eID
{
	eID_OK,

	eID_BTN1,
	eID_BTN2,
	eID_BTN3,
	eID_BTN4,
	ID_LISTBOX1,

};

/*============================================================================*/
static HWND hDlg1,hDlg2,hDlg3;

typedef struct
{
	int count;
}dlg_exdata_t;


/*============================================================================*/

static LRESULT	DlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0,m_rc[4];
	dlg_exdata_t *ex;

	ex = (dlg_exdata_t*)GetWindowLong(hwnd,GWL_EXDATA_ADDR);

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{


			ex->count=0;

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			MakeMatrixRect(m_rc,&rc0,2,2,4,1);
			//rc.w =64;
			//rc.h =40;
			//rc.x =8;
			//rc.y =20;

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[0];
			CreateWindow(BUTTON,L">>",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN1,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[1];
			CreateWindow(BUTTON,L"...",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN2,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[2];
			CreateWindow(BUTTON,L"-",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN3,NULL,NULL);

			//OffsetRect(&rc,rc.w+4,0);
			rc =m_rc[3];
			CreateWindow(BUTTON,L"+",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN4,NULL,NULL);

			SetTimer(hwnd,1,200,TMR_START,NULL);
		}
		return TRUE;
		////


		case WM_NCLBUTTONDOWN:
		//	SetWindowToTop(hwnd);
			return DefWindowProc(hwnd,msg,wParam,lParam);

		case WM_TIMER:
		{
			WCHAR wbuf[128];

			x_wsprintf(wbuf,L"%04d",ex->count);
			SetWindowText(GetDlgItem(hwnd,eID_BTN2),wbuf);

			ex->count++;
			InvalidateRect(hwnd,NULL,TRUE);
		}
		break;
		/////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==eID_BTN1 && code==BN_CLICKED) // 按钮“单击”了.
			{
				//PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.

			}

			if(id==eID_BTN3 && code==BN_CLICKED) // 按钮“单击”了.
			{
				ex->count -= 10;

			}

			if(id==eID_BTN4 && code==BN_CLICKED) // 按钮“单击”了.
			{
				ex->count += 10;

			}



		}
		break;
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
			SetBrushColor(hdc,MapRGB(hdc,10,100,10));

			FillRect(hdc,&rc);

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
#define	MOVE_STOP	0
#define	MOVE_IN		1
#define	MOVE_OUT	2

static int move_1=MOVE_STOP;
static int move_2=MOVE_STOP;

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;
	HWND wnd;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			rc.w =100;
			rc.h =60;
			rc.x =rc0.w-rc.w-100;
			rc.y =20;

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).

			OffsetRect(&rc,0,rc.h+20);
			CreateWindow(BUTTON,L"BTN-1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN1,NULL,NULL);
			OffsetRect(&rc,0,rc.h+8);
			CreateWindow(BUTTON,L"BTN-2",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN2,NULL,NULL);
			OffsetRect(&rc,0,rc.h+8);
			CreateWindow(BUTTON,L"BTN-3",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN3,NULL,NULL);
			OffsetRect(&rc,0,rc.h+8);
			CreateWindow(BUTTON,L"BTN-4",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_BTN4,NULL,NULL);

			SetTimer(hwnd,1,20,TMR_START,NULL);

			////
			move_1 =MOVE_STOP;
			move_2 =MOVE_STOP;

			wcex_dlg.Tag 		    = WNDCLASS_TAG;

			wcex_dlg.Style			= CS_HREDRAW | CS_VREDRAW;
			wcex_dlg.lpfnWndProc	= DlgProc; //设置主窗口消息处理的回调函数.
			wcex_dlg.cbClsExtra		= 0;
			wcex_dlg.cbWndExtra		= sizeof(dlg_exdata_t);
			wcex_dlg.hInstance		= NULL;
			wcex_dlg.hIcon			= NULL;
			wcex_dlg.hCursor		= NULL;

			rc.w =200;
			rc.h =80;
			rc.x =rc0.w-50;
			rc.y =40;
			hDlg1	=CreateWindow(&wcex_dlg,L"Win1",WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,0x2000,NULL,NULL);

			OffsetRect(&rc,0,rc.h+8);
			hDlg2	=CreateWindow(&wcex_dlg,L"Win2(Overlapped)",WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,0x2001,NULL,NULL);

			OffsetRect(&rc,0,rc.h+8);
			hDlg3	=CreateWindow(&wcex_dlg,L"Win3",WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,0x2002,NULL,NULL);

			wnd=CreateWindow(LISTBOX,_T("Listbox"),WS_OVERLAPPED|WS_CAPTION|LBS_LINE|LBS_NOTIFY|WS_BORDER|WS_VISIBLE,8,20,200,150,hwnd,ID_LISTBOX1,NULL,NULL);
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

		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==eID_BTN1 && code==BN_CLICKED)
			{
				GetWindowRect(hDlg1,&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				if(rc.x <= 100)
				{
					move_1 =MOVE_IN;
				}
				else
				{
					move_1 =MOVE_OUT;
				}

			}

			if(id==eID_BTN2 && code==BN_CLICKED)
			{
				GetWindowRect(hDlg2,&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				if(rc.x <= 100)
				{
					move_2 =MOVE_IN;
				}
				else
				{
					move_2 =MOVE_OUT;
				}

			}

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////
		case WM_TIMER:
		{
			//////////////////////
			if(move_1==MOVE_OUT)
			{
				GetWindowRect(hDlg1,&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);

				if(rc.x>100)
				{
					rc.x -= 30;
					MoveWindow(hDlg1,rc.x,rc.y,rc.w,rc.h,TRUE);
					RedrawWindow(hwnd,NULL,RDW_ALLCHILDREN);
				}
				else
				{
					move_1 =MOVE_STOP;
				}
			}

			if(move_1==MOVE_IN)
			{
				RECT rc0;

				GetClientRect(hwnd,&rc0);

				GetWindowRect(hDlg1,&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				if(rc.x<(rc0.w-50))
				{
					rc.x += 30;
					MoveWindow(hDlg1,rc.x,rc.y,rc.w,rc.h,TRUE);

				}
				else
				{
					move_1 =MOVE_STOP;
				}
			}

			//////////////////////
			if(move_2==MOVE_OUT)
			{
				GetWindowRect(hDlg2,&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);

				if(rc.x>100)
				{
					rc.x -= 30;
					MoveWindow(hDlg2,rc.x,rc.y,rc.w,rc.h,TRUE);
				}
				else
				{
					move_2 =MOVE_STOP;
				}
			}

			if(move_2==MOVE_IN)
			{
				RECT rc0;

				GetClientRect(hwnd,&rc0);

				GetWindowRect(hDlg2,&rc);
				ScreenToClient(hwnd,(POINT*)&rc.x,1);
				if(rc.x<(rc0.w-50))
				{
					rc.x += 30;
					MoveWindow(hDlg2,rc.x,rc.y,rc.w,rc.h,TRUE);
				}
				else
				{
					move_2 =MOVE_STOP;
				}
			}

		}
		break;
		////

		case WM_MOUSELEAVE:
			printf("WM_MOUSELEAVE.");
			break;

		case WM_ERASEBKGND:
		{
			HDC hdc;
			RECT rc;

			hdc =(HDC)wParam;
			rc =*(RECT*)lParam;

			SetBrushColor(hdc,MapRGB(hdc,100,100,150));
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

void	GUI_DEMO_MultiWindow(void)
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
	hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
								&wcex,
								_T("GUI_DEMO: Multi-Window(Overlapped)"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								10,20,600,400,    //窗口位置和大小
								NULL,NULL,NULL,NULL);


	ShowWindow(hwnd,SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

/*============================================================================*/
