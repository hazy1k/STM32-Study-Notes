/*
 *GUI_DEMO_IconViewer.c
 *2018��3��17������7:41:26
 *
 */

#include <emXGUI.h>
#include	"rtthread.h"

#include "emXGUI_JPEG.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"


/*============================================================================*/

#define	ID_EXIT		0x1000
//#define ICON_VIEWER_ID_PREV   0x1003
//#define	ICON_VIEWER_ID_NEXT		0x1004
#define	ID_RB1		0x1101
#define	ID_RB2		0x1102
#define	ID_RB3		0x1103
#define	ID_RB4		0x1104
#define	ID_RB5		0x1105
#define	ID_RB6		0x1106


//static int win_pos=0;
//static u8 hour,min,sec;
//static const WCHAR *pOEMText=NULL;
//static u8 List1_Col,List1_Row;

#define	ID_LIST_1 0x1000
#define ID_LIST_2 0x1001

//extern const char app_1[];

//static const void *pIcon_app1 =app_1;
//static const void *pIcon_app2 =app_2;


static void dummy(HWND hwnd)
{

}

extern void	GUI_DEMO_Hello(void);
extern void	GUI_DEMO_Graphics_Accelerator(void);
extern void	GUI_DEMO_Climate_Cabinet(void);

static void App_GUI_DEMO_Hello(HWND hwnd)
{
	static int thread=0;
	static int app=0;
  rt_thread_t h;


	if(thread==0)
	{  //����һ�������߳�����������...
//		CreateThread((FN_ThreadEntry*)app_gui_test,hwnd,8192);
    h=rt_thread_create("GUI_APP_DEMO_Hello",(void(*)(void*))App_GUI_DEMO_Hello,NULL,8192,5,5);
    rt_thread_startup(h);				

		thread =1;
		return;
	}

	if(thread==1) //�߳��Ѵ�����?
	{
		if(app==0)
		{
			app=1;
			GUI_DEMO_Hello();

			app=0;
			thread=0;
		}
		else
		{
//			MSGBOX(hwnd,L"��������������...",L"��Ϣ��ʾ!");
		}
	}
}


static void  App_GUI_Climate_Cabinet(HWND hwnd)
{
	static int thread=0;
	static int app=0;
  rt_thread_t h;


	if(thread==0)
	{  //����һ�������߳�����������...
//		CreateThread((FN_ThreadEntry*)app_gui_test,hwnd,8192);
    h=rt_thread_create("GUI_APP_DEMO_Climate_Cabinet",(void(*)(void*))App_GUI_Climate_Cabinet,NULL,10*1024,5,5);
    rt_thread_startup(h);				

		thread =1;
		return;
	}

	if(thread==1) //�߳��Ѵ�����?
	{
		if(app==0)
		{
			app=1;
			GUI_DEMO_Climate_Cabinet();

			app=0;
			thread=0;
		}
		else
		{
//			MSGBOX(hwnd,L"��������������...",L"��Ϣ��ʾ!");
		}
	}
}


static void App_GUI_Graphics_Accelerator(HWND hwnd)
{
	static int thread=0;
	static int app=0;
  rt_thread_t h;


	if(thread==0)
	{  //����һ�������߳�����������...
//		CreateThread((FN_ThreadEntry*)app_gui_test,hwnd,8192);
    h=rt_thread_create("GUI_APP_Graphics_Accelerator",(void(*)(void*))App_GUI_Graphics_Accelerator,NULL,10*1024,5,5);
    rt_thread_startup(h);				

		thread =1;
		return;
	}

	if(thread==1) //�߳��Ѵ�����?
	{
		if(app==0)
		{
			app=1;
			GUI_DEMO_Graphics_Accelerator();

			app=0;
			thread=0;
		}
		else
		{
//			MSGBOX(hwnd,L"��������������...",L"��Ϣ��ʾ!");
		}
	}
}



static struct __obj_list menu_list_1[]={

