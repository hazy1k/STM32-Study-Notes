
#include	"GUI_Drv.h"
#include    "GUI_Drv_Cfg.h"

/*===================================================================================*/

void* GetFrameBuffer();

//void LCD_HardInit(void);
//void LCD_BkLight(int on);

static SURFACE	SurfSereen;

/*===================================================================================*/
/*===================================================================================*/

//#if 1
//#include	"x_libc.h"
//#include	<stdlib.h>
//
////#define	GUI_SYSMEM_BASE				(u8*)gui_sys_mem//EXT_SRAM_ADDR_BASE
//#define	GUI_SYSMEM_SIZE				(32<<20)
//#define	GUI_SYSMEM_ALLOC_UNIT		32
//
//
//static	heap_t heap_gui_sys;
//static	GUI_MUTEX	*mutex_gui_sys=NULL;
//
//void* vmalloc (U32 size)
//{
//	void *p=NULL;
//
//	GUI_MutexLock(mutex_gui_sys,0xFFFFFFFF);
//	p =x_heap_alloc(&heap_gui_sys,size);
//	GUI_MutexUnlock(mutex_gui_sys);
//
//	if(p==NULL)
//	{
//	    GUI_Printf("#Error: GUI_MEM_Alloc.\r\n");
//	}
//
//	return p;
//
//}
//
//void vfree(void *p)
//{
//	GUI_MutexLock(mutex_gui_sys,0xFFFFFFFF);
//	x_heap_free(&heap_gui_sys,p);
//	GUI_MutexUnlock(mutex_gui_sys);
//}
//#endif

/*===================================================================================*/
///*
//��������: GUI�ͼ���ĳ�ʼ��
//����: ��
//����: TRUE:�ɹ�; FALSE��ʧ��.
//˵��: ����GUI�ĵ�һ����ʼ������.
//*/
//BOOL GUI_LowLevelInit(void)
//{
//
//	GUI_VMEM_Init();
//	//char *p;
//
//	//p =(char*)malloc(GUI_SYSMEM_SIZE);
//
//	//mutex_gui_sys =GUI_MutexCreate();
//
//	//x_heap_init(&heap_gui_sys,p,GUI_SYSMEM_SIZE,GUI_SYSMEM_ALLOC_UNIT);
//
//	return TRUE;
//}


/*===================================================================================*/
/*===================================================================================*/
/*
��������: GUI��ʾ�豸�ĳ�ʼ��
����: ��
����: ��ʾ�豸Surface����ָ��
˵��:
*/
//SURFACE* GUI_DisplayInit(void)
//{
//
//	int width_bytes;
//	u8 *lcd_buffer;
//	SURFACE *pSurf;
//	////
//
//
//	lcd_buffer =(u8*)GetFrameBuffer();
//	pSurf = &SurfSereen;
//
//#if	(LCD_FORMAT == COLOR_FORMAT_RGB565)
//	//��ʼ��LCD Surface�ṹ����(RGB565��ʽ)
//
//	SurfaceInit_RGB565(pSurf,
//						&GL_MEM_16PP,
//						LCD_XSIZE,LCD_YSIZE,
//						LCD_XSIZE*2,
//						(void*)lcd_buffer);
//
//	/* ֱ��ָ����ַ�ķ�ʽ�� �Դ��ַ��*/
//	//pSurf = GUI_CreateSurface(SURF_RGB565,
//	//							LCD_XSIZE, LCD_YSIZE,
//	//							LCD_XSIZE * 2,
//	//							(void*)lcd_buffer);
//
//#endif
//
//#if	(LCD_FORMAT == COLOR_FORMAT_XRGB8888)
//	//��ʼ��LCD Surface�ṹ����(XRGB8888��ʽ)
//	pSurf =CreateSurface(SURF_XRGB8888,LCD_XSIZE,LCD_YSIZE,LCD_XSIZE*4,(void*)lcd_buffer);
//#endif
//
//#if FRAME_BUFFER_EN //ʹ��֡����֧��(֡����ʹ����Ļ��ͬ�ĸ�ʽ�ͳߴ�).
//	{
//		const SURFACE  *pSurf_FB;
//
//		pSurf_FB =CreateSurface(pSurf->Format,pSurf->Width,pSurf->Height,0,NULL);
//		GUI_SetFrameBufferSurface(pSurf_FB);
//	}
//#endif
//
//	LCD_HardInit(); //LCDӲ����ʼ��
//	pSurf->GL->FillArea(pSurf,0,0,LCD_XSIZE,LCD_YSIZE,pSurf->CC->MapRGB(0,0,0)); //����
//	LCD_BkLight(TRUE); //�򿪱���
//
//	while(0)
//	{ //����
//
//		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,255,255));
//		GUI_msleep(1000);
//		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(255,0,0));
//		GUI_msleep(1000);
//		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,255,0));
//		GUI_msleep(1000);
//		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,255));
//		GUI_msleep(1000);
//		pSurf->GL->FillArea(pSurf,20,30,128,80,pSurf->CC->MapRGB(0,0,0));
//		GUI_msleep(1000);
//		break;
//	}
//
//	return pSurf;
//}


