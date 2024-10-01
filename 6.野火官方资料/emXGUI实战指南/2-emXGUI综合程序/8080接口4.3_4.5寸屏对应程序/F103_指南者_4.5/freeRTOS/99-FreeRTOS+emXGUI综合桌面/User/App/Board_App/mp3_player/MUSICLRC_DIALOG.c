#include "emXGUI.h"


static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_CREATE:
      {
      
      }
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
}


//���ָ�ʾ��
HWND	MusicLrc_hwnd;
void	GUI_MUSICLRC_DIALOG(void)
{
	
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	MusicLrc_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
		&wcex,
		L"GUI_MUSICPLAYER_DIALOG",
		WS_VISIBLE,
		0, 0, GUI_XSIZE, 480,
		NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(MusicLrc_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MusicLrc_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

