
/*
 *GUI_DEMO_MEMDC_Blt.c
 *2018��9��29������4:23:09
 *
 */

#include "emXGUI.h"
#include "GUI_Font_XFT.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
//#include "cpuusage.h"

/*============================================================================*/
extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

#if 0
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];
extern const char ASCII_40_4BPP[];
#else
extern const char ASCII_16_4BPP[];
#define ASCII_32_4BPP "ASCII_32_4BPP.xft"
#define ASCII_40_4BPP "ASCII_40_4BPP.xft"

#endif

/*============================================================================*/
#define	MEMDC_W	230  //MEMDC���.
#define	MEMDC_H 220  //MEMDC�߶�.
#define	BGCOLOR	RGB888(0,0,0)  //����ɫ(RGB888).


#define OBJNUM     10 //��ʾ�Ķ�������.
#define	SPEED_MIN	2 //�ƶ��ٶ���Сֵ.
#define	SPEED_MAX	6 //�ƶ��ٶ����ֵ.
#define RECT_SIZE_MIN 30
#define RECT_SIZE_MAX 80

//����ؼ�ID
enum	eID{
	ID_EXIT =0x1000,
	ID_NEXT,
  ID_DMA2D_EN,
	ID_TITLE,
  ID_ART_ACTIVE,
	ID_FPS,
  ID_CPU_LOAD,
  ID_INFO,

	ID_RANDOM,
	ID_BITMAP,
	ID_TEXT,
	ID_RECT,

};

/*============================================================================*/
//dma2dʹ�ܵĶ�̬����
extern BOOL g_dma2d_en ;

static 	RECT	ObjRect[OBJNUM];
static 	u32		ObjColor[OBJNUM];
static  HFONT   ObjFont[OBJNUM];
static  BITMAP *ObjBitmap[OBJNUM];
static 	u16		ObjType[OBJNUM];
static 	s8		ObjActive[OBJNUM];
static 	s8		ObjSpeedX[OBJNUM];
static 	s8		ObjSpeedY[OBJNUM];

static HDC hdc_mem=NULL;
static HDC hdc_bk=NULL;
static BITMAP bm1,bm2,bm3;
static HFONT GA_hFont24=NULL;
static HFONT GA_hFont32=NULL;
static HFONT GA_hFont40=NULL;
static int t0,frame,fps;

static int type_id=0;

static  HDC blue_fish_hdc;
static  HDC red_fish_hdc;
static  HDC crocodile_hdc;

extern const char ASCII_24_4BPP[];

static  HFONT *FontTbl[4]={

		&GA_hFont32,
		&GA_hFont24,
		&GA_hFont32,
		&GA_hFont40,

};

static  BITMAP *BitmapTbl[4]={

		&bm1,
		&bm2,
		&bm3,
		&bm1,
};

/*============================================================================*/
#define BLUE_FISH_PIC "blue_fish_ARGB8888.bmp"
#define RED_FISH_PIC "red_fish_ARGB8888.bmp"
#define CROCODILE_PIC "crocodile_ARGB8888.bmp"

