/**
  *********************************************************************
  * @file    desktop.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI 桌面
  *********************************************************************
  * 官网    :www.embedfire.com
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 


#include "emXGUI.h"
#include "GUI_AppDef.h"


/*===================================================================================*/

/**
  * @brief  桌面背景绘制函数，触发背景绘制时会调用本函数，
			通过修改本函数的内容可更改桌面的背景
  * @param  hdc 绘图上下文
  * @param  lprc 要绘制的矩形区域，为NULL时会自动绘制hwnd的客户区
  * @param  hwnd 窗口对象句柄
  * @retval 无
  */
static	void	_EraseBackgnd(HDC hdc, const RECT *lprc, HWND hwnd)
{
	RECT rc;

	if (lprc == NULL)
	{
		GetClientRect(hwnd, &rc);
	}
	else
	{
		CopyRect(&rc, lprc);
	}

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
	FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));

	//  SetFont(hdc, iconFont);
	//	DrawText(hdc,L" A B C D E \r\n F G H I J",-1,&rc,DT_LEFT|DT_VCENTER);

	SetFont(hdc, GB2312_32_Font);

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	rc.y += 20;
	DrawText(hdc, L"emXGUI@Embedfire STM32F429 ", -1, &rc, DT_CENTER);

	/* 背景 */
	GetClientRect(hwnd, &rc);
	SetBrushColor(hdc, MapRGB(hdc, 82, 85, 82));
	rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
	rc.h = HEAD_INFO_HEIGHT;
	FillRect(hdc, &rc);

	/* 首栏 */
	SetFont(hdc, logoFont);
	/* 显示logo */
	GetClientRect(hwnd, &rc);
	rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT - 10;
	rc.h = HEAD_INFO_HEIGHT;

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	DrawText(hdc, L" B", -1, &rc, DT_LEFT | DT_VCENTER);


	GetClientRect(hwnd, &rc);
	rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
	rc.h = HEAD_INFO_HEIGHT;

	/* 恢复默认字体 */
	SetFont(hdc, defaultFont);
	rc.x += 50;
	DrawText(hdc, L" 野火@emXGUI", -1, &rc, DT_LEFT | DT_VCENTER);

	GetClientRect(hwnd, &rc);
	rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT + 10;
	rc.h = HEAD_INFO_HEIGHT;

	/* 控制图标字体 */
  //  SetFont(hdc, controlFont);

	/* 向上图标 */
	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	//  DrawText(hdc,L"D",-1,&rc,DT_TOP|DT_CENTER);
	DrawText(hdc, L"∧", -1, &rc, DT_TOP | DT_CENTER);

	/* 恢复默认字体 */
	SetFont(hdc, defaultFont);

	rc.y -= 20;
	DrawText(hdc, L"\r\n\r\n详细", -1, &rc, DT_BOTTOM | DT_CENTER);

	GetClientRect(hwnd, &rc);
	rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
	rc.h = HEAD_INFO_HEIGHT;

	DrawText(hdc, L"www.embedFire.com  ", -1, &rc, DT_RIGHT | DT_VCENTER);


}

/**
  * @brief  桌面回调函数
  * @param  hwnd 当前处理该消息的窗口对象句柄
  * @param  msg 消息类型值，用以标识和区分当前产生的消息
  * @param  wParam 消息参数值，根据msg消息代码值不同
  * @param  lParam 消息参数值，根据msg消息代码值不同
  * @retval 返回给SendMessage的值
  */
static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
    /* 桌面创建时,会产生该消息,可以在这里做一些初始化工作. */
		case	WM_CREATE:	
			//初始化输入设备
			GUI_InputInit();

			////创建1个20ms定时器，处理循环事件.
			SetTimer(hwnd, 1, 20, TMR_START, NULL);

			//创建App线程
			GUI_UserAppStart();
			/* slide win 线程 */
			GUI_SlideWinAppStart();
	
		break;

		////////
		case	WM_TIMER:
		{
			u16 id;

			id = LOWORD(wParam);
			if (id == 1)
			{
				GUI_InputHandler(); //处理输入设备
			}
		}
		break;

		/* 检测是否触摸到“详细”一栏 */
		case WM_LBUTTONDOWN:
		{

			POINT pt;
			RECT rc;

			pt.x = GET_LPARAM_X(lParam);
			pt.y = GET_LPARAM_Y(lParam);

			GetClientRect(hwnd, &rc);

			rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
			rc.h = HEAD_INFO_HEIGHT;

			/* 若触摸到，则发送消息到slide window */
			if (PtInRect(&rc, &pt))
			{
				PostMessage(GetDlgItem(hwnd, ID_SLIDE_WINDOW), WM_MSG_FRAME_DOWN, 0, 0);
			}
		}
		break;
				      
    
    /* 客户区背景需要被擦除 */
		case	WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			_EraseBackgnd(hdc,NULL,hwnd);
		}
		return TRUE;

    /* 用户不关心的信息，由系统处理 */
		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

/**
  * @brief  桌面启动函数
  * @param  无
  * @retval 无
  */
void GUI_DesktopStartup(void)
{
	WNDCLASS	wcex;
	HWND hwnd;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= desktop_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建桌面窗口.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

	//显示桌面窗口.
	ShowWindow(hwnd,SW_SHOW);

	//设置系统打开光标显示(可以按实际情况看是否需要).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/