    L"Speed",		NULL, 	L"A",RGB_WHITE, 				App_GUI_DEMO_Hello,
		L"Hello",		NULL,	  L"B",RGB_WHITE, 				App_GUI_Graphics_Accelerator,
//  	L"Hello",		NULL,	  L"B", 	RGB_WHITE,			dummy,
//		L"Button",		NULL,	  L"C",RGB_WHITE, 				App_GUI_Climate_Cabinet,
  		L"Button",		NULL,	  L"C", RGB_WHITE,				dummy,

		L"Checkbox",	NULL, 	L"D", RGB_WHITE,				dummy,
		L"Radiobox",	NULL,   L"E",RGB_WHITE, 				dummy,
		L"Textbox",	NULL,	  L"F", RGB_WHITE,				dummy,
//    L"Speed",		NULL,	  L"G", 	RGB_WHITE,			dummy,
//    L"Hello",		NULL,	  L"H", 	RGB_WHITE,			dummy,
//    L"Button",	  NULL,	  L"I", RGB_WHITE,				dummy,
//    L"Checkbox",	NULL,	  L"J", RGB_WHITE,				dummy,

		NULL,	NULL,NULL,	NULL, //������־!

};


static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd =ds->hwnd; //button�Ĵ��ھ��.
	hdc =ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc =ds->rc;     //button�Ļ��ƾ�����.

	SetBrushColor(hdc,MapRGB(hdc,COLOR_DESKTOP_BACK_GROUND));
  FillRect(hdc,&rc); //�þ�����䱳��
  
  if(IsWindowEnabled(hwnd) == FALSE)
  {
  	SetTextColor(hdc,MapRGB(hdc,COLOR_INVALID));
  }
	else if(ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc,MapRGB(hdc,250,0,0));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc,MapRGB(hdc,255,255,255));
	}
  
  
//	SetBrushColor(hdc,COLOR_BACK_GROUND);

//	FillRect(hdc,&rc); //�þ�����䱳��
//	DrawRect(hdc,&rc); //���������
//  
//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //�þ�����䱳��FillCircle
//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //���������

  /* ʹ�ÿ���ͼ������ */
  SetFont(hdc, controlFont);      
//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������
  
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)

  
  /* �ָ�Ĭ������ */
  SetFont(hdc, defaultFont);      

}


static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  HWND wnd;
	switch(msg)
	{
		case WM_CREATE:
		{
			list_menu_cfg_t cfg;
			RECT rc;

//			win_pos =0;
//			GetTime(&hour,&min,&sec);

			GetClientRect(hwnd,&rc);

//			rc.x =0;
//			rc.y =0;
//			rc.w =rc0.w;
//			rc.h =200;

			//ListMenu�ؼ�����Ҫ�ڴ���ʱ����һ�� list_menu_cfg_t �Ľṹ�����.
			cfg.list_objs =menu_list_1; //ָ��list�б�.
			cfg.x_num =3; //ˮƽ����.
			cfg.y_num =1; //��ֱ����.
			CreateWindow(&wcex_ListMenu,
                      L"ListMenu1",
                      WS_VISIBLE|LMS_ICONFRAME,
                      rc.x+100,rc.y+10,rc.w-200,rc.h-10,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      &cfg);
      
      /* ��һ����ť */      
     wnd = CreateWindow(BUTTON,L"A",BS_FLAT|BS_NOTIFY|WS_OWNERDRAW|WS_VISIBLE,
                    0,rc.h*1/3,70,70,hwnd,ICON_VIEWER_ID_PREV,NULL,NULL);
     SetWindowFont(wnd,controlFont); //���ÿؼ���������.

      /* ��һ����ť */      
     wnd =CreateWindow(BUTTON,L"B",BS_FLAT|BS_NOTIFY|WS_OWNERDRAW|WS_VISIBLE,
                    rc.w-65,rc.h*1/3,70,70,hwnd,ICON_VIEWER_ID_NEXT,NULL,NULL);
     SetWindowFont(wnd,controlFont); //���ÿؼ���������.
                
			SetTimer(hwnd,1,50,TMR_START,NULL);
		}
		break;
		////

		case WM_NOTIFY:
		{
			u16 code,id;
			LM_NMHDR *nm;

			code =HIWORD(wParam);
			id   =LOWORD(wParam);

			nm   =(LM_NMHDR*)lParam;

			if(code==LMN_CLICKED)
			{
				switch(id)
				{
				case ID_LIST_1:
					menu_list_1[nm->idx].cbStartup(hwnd);
					break;
					////
				case ID_LIST_2:
//					menu_list_2[nm->idx].cbStartup(hwnd);
					break;
					////
				}

			}
			////
			if(code == BN_CLICKED && id==ICON_VIEWER_ID_PREV)
			{
				SendMessage(GetDlgItem(hwnd,ID_LIST_1),MSG_MOVE_PREV,TRUE,0);
			}
			////
			if(code == BN_CLICKED && id==ICON_VIEWER_ID_NEXT)
			{
				SendMessage(GetDlgItem(hwnd,ID_LIST_1),MSG_MOVE_NEXT,TRUE,0);
			}

		}
		break;
		////
    
    case WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc;

			GetClientRect(hwnd,&rc);
      SetBrushColor(hdc,MapRGB(hdc,COLOR_DESKTOP_BACK_GROUND));
			FillRect(hdc,&rc);      
		}
    break;

		case WM_PAINT:
		{
//			HDC hdc;
			PAINTSTRUCT ps;
//			RECT rc;
//			WCHAR wbuf[128];

			BeginPaint(hwnd,&ps);

			EndPaint(hwnd,&ps);
			////

		}
		break;
		////