static void BitmapInit(void)
{
#if 0
	bm1.Format	= BM_ARGB8888;
	bm1.Width = 92;
	bm1.Height = 184;
	bm1.WidthBytes =bm1.Width*4;
	bm1.LUT =NULL;
	bm1.Bits =(void*)gImage_0;

	bm2.Format	= BM_ARGB8888;
	bm2.Width = 130;
	bm2.Height = 260;
	bm2.WidthBytes =bm2.Width*4;
	bm2.LUT =NULL;
	bm2.Bits =(void*)gImage_1;

	bm3.Format = BM_ARGB8888;
	bm3.Width = 92;
	bm3.Height = 92;
	bm3.WidthBytes =bm3.Width*4;
	bm3.LUT =NULL;
	bm3.Bits =(void*)gImage_5;
#else

  /* ���������memdc */
  blue_fish_hdc = CreateMemoryDC(COLOR_FORMAT_ARGB8888,92,184); 
  /* ��ձ���Ϊ͸�� */
  ClrDisplay(blue_fish_hdc,NULL,0);
  /* ����bmp��hdc */
  PIC_BMP_Draw_Res(blue_fish_hdc,0,0,BLUE_FISH_PIC, NULL);
  /* ת����bitmap */
  DCtoBitmap(blue_fish_hdc,&bm1);
  
  red_fish_hdc = CreateMemoryDC(COLOR_FORMAT_ARGB8888,92,92);
  ClrDisplay(red_fish_hdc,NULL,0);  
  PIC_BMP_Draw_Res(red_fish_hdc,0,0,RED_FISH_PIC, NULL);
  DCtoBitmap(red_fish_hdc,&bm2);
  
  crocodile_hdc = CreateMemoryDC(COLOR_FORMAT_ARGB8888,130,260);  
  ClrDisplay(crocodile_hdc,NULL,0);
  PIC_BMP_Draw_Res(crocodile_hdc,0,0,CROCODILE_PIC, NULL);
  DCtoBitmap(crocodile_hdc,&bm3);
  
#endif
}
static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HDC hdc;
  RECT rc, rc_tmp;
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;

  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

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
static void GA_BUTTON_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc,hdc_mem;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];
  
  
	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  
  
  
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  hdc_mem = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  if(ds->ID == ID_ART_ACTIVE)
    SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 255, 0));
  else
    SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
	

	  /* ʹ�ÿ���ͼ������ */
	
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc_mem, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);
  DeleteDC(hdc_mem);

}
static void DrawHandler(HDC hdc,int Width,int Height)
{
	int i;
  RECT rc;
  

	ClrDisplay(hdc,NULL,MapRGB888(hdc,BGCOLOR));
  
    for(i=0; i<OBJNUM; i++)
    {
	      if(   ObjRect[i].x <= -40
              || ObjRect[i].x >=Width
              || ObjRect[i].y <= 0
              || ObjRect[i].y >=Height )
	      {
		        ObjActive[i] = 0;
	      }

          if(ObjActive[i] != 0)
	      {

        	  int x,y;

        	  x =ObjRect[i].x;
        	  y =ObjRect[i].y;

		       switch (ObjType[i])
	  	       {

              case 0:
              {
                WCHAR wbuf[20];
                //SetFont(hdc,GA_hFont32);
                SetFont(hdc,ObjFont[i]);
                SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                x_wsprintf(wbuf,L"FPS:%d",fps);
                TextOut(hdc,x,y,wbuf,-1);
              }
              break;
              ////

              case 1:
              {
                WCHAR wbuf[20];
                //SetFont(hdc,GA_hFont32);
                SetFont(hdc,ObjFont[i]);
                SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                x_wsprintf(wbuf,L"X:%04d:Y:%04d",x,y);
                TextOut(hdc,x,y,wbuf,-1);
              }
              break;
              ////
              case 2:
              {
//                WCHAR wbuf[20];
                //SetFont(hdc,GA_hFont40);
                SetFont(hdc,ObjFont[i]);
                SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                TextOut(hdc,x,y,L"emXGUI",-1);
              }
              break;
              ////

              case 3:
              {
                SetBrushColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                FillRect(hdc,&ObjRect[i]);
              }
              break;
              ////
              case 4:
              default:
                DrawBitmap(hdc,x,y,ObjBitmap[i],NULL);
                //DrawBitmap(hdc,x,y,&bm3,NULL);
                break;

	  	         }

		       ObjRect[i].x += ObjSpeedX[i];
		       ObjRect[i].y += ObjSpeedY[i];
	      }

          if (ObjActive[i]==0 )
          {
        	  	u32 rand_val;

        	  	rand_val =x_rand();

        	    ObjRect[i].x =rand_val%Width;
        	    ObjRect[i].y =1;
              ObjRect[i].w =RECT_SIZE_MIN + ((rand_val>>4)%RECT_SIZE_MAX);
              ObjRect[i].h =RECT_SIZE_MIN + ((rand_val>>8)%RECT_SIZE_MAX);


              ObjSpeedX[i] =(int)((int)x_rand()%SPEED_MAX)-(int)(x_rand()%SPEED_MAX);
              ObjSpeedY[i] =SPEED_MIN + ((rand_val>>6)%SPEED_MAX);

              switch(type_id)
              {
                case 0:
                  ObjType[i] =2;
                  break;

                case 1:
                  ObjType[i] =3;
                  break;

                case 2:
                  ObjType[i] =4;
                  break;

                default:
                  ObjType[i] = (rand_val>>2)%8;
                  break;
              }

              rand_val =x_rand();
              ObjColor[i]   = rand_val;
              ObjFont[i]    = *FontTbl[(rand_val>>4)%ARR_SIZE(FontTbl)];
              ObjBitmap[i]  = BitmapTbl[(rand_val>>8)%ARR_SIZE(BitmapTbl)];
              ObjActive[i]  = 1;

          }
    }
    
      //      			GetClientRect(hdc,&rc);
      rc.x = 0;
      rc.y = 0;
      rc.w = Width-1;
      rc.h = Height-1;
      
      SetPenColor(hdc,MapRGB(hdc,255,255,255));
      DrawRect(hdc,&rc);

      SetPenColor(hdc,MapRGB(hdc,100,100,150));
      InflateRect(&rc,-1,-1);
      DrawRect(hdc,&rc);
    
      SetPenColor(hdc,MapRGB(hdc,100,100,150));
      InflateRect(&rc,-1,-1);
      DrawRect(hdc,&rc);
    
      SetFont(hdc, defaultFont);
      SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
    	TextOut(hdc,rc.x+5,rc.h-18,L"ˢͼ���ֱ��ʣ�230x220",-1); 
}



