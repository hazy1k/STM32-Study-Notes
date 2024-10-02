#include "test.h"

extern void	Text_Create(void *param);

static 	 LRESULT  	desktop_proc2(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
    /* ���洴��ʱ,���������Ϣ,������������һЩ��ʼ������. */
		case	WM_CREATE:
	{
		/* ��ȡ�����ھ���. */
		RECT rc;
		HWND wnd;
		GetClientRect(hwnd,&rc);
		
		/* �����˳���ť. */
		CreateWindow(BUTTON, L"EXIT", WS_VISIBLE,rc.x,rc.y, 100, 40, hwnd, ID_EXIT, NULL, NULL);//�˳���ť
		/* ���������ı���ť. */
		CreateWindow(BUTTON, L"TEXT", WS_VISIBLE,rc.x+200,rc.y, 100, 40, hwnd, ID_TEXT, NULL, NULL);//�����ı���ť
		
		/* ����������. */
		PROGRESSBAR_CFG Pcfg;
		Pcfg.cbSize = sizeof(PROGRESSBAR_CFG);
		Pcfg.Rangle = 100;
		Pcfg.Value = 0;
		Pcfg.fMask    = PB_CFG_ALL;
		
		wnd = CreateWindow(PROGRESSBAR, L"PROGRESSBAR",WS_VISIBLE| PBS_FLAT|PBS_ALIGN_BOTTOM,rc.x+200,rc.y+40, 20, 200, hwnd, ID_PROGRESSBAR, NULL, NULL);//������

		SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&Pcfg);		
		
		/* ����������. */
		SCROLLINFO Scfg;
		Scfg.cbSize = sizeof(Scfg);
		Scfg.fMask =SIF_ALL;
		Scfg.nMin = 0;
		Scfg.nMax = 100;
		Scfg.nValue = 50;
		Scfg.TrackSize = 30;
		Scfg.ArrowSize = 30;

		wnd = CreateWindow(SCROLLBAR,L"VScroll",SBS_HORZ|WS_VISIBLE,\
                              0,240,200,30,hwnd,ID_SCROLLBAR1,NULL,NULL);
    SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&Scfg);
		
		/* ������ʱ��. */
		SetTimer(hwnd,ID_TMR1,1000,TMR_START,NULL);//��ʱ1S
		
		printf("�Ӵ��ڱ�������!\r\n");
	}
			break;

		/* �����ػ� */
		case	WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;//���ڴ�С
      GetClientRect(hwnd, &rc); //��ÿͻ�������.
      hdc = BeginPaint(hwnd, &ps);
      //����
      SetBrushColor(hdc, MapRGB(hdc, 255,255,255));
      FillRect(hdc, &rc); 
			EndPaint(hwnd,&ps);
			return TRUE;
		}
		
		case WM_TIMER:
		{
		  uint16_t id;
			id = LOWORD(wParam);
			if(id == ID_TMR1 && GetDlgItem(hwnd,ID_TEXT_SON)!= NULL)
			{
				HWND wnd;
				WCHAR wbuf[20];
				count++;
				x_wsprintf(wbuf,L"%d",count);
				wnd =GetDlgItem(hwnd,ID_TEXT_SON);//��ȡ�ı����ھ��
				SetWindowText(wnd,wbuf);//�����ı���ʾ
			}
			if(id == ID_TMR1 && GetDlgItem(hwnd,ID_PROGRESSBAR)!= NULL)
			{
				HWND wnd;
				wnd =GetDlgItem(hwnd,ID_PROGRESSBAR);//��ȡ���������
				if(count <= 100)
				{
				  SendMessage(wnd,PBM_SET_VALUE,TRUE,count);
				}
			}
		}
        
     /* ��Ϣ���� */    
    case WM_NOTIFY:
		{
      uint16_t code,id;
			
			id   = LOWORD(wParam);
			code = HIWORD(wParam);
			
			if(code == BN_CLICKED && id == ID_EXIT )
			{
				int ret;
				const WCHAR *btn[] ={L"YES",L"NO",};
				/* �����Ի���. */
    		MSGBOX_OPTIONS cfg;
    		cfg.ButtonCount = 2;
				cfg.pButtonText =btn;
	    	cfg.Flag = MB_ICONQUESTION | MB_BTN_WIDTH(80);
				
				ret=MessageBox(hwnd,10,20,320,150,L"CLOSE WINDOW?",L"MsgBox",&cfg);
				if(ret == 0)
				{
					PostCloseMessage(hwnd);
				}
			}
			
			if(code == BN_CLICKED && id == ID_TEXT )//����һ���ı������
			{
//		    PAINTSTRUCT ps;
//	    	HDC hdc;
		    RECT rc;//���ڴ�С
        GetClientRect(hwnd, &rc); //��ÿͻ�������.
			  CreateWindow(TEXTBOX, L"TEXT1", WS_VISIBLE,rc.x,rc.y+40, 200, 200, hwnd, ID_TEXT_SON, NULL, NULL);
				printf("�ı����ڱ�������!\r\n");
			}
		}
		break;

    /* �ͻ���������Ҫ������ */
		case	WM_ERASEBKGND:
		{   
      RECT rc = *(RECT*)lParam;
			HDC hdc =(HDC)wParam; 
      
        /* ������Դ������ɺ����ʾ�������棬�տ�ʼʱֻ��ʾ��ɫ */
       if(TRUE)
       {
          //_EraseBackgnd(hdc,NULL,hwnd);
       }
       else
       {
          SetBrushColor(hdc, MapRGB(hdc, 138, 255, 0));
          FillRect(hdc, &rc);
       }
		}
		return TRUE;  
	
		case WM_CLOSE:
 {
   DestroyWindow(hwnd); //??DestroyWindow???????(??????WM_DESTROY??)?
   return TRUE; 
 }
    /* �û������ĵ���Ϣ����ϵͳ���� */
		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

void	GUI_Boot_Interface_Dialog(void *param)
{
		HWND hwndhandle;
    WNDCLASS	wcex;
		MSG msg;

		wcex.Tag 		    = WNDCLASS_TAG;
		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= desktop_proc2;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= NULL;//hInst;
		wcex.hIcon			= NULL;
		wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

//  GUI_DEBUG("Create desktop");
	//�������洰��.
	
  	hwndhandle = CreateWindowEx(	//WS_EX_LOCKPOS
	                            NULL,
                              &wcex,
                              _T("This is a son Window!"),
                              WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
                              10,10,300,300,
                              NULL,0,NULL,NULL);

	//GUI_DEBUG("HWND_Desktop=%08XH\r\n",	hwnd);

	//��ʾ���洰��.
	ShowWindow(hwndhandle,SW_SHOW);
#if (GUI_SHOW_CURSOR_EN)
	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
	ShowCursor(TRUE);
#endif

  while(GetMessage(&msg,hwndhandle))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
	GUI_Thread_Delete(GUI_GetCurThreadHandle());
}

