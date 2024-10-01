
/*
 *GUI_DEMO_Messagebox.c
 *2018��4��21������8:31:34
 *
 */

#include <emXGUI.h>
/*============================================================================*/
//����ؼ�ID
#define	ID_OK		0x1000
#define	ID_BTN1		0x1001
#define	ID_BTN2		0x1002
#define	ID_BTN3		0x1003

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL);

			rc.x =20;
			rc.y =30;
			rc.w =100;
			rc.h =40;
			CreateWindow(BUTTON,L"Msgbox-1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN1,NULL,NULL);

			OffsetRect(&rc,rc.w+10,0);
			CreateWindow(BUTTON,L"Msgbox-2",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN2,NULL,NULL);

			OffsetRect(&rc,rc.w+10,0);
			CreateWindow(BUTTON,L"Msgbox-3",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN3,NULL,NULL);


		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;
			int ret=0;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_OK && code==BN_CLICKED)
			{
				PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�رմ���
			}

			if(id==ID_BTN1 && code==BN_CLICKED)
			{

				ret=MessageBox(hwnd,10,20,200,120,L"Hello!",L"MsgBox-1",NULL);
			}

			if(id==ID_BTN2 && code==BN_CLICKED)
			{
				const WCHAR *btn[] ={L"YES",L"NO",};
				MSGBOX_OPTIONS mb;
				mb.Flag =MB_BTN_WIDTH(80)|MB_ICONWARNING;
				mb.pButtonText =btn;
				mb.ButtonCount=2;

				ret=MessageBox(hwnd,10,20,320,150,L"MB_YESNO + MB_ICON\r\nABCDEFG",L"MsgBox-2",&mb);
			}

			if(id==ID_BTN3 && code==BN_CLICKED)
			{

				const WCHAR *btn[] ={L"Yes to all",L"ȡ��",L"ȷ��>>"};

				MSGBOX_OPTIONS mb;
				mb.Flag =MB_ICONINFORMATION;
				mb.pButtonText =btn;
				mb.ButtonCount=3;

				ret=MessageBox(hwnd,10,20,360,180,L"Auto size + MB_ICON\r\nMulti-Line",L"MsgBox-3",&mb);
			}
			GUI_Printf("Messagebox Return:%d\r\n",ret);

		}
		break;
		////

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
//			HDC hdc;

			BeginPaint(hwnd,&ps);

			////�û��Ļ�������...
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps);
		}
		break;
		////

		case WM_DESTROY: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
		{
			return PostQuitMessage(hwnd); //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
		}
//		break;
		////

		default:
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_Messagebox(void)
{
	HWND	hwnd;
	WNDCLASSEX	wcex;
	MSG msg;

	/////
	wcex.Tag 		    = WNDCLASSEX_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	//����������
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_DEMO-Messagebox"),
								/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,440,240,
								NULL,NULL,NULL,NULL);

	//��ʾ������
	ShowWindow(hwnd,SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