#if 1    
    case	WM_DRAWITEM:
		{
			/*�����ؼ�ָ����WS_OWNERDRAW�����ÿ���ڻ���ǰ����������ڷ���WM_DRAWITEM��Ϣ��
			 *  wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��DRAWITEM_HDR�Ľṹ��ָ�룬
			 *  ��ָ���Ա������һЩ�ؼ�������صĲ���.
		     */

			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;

      button_owner_draw(ds); //ִ���Ի��ư�ť

//			if(ds->ID == ICON_VIEWER_ID_PREV)
//			{
//				button_owner_draw(ds); //ִ���Ի��ư�ť
//			}
//      else if(ds->ID == ICON_VIEWER_ID_NEXT)
//      {
//        button_owner_draw(ds); //ִ���Ի��ư�ť
//      }
      
     /* ����TRUE����ʹ���ػ���� */
     return TRUE;
		}
//		break;
#else
    
    
    case WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);
      
      if(id == ICON_VIEWER_ID_PREV || id ==ICON_VIEWER_ID_NEXT)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;
        
        if(IsWindowEnabled(GetDlgItem(hwnd,id)) == FALSE)
        {
          cr->TextColor =RGB888_COLOR_INVALID;
          cr->BackColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          cr->BorderColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          GUI_DEBUG("disable");
//          cr->ForeColor = RGB888(255,255,255);
        }
        else if(	SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
        {
          cr->TextColor =RGB888(255,0,0);
          cr->BackColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          cr->BorderColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
        }          
        else
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          cr->BorderColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
        }
				return TRUE;
      }        
      else
      {
				//������ťʹ��ϵͳĬ�ϵ���ɫ���л��ƣ�����ֱ�ӷ���FALSE.
				return FALSE;
			}
    }
    #endif
//    break;

		case WM_LBUTTONDOWN:
		{
		}
		break;
		////

		case WM_TIMER:
		{
		}
		break;
		////

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);


	}
	return WM_NULL;

}


/*============================================================================*/

void	GUI_DEMO_IconViewer(void)
//static void	AppMain(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	hwnd	=CreateWindowEx(	NULL,
                          &wcex,
                          _T("IconViewer"),
          //								/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
                            /*WS_MEMSURFACE|*/WS_CLIPCHILDREN,

                          0,100,GUI_XSIZE,300,
                          GetDesktopWindow(),NULL,NULL,NULL);

	//��ʾ������
	ShowWindow(hwnd,SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/*============================================================================*/
