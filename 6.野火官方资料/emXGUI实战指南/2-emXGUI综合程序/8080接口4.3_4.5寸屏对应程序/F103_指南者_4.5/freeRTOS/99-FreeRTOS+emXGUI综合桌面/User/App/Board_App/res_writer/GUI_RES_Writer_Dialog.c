
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "res_mgr.h"
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

HWND wnd_info_textbox ;
HWND wnd_res_writer_dialog;
HWND wnd_progbar;

/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
extern BOOL res_not_found_flag;


/**
  * @brief  ��¼Ӧ���߳�
  */
static void App_FLASH_Writer(void )
{
  static int thread=0;
	static TaskHandle_t h_flash;
  
   //HDC hdc;
  u32 result;
   
	if(thread==0)
	{  
//      h_flash=rt_thread_create("Flash writer",(void(*)(void*))App_FLASH_Writer,NULL,5*1024,1,5);
      xTaskCreate((TaskFunction_t )(void(*)(void*))App_FLASH_Writer,  /* ������ں��� */
                            (const char*    )"App_FLASH_Writer",/* �������� */
                            (uint16_t       )4*1024/4,  /* ����ջ��СFreeRTOS������ջ����Ϊ��λ */
                            (void*          )NULL,/* ������ں������� */
                            (UBaseType_t    )5, /* ��������ȼ� */
                            (TaskHandle_t  )&h_flash);/* ������ƿ�ָ�� */
      thread =1;
//      rt_thread_startup(h_flash);//�����߳�
      return;
	}
	while(thread) //�߳��Ѵ�����
	{     
    result = (u32)BurnFile();
    
    //����Ϣ��wnd_res_writer_dialog,��¼���
    SendMessage(wnd_res_writer_dialog,MSG_MYWRITE_RESULT,result,0);

    thread = 0;       

    GUI_Thread_Delete(h_flash);
//    rt_thread_delete

	}
  return;
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
  
  const WCHAR no_res_info[] = L"FLASH���Ҳ���ĳ����Դ�ļ�.\r\n\
�밴������˵�����в���:\r\n\r\n\
1.�������srcdata�ļ��е�SD��.\r\n\
2.���¸������ϵ�.\r\n\
3.��������İ�ť�Լ�����Դ.";
  
  const WCHAR normal_res_info[] = L"�������֪������ô��,�밴�˳�!!!\r\n\
��Ӧ�ó����������¼�����Դ\r\n\
����������¼�����Դ:\r\n\
1.�������srcdata�ļ��е�SD��.\r\n\
2.���¸������ϵ�.\r\n\
3.��������İ�ť�Լ�����Դ.";
  
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
      
          /* ���� */
          rc0.x = 0;
          rc0.y = 0;
          rc0.w = rc.w;
          rc0.h = rc.h/5;
          
          wnd = CreateWindow(TEXTBOX,L"FLASH��¼����" ,WS_VISIBLE,
                                rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_TITLE, NULL, NULL); 

          SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);


          /* ���˳���ʾ */
          if(!res_not_found_flag)
          {  
            /* �˳���ť */
            rc0.w = 60;
            rc0.x = rc.w - rc0.w - 5*2;
            rc0.h = 40;
            CreateWindow(BUTTON,L"�˳�",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);
//            CreateWindow(BUTTON, L"�˳�",BS_FLAT | WS_VISIBLE,
//                          /*rc0.x, rc0.y, rc0.w, rc0.h,*/rc.w-100,8,80,48, hwnd, ID_EXIT, NULL, NULL); 
          }
          
          /* ��ʾ��Ϣ */
          rc0.x = 5;
          rc0.y = 1*rc.h/5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 2*rc.h/5;
      
          CreateWindow(TEXTBOX,pStr ,WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_INFO, NULL, NULL); 

          /* ������ */
          rc0.x = 5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 36;
          rc0.y = 4*rc.h/5 - rc0.h-10;
          
          //PROGRESSBAR_CFG�ṹ��Ĵ�С
					cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
          //�������еĹ���
					cfg.fMask    = PB_CFG_ALL;
          //���ָ�ʽˮƽ����ֱ����
					cfg.TextFlag = DT_VCENTER|DT_CENTER;  

					wnd_progbar = CreateWindow(PROGRESSBAR,L"",
                                  PBS_TEXT|PBS_ALIGN_LEFT,
                                  rc0.x, rc0.y, rc0.w, rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

          SendMessage(wnd_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
			 SendMessage(wnd_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
          SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,0);

          /* ��¼��ť */
          rc0.x = 5;
          rc0.w = rc.w/2- rc0.x*2;
          rc0.h = 45;
          rc0.y = rc.h - rc0.h;       
          CreateWindow(BUTTON, L"�����˴�������¼��Դ",BS_FLAT | WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_BURN, NULL, NULL); 

          /* ��λ��ť */
          rc0.x = rc.w/2 +5;
          rc0.w = rc.w/2 - 5*2;
          rc0.h = 45;
          rc0.y = rc.h - rc0.h;
          CreateWindow(BUTTON, L"�����˴���λϵͳ",BS_FLAT ,
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

        SetWindowText(wnd_info_textbox,L"��¼��Դ�ɹ�!\r\n\r\n��������İ�ť�Ը�λϵͳ!");

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
//         rt_enter_critical();
         
         wnd_info_textbox = GetDlgItem(hwnd,ID_INFO);
         App_FLASH_Writer();
        
         EnableWindow(GetDlgItem(hwnd,ID_BURN),DISABLE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT_INFO),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_SHOW);

//         rt_exit_critical();

      }
      
      if(id == ID_RESET && code == BN_CLICKED)
      {
         NVIC_SystemReset();
      }      
      
      
		break;
	}

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
      RECT rc;
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      
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


void	GUI_RES_WRITER_DIALOG(void)
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

}
