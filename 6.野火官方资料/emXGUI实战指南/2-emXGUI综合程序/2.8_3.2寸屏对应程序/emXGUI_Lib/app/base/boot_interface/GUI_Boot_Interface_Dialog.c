
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"
#include "gui_font_port.h"
#include "emXGUI_JPEG.h"
#include <string.h>

/**********************�ֽ���*********************/

#define GUI_HOME_BACKGROUNG_PIC      "home_desktop.jpg"

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
HDC hdc_home_bk;

/**
  * @brief  ������Դ�߳�
  */

static void App_Load_Res(void )
{
  static int thread=0;

  if(thread==0)
  { 
    /* �����߳������Լ� */
    GUI_Thread_Create((void(*)(void*))App_Load_Res,  /* ������ں��� */
                        "Load Res",/* �������� */
                        8*1024,  /* ����ջ��С */
                        NULL, /* ������ں������� */
                        1,    /* ��������ȼ� */
                        10); /* ����ʱ��Ƭ����������֧�� */
    thread =1;
    return;
  }
  while(thread) //�߳��Ѵ�����
  { 
    HFONT hFont;

    /* �������嵽�ⲿSDRAM������defaultFont */    
    hFont = GUI_Init_Extern_Font();
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
#if 0
    if (Load_state != FALSE)
    {
      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      /* �������汳��ͼƬ */
      if (strstr(GUI_HOME_BACKGROUNG_PIC, "0:/") != NULL)
      {
        res = FS_Load_Content(GUI_HOME_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      }
      else
      {
        res = RES_Load_Content(GUI_HOME_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      }
      
      hdc_home_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      ClrDisplay(hdc_home_bk, NULL, 0);
      if(res)
      {
        /* ����ͼƬ���ݴ���JPG_DEC��� */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* �������ڴ���� */
        JPG_Draw(hdc_home_bk, 0, 0, dec);

        /* �ر�JPG_DEC��� */
        JPG_Close(dec);
      }
      else
      {
        Load_state = FALSE;
        GUI_ERROR("Failed to load home page background image.");
      }
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&jpeg_buf);
    }
#endif  
    //����Ϣ���������ڣ��ر�
    SendMessage(GUI_Boot_hwnd,WM_CLOSE,0,0);
    thread = 0;  

    /* ɾ���߳��Լ� */
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
  }
  return;
}

void LCD_BkLight(int on);

/**
  * @brief  ��������ص�����
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static  BITMAP png_bm;
  static  PNG_DEC *png_dec; 
  switch (msg)
  {  
    case WM_CREATE:
    {
      RECT rc,rc0;

      GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������
      CopyRect(&rc0,&rc);
      
      /* �����ڴ�ֱ��Ϊ2�� */

      /* ����ͼƬ���ݴ���PNG_DEC��� */
//      png_dec = PNG_Open((u8 *)bootlogo, bootlogo_size());
      /* ��ͼƬת����bitmap */
//      PNG_GetBitmap(png_dec, &png_bm);
      
      rc0.x = (rc.w - png_bm.Width)/2;
      rc0.y = rc.h/2 - png_bm.Height - 10;
      rc0.w = rc.w;
//      DrawBitmap(hdc, 250, 80, &png_bm, NULL); 
      
      OffsetRect(&rc0,0,png_bm.Height);
      rc0.x = 0;
      rc0.y = rc.h/2;
      rc0.h = 20;      
      rc0.w = rc.w;

      CreateWindow(TEXTBOX, L"emXGUI booting", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT1, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT1),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 

      OffsetRect(&rc0,0,rc0.h);

      CreateWindow(TEXTBOX, L"Load resource, please wait...", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT2, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT2),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 

//      OffsetRect(&rc0,0,rc0.h+10);

//      rc0.x = 10;
//      rc0.h = 15;
//      rc0.w = rc.w - 2*rc0.x;

//      //PROGRESSBAR_CFG�ṹ��Ĵ�С
//      cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
//      //�������еĹ���
//      cfg.fMask    = PB_CFG_ALL;
//      //���ָ�ʽˮƽ����ֱ����
//      cfg.TextFlag = DT_VCENTER|DT_CENTER;  

//      Boot_progbar = CreateWindow(PROGRESSBAR,L"Loading",
//                                     PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE,
//                                    rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

//      SendMessage(Boot_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
//      SendMessage(Boot_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
//      SendMessage(Boot_progbar,PBM_SET_RANGLE,TRUE, FONT_NUM);
//      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,0); 
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
      rc.y = rc.h/2 - png_bm.Height - 20;;
      /* ��ʾͼƬ */
//      DrawBitmap(hdc, rc.x, rc.y, &png_bm, NULL);  
      return TRUE;

    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);

      EndPaint(hwnd, &ps);
      LCD_BkLight(TRUE);
      break;
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

