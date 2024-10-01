
#include	"emXGUI.h"

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/

//����ؼ�ID
#define	ID_EXIT		0x1000
#define	ID_BTN1		0x1101
#define	ID_BTN2		0x1102
#define	ID_BTN3		0x1103
#define	ID_BTN4		0x1104
#define	ID_BTN5		0x1105
#define	ID_BTN6		0x1106
#define	ID_BTN7		0x1107
/*************************************/
#define ID_MYBTN1    0x1001
#define ID_MYBTN2    0x1002

//����ؼ���֪ͨ��
#define MYBTN_UP   0
#define MYBTN_DOWN 1

//����ؼ���˽����Ϣ(���пؼ���˽����ϢҪ�� WM_WIDGET ��ʼ����)
#define MSG_MYBTN_SET_STATE WM_WIDGET+0 //���ð�Ť״̬
#define MSG_MYBTN_GET_STATE WM_WIDGET+1 //��ð�Ť״̬
//����ؼ���״̬
#define BTN_STATE_PRESSED (1<<0)
//����ؼ�����չ���ݽṹ.
typedef struct
{
	u32 state; //���ڼ�¼��ť״̬.
}my_btn_ext;
//�ؼ��Ĵ��ڹ��̺���
static LRESULT	my_btn_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	my_btn_ext *btn_ex;
	btn_ex = (my_btn_ext*)GetWindowLong(hwnd, GWL_EXDATA_ADDR); //��ô�����չ��������
	switch (msg)
	{
	case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
	{
		btn_ex->state = 0;
	}
	return TRUE;
	case WM_LBUTTONDOWN: //������/�����ڴ����ڰ����ˡ�
	{
		btn_ex->state = ~btn_ex->state; //���� �����¡�״̬.
		NotifyParent(hwnd, MYBTN_DOWN); //��֪ͨ��Ϣ��������
		//ʹ�ô����ػ�.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	//		case WM_LBUTTONUP:  //������/�����ڴ����ڡ�̧���ˡ�
	//		{
	//			btn_ex->state  &= ~BTN_STATE_PRESSED; //��������¡�״̬.
	//			NotifyParent(hwnd,MYBTN_UP); //��֪ͨ��Ϣ��������
	//			//ʹ�ô����ػ�.
	//			InvalidateRect(hwnd,NULL,FALSE);
	//		}
	case MSG_MYBTN_SET_STATE: //���ð�Ť״̬
	{
		btn_ex->state = wParam;

		InvalidateRect(hwnd, NULL, FALSE); //�ı�״̬��Ӧ��Ҫ�ػ���һ��...
	}
	break;
	case MSG_MYBTN_GET_STATE: //��ð�Ť״̬
	{
		return btn_ex->state;
	}
	case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc;
		WCHAR wbuf[64];
		hdc = BeginPaint(hwnd, &ps); //��ʼ��ͼ
		GetWindowText(hwnd, wbuf, 64); //��ô�������.
		GetClientRect(hwnd, &rc);
		if (btn_ex->state)
		{ // �����¡�״̬
			SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
			SetPenColor(hdc, MapRGB(hdc, 100, 0, 0));
			SetBrushColor(hdc, MapRGB(hdc, 135, 206, 250));
		}
		else
		{	// �ǡ����¡�״̬
			SetTextColor(hdc, MapRGB(hdc, 0, 30, 30));
			SetPenColor(hdc, MapRGB(hdc, 0, 100, 100));
			SetBrushColor(hdc, MapRGB(hdc, 220, 220, 220));
		}
		DrawCircle(hdc, rc.x + rc.w / 4, rc.y + rc.w / 4, rc.w / 4 - 2);
		FillCircle(hdc, rc.x + rc.w / 4, rc.x + rc.w / 4, rc.w / 4);

		EndPaint(hwnd, &ps); //������ͼ
	}
	break;
	//		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
	//		{
	//			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ���.
	//		}
	default: //�û������ĵ���Ϣ,��ϵͳ����.
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}
//�ؼ��Ĵ��ڹ��̺���
static LRESULT	my_rectbtn_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	my_btn_ext *btn_ex;
	btn_ex = (my_btn_ext*)GetWindowLong(hwnd, GWL_EXDATA_ADDR); //��ô�����չ��������
	switch (msg)
	{
	case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
	{
		btn_ex->state = 0;
	}
	return TRUE;
	case WM_LBUTTONDOWN: //������/�����ڴ����ڰ����ˡ�
	{
		btn_ex->state |= BTN_STATE_PRESSED; //���� �����¡�״̬.
		NotifyParent(hwnd, MYBTN_DOWN); //��֪ͨ��Ϣ��������
		//ʹ�ô����ػ�.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	case WM_LBUTTONUP:  //������/�����ڴ����ڡ�̧���ˡ�
	{
		btn_ex->state &= ~BTN_STATE_PRESSED; //��������¡�״̬.
		NotifyParent(hwnd, MYBTN_UP); //��֪ͨ��Ϣ��������
		//ʹ�ô����ػ�.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	break;
	case MSG_MYBTN_SET_STATE: //���ð�Ť״̬
	{
		btn_ex->state = wParam;

		InvalidateRect(hwnd, NULL, FALSE); //�ı�״̬��Ӧ��Ҫ�ػ���һ��...
	}
	break;
	case MSG_MYBTN_GET_STATE: //��ð�Ť״̬
	{
		return btn_ex->state;
	}
	case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT rc;
		WCHAR wbuf[64];
		hdc = BeginPaint(hwnd, &ps); //��ʼ��ͼ
		GetWindowText(hwnd, wbuf, 64); //��ô�������.
		GetClientRect(hwnd, &rc);
		if (btn_ex->state&BTN_STATE_PRESSED)
		{ // �����¡�״̬
			SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
			SetPenColor(hdc, MapRGB(hdc, 100, 0, 0));
			SetBrushColor(hdc, MapRGB(hdc, 135, 206, 250));
		}
		else
		{
			// �ǡ����¡�״̬
			SetTextColor(hdc, MapRGB(hdc, 0, 30, 30));
			SetPenColor(hdc, MapRGB(hdc, 0, 100, 100));
			SetBrushColor(hdc, MapRGB(hdc, 220, 220, 220));
		}
		DrawRoundRect(hdc, &rc, 20);
		InflateRect(&rc, -1, -1); //���ε�ˮƽ����ֱ��Сһ������.
		FillRoundRect(hdc, &rc, 20);
		EndPaint(hwnd, &ps); //������ͼ
	}
	break;
	case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
	{
		return DestroyWindow(hwnd); //����DestroyWindow�������ٴ���.
	}
	default: //�û������ĵ���Ϣ,��ϵͳ����.
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return WM_NULL;
}
//����һ���ؼ���ṹʵ��.
const WNDCLASS my_roundbutton =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = my_btn_proc,
	.cbClsExtra = 0,
	.cbWndExtra = sizeof(my_btn_ext),
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};
//����һ���ؼ���ṹʵ��.
const WNDCLASS my_rectbutton =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = my_rectbtn_proc,
	.cbClsExtra = 0,
	.cbWndExtra = sizeof(my_btn_ext),
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};
/*===================================================================================*/