/*===================================================================================*/
///*
//��������: GUI�����豸�ĳ�ʼ��
//����: ��
//����: ��
//˵��:
//*/
//void GUI_InputInit(void)
//{
//#if(GUI_KEYBOARD_EN)
//{
//	KeyBoardDev_Init();
//}
//#endif
//
//#if(GUI_TOUCHSCREEN_EN)
//{
//	if(TouchDev_Init()) /*�������豸��ʼ��*/
//	{
//#if(GUI_TOUCHSCREEN_CALIBRATE)
//		TS_CFG_DATA ts_cfg;
//		if(TouchDev_LoadCfg(&ts_cfg)) /*����У������(��������Ҫ)*/
//		{
//			TouchPanel_TranslateInit(&ts_cfg); /*��ʼ������ת������(��������Ҫ)*/
//		}
//#endif
//	}
//
//}
//#endif
//
//#if(GUI_MOUSE_EN)
//{
//	MouseDev_Init();
//}
//#endif
//
//}

/*===================================================================================*/
///*
//��������: GUI�����豸��ִ��
//����: ��
//����: ��
//˵��: �ú����ᱻ��������ѯ����
//*/
//void GUI_InputHandler(void)
//{
//
//#if(GUI_KEYBOARD_EN)
//{
//	if(KeyBoardDev_IsActive())
//	{
//		KeyBoardDev_Handler();
//	}
//}
//#endif
//
//#if(GUI_TOUCHSCREEN_EN)
//{
//	GUI_TouchHandler(); //���ô�����������
//}
//#endif
//
//#if(GUI_MOUSE_EN)
//{
//	if(MouseDev_IsActive())
//	{
//		MouseDev_Handler();
//	}
//}
//#endif
//
//}

/*===================================================================================*/
///*
//��������: ��̬�ڴ�����(����GUI�ں˶���)
//����: size: ��Ҫ�����ڴ���ֽ���
//����: �ڴ���ַ
//˵��:
//*/
//
//void*	GUI_MEM_Alloc(U32 size)
//{
//	void *p=NULL;
//
//	p =vmalloc(size);
//	if(p==NULL)
//	{
//	    GUI_Printf("#Error: GUI_MEM_Alloc.\r\n");
//	}
//
//	return p;
//}
//
///*===================================================================================*/
///*
//��������: ��̬�ڴ��ͷ�(����GUI�ں˶���)
//����: p:��Ҫ�ͷŵ��ڴ���ַ
//����: ��
//˵��:
//*/
//
//void	GUI_MEM_Free(void *p)
//{
//	vfree(p);
//}
//
///*===================================================================================*/
///*
//��������: ��ʾ��̬�ڴ�����(����GUI��ʾ����)
//����: size: ��Ҫ�����ڴ���ֽ���
//����: �ڴ���ַ
//˵��:
//*/
//
//void*	GUI_GRAM_Alloc(U32 size)
//{
//	char *p=NULL;
//
//	p =vmalloc(size);
//
//	return p;
//}
//
///*===================================================================================*/
///*
//��������: ��ʾ��̬�ڴ��ͷ�(����GUI��ʾ����)
//����: p:��Ҫ�ͷŵ��ڴ���ַ
//����: ��
//˵��:
//*/
//
//void	GUI_GRAM_Free(void *p)
//{
//	vfree(p);
//}

/*===================================================================================*/
//
///*
//��������: ��ʽ���ַ������
//����: �ɱ����
//����: ��
//˵��: ����������GUI������Ϣ,�����������Ϊ��
//*/
//
//void	GUI_Printf(const char *fmt,...)
//{
//#if 0
//  	x_va_list ap;
//    char *buf;
//    static	char str_buf[256];
//    static  GUI_MUTEX *mutex_gui_printf=NULL;
//  	////
//  	buf	=str_buf/*(char*)GUI_MEM_Alloc(256)*/;
//  	if(buf)
//  	{
//
//    	if(GUI_MutexLock(mutex_gui_printf,0xFFFFFFFF))
//    	{
//    	    x_va_start(ap,fmt);
//       	 	x_vsprintf(buf,fmt,ap);
//       		DebugPuts(buf);
//       		//DebugOutput(buf);
//
//       		GUI_MutexUnlock(mutex_gui_printf);
//       		x_va_end(ap);
//   		}
//
//		//GUI_MEM_Free(buf);
//	}
//#endif
//}

/*===================================================================================*/
/*===================================================================================*/


#include <GUI_Font_XFT.h>

