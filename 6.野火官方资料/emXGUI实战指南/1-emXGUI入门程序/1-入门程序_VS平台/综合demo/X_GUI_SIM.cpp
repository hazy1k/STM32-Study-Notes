//============================================================================
// Name        : VC_Test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <iostream>
#include <stdio.h>
//#include <stdint.h>

// Windows Header Files:
#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#include <wingdi.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/* 添加库文件 */
#pragma comment(lib,"./../X_GUI/emXGUI_MSVC.a")
#pragma comment(lib,"./../X_GUI/x_libc_MSVC.a")
#pragma comment(lib,"./../X_GUI/libjpeg_MSVC.lib")
#pragma comment(lib,"./../X_GUI/libpng_MSVC.lib")

#include "gui_drv_cfg.h"

//#pragma comment(lib, "gdi32.lib")

HINSTANCE hInst=NULL;
HWND hwndWinMain=NULL;

extern "C"{

void*	VLCD_GetFrameBuffer(void);
void VLCD_PendVsync(int time);
void VLCD_BeginSync(void);
void VLCD_EndSync(void);

int emXGUI_VLCD(HINSTANCE hInstance,int nCmdShow,int BPP,int Width,int Height);

void	GUI_AppMain(void);
void	GUI_SlideWinAppMain(void);
void	GUI_DEMO_Hello(void);
void	GUI_DEMO_Graphics_Accelerator(void);
void	GUI_DEMO_ShowWave(void);

}

extern "C" void	FrameSync(int time)
{
	VLCD_PendVsync(time);
}

extern "C" void	BeginSync(void)
{
	VLCD_BeginSync();
}

extern "C" void	EndSync(void)
{
	VLCD_EndSync();
}


extern "C" void*	GetFrameBuffer(void)
{
	//return (void*)disp_buf;
	return VLCD_GetFrameBuffer();
}


static DWORD gui_app(LPVOID lpThreadParameter)
{
	Sleep(200);
	while(1)
	{
		GUI_AppMain();
		Sleep(200);
	}
	return 0;
}


extern "C" void GUI_UserAppStart(void)
{
	CreateThread(NULL,256*1024,(LPTHREAD_START_ROUTINE)gui_app,NULL,0,NULL);

}


static DWORD gui_slide_win_app(LPVOID lpThreadParameter)
{
	Sleep(200);
	while (1)
	{
		GUI_SlideWinAppMain();
		Sleep(200);
	}
	return 0;
}

extern "C" void GUI_SlideWinAppStart(void)
{
	CreateThread(NULL, 256 * 1024, (LPTHREAD_START_ROUTINE)gui_slide_win_app, NULL, 0, NULL);

}


extern "C" void App_GUI_DEMO_Hello(void)
{
	static int thread = 0;
	static int app = 0;

	if (thread == 0)
	{  
		//创建一个独立线程来运行自已...
		CreateThread(NULL, 256 * 1024, (LPTHREAD_START_ROUTINE)App_GUI_DEMO_Hello, NULL, 0, NULL);

		thread = 1;
		return;
	}

	if (thread == 1) //线程已创建了?
	{
		if (app == 0)
		{
			app = 1;
			GUI_DEMO_Hello();

			app = 0;
			thread = 0;
		}
		else
		{
			//			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}


extern "C" void App_GUI_Graphics_Accelerator(void)
{
	static int thread = 0;
	static int app = 0;

	if (thread == 0)
	{
		//创建一个独立线程来运行自已...
		CreateThread(NULL, 256 * 1024, (LPTHREAD_START_ROUTINE)App_GUI_Graphics_Accelerator, NULL, 0, NULL);

		thread = 1;
		return;
	}

	if (thread == 1) //线程已创建了?
	{
		if (app == 0)
		{
			app = 1;
			GUI_DEMO_Graphics_Accelerator();

			app = 0;
			thread = 0;
		}
		else
		{
			//			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}


extern "C" void App_GUI_ShowWave(void)
{
	static int thread = 0;
	static int app = 0;

	if (thread == 0)
	{
		//创建一个独立线程来运行自已...
		CreateThread(NULL, 256 * 1024, (LPTHREAD_START_ROUTINE)App_GUI_ShowWave, NULL, 0, NULL);

		thread = 1;
		return;
	}

	if (thread == 1) //线程已创建了?
	{
		if (app == 0)
		{
			app = 1;
			GUI_DEMO_ShowWave();

			app = 0;
			thread = 0;
		}
		else
		{
			//			MSGBOX(hwnd,L"程序已在运行中...",L"消息提示!");
		}
	}
}

int WINAPI
WinMain(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,int nCmdShow)
{
	int LCD_BPP;

	hInst =hInstance;

	//LCD_BPP =16; //RGB565
	LCD_BPP =32; //XRGB8888

	return emXGUI_VLCD(hInst,nCmdShow,LCD_BPP,LCD_XSIZE,LCD_YSIZE);
}


int x_main() {

	printf("hello!!-1\r\n");

	return 0;
}

