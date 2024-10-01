
#include <emXGUI.h>
#include <x_libc.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT			0x1000
#define	ID_PROGBAR1		0x1100
#define	ID_PROGBAR2		0x1101
#define	ID_PROGBAR3		0x1102

/*===================================================================================*/

static void progressbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc,m_rc[2];
//	int range,val;
	WCHAR wbuf[128];
	PROGRESSBAR_CFG cfg;
	hwnd =ds->hwnd;
	hdc =ds->hDC;
   /*************��һ��***************/
   //��ȡ�ͻ�������λ�ã���С
	GetClientRect(hwnd,&rc);
   //���ý������ı�����ɫ
	SetBrushColor(hdc,MapRGB(hdc,150,200,250));
   //���������ı���
	FillRect(hdc,&ds->rc);   
   //���û�����ɫ
	SetPenColor(hdc,MapRGB(hdc,100,10,10));
   //���ƽ������ı����߿�
   DrawRect(hdc,&rc);
   /*************�ڶ���***************/	
   cfg.cbSize =sizeof(cfg);
	cfg.fMask =PB_CFG_ALL;
	SendMessage(hwnd,PBM_GET_CFG,0,(LPARAM)&cfg);
   //���ɽ���������
	MakeProgressRect(m_rc,&rc,cfg.Rangle,cfg.Value,PB_ORG_BOTTOM);
   //���ý���������ɫ
	SetBrushColor(hdc,MapRGB(hdc,250,10,10));
   //��������
	FillRoundRect(hdc,&m_rc[0],2);

   //���ƽ������ı߿򣬲���Բ�Ǳ߿�
	DrawRoundRect(hdc,&m_rc[0],2);
   /************��ʾ����ֵ****************/
	x_wsprintf(wbuf,L"%d",cfg.Value);
	//InflateRect(&rc,40,0);
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);
}


static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	HDC hdc;
	RECT rc;
//	WCHAR wbuf[128];
//	int i,x,y;
	HWND wnd;
   PROGRESSBAR_CFG cfg;	
   static u8 pb1_val=0;
   static u8 pb2_val=0;
	switch(msg)
	{
		case WM_DESTROY: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
		{
         pb1_val = pb2_val = 0;
			return PostQuitMessage(hwnd); //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
		}
		case	WM_CREATE:
      {  		
				GetClientRect(hwnd,&rc);	
				if(1)
				{
               //PROGRESSBAR_CFG�ṹ��Ĵ�С
					cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
               //�������еĹ���
					cfg.fMask    = PB_CFG_ALL;
               //���ָ�ʽˮƽ����ֱ����
					cfg.TextFlag = DT_VCENTER|DT_CENTER;  
               // �����Ի��ƵĽ�����VProgressbar
					wnd = CreateWindow(PROGRESSBAR,L"VProgressbar",
                                  WS_OWNERDRAW|PBS_ALIGN_BOTTOM|WS_VISIBLE,
                                  20,20,48,320,hwnd,ID_PROGBAR1,NULL,NULL);
               //��ʼ��PROGRESSBAR_CFG�ṹ��
					SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
					SendMessage(wnd,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
               // �����������������Ľ�����VProgressbar
					wnd = CreateWindow(PROGRESSBAR,L"PROGBAR2 & Right align",
                                  PBS_TEXT|PBS_ALIGN_RIGHT|WS_VISIBLE,
                                  100,100,280,48,hwnd,ID_PROGBAR2,NULL,NULL);
					SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
					SendMessage(wnd,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
               //�����������������Ľ�����VProgressbar
					wnd = CreateWindow(PROGRESSBAR,L"PROGBAR3 & Left align",
                                  PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE,
                                  100,200,280,48,hwnd,ID_PROGBAR3,NULL,NULL);
					SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
					SendMessage(wnd,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
				}
            //����EXIT��ť
				CreateWindow(BUTTON,L"EXIT",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);
            //����200ms��ʱ��
				SetTimer(hwnd,1,200,TMR_START,NULL);
				return TRUE;
         }		
		case	WM_TIMER:
		{
			pb1_val +=1;
			if(pb1_val > 100)
			{
				pb1_val =0;
			}
			wnd =GetDlgItem(hwnd,ID_PROGBAR1);
         //���ý���ֵ
			SendMessage(wnd,PBM_SET_VALUE,TRUE,pb1_val);
			pb2_val +=2;
			if(pb2_val > 100)
			{
				pb2_val =0;
			}
         //��ȡ���ھ��
			wnd =GetDlgItem(hwnd,ID_PROGBAR2);
			SendMessage(wnd,PBM_SET_VALUE,TRUE,pb2_val);
         
			wnd =GetDlgItem(hwnd,ID_PROGBAR3);
			SendMessage(wnd,PBM_SET_VALUE,TRUE,pb2_val);
         return TRUE;
		}
		case	WM_NOTIFY:
		{
			u16 code,id;
			id  =LOWORD(wParam);
			code=HIWORD(wParam);               
			if(id== ID_EXIT && code==BN_CLICKED)
			{ // EXIT��ť����
				PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
			}
         break;
		}
		
		case	WM_CTLCOLOR:
		{
			u16 id;
			CTLCOLOR *cr;

			id =wParam;
			cr =(CTLCOLOR*)lParam;
			if(id==ID_PROGBAR3)
			{
				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =RGB888(0,0,0);
				cr->BorderColor =RGB888(130,30,130);
				cr->ForeColor  =RGB888(200,0,200);
				return TRUE;
			}
			return FALSE;
		}
		case	WM_ERASEBKGND:
		{
         return TRUE;
		}
		case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
		{	
         PAINTSTRUCT ps; 
         HDC hdc;		
         GetClientRect(hwnd,&rc);
         hdc	=BeginPaint(hwnd,&ps);
         SetBrushColor(hdc,MapRGB(hdc,0,0,139));
         FillRect(hdc,&rc);         
			EndPaint(hwnd,&ps);
			return	TRUE;
		}			
		case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;
			ds =(DRAWITEM_HDR*)lParam;
			progressbar_owner_draw(ds);
			return TRUE;
		}
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Progressbar(void)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;
		////��1���֣�����wcex������
		wcex.Tag 		    = WNDCLASS_TAG;
		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
		//��2���֣�����������
		hwnd	=CreateWindowEx(NULL,
									&wcex,
									_T("GUI Demo - Progressbar"),
									WS_CLIPCHILDREN,
									0,0,GUI_XSIZE,GUI_YSIZE,
									NULL,NULL,NULL,NULL);
		//��3���֣���ʾ������
		ShowWindow(hwnd,SW_SHOW);
      //������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��   
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