static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;
	HWND wnd;
  static   uint8_t cpu_load_major,cpu_load_minor;
  static   u8 *font_32,*font_40;

	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
//      HDC hdc;

			GetClientRect(hwnd,&rc0); //��ô��ڵĿͻ�������.

			BitmapInit(); //��ʼ��λͼ����
#if 1
			GA_hFont24 =XFT_CreateFont(ASCII_16_4BPP);
			GA_hFont32 =XFT_CreateFont(ASCII_32_4BPP);
			GA_hFont40 =XFT_CreateFont(ASCII_40_4BPP);
#else
			GA_hFont24 =defaultFont;
			GA_hFont32 =GUI_Init_Extern_Font_2RAM(ASCII_32_4BPP,&font_32);         
			GA_hFont40 =GUI_Init_Extern_Font_2RAM(ASCII_40_4BPP,&font_40); 
      
      /* �������ɹ��������ڴ治�� */
      if(GA_hFont32 == NULL )
      {
        /* ʹ������ʽ���� */
        GA_hFont32 =GUI_Init_Extern_Font(ASCII_32_4BPP);
        if(GA_hFont32 == NULL)
        {
            /* ��Ȼʧ�ܾ���Ĭ������ */
            GA_hFont32 = defaultFont;
        }
      }
      
      /* �������ɹ��������ڴ治�� */
      if(GA_hFont40 == NULL )
      {
        /* ʹ������ʽ���� */
        GA_hFont40 =GUI_Init_Extern_Font(ASCII_40_4BPP);
        if(GA_hFont40 == NULL)
        {
            /* ��Ȼʧ�ܾ���Ĭ������ */
            GA_hFont40 = defaultFont;
        }
      }
#endif      

			type_id =3;

			frame=0;
			fps =0;
			t0 =GUI_GetTickCount();
			hdc_mem =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H);

			rc.w =60;//50;
			rc.h =25;//25;
			rc.x = MEMDC_W + (rc0.w - MEMDC_W - rc.w)/2;
			rc.y =45;//20;
   
      
//      hdc =BeginPaint(hwnd,&ps);

      /* Home��ť */    
			wnd=CreateWindow(BUTTON,L"O",	WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,286, 10, 23, 23,hwnd,ID_EXIT,NULL,NULL); //����һ����ť.
			SetWindowFont(wnd,controlFont_24); //���ÿؼ���������.

//      SetFont(hdc, defaultFont);
//      SetTextColor(hdc,MapRGB(hdc,255,255,255)); 

//			OffsetRect(&rc,0,rc.h+10);
//			TextOut(hdc,rc.x,rc.y,L"֡��",-1); 
      wnd=CreateWindow(BUTTON,L"֡��",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w-20,rc.h,hwnd,ID_INFO,NULL,NULL); //����һ�����ֿ�.
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.


			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(TEXTBOX,L"---FPS",TBS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_FPS,NULL,NULL); //����һ�����ֿ�.
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.

			OffsetRect(&rc,0,rc.h+5);
//			TextOut(hdc,rc.x,rc.y,L"CPUʹ����",-1); 
      wnd=CreateWindow(BUTTON,L"CPUʹ����",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_INFO,NULL,NULL); //����һ�����ֿ�.
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.

			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(TEXTBOX,L"---%",TBS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CPU_LOAD,NULL,NULL); //����һ�����ֿ�.
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.

			OffsetRect(&rc,0,rc.h+5);