static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

	if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬

		SetBrushColor(hdc, MapRGB(hdc, 150, 200, 250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		SetPenColor(hdc, MapRGB(hdc, 250, 0, 0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 250, 0, 0));      //��������ɫ

	}
	else
	{ //��ť�ǵ���״̬
		SetBrushColor(hdc, MapRGB(hdc, 30, 150, 30));
		SetPenColor(hdc, MapRGB(hdc, 0, 250, 0));
		SetTextColor(hdc, MapRGB(hdc, 0, 50, 100));
	}

	FillRect(hdc, &rc); //�þ�����䱳��
	DrawRect(hdc, &rc); //���������
	if (ds->hwnd == GetFocus())
	{
		RECT rc;
		GetClientRect(ds->hwnd, &rc);
		CommFace_DrawFocusRect(hdc, &rc);
	}
	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

}


static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rc;

	switch (msg)
	{
	case	WM_CREATE:

		GetClientRect(hwnd, &rc); //��ô��ڵĿͻ�������

		CreateWindow(BUTTON, L"EXIT", WS_VISIBLE, rc.w - 100, 8, 80, 48, hwnd, ID_EXIT, NULL, NULL);

		//���þ��β��������ڴ�����ť
		rc.x = 20;
		rc.y = 50;
		rc.w = 100;
		rc.h = 40;
		CreateWindow(BUTTON, L"Button1", WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN1, NULL, NULL);
		//�����ƶ�����λ��(X�᲻��,Y��λ������rc.h+10������)
		OffsetRect(&rc, 0, rc.h + 10);
		//����������İ�ť(BS_3D)
		CreateWindow(BUTTON, L"Button2", BS_3D | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN2, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//����ƽ����İ�ť(BS_FLAT)
		CreateWindow(BUTTON, L"Button3", BS_FLAT | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN3, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//����Բ�Ƿ��İ�ť (BS_ROUND )
		CreateWindow(BUTTON, L"Button4", BS_ROUND | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN4, NULL, NULL);
		rc.x = 150;
		rc.y = 30;
		rc.w = 120;
		rc.h = 60;
		//����Բ�Ƿ��İ�ť(BS_ROUND+BS_NOTIFY)
		CreateWindow(BUTTON, L"Button5", BS_ROUND | BS_NOTIFY | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN5, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//�����Ի��ư�ť(WS_OWNERDRAW)
		CreateWindow(BUTTON, L"Button6\r\nOwnerDraw", WS_OWNERDRAW | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN6, NULL, NULL);

		OffsetRect(&rc, 0, rc.h + 10);
		//����Բ�Ƿ��İ�ť(BS_ROUND+WS_DISABLE)
		CreateWindow(BUTTON, L"Button7", BS_ROUND | WS_DISABLED | WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_BTN7, NULL, NULL);

		rc.y = 30;
		OffsetRect(&rc, rc.w + 30, 0);
		CreateWindow(&my_roundbutton, L"Button8", WS_VISIBLE,
			rc.x, rc.y, rc.w, rc.h, hwnd, ID_MYBTN1, NULL, NULL);
		OffsetRect(&rc, 0, rc.h + 10);
		CreateWindow(&my_rectbutton, L"Button9", WS_VISIBLE,
			rc.x, rc.y, rc.w / 2, rc.h, hwnd, ID_MYBTN2, NULL, NULL);
		return TRUE;
		////////
	case	WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
	{
		u16 code, id;
		//			NMHDR *nr=(NMHDR*)lParam;
		id = LOWORD(wParam);
		code = HIWORD(wParam);
		if (id >= ID_BTN1 && id <= ID_BTN6)
		{
			if (code == BN_PUSHED)
			{ //��ť��������.
				GUI_Printf("Button PUSHED: ID:%04XH\r\n", id);
			}

			if (code == BN_CLICKED)
			{ //��ť������.
				GUI_Printf("Button UNPUSHED: ID:%04XH\r\n", id);
			}
		}
		if (id == ID_EXIT && code == BN_CLICKED)
		{ // EXIT��ť����
			PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
		}
	}
	break;
	////
	case	WM_CTLCOLOR:
	{
		/* �ؼ��ڻ���ǰ���ᷢ�� WM_CTLCOLOR��������.
		 * wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��CTLCOLOR�Ľṹ��ָ��.
		 * �û�����ͨ������ṹ��ı�ؼ�����ɫֵ.�û��޸���ɫ�������践��TRUE������ϵͳ
		 * �����Ա��β���������ʹ��Ĭ�ϵ���ɫ���л���.
		 *
		 */
		u16 id;

		id = LOWORD(wParam);

		if (id == ID_BTN5) //ֻ�ı� BTN5����ɫ.
		{
			CTLCOLOR *cr;
			cr = (CTLCOLOR*)lParam;

			if (SendMessage(GetDlgItem(hwnd, id), BM_GETSTATE, 0, 0)&BST_PUSHED)
			{
				cr->TextColor = RGB888(50, 220, 50);  //������ɫ��RGB32��ɫ��ʽ)
				cr->BackColor = RGB888(20, 100, 20);  //������ɫ��RGB32��ɫ��ʽ)
				cr->BorderColor = RGB888(30, 30, 30); //�߿���ɫ��RGB32��ɫ��ʽ)
			}
			else
			{
				cr->TextColor = RGB888(250, 250, 250);
				cr->BackColor = RGB888(200, 0, 0);
				cr->BorderColor = RGB888(50, 50, 50);
			}
			return TRUE;
		}
		else
		{
			//������ťʹ��ϵͳĬ�ϵ���ɫ���л��ƣ�����ֱ�ӷ���FALSE.
			return FALSE;
		}
	}
	case	WM_DRAWITEM:
	{
		/*�����ؼ�ָ����WS_OWNERDRAW�����ÿ���ڻ���ǰ����������ڷ���WM_DRAWITEM��Ϣ��
		 *  wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��DRAWITEM_HDR�Ľṹ��ָ�룬
		 *  ��ָ���Ա������һЩ�ؼ�������صĲ���.
		 */
		DRAWITEM_HDR *ds;
		ds = (DRAWITEM_HDR*)lParam;
		if (ds->ID >= ID_BTN1 && ds->ID <= ID_BTN6)
		{
			button_owner_draw(ds); //ִ���Ի��ư�ť
		}
		return TRUE;
	}
	case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
	{
		PAINTSTRUCT ps;

		hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, MapRGB(hdc, 0, 0, 255));
		TextOut(hdc, 8, 4, L"Button Test:", -1);


		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return	WM_NULL;
}

void	GUI_DEMO_Button(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;
	//��1���֣�����wcex������
	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//��2���֣�����������
	hwnd = CreateWindowEx(NULL,
		&wcex,
		_T("GUI Demo - Button"),
		WS_CAPTION | WS_DLGFRAME | WS_BORDER | WS_CLIPCHILDREN,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//��3���֣���ʾ������
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