extern const char GB2312_16_2BPP[];
extern const char GB2312_16_4BPP[];
extern const char GB2312_20_2BPP[];
extern const char GB2312_24_2BPP[];
extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];

static const char font_path_xft[] ="G:\\GIT_OSC\\emXGUI_develop\\1-vs_demo\\X_GUI_SIM(MS-WIN32)\\GUI_App\\GUI_DEMO\\res\\PANNEL_50_4BPP.xft";
//static const char font_path_xft[] = "$(ProjectDir)\\PANNEL_50_4BPP.xft";

//static const char font_path_xft[] = "G:\\test.txt";

static u8 *pFontData_XFT=NULL;

/* �Ǳ������� */
HFONT pannelFont = NULL;
static u8 *p_pannelFont = NULL;



//////////////


#if FONT_TTF_EN

#include    "FontDrv_FreeType.h"

struct __freetype_font_obj *ft_obj;


#endif


/*===================================================================================*/

#if 0

int RES_GetOffset(const char *res_name);
int RES_Read(u8 *buf,u32 addr,int size);

static int read_data(void *buf,int offset,int size,LONG lParam)
{
	offset += lParam;
	RES_Read(buf,offset,size);
	return size;
}

#endif

/*===================================================================================*/
/*
��������: GUIĬ�������ʼ��
����: ��
����: ����Ĭ������ľ��
˵��:
*/
//
//#include <stdio.h>
//HFONT GUI_FontInit(void)
//{
//
//	HFONT hFont=NULL;
//
//{
//
//#if 0
//{
//	/*���ļ�ϵͳ���ر����ڴ�*/
//
//    FILE *fp;
//    int fsize;
//
//    fp =fopen(font_path_xft,"rb+");
//    if(fp != NULL)
//    {
//		fseek(fp, 0L, SEEK_END);
//    	fsize = ftell(fp);
//		fseek(fp, 0L, SEEK_SET);
//
//    	pFontData_XFT =(u8*)vmalloc(fsize);
//    	if(pFontData_XFT!=NULL)
//    	{
//    		fread((u8*)pFontData_XFT,fsize,1,fp);
//    	}
//    	fclose(fp);
//    	////
//    	hFont 	=XFT_CreateFont(pFontData_XFT);
//    }
//}
//#endif
//
//#if 0
//    if(hFont==NULL)
//    { /*�����豸����*/
//
//    	int offset;
//    	offset =RES_GetOffset("GB2312_20_4BPP.xft");
//    	if(offset > 0)
//    	{
//    		hFont =XFT_CreateFontEx(read_data,offset);
//    	}
//    }
//#endif
//
//    if(hFont==NULL)
//    { /*�Ա��ؼ���(������������)*/
//
//       	//hFont =XFT_CreateFont(GB2312_16_2BPP);
//       	hFont =XFT_CreateFont(GB2312_16_4BPP);
//    	//hFont =XFT_CreateFont(ASCII_20_4BPP);
//    	//hFont =XFT_CreateFont(GB2312_20_2BPP);
//    }
//
//}
//
//
//#if	FONT_TTF_EN /*TTF��ʽ����*/
//{
//
//    X_FILE *fp;
//    u8 *pFontBuffer=NULL;
//    const char *font_path ="C:kernel/fonts/def_font.ttf";
//    int fsize;
//
//    fp =x_fopen(font_path,"rb");
//    if(fp != NULL)
//    {
//    	fsize =x_fsize(fp);
//    	pFontBuffer =(u8*)vmalloc(fsize);
//    	{
//    		x_fread((u8*)pFontBuffer,fsize,1,fp);
//    	}
//    	x_fclose(fp);
//    	////
//    }
//
//    ft_obj=freetype_font_obj_new(20,20,72,72,pFontBuffer);
//    DefFontObj =freetype_CreateFont(ft_obj);
//
//}
//#endif
//#if 1
//{
//	/*���ļ�ϵͳ���ر����ڴ�*/
//
//	FILE *fp;
//	int fsize;
//
//	fp = fopen(font_path_xft, "rb+");
//	if (fp != NULL)
//	{
//		fseek(fp, 0L, SEEK_END);
//		fsize = ftell(fp);
//		fseek(fp, 0L, SEEK_SET);
//
//		p_pannelFont = (u8*)GUI_VMEM_Alloc(fsize);
//		if (p_pannelFont != NULL)
//		{
//			fread((u8*)p_pannelFont, fsize, 1, fp);
//		}
//		fclose(fp);
//		////
//		pannelFont = XFT_CreateFont(p_pannelFont);
//	}
//}
//#endif
//	return hFont;
//}


/*===================================================================================*/
void	GUI_UserAppStart(void);
//void    ShellWindowExecu(void);

