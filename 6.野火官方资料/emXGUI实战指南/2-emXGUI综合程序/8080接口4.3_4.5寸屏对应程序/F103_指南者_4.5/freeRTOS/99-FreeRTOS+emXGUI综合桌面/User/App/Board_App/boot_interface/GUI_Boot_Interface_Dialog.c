
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"
#include "gui_font_port.h"


/**********************�ֽ���*********************/

BOOL Load_state = FALSE;
HWND GUI_Boot_hwnd;
/* ����ؼ�ID */
enum eID
{
  ID_LOGO,
  ID_TEXT1,
  ID_TEXT2,
  ID_PROGBAR,
};
/* �ⲿͼƬ���� */
extern char bootlogo[];
/* �ⲿͼƬ���ݴ�С */
extern unsigned int bootlogo_size(void);
HWND Boot_progbar = NULL;

/**
  * @brief  ������Դ�߳�
  */
static void App_Load_Res(void )
{
  static int thread=0;
  static rt_thread_t h_load;

  if(thread==0)
  {  
    h_load=rt_thread_create("Load Res",(void(*)(void*))App_Load_Res,NULL,5*1024,1,5);
    thread =1;
    rt_thread_startup(h_load);//�����߳�
    return;
  }
  while(thread) //�߳��Ѵ�����
  {     
    HFONT hFont;
 
    /* �������嵽�ⲿSDRAM������defaultFont */    
    hFont = GUI_Extern_FontInit();
    if(hFont==NULL)
    {
      GUI_ERROR("GUI Extern Default Font Init Failed.");

      Load_state = FALSE;
    }
    else
    {   
      Load_state = TRUE;
      /* ����Ĭ������ */
      GUI_SetDefFont(hFont);  
    }    
    
    //����Ϣ���������ڣ��ر�
    SendMessage(GUI_Boot_hwnd,WM_CLOSE,0,0);
    thread = 0;       

    rt_thread_delete(h_load);

  }
  return;
}
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
	SetBrushColor(hdc,MapRGB(hdc,255,255,255));
   //���������ı���
	FillRoundRect(hdc,&ds->rc, MIN(rc.w,rc.h)/2);   
//   //���û�����ɫ
	//SetPenColor(hdc,MapRGB(hdc,100,10,10));
//   //���ƽ������ı����߿�
//   DrawRect(hdc,&rc);
   /*************�ڶ���***************/	
  cfg.cbSize =sizeof(cfg);
	cfg.fMask =PB_CFG_ALL;
	SendMessage(hwnd,PBM_GET_CFG,0,(LPARAM)&cfg);
   //���ɽ���������
	MakeProgressRect(m_rc,&rc,cfg.Rangle,cfg.Value,PB_ORG_LEFT);
   //���ý���������ɫ
	SetBrushColor(hdc,MapRGB(hdc,210,10,10));
   //��������
  //GUI_DEBUG("%d %d %d %d", m_rc[0].x, m_rc[0].y,m_rc[0].w, m_rc[0].h);
	FillRoundRect(hdc,&m_rc[0],MIN(rc.w,rc.h)/2);

   //���ƽ������ı߿򣬲���Բ�Ǳ߿�
	DrawRoundRect(hdc,&m_rc[0],MIN(rc.w,rc.h)/2);
   /************��ʾ����ֵ****************/
	
	//InflateRect(&rc,40,0);
	//DrawText(hdc,L"������...",-1,&rc,DT_CENTER);
}
/**
  * @brief  ��������ص�����
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static  BITMAP png_bm;
  static  PNG_DEC *png_dec; 
  PROGRESSBAR_CFG cfg;
  switch (msg)
  {  
    case WM_CREATE:
    {
      RECT rc,rc0;

      GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������
      CopyRect(&rc0,&rc);
      
      /* �����ڴ�ֱ��Ϊ2�� */

      /* ����ͼƬ���ݴ���PNG_DEC��� */
      png_dec = PNG_Open((u8 *)bootlogo, bootlogo_size());
      /* ��ͼƬת����bitmap */
      PNG_GetBitmap(png_dec, &png_bm);
      
      rc0.x = (rc.w - png_bm.Width)/2;
      rc0.y = rc.h/2 - png_bm.Height - 10;
      rc0.w = rc.w;
