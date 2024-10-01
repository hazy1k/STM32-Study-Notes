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

/* Ìí¼Ó¿âÎÄ¼þ */
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

void GUI_AppMain(void);
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