#if 0
static	int	gui_app_thread(void *p)
{

	SYS_thread_set_priority(NULL,-2);

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

    GUI_UserAppStart();
    ShellWindowExecu();
    return 0;
}
#endif

/*===================================================================================*/
//
//static	void	_EraseBackgnd(HDC hdc,const RECT *lprc,HWND hwnd)
//{
//	RECT rc;
//
//
//#if 1
//	if(lprc==NULL)
//	{
//		GetClientRect(hwnd,&rc);
//	}
//	else
//	{
//		CopyRect(&rc,lprc);
//	}
//
//
//
//	SetBrushColor(hdc,MapRGB(hdc,32,72,144));
//	FillRect(hdc,&rc);
//
//	SetTextColor(hdc,MapRGB(hdc,250,250,250));
//	TextOut(hdc,20,30,L"���洰��",-1);
//#endif
//
//}

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
//
////�û�������ص�����.
//static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
//{
//    RECT rc;
//
//	switch(msg)
//	{
//		case	WM_CREATE:	//���洴��ʱ,���������Ϣ,������������һЩ��ʼ������.
//
//				GetClientRect(hwnd,&rc);
//
//		        //��ʼ�������豸
//		        GUI_InputInit();
//
//				////����1��20ms��ʱ��������ѭ���¼�.
//				SetTimer(hwnd,1,20,TMR_START,NULL);
//
//
//				//����App�߳�
//				GUI_UserAppStart();
//
//				break;
//				////
//
//		case	WM_LBUTTONUP:
//		{
//#if 0
//			static int a=ROTATE_0;
//
//			POINT pt;
//
//			pt.x =LOWORD(lParam);
//			pt.y =HIWORD(lParam);
//
//			rc.x =0;
//			rc.y =0;
//			rc.w =20;
//			rc.h =20;
//
//			if(PtInRect(&rc,&pt))
//			{
//				a=disp_rotate;
//				if(++a > ROTATE_270) a=ROTATE_0;
//
//				gdrvSetRotate(a);
//
//				MoveWindow(hwnd,0,0,surf_screen.Width,surf_screen.Height,TRUE);
//
//				GetWindowRect(hwnd,&rc);
//				ClipCursor(&rc);
//
//				RedrawWindow(hwnd,NULL,NULL,RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE|RDW_FRAME);
//
//			}
//
//#endif
//			return	DefDesktopProc(hwnd,msg,wParam,lParam);
//		}
//
//		////////
//		case	WM_TIMER:
//		{
//			u16 id;
//
//			id =LOWORD(wParam);
//			if(id==1)
//			{
//				GUI_InputHandler(); //���������豸
//			}
//		}
//		break;
//		////
//		case WM_ERASEBKGND:
//		{
//
//			HDC hdc =(HDC)wParam;
//			RECT rc =*(RECT*)lParam;
//
//			SetBrushColor(hdc,MapRGB(hdc,50,80,150));
//			FillRect(hdc,&rc);
//		}
//		return TRUE;
//		////
//		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
//		{
//			PAINTSTRUCT ps;
//			HDC hdc;
//			RECT rc;
//
//			//GetClientRect(hwnd,&rc);
//			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ
//
//
//			rc.x =10;
//			rc.y =20;
//			rc.w =128;
//			rc.h =28;
//			SetBrushColor(hdc,MapRGB(hdc,40,60,100));
//			SetTextColor(hdc,MapRGB(hdc,250,250,250));
//			DrawText(hdc,L"emXGUI Desktop",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);
//
//			EndPaint(hwnd,&ps); //������ͼ
//		}
//		break;
//		////
//
///*
//		case	WM_ERASEBKGND:
//		{
//			HDC hdc =(HDC)wParam;
//			_EraseBackgnd(hdc,NULL,hwnd);
//		}
//		return TRUE;
//		////
//*/
//		default:
//				return	DefDesktopProc(hwnd,msg,wParam,lParam);
//	}
//
//	return WM_NULL;
//
//}
//
//void GUI_DesktopStartup(void)
//{
//	WNDCLASS	wcex;
//	HWND hwnd;
//	MSG msg;
//
//	wcex.Tag 		    = WNDCLASS_TAG;
//
//	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= desktop_proc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= NULL;//hInst;
//	wcex.hIcon			= NULL;
//	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
//
//	//�������洰��.
//	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
//								&wcex,
//								L"DESKTOP",
//								WS_VISIBLE|WS_CLIPCHILDREN,
//								0,0,GUI_XSIZE,GUI_YSIZE,
//								NULL,0,NULL,NULL);
//
//	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);
//
//	//��ʾ���洰��.
//	ShowWindow(hwnd,SW_SHOW);
//
//	//����ϵͳ�򿪹����ʾ(���԰�ʵ��������Ƿ���Ҫ).
//	ShowCursor(TRUE);
//
//	while(GetMessage(&msg,hwnd))
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//
//}
