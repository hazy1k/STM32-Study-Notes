
#include <emXGUI.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
//控件ID
#define	ID_EXIT		0x1000
#define	ID_RB1		0x1101
#define	ID_RB2		0x1102
#define	ID_RB3		0x1103
#define	ID_RB4		0x1104
#define	ID_RB5		0x1105
#define	ID_RB6		0x1106

/*===================================================================================*/

//static void radiobox_owner_draw(DRAWITEM_HDR *ds)
//{

//}

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case	WM_CREATE:
      {
            //获得客户区的大小
            GetClientRect(hwnd,&rc);
      
				//CreateWindow(BUTTON,L"EXIT",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);
      
				//创建第1组单选按钮(窗口ID的高16位为GroupID号，只有在同一组内的单选按钮，才会有"多选一"的效果)
				rc.x =50;
				rc.y =60;
				rc.w =240;
				rc.h =64;
				CreateWindow(BUTTON,L"Radiobox1",BS_RADIOBOX|WS_VISIBLE,
                         rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|ID_RB1,NULL,NULL);
				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Radiobox2",WS_DISABLED|BS_RADIOBOX|WS_VISIBLE,
                         rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|ID_RB2,NULL,NULL);
      
				//创建第2组单选按钮(GroupID号为2,使用常规按钮风格(BS_PUSHLIKE)).
				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Radiobox3",BS_RADIOBOX|BS_PUSHLIKE|WS_VISIBLE,
                         rc.x,rc.y,rc.w,rc.h,hwnd,(2<<16)|ID_RB3,NULL,NULL);
				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Radiobox4",BS_RADIOBOX|BS_PUSHLIKE|WS_VISIBLE,
                         rc.x,rc.y,rc.w,rc.h,hwnd,(2<<16)|ID_RB4,NULL,NULL);
      
				//创建第3组单选按钮(GroupID号为3).
				rc.x =310;
				rc.y =60;
				rc.w =240;
				rc.h =64;
				CreateWindow(BUTTON,L"Radiobox5",BS_RADIOBOX|WS_VISIBLE,
                         rc.x,rc.y,rc.w,rc.h,hwnd,(3<<16)|ID_RB5,NULL,NULL);
				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Radiobox6",BS_RADIOBOX|WS_VISIBLE,
                         rc.x,rc.y,rc.w,rc.h,hwnd,(3<<16)|ID_RB6,NULL,NULL);
				return TRUE;
         }
		case	WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			id  =LOWORD(wParam);
			code=HIWORD(wParam);
			if(id >= ID_RB1 && id<= ID_RB6)
			{
				if(code == BN_CHECKED)
				{ //单选框选中.
					GUI_Printf("Radiobox Checked: ID:%04XH\r\n",id);
				}
			}      
         if(id == ID_EXIT && code == BN_CLICKED)
         {
               PostCloseMessage(hwnd);//产生WM_CLOSE消息关闭主窗口
         }  
       	break;  
		}
	
		case	WM_CTLCOLOR:
		{
			u16 id;
			id =LOWORD(wParam);
			if(id== ID_RB5 || id == ID_RB6) //只改变 RB5,RB6的颜色.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_CHECKED)
				{
					cr->TextColor =RGB888(250,0,0);      //文字颜色（RGB888颜色格式)
					cr->BackColor =RGB888(220,200,200);    //背景颜色（RGB888颜色格式)
					cr->BorderColor =RGB888(30,30,230);  //边框颜色（RGB888颜色格式)
					cr->ForeColor =RGB888(100,250,100);  //前景颜色（RGB888颜色格式)
				}
				else
				{
					cr->TextColor =RGB888(20,20,250);
					cr->BackColor =RGB888(200,220,200);
					cr->BorderColor =RGB888(50,50,50);
					cr->ForeColor =RGB888(180,200,230);
				}
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;

			GetClientRect(hwnd, &rc); //获得窗口的客户区矩形
			SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
			FillRect(hdc, &rc);
		}
		break;

		case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
		{	
         PAINTSTRUCT ps;  
         HDC hdc;
			hdc	=BeginPaint(hwnd,&ps);           
			SetTextColor(hdc,MapRGB(hdc,0,0,255)); 
			TextOut(hdc,8,4,L"Radiobox Test:",-1);  
			EndPaint(hwnd,&ps);
			return	TRUE;
		}		     
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return	WM_NULL;
}

void	GUI_DEMO_Radiobox(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;
		//配置wcex参数
		wcex.Tag 		    = WNDCLASS_TAG;
		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
		//创建主窗口
		hwnd	=CreateWindowEx(	NULL,
									&wcex,
									_T("GUI Demo - Radiobox"),
									WS_OVERLAPPEDWINDOW,
									0,0,GUI_XSIZE,GUI_YSIZE,
									NULL,NULL,NULL,NULL);		
      //显示主窗口
		ShowWindow(hwnd,SW_SHOW);	
      //开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}	
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
