/**
  *********************************************************************
  * @file    desktop.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI ����
  *********************************************************************
  * ����    :www.embedfire.com
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 


#include "emXGUI.h"
//#include "GUI_AppDef.h"


/*===================================================================================*/

/**
  * @brief  ���汳�����ƺ�����������������ʱ����ñ�������
            ͨ���޸ı����������ݿɸ�������ı���
  * @param  hdc ��ͼ������
  * @param  lprc Ҫ���Ƶľ�������ΪNULLʱ���Զ�����hwnd�Ŀͻ���
  * @param  hwnd ���ڶ�����
  * @retval ��
  */
static	void	_EraseBackgnd(HDC hdc,const RECT *lprc,HWND hwnd)
{
	RECT rc;

	if(lprc==NULL)
	{
		GetClientRect(hwnd,&rc);
	}
	else
	{
		CopyRect(&rc,lprc);
	}

	SetBrushColor(hdc,MapRGB(hdc, 215, 61, 50));
	FillRect(hdc,&rc);
 }

/**
  * @brief  ����ص�����
  * @param  hwnd ��ǰ�������Ϣ�Ĵ��ڶ�����
  * @param  msg ��Ϣ����ֵ�����Ա�ʶ�����ֵ�ǰ��������Ϣ
  * @param  wParam ��Ϣ����ֵ������msg��Ϣ����ֵ��ͬ
  * @param  lParam ��Ϣ����ֵ������msg��Ϣ����ֵ��ͬ
  * @retval ���ظ�SendMessage��ֵ
  */
static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
    /* ���洴��ʱ,���������Ϣ,������������һЩ��ʼ������. */
		case	WM_CREATE:	
			//��ʼ�������豸
			GUI_InputInit();

			////����1��20ms��ʱ��������ѭ���¼�.
			SetTimer(hwnd, 1, 20, TMR_START, NULL);

			//����App�߳�
			GUI_UserAppStart();
	
		break;

		////////
		case	WM_TIMER:
		{
			u16 id;

			id = LOWORD(wParam);
			if (id == 1)
			{
				GUI_InputHandler(); //���������豸
			}
		}
		break;
				      
    
    /* �ͻ���������Ҫ������ */
		case	WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;
			_EraseBackgnd(hdc,NULL,hwnd);
		}
		return TRUE;

    /* �û������ĵ���Ϣ����ϵͳ���� */
		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

/**
  * @brief  ������������
  * @param  ��
  * @retval ��
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

	//�������洰��.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

	//��ʾ���洰��.
	ShowWindow(hwnd,SW_SHOW);

	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/