extern void	GUI_RES_Writer_Dialog(void *param);
extern void	GUI_DEMO_SlideWindow(void *p);
extern void	GUI_Board_App_Desktop(void *p);
extern void PhoneCallMonitorTask(void *p);
extern TaskHandle_t* CallCallMonitorHandle;    // ������������ƿ�

void	GUI_Boot_Interface_Dialog(void *param)
{

  WNDCLASS	wcex;
  MSG msg;

  /* ���ж��Ƿ���Ҫ����У׼ */
  #if(GUI_TOUCHSCREEN_EN & GUI_TOUCHSCREEN_CALIBRATE)
  {
    TS_CFG_DATA ts_cfg;
    TouchDev_LoadCfg(&ts_cfg); /*����У������(��������Ҫ)*/

    int i=0;
    while(TouchPanel_IsPenDown() || ts_cfg.rsv != 0)    // �������� 1S ����û�м�⵽У׼��־λ��ʼУ׼
    {
      GUI_msleep(100);
      if(i++>10)
      {
        ShowCursor(FALSE);
        #ifdef STM32F10X_HD
          LCD_BkLight(TRUE);
          TouchScreenCalibrate();
        #endif
        ShowCursor(TRUE);
        break;
      }
    }
  }
  #endif
  
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
//  {  
#if (GUI_APP_RES_WRITER_EN )  
    /* ��Ϊ����ΪTRUE�������� */
    //res_not_found_flag = TRUE; 
  
     if(res_not_found_flag)
     {
        GUI_INFO("�ⲿSPI FLASHȱ����Դ��������ʼ��¼��Դ����...");

        /* ���Ҳ�����Դ��������Դ��¼Ӧ�� */      
        GUI_Thread_Create(GUI_RES_Writer_Dialog,  /* ������ں��� */
                              "GUI_FLASH_WRITER",/* �������� */
                              3*1024,  /* ����ջ��С */
                              NULL, /* ������ں������� */
                              5,    /* ��������ȼ� */
                              10); /* ����ʱ��Ƭ����������֧�� */


     }
#endif     
     else
     {	
        /* �ҵ���Դ��������Ӧ��*/      
        GUI_Thread_Create(GUI_DEMO_SlideWindow,  /* ������ں��� */
                              "GUI Slide Window",/* �������� */
                              1024,  /* ����ջ��С */
                              NULL, /* ������ں������� */
                              6,    /* ��������ȼ� */
                              10); /* ����ʱ��Ƭ����������֧�� */
       GUI_Thread_Create(GUI_Board_App_Desktop,  /* ������ں��� */
                              "GUI_APP",/* �������� */
                              5*1024,  /* ����ջ��С */
                              NULL, /* ������ں������� */
                              10,    /* ��������ȼ� */
                              10); /* ����ʱ��Ƭ����������֧�� */
       
//       xTaskCreate(PhoneCallMonitorTask,       /* ������ں��� */
//                            "Phone_Call_Monitor",  /* �������� */
//                            2*1024/4,                /* ����ջ��С */
//                            NULL,                  /* ������ں������� */
//                            6,                     /* ��������ȼ� */
//                            CallCallMonitorHandle);                   /* ����ʱ��Ƭ����������֧�� */
     }   
//  } 
    /* ���ֲ���ϵͳ���˳�������ʱ������ɾ���߳��Լ� */
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
}
