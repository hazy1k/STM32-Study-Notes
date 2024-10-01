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


#include	"rtthread.h"
#include "board.h"
#include "emXGUI.h"
#include "GUI_AppDef.h"




/*===================================================================================*/
extern void GUI_AppMain(void);


static	void	gui_app_thread(void *p)
{
    #if(GUI_TOUCHSCREEN_EN & GUI_TOUCHSCREEN_CALIBRATE)
    {
        int i=0;
        while(TouchPanel_IsPenDown())
    	{
    		GUI_msleep(100);
    		if(i++>10)
    		{
    			ShowCursor(FALSE);
    			TouchScreenCalibrate(NULL);
    			ShowCursor(TRUE);
    			break;
    		}
    	}
    }
    #endif
		
  /* ����APP���� */  
	GUI_AppMain();
 //   GUI_UserAppStart();
//   	ShellWindowStartup();
  //  return 0;
}
extern void	GUI_DEMO_SlideWindow(void);

static	void	gui_slide_win(void *p)
{

		
  /* ����APP���� */  
  GUI_DEMO_SlideWindow();
  //   GUI_UserAppStart();
  //   	ShellWindowStartup();
  //  return 0;
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

	SetBrushColor(hdc,MapRGB(hdc,COLOR_DESKTOP_BACK_GROUND));
	FillRect(hdc,&rc);
  	
  SetTextColor(hdc,MapRGB(hdc,255,255,255));
  
//  SetFont(hdc, iconFont_100);
//	DrawText(hdc,L" A B C D E \r\n F G H I J",-1,&rc,DT_LEFT|DT_VCENTER);

  SetFont(hdc, GB2312_32_Font);
  
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
  rc.y +=20;
  DrawText(hdc,L"emXGUI@Embedfire STM32F429 ",-1,&rc,DT_CENTER);
    
  /* ���� */
  GetClientRect(hwnd,&rc);
  SetBrushColor(hdc,MapRGB(hdc,82,85,82));
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;
  FillRect(hdc,&rc);
  
    /* ���� */ 
  SetFont(hdc, logoFont);
  /* ��ʾlogo */
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT-10;
  rc.h = HEAD_INFO_HEIGHT;
  
  SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
  DrawText(hdc,L" B",-1,&rc,DT_LEFT|DT_VCENTER);
  
  
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;

  /* �ָ�Ĭ������ */
  SetFont(hdc, defaultFont);
  rc.x +=50;
  DrawText(hdc,L" Ұ��@emXGUI",-1,&rc,DT_LEFT|DT_VCENTER);

  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT+10;
  rc.h = HEAD_INFO_HEIGHT;
      
  /* ����ͼ������ */
//  SetFont(hdc, controlFont_64);

  /* ����ͼ�� */
  SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
//  DrawText(hdc,L"D",-1,&rc,DT_TOP|DT_CENTER);
  DrawText(hdc,L"��",-1,&rc,DT_TOP|DT_CENTER);

 /* �ָ�Ĭ������ */
  SetFont(hdc, defaultFont);

  rc.y -= 20;
  DrawText(hdc,L"\r\n\r\n��ϸ",-1,&rc,DT_BOTTOM|DT_CENTER);
  
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;

  DrawText(hdc,L"www.embedFire.com  ",-1,&rc,DT_RIGHT|DT_VCENTER);  


}

#if 0
static	int	gui_input_thread(void *p)
{
	SYS_thread_set_priority(NULL,+4);
	while(1)
	{
		GUI_InputHandler(); //���������豸
		GUI_msleep(20);
	}
}
#endif


#if(GUI_PIC_CAPTURE_SCREEN_EN)
/**
  * @brief  ��ͼ�߳�
  * @param  ��
  * @retval ��
  */
static void capture_screen_thread(void *argv)
{  
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )/* K1 ������ */
    {
      /*LED1��ת*/
      LED1_TOGGLE;
      GUI_INFO("Capture screen start..");
      
      if (PIC_Capture_Screen_To_BMP("0:screenShot.bmp"))          
        GUI_INFO("Capture screen success.");
      else
        GUI_ERROR("Capture screen failed");

      LED1_TOGGLE;
    }     
    GUI_msleep(5);
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


			   ////����1��20ms��ʱ��������ѭ���¼�.
				 SetTimer(hwnd,1,20,TMR_START,NULL);

				//����App�߳�						
				if(1)
				{
						rt_thread_t h;
					
						h=rt_thread_create("GUI_APP",gui_app_thread,NULL,4096,5,5);
						rt_thread_startup(h);			

						h=rt_thread_create("GUI_SLIDE_WIN",gui_slide_win,NULL,4096,5,5);
						rt_thread_startup(h);			

#if(GUI_PIC_CAPTURE_SCREEN_EN)
						h=rt_thread_create("CAPTURE_SCREEN_APP",capture_screen_thread,NULL,2048,2,5);
						rt_thread_startup(h);		
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
			HDC hdc =(HDC)wParam;
			_EraseBackgnd(hdc,NULL,hwnd);
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

	//�������洰��.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);

	//��ʾ���洰��.
	ShowWindow(hwnd,SW_SHOW);

	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/

