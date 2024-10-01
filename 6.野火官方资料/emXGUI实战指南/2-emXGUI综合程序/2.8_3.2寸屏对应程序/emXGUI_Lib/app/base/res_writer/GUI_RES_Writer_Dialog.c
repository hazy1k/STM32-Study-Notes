
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "backend_res_mgr.h"
#include "GUI_AppDef.h"




/**********************�ֽ���*********************/

/* ����ؼ�ID */
#define ID_EXIT            0x3000
#define ID_BURN           0x3001
#define ID_RESET           0x3002
#define ID_INFO           0x3003
#define ID_TITLE          0x3004
#define ID_EXIT_INFO       0x3005
#define ID_PROGBAR		      0x3006

//����ؼ���˽����Ϣ(���пؼ���˽����ϢҪ�� WM_WIDGET ��ʼ����)
#define MSG_MYWRITE_RESULT WM_WIDGET+1 //��¼���

HWND wnd_res_writer_info_textbox ;
HWND wnd_res_writer_dialog;
HWND wnd_res_writer_progbar;

/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
extern BOOL res_not_found_flag;


/**
  * @brief  ��¼Ӧ���߳�
  */
static void App_FLASH_Writer(void )
{
  static int thread=0;
  
   //HDC hdc;
  u32 result;
   
	if(thread==0)
	{ 
    /* �����߳������Լ� */
    GUI_Thread_Create((void(*)(void*))App_FLASH_Writer,  /* ������ں��� */
                            "Flash writer",/* �������� */
                            3*1024,  /* ����ջ��С */
                            NULL, /* ������ں������� */
                            1,    /* ��������ȼ� */
                            10); /* ����ʱ��Ƭ����������֧�� */
    thread =1;

    return;
	}
	while(thread) //�߳��Ѵ�����
	{     
    result = (u32)BurnFile();
    
    //����Ϣ��wnd_res_writer_dialog,��¼���
    SendMessage(wnd_res_writer_dialog,MSG_MYWRITE_RESULT,result,0);

    thread = 0;       
	}
  /* ɾ���߳��Լ� */
  GUI_Thread_Delete(GUI_GetCurThreadHandle());
}

static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
  FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HDC hdc;
  RECT rc;
  WCHAR wbuf[128];

	hdc = ds->hDC;   
	rc = ds->rc; 
  
  SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
  FillRect(hdc, &rc);

  if (ds->Style & WS_DISABLED)
  {
    SetBrushColor(hdc, MapRGB(hdc, 100, 100, 100));
		SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
  }
  else if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
    InflateRect(&rc, -1, -1);
    SetBrushColor(hdc, MapRGB(hdc, 150, 150, 120));
    SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
    SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
		SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	}
  EnableAntiAlias(hdc, ENABLE);
  FillRoundRect(hdc, &rc, MIN(rc.h/2, rc.w/2));
  EnableAntiAlias(hdc, DISABLE);
  GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ������� 
  DrawText(hdc, wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)DT_CENTER
}

