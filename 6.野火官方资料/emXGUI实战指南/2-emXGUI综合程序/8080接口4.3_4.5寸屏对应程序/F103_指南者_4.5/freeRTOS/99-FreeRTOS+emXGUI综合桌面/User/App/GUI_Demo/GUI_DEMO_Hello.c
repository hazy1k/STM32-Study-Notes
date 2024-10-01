
/*
 *GUI_DEMO_Hello.c
 *2018��5��25������1:27:05
 *
 */

#include <emXGUI.h>
/*============================================================================*/
//����ؼ�ID
#define	ID_OK		0x1000

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //����һ����ť(ʾ��).
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_OK && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
		}
		break;
		////

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...

			SetTextColor(hdc,MapRGB(hdc,250,10,10)); //����������ɫ.
			TextOut(hdc,10,10,L"Hello! emXGUI Test",-1); //�������

			SetPenColor(hdc,MapRGB(hdc,0,0,250)); //���û�����ɫ(���ڻ��ߣ���).

			//���þ��β���
			rc.x =10;
			rc.y =50;
			rc.w =100;
			rc.h =60;
			DrawRect(hdc,&rc); //����һ�����ľ���

			DrawCircle(hdc,80,120,50); //����һ������Բ.

			Line(hdc,10,150,120,200); //����

			SetBrushColor(hdc,MapRGB(hdc,0,200,0)); //���û�ˢ��ɫ(�������).

			//���þ��β���
			rc.x =150;
			rc.y =50;
			rc.w =120;
			rc.h =60;
			FillRect(hdc,&rc); //���һ��ʵ�ľ���

			OffsetRect(&rc,0,rc.h+4);  //�ƶ�����λ��
			FillRoundRect(hdc,&rc,15); //���һ��ʵ��Բ�Ǿ���.

			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
		////

		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{

			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
//		break;
		////

		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_Hello(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
//	wcex.hIconSm		= NULL;

	//����������
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("emXGUI Window"), //��������
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,320,240,    //����λ�úʹ�С
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
