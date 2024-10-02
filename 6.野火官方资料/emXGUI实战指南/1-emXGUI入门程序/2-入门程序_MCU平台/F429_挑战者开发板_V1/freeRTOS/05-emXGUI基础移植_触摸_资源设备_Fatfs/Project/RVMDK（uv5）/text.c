#include "text.h"

static 	 LRESULT  	Text_proc2(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

void	Text_Create(void *param)
{
    HWND hwnd;
    WNDCLASS	wcex;
		MSG msg;

		wcex.Tag 		    = WNDCLASS_TAG;
		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= Text_proc2;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= NULL;//hInst;
		wcex.hIcon			= NULL;
		wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

		RECT rc;//���ڴ�С
    GetClientRect(hwnd, &rc); //��ÿͻ�������.
//  GUI_DEBUG("Create desktop");
	//�������洰��.
  	hwnd = CreateWindowEx(	//WS_EX_LOCKPOS
	                            NULL,
                              &wcex,
                              _T("TEXT Window!"),
                              WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
                              10,10,300,300,
                              NULL,0,NULL,NULL);

	//GUI_DEBUG("HWND_Desktop=%08XH\r\n",	hwnd);

	//��ʾ���洰��.
	ShowWindow(hwnd,SW_SHOW);
#if (GUI_SHOW_CURSOR_EN)
	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
	ShowCursor(TRUE);
#endif

  while(GetMessage(&msg,hwnd))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
	GUI_Thread_Delete(GUI_GetCurThreadHandle());
}
