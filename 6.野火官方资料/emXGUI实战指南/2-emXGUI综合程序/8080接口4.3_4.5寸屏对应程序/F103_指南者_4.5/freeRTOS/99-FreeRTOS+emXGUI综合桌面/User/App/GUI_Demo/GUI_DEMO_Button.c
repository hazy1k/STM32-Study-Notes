
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

/*===================================================================================*/

static int count = 0;

static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

//	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc =ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc =ds->rc;     //button�Ļ��ƾ�����.

	if(ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬

		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc,MapRGB(hdc,250,0,0));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
		SetBrushColor(hdc,MapRGB(hdc,30,150,30));
		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc,MapRGB(hdc,0,50,100));
	}

	FillRect(hdc,&rc); //�þ�����䱳��
//	DrawRect(hdc,&rc); //���������

	GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
  
  if (ds->ID == ID_BTN6)
  {
    SetTextColor(hdc,MapRGB(hdc,0,0,0));
    x_wsprintf(wbuf, L"%d", count);
    DrawText(hdc,wbuf,-1,&rc,DT_BOTTOM|DT_RIGHT);//��������(���ж��뷽ʽ)
  }

}


static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	RECT rc;

	switch(msg)
	{
		case	WM_CREATE:

				GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������
				
				CreateWindow(BUTTON,L"EXIT",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);

				//���þ��β��������ڴ�����ť
				rc.x =20;
				rc.y =30;
				rc.w =100;
				rc.h =40;

				//����Ĭ�Ϸ��İ�ť
				CreateWindow(BUTTON,L"Button1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN1,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10); //�����ƶ�����λ��(X�᲻��,Y��λ������rc.h+10������)
				//����������İ�ť(BS_3D)
				CreateWindow(BUTTON,L"Button2",BS_3D|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN2,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				//����ƽ����İ�ť(BS_FLAT)
				CreateWindow(BUTTON,L"Button3",BS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN3,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				//����Բ�Ƿ��İ�ť (BS_ROUND )
				CreateWindow(BUTTON,L"Button4",BS_ROUND|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN4,NULL,NULL);

				rc.x =150;
				rc.y =30;
				rc.w =120;
				rc.h =60;
				//����Բ�Ƿ��İ�ť(BS_ROUND+BS_NOTIFY)
				CreateWindow(BUTTON,L"Button5",BS_ROUND|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN5,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				//�����Ի��ư�ť(WS_OWNERDRAW)
				CreateWindow(BUTTON,L"Button6\r\nOwnerDraw",WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN6,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				//����Բ�Ƿ��İ�ť(BS_ROUND+WS_DISABLE)
				CreateWindow(BUTTON,L"Button7",BS_ROUND|WS_DISABLED|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN7,NULL,NULL);
        
        SetTimer(hwnd, 1, 1000, TMR_START, NULL);

				return TRUE;
				////////
        
      case WM_TIMER:
      {
        count++;
        InvalidateRect(GetDlgItem(hwnd, ID_BTN6), NULL, TRUE);    // ������� ID_BTN6 ��ť�ػ�
        InvalidateRect(hwnd, NULL, TRUE);    // ������� WM_PAINT ��Ϣ
      }  
			break; 


		case	WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;
//			NMHDR *nr=(NMHDR*)lParam;

			id  =LOWORD(wParam);
			code=HIWORD(wParam);

			if(id >= ID_BTN1 && id<= ID_BTN6)
			{
				if(code == BN_PUSHED)
				{ //��ť��������.
					GUI_Printf("Button PUSHED: ID:%04XH\r\n",id);
				}

				if(code == BN_CLICKED)
				{ //��ť������.
					GUI_Printf("Button UNPUSHED: ID:%04XH\r\n",id);
				}
			}

			if(id== ID_EXIT && code==BN_CLICKED)
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

			id =LOWORD(wParam);

			if(id== ID_BTN5) //ֻ�ı� BTN5����ɫ.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
				{
					cr->TextColor =RGB888(50,220,50);  //������ɫ��RGB32��ɫ��ʽ)
					cr->BackColor =RGB888(20,100,20);  //������ɫ��RGB32��ɫ��ʽ)
					cr->BorderColor =RGB888(30,30,30); //�߿���ɫ��RGB32��ɫ��ʽ)
				}
				else
				{
					cr->TextColor =RGB888(250,250,250);
					cr->BackColor =RGB888(200,0,0);
					cr->BorderColor =RGB888(50,50,50);
				}
				return TRUE;
			}
			else
			{
				//������ťʹ��ϵͳĬ�ϵ���ɫ���л��ƣ�����ֱ�ӷ���FALSE.
				return FALSE;
			}

		}
//		break;
		////

		case	WM_DRAWITEM:
		{
			/*�����ؼ�ָ����WS_OWNERDRAW�����ÿ���ڻ���ǰ����������ڷ���WM_DRAWITEM��Ϣ��
			 *  wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��DRAWITEM_HDR�Ľṹ��ָ�룬
			 *  ��ָ���Ա������һЩ�ؼ�������صĲ���.
		     */

			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;

			if(ds->ID >= ID_BTN1 && ds->ID<= ID_BTN6)
			{
				button_owner_draw(ds); //ִ���Ի��ư�ť
			}
			return TRUE;
		}
//		break;
		////

		case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
		{	PAINTSTRUCT ps;
      WCHAR wbuf[128];

			hdc	=BeginPaint(hwnd,&ps);

			SetTextColor(hdc,MapRGB(hdc,0,0,255));
			TextOut(hdc,8,4,L"Button Test:",-1);
      x_wsprintf(wbuf, L"%d", count);
      TextOut(hdc,100,4,wbuf,-1);

			EndPaint(hwnd,&ps);
			return	TRUE;
		}
		////
			
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Button(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;

		/////
		wcex.Tag 		    = WNDCLASS_TAG;

		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc; //������������Ϣ����Ļص�����.
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= NULL;//hInst;
		wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
		
		//����������
		hwnd	=CreateWindowEx(	NULL,
									&wcex,
									_T("GUI Demo - Button"),
									/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
									0,0,GUI_XSIZE,GUI_YSIZE,
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
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
