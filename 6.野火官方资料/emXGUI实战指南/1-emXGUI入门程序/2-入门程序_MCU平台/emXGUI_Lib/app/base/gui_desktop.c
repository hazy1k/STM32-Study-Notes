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
  */ 



#include "emXGUI.h"


/* �ⲿ��Դ������ɱ�־ */
BOOL Load_state = FALSE;

/*===================================================================================*/
extern void	GUI_Boot_Interface_Dialog(void *param);
extern void GUI_AppMain(void);


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
          #ifdef  GUI_TOUCHSCREEN_CALIBRATE
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

  /* �ָ�Ĭ������ */
  SetFont(hdc, defaultFont);

	SetBrushColor(hdc,MapRGB(hdc,32,72,144));
	FillRect(hdc,&rc);
  	
  SetTextColor(hdc,MapRGB(hdc,250,250,250));

#if (GUI_EXTERN_FONT_EN || GUI_INER_CN_FONT_EN)
  /* ������ʾ��� */
	DrawText(hdc,L"���ã�Ұ��emXGUI!",-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);  
#else
  /* ������ʾ��� */
	DrawText(hdc,L"Hello emXGUI@Embedfire!",-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
#endif
  
	SetTextColor(hdc,MapRGB(hdc,250,250,250));
	TextOut(hdc,20,20,L"emXGUI@Embedfire",-1);

}

extern GUI_SEM *Input_Sem;
/* ʹ��ר�õ��߳����������� */
#if 0
static	int	gui_input_thread(void *p)
{
	while(1)
	{
		GUI_SemWait(Input_Sem, 0xFFFFFFFF);
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
				 SetTimer(hwnd,1,30,TMR_START,NULL);

				//����App�߳�						
				{
#if (GUI_APP_BOOT_INTERFACE_EN )  
          
         GUI_Thread_Create(GUI_Boot_Interface_Dialog,  /* ������ں��� */
                              "Boot_Interface",/* �������� */
                              10*1024,  /* ����ջ��С */
                              NULL, /* ������ں������� */
                              5,    /* ��������ȼ� */
                              10); /* ����ʱ��Ƭ����������֧�� */
//				GUI_Thread_Create(gui_input_thread,  /* ������ں��� */
//															"gui_input_thread",/* �������� */
//															1*1024,  /* ����ջ��С */
//															NULL, /* ������ں������� */
//															11,    /* ��������ȼ� */
//															10); /* ����ʱ��Ƭ����������֧�� */#else          
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
          SetBrushColor(hdc, MapRGB(hdc, 255, 0, 0));
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
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
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