//      DrawBitmap(hdc, 250, 80, &png_bm, NULL); 
      
      OffsetRect(&rc0,0,png_bm.Height);
      rc0.x = 0;
      rc0.y = rc.h/2;
      rc0.h = 35;      
      rc0.w = rc.w;

      CreateWindow(TEXTBOX, L"ϵͳ������", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT1, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT1),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 

      OffsetRect(&rc0,0,rc0.h);

      CreateWindow(TEXTBOX, L"�����ⲿFLASH�����ֿ⵽SDRAM", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT2, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT2),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 

      OffsetRect(&rc0,0,rc0.h+15);

      
      rc0.h = 35;
      rc0.w = 360;
      rc0.x = 400-rc0.w/2;
      //PROGRESSBAR_CFG�ṹ��Ĵ�С
      cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
      //�������еĹ���
      cfg.fMask    = PB_CFG_ALL;
      //���ָ�ʽˮƽ����ֱ����
      cfg.TextFlag = DT_VCENTER|DT_CENTER;  

      Boot_progbar = CreateWindow(PROGRESSBAR,L"Loading",
                                     PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE|WS_OWNERDRAW,
                                    rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

      SendMessage(Boot_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
      SendMessage(Boot_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
      SendMessage(Boot_progbar,PBM_SET_RANGLE,TRUE, FONT_NUM);
      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,0); 
      SetTimer(hwnd, 1, 20, TMR_SINGLE|TMR_START, NULL);
      
      break;
    }
    
    case WM_TIMER:
    {
      /* �������洴����timerʱ���ſ�ʼ���� */
      App_Load_Res();
      break;         
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;

      GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������

      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);    
      
      rc.x = (rc.w - png_bm.Width)/2;
      rc.y = rc.h/2 - png_bm.Height - 10;;
      /* ��ʾͼƬ */
      DrawBitmap(hdc, rc.x, rc.y, &png_bm, NULL);  
      return TRUE;

    }

		case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;
			ds =(DRAWITEM_HDR*)lParam;
			progressbar_owner_draw(ds);
			return TRUE;
		}
    case	WM_CTLCOLOR:
    {
      u16 id;
      CTLCOLOR *cr;
      id =LOWORD(wParam);				
      cr =(CTLCOLOR*)lParam;
      if(id == ID_TEXT1 || id == ID_TEXT2)
      {
        cr->TextColor =RGB888(255,255,255);//������ɫ��RGB888��ɫ��ʽ)
        cr->BackColor =RGB888(0,0,0);//������ɫ��RGB888��ɫ��ʽ)
        //cr->BorderColor =RGB888(255,10,10);//�߿���ɫ��RGB888��ɫ��ʽ)
        return TRUE;
      }

      break;
    }  
    case WM_CLOSE: //��������ʱ�����Զ���������Ϣ����������һЩ��Դ�ͷŵĲ���.
    {
      /* �ر�PNG_DEC��� */
      PNG_Close(png_dec);
      DestroyWindow(hwnd); 
      Boot_progbar = NULL;
      return TRUE; //����PostQuitMessage��ʹ�������ڽ������˳���Ϣѭ��.
    }      
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return	WM_NULL;                                     
}

extern void 	GUI_Board_App_Desktop(void);
extern void	GUI_RES_WRITER_DIALOG(void);
extern void	GUI_DEMO_SlideWindow(void);

void	GUI_Boot_Interface_DIALOG(void)
{

  WNDCLASS	wcex;
  MSG msg;
  
  /* ��Ĭ������ΪASCII �ڲ�flash�ֿ⣬��ֹ���� */
  GUI_SetDefFont(defaultFontEn);  

  wcex.Tag 		    = WNDCLASS_TAG;

  wcex.Style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= win_proc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= NULL;//hInst;
  wcex.hIcon			= NULL;
  wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

  //����������ʾ
  GUI_Boot_hwnd = CreateWindowEx(	WS_EX_LOCKPOS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"Booting",
                              WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);


  //��ʾ����.
  ShowWindow(GUI_Boot_hwnd,SW_SHOW);

  while(GetMessage(&msg,GUI_Boot_hwnd))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  /* ���������ڼ�������Դ���رգ�ִ�����´��룬����Ӧ���߳� */
  {
     rt_thread_t h;             
     
     if(res_not_found_flag)
     {
        GUI_INFO("�ⲿSPI FLASHȱ����Դ��������ʼ��¼��Դ����...");

        /* ���Ҳ�����Դ��������Դ��¼Ӧ�� */
        h=rt_thread_create("GUI_FLASH_WRITER",GUI_RES_WRITER_DIALOG,NULL,8*1024,5,5);
        rt_thread_startup(h);			

     }
     else
     {	
        /* �ҵ���Դ��������Ӧ��*/ 
     
        h=rt_thread_create("GUI_APP",GUI_Board_App_Desktop,NULL,8*1024,5,5);
        rt_thread_startup(h);			
        h=rt_thread_create("GUI_SLIDE_WIN",GUI_DEMO_SlideWindow,NULL,4096,5,5);
        rt_thread_startup(h);
     }   
  } 


}
