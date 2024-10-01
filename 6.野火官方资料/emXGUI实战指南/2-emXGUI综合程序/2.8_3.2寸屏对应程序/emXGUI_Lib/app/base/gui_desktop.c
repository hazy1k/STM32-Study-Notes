/**
  *********************************************************************
  * @file    desktop.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI ����
  *********************************************************************
  * ʵ��ƽ̨:Ұ�� F429-��ս�� STM32 ������
  * ����    :www.embedfire.com
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
***/ 


#include "GUI_AppDef.h"
#include "emXGUI.h"


/* �ⲿ��Դ������ɱ�־ */
BOOL Load_state = FALSE;

/*===================================================================================*/
extern void	GUI_Boot_Interface_Dialog(void *param);
extern void GUI_AppMain(void);

void LCD_BkLight(int on);
void	gui_app_thread(void *p)
{
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
          #ifdef  STM32F10X_HD
            LCD_BkLight(TRUE);
            TouchScreenCalibrate();
          #endif
    			ShowCursor(TRUE);
    			break;
    		}
    	}
    }
    #endif

    while(1)
    {
      GUI_msleep(500);
    }
}

/*===================================================================================*/

/**
  * @brief  ���汳�����ƺ�����������������ʱ����ñ�������
            ͨ���޸ı����������ݿɸ�������ı���
  * @param  hdc ��ͼ������
  * @param  lprc Ҫ���Ƶľ�������ΪNULLʱ���Զ�����hwnd�Ŀͻ���
  * @param  hwnd ���ڶ�����
  * @retval ��
  */
static	void	_EraseBackgnd(HDC hdc,const RECT *lprc,HWND hwnd)
{
	RECT rc;

	if(lprc==NULL)
	{
		GetClientRect(hwnd,&rc);
	}
	else
	{
		CopyRect(&rc,lprc);
	}

  if (Theme_Flag == 0) 
  {
//    BitBlt(hdc, rc.x, rc.h - HEAD_INFO_HEIGHT, rc.w, HEAD_INFO_HEIGHT, hdc_home_bk, rc.x, rc.h - HEAD_INFO_HEIGHT, SRCCOPY);
//    BitBlt(hdc, rc.x, rc.h - HEAD_INFO_HEIGHT, rc.w, HEAD_INFO_HEIGHT, hdc, rc.x, rc.h - HEAD_INFO_HEIGHT, SRCCOPY);
		SetBrushColor(hdc,MapRGB(hdc,32,72,144));
		FillRect(hdc,&rc);
	}
  else if (Theme_Flag == 1)
  {
    SetBrushColor(hdc,MapRGB(hdc,82,85,82));
    FillRect(hdc,&rc);
  }
  else
  {
    SetBrushColor(hdc, MapRGB(hdc, 100, 100, 100));
    FillRect(hdc, &rc);
  }

  SetTextColor(hdc,MapRGB(hdc,255,255,255));
  
//  SetFont(hdc, iconFont_100);
//	DrawText(hdc,L" A B C D E \r\n F G H I J",-1,&rc,DT_LEFT|DT_VCENTER);
//  SetFont(hdc, GB2312_32_Font);
  
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
  rc.y +=10;
//  DrawText(hdc,L"emXGUI@Embedfire STM32F103 ",-1,&rc,DT_CENTER);
    
  /* ���� */
  // GetClientRect(hwnd,&rc);
  // SetBrushColor(hdc,MapRGB(hdc,82,85,82));
  // rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  // rc.h = HEAD_INFO_HEIGHT;
  // FillRect(hdc,&rc);
  
    /* ���� */ 
  SetFont(hdc, logoFont);
  /* ��ʾlogo */
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT-2;
  rc.h = HEAD_INFO_HEIGHT;
  
  SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
  DrawText(hdc,L"B",-1,&rc,DT_LEFT|DT_VCENTER);
  
  
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT-2;
  rc.h = HEAD_INFO_HEIGHT;

  /* �ָ�Ĭ������ */
  SetFont(hdc, defaultFont);
  rc.x += 20;
  DrawText(hdc,L"Ұ��@emXGUI",-1,&rc,DT_LEFT|DT_VCENTER);

  GetClientRect(hwnd,&rc);
  rc.w = 52;
  rc.x = GUI_XSIZE/2 - rc.w/2;
  rc.h = 20;
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT+9;
  
  
  /* ����ͼ������ */
  SetFont(hdc, controlFont_32);

  /* ����ͼ�� */
  SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
  DrawText(hdc,L"f",-1,&rc,DT_LEFT|DT_VCENTER);
  
  SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
  EnableAntiAlias(hdc, ENABLE);
  DrawRoundRect(hdc, &rc, MIN(rc.w, rc.h)>>1);
  EnableAntiAlias(hdc, DISABLE);

 /* �ָ�Ĭ������ */
  SetFont(hdc, defaultFont);
//  OffsetRect(&rc,0,-3);
  DrawText(hdc,L" ˵��",-1,&rc,DT_CENTER|DT_VCENTER);
  

//  rc.y -= 20;
//  DrawText(hdc,L"\r\n\r\n��ϸ",-1,&rc,DT_BOTTOM|DT_CENTER);
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;

  DrawText(hdc,L"www.embedFire.com",-1,&rc,DT_RIGHT|DT_VCENTER);  

}