//			TextOut(hdc,rc.x,rc.y,L"��ʾ����",-1); 
      wnd=CreateWindow(BUTTON,L"��ʾ����",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_INFO,NULL,NULL); //����һ�����ֿ�.
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.

			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(BUTTON,L"��һ��",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_NEXT,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.

//			OffsetRect(&rc,0,rc.h+5);
////			TextOut(hdc,rc.x,rc.y,L"Chrome������",-1); 
//      wnd=CreateWindow(BUTTON,L"Chrom������",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w+40,rc.h,hwnd,ID_INFO,NULL,NULL); //����һ�����ֿ�.
//			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.
//      
//			OffsetRect(&rc,0,rc.h);
//			wnd=CreateWindow(BUTTON,L"����/�ر�",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_DMA2D_EN,NULL,NULL);
//			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.


			wnd=CreateWindow(BUTTON,L"���ͼ��",TBS_FLAT|WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,0,0,100,20,hwnd,ID_TITLE,NULL,NULL); //����һ�����ֿ�.
			SetWindowFont(wnd,defaultFont); //���ÿؼ���������.
      
      /* Chrom-ART ���� */      
//      wnd=CreateWindow(BUTTON,L"Chrom-ART ����",TBS_FLAT|WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,140,0,100,20,hwnd,ID_ART_ACTIVE,NULL,NULL); //����һ�����ֿ�.
//      SetWindowFont(wnd,defaultFont); //���ÿؼ���������.
      g_dma2d_en = TRUE;
//			EndPaint(hwnd,&ps);
      hdc_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      
		}
		return TRUE;
		////
	case	WM_DRAWITEM:
	{
		/*�����ؼ�ָ����WS_OWNERDRAW�����ÿ���ڻ���ǰ����������ڷ���WM_DRAWITEM��Ϣ��
		 *  wParam����ָ���˷��͸���Ϣ�Ŀؼ�ID;lParam����ָ��һ��DRAWITEM_HDR�Ľṹ��ָ�룬
		 *  ��ָ���Ա������һЩ�ؼ�������صĲ���.
		 */

		DRAWITEM_HDR *ds;

		ds = (DRAWITEM_HDR*)lParam;
      if(ds->ID == ID_EXIT)
         exit_owner_draw(ds);
      else
         GA_BUTTON_OwnerDraw(ds);
	   /* ����TRUE����ʹ���ػ���� */
		return TRUE;
	}
		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id ==ID_NEXT)
			{
				if(code == BN_CLICKED)
				{
          type_id++;
          type_id %=4;
//          GUI_DEBUG("type_id =%d",type_id);
             
          wnd =GetDlgItem(hwnd,ID_TITLE);

          switch(type_id)
          {
            case 0:      
               SetWindowText(wnd,L"������ʾ");
              break;
            case 1:
              SetWindowText(wnd,L"���ƾ���"); 
              break;

            case 2:  
              SetWindowText(wnd,L"����Bitmap");       
              break;

             case 3:             
              SetWindowText(wnd,L"���ͼ��");
              break;
             
             default:
               break;
          } 
				}
			}
      
//      if(id == ID_DMA2D_EN)
//      {
//        if(code == BN_CLICKED)
//				{
//          g_dma2d_en =!g_dma2d_en;
//          
//          if(g_dma2d_en)
//          {
//            wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
//            SetWindowText(wnd,L"Chrom-ART ����");

////            ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_SHOW);
//          }
//          else
//          {
//            wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
//            SetWindowText(wnd,L"");

////            ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_HIDE);
//          }
//        }
//      }

			if(id==ID_EXIT && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
		}
		break;
		////

		case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc =*(RECT*)lParam;;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc_bk,MapRGB888(hdc_bk,BGCOLOR));
			FillRect(hdc_bk,&rc);
      BitBlt(hdc, 0,0,GUI_XSIZE,GUI_YSIZE,hdc_bk,0,0,SRCCOPY);
			return TRUE;
		}
//		break;
		////
		case WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

//			if(id == ID_TITLE ||id == ID_INFO) //ID_TEXT�ؼ�����ɫ.
//			{
//				CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;

//				cr->TextColor =RGB888(250,250,250);
//				cr->BackColor =BGCOLOR;
//				cr->BorderColor =RGB888(50,50,50);
//				return TRUE;
//			}
//			else 
//      if( id == ID_ART_ACTIVE ) //ID_TEXT�ؼ�����ɫ.
//			{
//				CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;