/**
  * @brief  ��¼Ӧ�ûص�����
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//	HDC hdc;
	RECT rc,rc0;
	HWND wnd;
  wnd_res_writer_dialog = hwnd;

	//static RECT rc_R, rc_G, rc_B;//RGB����ָʾ��
  
  const WCHAR no_res_info[] = L"   Some resources not found in the \r\nFLASH.\
Follow the instructions below:\r\n\r\n\
1.Insert an SD card with [srcdata] \r\n\resource.\
3.Power up again the board.\r\n\
2.Click the button below to load \r\n\
the resources.";
  
  const WCHAR normal_res_info[] = L"Please [Exit] if you don't know what\r\nto do!!!\
\r\nThis app is use to reload resources\r\n\
If you really want to reload resou-\r\nrces:\r\n\
1.Insert an SD card with [srcdata] \r\nresource.\
3.Power up again the board.\r\n\
2.Click the button below to load \r\n\
  the resources.";
  
  /* Ĭ����ʾ��Ϣ */
  const WCHAR *pStr = normal_res_info;

  PROGRESSBAR_CFG cfg;	

   //HDC hdc_mem2pic;
	switch (msg)
	{
    case WM_CREATE: {  
      
          /* �Ҳ�����Դʱ��ʾ��Ϣ */
          if(res_not_found_flag)
            pStr = no_res_info;
          
          GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.
          CopyRect(&rc0,&rc);
          
          /* �����ڴ�ֱ��Ϊ5�� */
      
          /* ���� */
          rc0.x = 0;
          rc0.y = 0;
          rc0.w = rc.w;
          rc0.h = rc.h/5;
          
          wnd = CreateWindow(TEXTBOX,L"GUI FLASH Writer" ,WS_VISIBLE,
                                rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_TITLE, NULL, NULL); 

          SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);          

          /* ���˳���ʾ */
          if(!res_not_found_flag)
          {  
            /* �˳���ť */
            rc0.w = 23;
            rc0.y = 10;
            rc0.x = rc.w - rc0.w - 5*2;
            rc0.h = 25;

            CreateWindow(BUTTON, L"Exit",BS_FLAT | WS_VISIBLE | WS_OWNERDRAW,
                          rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_EXIT, NULL, NULL); 
          }
          
          /* ��ʾ��Ϣ */
          rc0.x = 5;
          rc0.y = 1*rc.h/5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 3*rc.h/5+5;
      
          CreateWindow(TEXTBOX,pStr ,WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_INFO, NULL, NULL);

          /* ������ */
          rc0.x = 5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 20;
          rc0.y = 4*rc.h/5 - rc0.h-10;
 
          //PROGRESSBAR_CFG�ṹ��Ĵ�С
					cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
          //�������еĹ���
					cfg.fMask    = PB_CFG_ALL;
          //���ָ�ʽˮƽ����ֱ����
					cfg.TextFlag = DT_VCENTER|DT_CENTER;  

					wnd_res_writer_progbar = CreateWindow(PROGRESSBAR,L"",
                                  PBS_TEXT|PBS_ALIGN_LEFT,
                                  rc0.x, rc0.y, rc0.w, rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

          SendMessage(wnd_res_writer_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
          SendMessage(wnd_res_writer_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
          SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,0);

          /* ��¼��ť */
          rc0.w = 230;
          rc0.h = 25;
          rc0.y = rc.h - rc0.h - 10;
          rc0.x = (rc.w - rc0.w)/2;

          CreateWindow(BUTTON, L"Click me to load resources",BS_FLAT | WS_VISIBLE | WS_OWNERDRAW,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_BURN, NULL, NULL); 

          /* ��λ��ť */
          rc0.w = 230;
          rc0.h = 25;
          rc0.y = rc.h - rc0.h - 10;
          rc0.x = (rc.w - rc0.w)/2;
          
          CreateWindow(BUTTON, L"Click me to reset system",BS_FLAT | WS_OWNERDRAW,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_RESET, NULL, NULL); 
          break;
	}
    
   case MSG_MYWRITE_RESULT:
    {
      u32 result = wParam;

      /* ��¼ʧ�� */
      if(result)
      {
        EnableWindow(GetDlgItem(hwnd,ID_BURN),ENABLE);

      }
      else
      {
        /* ��¼�ɹ� */
        ShowWindow(GetDlgItem(hwnd,ID_RESET),SW_SHOW);
        ShowWindow(GetDlgItem(hwnd,ID_BURN),SW_HIDE);
        ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_HIDE);

        SetWindowText(wnd_res_writer_info_textbox,L"Load resources success!\r\n\r\nClick the button below to reset \r\nsystem!");

      }  
      

      break;
    }
 
	case WM_NOTIFY: {
      u16 code,  id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
		
      if(id == ID_EXIT && code == BN_CLICKED)
      {
         PostCloseMessage(hwnd);
      }
      
      if(id == ID_BURN && code == BN_CLICKED)
      {
         wnd_res_writer_info_textbox = GetDlgItem(hwnd,ID_INFO);
         App_FLASH_Writer();
        
         EnableWindow(GetDlgItem(hwnd,ID_BURN),DISABLE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT_INFO),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_SHOW);
      }
      
      if(id == ID_RESET && code == BN_CLICKED)
      {
         NVIC_SystemReset();
      }      
      
      
		break;
	}
  
  //�ػ��ƺ�����Ϣ
  case WM_DRAWITEM:
  {
     DRAWITEM_HDR *ds;
     ds = (DRAWITEM_HDR*)lParam;        
     if(ds->ID == ID_EXIT)
     {
        exit_owner_draw(ds);
        return TRUE;
     }
     else if(ds->ID == ID_BURN || ds->ID == ID_RESET)
     {
        button_owner_draw(ds);
        return TRUE;
     }
     
   }
  break;

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
			if(id== ID_INFO )
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(0,255,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;
			}
      else if(id == ID_TITLE)
      {
      	CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;

      }
      else if(id == ID_EXIT_INFO)
      {
      	CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
				cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
				return TRUE;

      }
//      else if(id == ID_BURN || id == ID_EXIT)
//      {
//        CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;
//				cr->TextColor =RGB888(255,0,0);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
//				cr->BorderColor =RGB888(0,0,0);//�߿���ɫ��RGB888��ɫ��ʽ)
//				return TRUE;
//      }

			else
			{
				return FALSE;
			}
      
   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc,rc0;
      GetClientRect(hwnd, &rc);

      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);

      /* ���� */
      rc0 = rc;

      rc0.x = 0;
      rc0.y = 0;
      rc0.w = rc.w;
      rc0.h = rc.h/5-1; 

      HLine(hdc, 0, rc0.h, GUI_XSIZE);

      rc0.h = 25;
      rc0.y = rc.h - rc0.h - 10;

      SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
      HLine(hdc, 0, rc0.y+rc0.h/2, GUI_XSIZE);

      return TRUE;
      
   }

	case	WM_PAINT: //������Ҫ�ػ���ʱ�����Զ��յ�����Ϣ.
	{	
      PAINTSTRUCT ps;
//      HDC hdc;//��Ļhdc
//      hdc = BeginPaint(hwnd, &ps); 
    BeginPaint(hwnd, &ps); 

		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_RES_Writer_Dialog(void *param)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

  /* ����Ĭ������ΪASCII �ڲ�flash�ֿ⣬��ֹ����ʱ���� */
  GUI_SetDefFont(defaultFontEn);  //����Ĭ�ϵ�����

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//����������
	hwnd = CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI FLASH Writer",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

  /* �ָ�����Ĭ������ */
  GUI_SetDefFont(defaultFont);  

  /* res_not_found_flagΪ��ʱ��res writerӦ���Ƕ��������̵߳ģ������˳�ʱҪɾ���Լ�
     res_not_found_flagΪ��ʱ��res writerӦ���ǿ������û��㿪��app����ɾ��app���߳�
     ������Լ�����Ҫ���е���
  */
  if(res_not_found_flag)
  {
    /* ���ֲ���ϵͳ���˳�������ʱ������ɾ���߳��Լ� */
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
  }
}