/* ʹ��ר�õ��߳����������� */
#if 0
static	int	gui_input_thread(void *p)
{
	while(1)
	{
		GUI_InputHandler(); //���������豸
		GUI_msleep(20);
	}
}
#endif


/**
  * @brief  ����ص�����
  * @param  hwnd ��ǰ�������Ϣ�Ĵ��ڶ�����
  * @param  msg ��Ϣ����ֵ�����Ա�ʶ�����ֵ�ǰ��������Ϣ
  * @param  wParam ��Ϣ����ֵ������msg��Ϣ����ֵ��ͬ
  * @param  lParam ��Ϣ����ֵ������msg��Ϣ����ֵ��ͬ
  * @retval ���ظ�SendMessage��ֵ
  */
static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
    /* ���洴��ʱ,���������Ϣ,������������һЩ��ʼ������. */
		case	WM_CREATE:	
			   //����1��20ms��ʱ��������ѭ���¼�.
				 SetTimer(hwnd,1,25,TMR_START,NULL);

				//����App�߳�						
				{
#if (GUI_APP_BOOT_INTERFACE_EN )  
          
         GUI_Thread_Create(GUI_Boot_Interface_Dialog,  /* ������ں��� */
                              "Boot_Interface",/* �������� */
                              2*1024,  /* ����ջ��С */
                              NULL, /* ������ں������� */
                              5,    /* ��������ȼ� */
                              10); /* ����ʱ��Ƭ����������֧�� */
#else
          
        GUI_Thread_Create(gui_app_thread,  /* ������ں��� */
                            "GUI_APP",/* �������� */
                            2*1024,  /* ����ջ��С */
                            NULL, /* ������ں������� */
                            5,    /* ��������ȼ� */
                            10); /* ����ʱ��Ƭ����������֧�� */    
#endif
				}

				break;

		/* ��ʱ���������豸����Ϣ */
		case	WM_TIMER:
      #if(GUI_INPUT_DEV_EN)
        {
          u16 id;

          id =LOWORD(wParam);
          if(id==1)
          {
            GUI_InputHandler(); //���������豸
          }
        }
      #endif
		break;
        
    /* ����Ƿ���������ϸ��һ�� */    
    case WM_LBUTTONDOWN:
		{

			POINT pt;
			RECT rc;

			pt.x =GET_LPARAM_X(lParam);
			pt.y =GET_LPARAM_Y(lParam);

			GetClientRect(hwnd,&rc);
      
      rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
      rc.h = HEAD_INFO_HEIGHT;          

      /* ����������������Ϣ��slide window */
			if(PtInRect(&rc,&pt))
			{
        PostMessage(GetDlgItem(hwnd,ID_SLIDE_WINDOW), WM_MSG_FRAME_DOWN,0,0);
			}
		}
		break;

    /* �ͻ���������Ҫ������ */
		case	WM_ERASEBKGND:
		{   
      RECT rc = *(RECT*)lParam;
      HDC hdc =(HDC)wParam; 
      /* ������Դ������ɺ����ʾ�������棬�տ�ʼʱֻ��ʾ��ɫ */
      if(Load_state == TRUE)
      {
        _EraseBackgnd(hdc,NULL,hwnd);
        
      }
      else
      {
        SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
        FillRect(hdc, &rc);
      }
      
		}
		return TRUE;  

    /* �û������ĵ���Ϣ����ϵͳ���� */
		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

/**
  * @brief  ������������
  * @param  ��
  * @retval ��
  */
void GUI_DesktopStartup(void)
{
	WNDCLASS	wcex;
	HWND hwnd;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;
	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= desktop_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

//  GUI_DEBUG("Create desktop");
	//�������洰��.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

//	GUI_DEBUG("HWND_Desktop=%08XH\r\n",	hwnd);

	//��ʾ���洰��.
	ShowWindow(hwnd,SW_SHOW);

#if (GUI_SHOW_CURSOR_EN)
	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
	ShowCursor(TRUE);
#endif

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/