//				cr->TextColor =RGB888(0,255,0);
//				cr->BackColor =BGCOLOR;
//				cr->BorderColor =RGB888(50,50,50);
//				return TRUE;
//			}
//			else 
      if(id == ID_FPS  ) //ID_FPS�ؼ�����ɫ.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(255,255,0);
				cr->BackColor =RGB888(0,50,70);
				cr->BorderColor =RGB888(50,50,50);
				return TRUE;
			}
      else if(id == ID_CPU_LOAD ) //ID_CPU_LOAD�ؼ�����ɫ.
			{
//        uint8_t major,minor;  
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
        
//        cpu_usage_get(&major,&minor);
        
        if(cpu_load_major<=25)
          cr->TextColor =RGB888(0,255,0);
				else if(cpu_load_major<=50)
          cr->TextColor =RGB888(200,255,0);
        else if(cpu_load_major<=75)
          cr->TextColor =RGB888(255,255,0);
        else if(cpu_load_major<=100)
          cr->TextColor =RGB888(255,0,0);

        cr->BackColor =RGB888(0,50,70);
				cr->BorderColor =RGB888(50,50,50);
        
				return TRUE;
			}
      else if(id == ID_NEXT  || id == ID_DMA2D_EN)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        /* ��ť���� */
        if(	SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
        {
          cr->TextColor =RGB888(0,0,0);
          cr->BackColor =RGB888(179,238,58);
          cr->BorderColor =RGB888(50,50,50);
        }          
        else
        {
          cr->TextColor =RGB888(0,0,0);
          cr->BackColor =RGB888(255,255,205);
          cr->BorderColor =RGB888(50,50,50);
        }
				return TRUE;
      }
      else if( id == ID_EXIT)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        if(	SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_INVALID;
          cr->BorderColor =RGB888(50,50,50);
        }          
        else
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          cr->BorderColor =RGB888(50,50,50);
        }
				return TRUE;
      }
			else
			{
				//������ťʹ��ϵͳĬ�ϵ���ɫ���л��ƣ�����ֱ�ӷ���FALSE.
				return FALSE;
			}
		}
//		break;
		////

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int t;
			WCHAR wbuf[20];

			t =GUI_GetTickCount();
			if((t-t0) >= 1000)
			{
				fps =frame;

				t0 =t;
				frame=0;

				wnd =GetDlgItem(hwnd,ID_FPS);
				x_wsprintf(wbuf,L"%d FPS",fps);
				SetWindowText(wnd,wbuf);
        
//        cpu_usage_get(&cpu_load_major);

        wnd =GetDlgItem(hwnd,ID_CPU_LOAD);
        if(cpu_load_major != 0)
          x_wsprintf(wbuf,L"%d %%",cpu_load_major);
        else
          x_wsprintf(wbuf,L"0.%d %%",cpu_load_minor);

				SetWindowText(wnd,wbuf);

			}
			else
			{
				frame++;
			}
 #if 0     
      if(g_dma2d_en)
      {
        wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
				SetWindowText(wnd,L"Chrom-ART ����");

//        ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_SHOW);
      }
      else
      {
        wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
				SetWindowText(wnd,L"");

//        ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_HIDE);
      }
#endif

			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...
			DrawHandler(hdc_mem,MEMDC_W,MEMDC_H);

			BitBlt(hdc,0,22,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);
      
			EndPaint(hwnd,&ps); //������ͼ

			////
			InvalidateRect(hwnd,NULL,FALSE); //������һ��WM_PAINT��Ϣ.
			GUI_msleep(2); //�ͷ�һ��CPU,����ȫռCPU���л���.
		}
		break;
		////

		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{

			DeleteDC(hdc_mem);
      DeleteDC(blue_fish_hdc);
      DeleteDC(red_fish_hdc);
      DeleteDC(crocodile_hdc);
      DeleteDC(hdc_bk);
      if(GA_hFont24 != defaultFont)
        DeleteFont(GA_hFont24);
      if(GA_hFont32 != defaultFont)
        DeleteFont(GA_hFont32);      
      if(GA_hFont40 != defaultFont)
        DeleteFont(GA_hFont40);  

      GUI_VMEM_Free((void*)font_32);      
      GUI_VMEM_Free((void*)font_40);      

			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
//		break;
		////

		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_Graphics_Accelerator(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
//  GUI_DEBUG("Enter");
	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//����������
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS|WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                            &wcex,
                            L"GUI_DEMO: MEMDC Blt", //��������
                            WS_VISIBLE|WS_CLIPCHILDREN,
                            0,0,GUI_XSIZE,GUI_YSIZE,    //����λ�úʹ�С
                            NULL,NULL,NULL,NULL);

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
