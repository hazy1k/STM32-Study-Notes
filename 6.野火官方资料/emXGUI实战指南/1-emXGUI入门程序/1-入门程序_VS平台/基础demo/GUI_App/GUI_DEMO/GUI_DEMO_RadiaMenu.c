
/*
 *GUI_DEMO_RadiaMenu.c
 *2018��12��26������8:16:38
 *
 */

#include <math.h>
#include "emXGUI.h"

extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

/*============================================================================*/
//����ؼ�ID
#define	ID_OK		0x1000

enum eMSG{
	MSG_MENU_CLICKED =WM_USER+1,

};
/*============================================================================*/

typedef struct {

  const int    id;
  const BITMAP *pBitmap;
  const WCHAR  *pText;

  /* �ع�������CListMenu�ı� */
  /* APP�·������� */
  const WCHAR *pName;
  /* ͼ���bmp�������ݣ�bmpΪNULLʱʹ��icon�ַ����� */
  const void *bmp;
  /* ͼ����ַ�������bmp�ǿ�ʱ��������Ч */
  void *icon;
  /* ͼ��ʹ�õ���ɫ */
  u32 color;
  /* ͼ���Ӧ��Ӧ�ó��� */
  void(*cbStartup)(HWND hwnd);

} BITMAP_ITEM;

typedef struct {
  int xPos;
  int yPos;
  int Index;
} ITEM_INFO;

typedef struct {
  int                 Pos;
  int                 NumItems;
  int                 xSizeItem;
  int                 ySizeItem;
  int                 xSizeWin;
  int                 ySizeWin;
  int                 rx;
  int                 ry;
  int                 mx;
  int                 my;
  int                 FinalMove;
  const BITMAP_ITEM *pBitmapItem;
  ITEM_INFO         *pItemInfo;

} PARA;

/*============================================================================*/

static 	BITMAP bm_fish_0;
static 	BITMAP bm_fish_1;
static 	BITMAP bm_fish_5;

static int temp_x0,temp_y0,move_n;

#define	ITEM_W	100
#define	ITEM_H	100
#define	MOVE_STEP	100

#define	BK_COLOR	RGB888(10,10,100)
#define SHIFT_RIGHT_16(x)    ((x) / 1000)

static PARA  Para;
static PARA  *pPara;
static int _xPosRect = 0;
static int _yPosRect = 0;

#if 0
static const BITMAP_ITEM _aBitmapItem[] = {

		{0x1001, &bm_fish_0,  L"Menu1" },
		{0x1002, &bm_fish_0,  L"Menu2" },
		{0x1003, &bm_fish_0,  L"Menu3" },
		{0x1004, &bm_fish_0,  L"Menu4" },
		{0x1005, &bm_fish_0,  L"Menu5" },
		{0x1006, &bm_fish_0,  L"Menu6" },
		{0x1007, &bm_fish_0,  L"Menu7" },
		{0x1008, &bm_fish_1,  L"Menu8" },
		{0x1008, &bm_fish_5,  L"Menu9" },

};

#else

/*
*   Ӧ�ó���Ŀջص�����
*/
static void dummy(HWND hwnd)
{

}

static const BITMAP_ITEM _aBitmapItem[] = {

    {0x1001, &bm_fish_0,  L"Hello emXGUI" ,      L"GUIӦ��",		NULL, 	L"A", 	0XFFFFFF,			dummy},
    {0x1002, &bm_fish_0,  L"Hello emXGUI" ,      L"��ť",		NULL, 	L"B", 	0XFFFFFF,			dummy},
    {0x1003, &bm_fish_0,  L"Hello emXGUI" ,      L"������",		NULL, 	L"C", 	0XFFFFFF,			dummy},
    {0x1004, &bm_fish_0,  L"Hello emXGUI" ,      L"��ʾ����",		NULL, 	L"D", 	0XFFFFFF,			dummy},
    {0x1005, &bm_fish_5,  L"Hello emXGUI" ,      L"GUIӦ��",		NULL, 	L"E", 	0XFFFFFF,			dummy},
    {0x1006, &bm_fish_0,  L"Hello emXGUI" ,      L"GUIӦ��",		NULL, 	L"F", 	0XFFFFFF,			dummy},
    {0x1007, &bm_fish_0,  L"Hello emXGUI" ,      L"GUIӦ��",		NULL, 	L"G", 	0XFFFFFF,			dummy},
    {0x1008, &bm_fish_0,  L"Hello emXGUI" ,      L"GUIӦ��",		NULL, 	L"H", 	0XFFFFFF,			dummy},
    {0x1009, &bm_fish_0,  L"Hello emXGUI" ,      L"GUIӦ��",		NULL, 	L"I", 	0XFFFFFF,			dummy},


    // {0x1001, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"A", 	0XFFFFFF,			dummy},
    //{0x1002, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"B", 	0XFFFFFF,			dummy},
    //{0x1003, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"C", 	0XFFFFFF,			dummy},
    //{0x1004, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"D", 	0XFFFFFF,			dummy},
    //{0x1005, &bm_fish_5,  L"Hello emXGUI" ,      L"",		NULL, 	L"E", 	0XFFFFFF,			dummy},
    //{0x1006, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"F", 	0XFFFFFF,			dummy},
    //{0x1007, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"G", 	0XFFFFFF,			dummy},
    //{0x1008, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"H", 	0XFFFFFF,			dummy},
    //{0x1009, &bm_fish_0,  L"Hello emXGUI" ,      L"",		NULL, 	L"I", 	0XFFFFFF,			dummy},


};

#endif


static ITEM_INFO   aItemInfo[ARR_SIZE(_aBitmapItem)];


int  GUI__CosHQ(int Ang1000)
{

	double f=Ang1000;
	double v;

	v= cos((2*3.1416*f)/360000);
	v *=1000;
	return (int)(v);
}

int  GUI__SinHQ(int Ang1000)
{
	double f=Ang1000;
	double v;

	v= sin((2*3.1416*f)/360000);
	v *=1000;
	return (int)(v);
}

static void _Init(int x,int y,int w,int h)
{

	move_n =0;

	bm_fish_0.Format	= BM_ARGB8888;
	bm_fish_0.Width = 92;
	bm_fish_0.Height = 184;
	bm_fish_0.WidthBytes =bm_fish_0.Width*4;
	bm_fish_0.LUT =NULL;
	bm_fish_0.Bits =(void*)gImage_0;

	bm_fish_1.Format	= BM_ARGB8888;
	bm_fish_1.Width = 130;
	bm_fish_1.Height = 260;
	bm_fish_1.WidthBytes =bm_fish_1.Width*4;
	bm_fish_1.LUT =NULL;
	bm_fish_1.Bits =(void*)gImage_1;

	bm_fish_5.Format = BM_ARGB8888;
	bm_fish_5.Width = 92;
	bm_fish_5.Height = 92;
	bm_fish_5.WidthBytes =bm_fish_5.Width*4;
	bm_fish_5.LUT =NULL;
	bm_fish_5.Bits =(void*)gImage_5;


	pPara =&Para;
	pPara->NumItems    = ARR_SIZE(_aBitmapItem);
	pPara->pBitmapItem = _aBitmapItem;
	pPara->pItemInfo   = aItemInfo;
	pPara->Pos         = 0;

	pPara->xSizeWin  = w;
	pPara->ySizeWin  = h;
  //printf("\r\nw = %d,h=%d", pPara->xSizeWin / 2, pPara->ySizeWin / 2);

	pPara->xSizeItem = ITEM_W;//pPara->pBitmapItem[0].pBitmap->Width;
	pPara->ySizeItem = ITEM_H;//pPara->pBitmapItem[0].pBitmap->Height;

	pPara->rx        = x+(pPara->xSizeWin - pPara->xSizeItem) / 2;
	pPara->ry        = y+(pPara->ySizeWin - pPara->ySizeItem) / 2;
	//pPara->mx        = pPara->rx;//x+pPara->xSizeWin / 2;
	//pPara->my        = pPara->ry;// + pPara->ySizeItem / 2;
	pPara->mx        = pPara->rx;;
	pPara->my        = pPara->ry;;


}




static void OnClick(HWND hwnd,int x,int y)
{
	POINT pt;
	RECT rc;
	int i;

	pt.x =x;
	pt.y =y;

	i = pPara->NumItems-1;

	rc.x =pPara->pItemInfo[i].xPos;
	rc.y =pPara->pItemInfo[i].yPos;
	rc.w =ITEM_W;
	rc.h =ITEM_H;

	if(move_n < 3)
	{
		if(PtInRect(&rc,&pt))
		{
			i=_aBitmapItem[pPara->pItemInfo[i].Index].id;
			PostMessage(hwnd,MSG_MENU_CLICKED,i,0);
		}
	}

}

static void _Draw(HDC hdc,HWND hwnd)
{
	ITEM_INFO   ItemInfo;
	int         a1000;
	int         Swap;
	int         i;
	int         SinHQ;
	int         CosHQ;
	int         a;
	int x,y;
	RECT rc;

	GetClientRect(hwnd,&rc);

	pPara =&Para;

	    //
	    // Calculate current positions of items
	    //
	    a1000 = (pPara->Pos * 3600) / pPara->NumItems;
	    for (i = 0; i < pPara->NumItems; i++)
	    {
	      a                         = (90000) + a1000 + (i * 360000) / pPara->NumItems;
	      SinHQ                     = GUI__SinHQ(a);
	      CosHQ                     = GUI__CosHQ(a);
	      pPara->pItemInfo[i].Index = i;
	      pPara->pItemInfo[i].xPos  = pPara->mx - (SHIFT_RIGHT_16(CosHQ * pPara->rx));
	      pPara->pItemInfo[i].yPos  = pPara->my + (SHIFT_RIGHT_16(SinHQ * pPara->ry));
	    }

	    //
	    // Bubble sort items to be able to draw background items first
	    //
      /* �����֤�����ͼ���Ȼ��ƣ�������ǰ���ͼ��*/
	    do
	    {
	      Swap = 0;
	      for (i = 0; i < (pPara->NumItems - 1); i++)
	      {
	        if (pPara->pItemInfo[i].yPos > pPara->pItemInfo[i + 1].yPos)
	        {
	          ItemInfo                = pPara->pItemInfo[i];
	          pPara->pItemInfo[i]     = pPara->pItemInfo[i + 1];
	          pPara->pItemInfo[i + 1] = ItemInfo;
	          Swap                    = 1;
	        }
	      }
	    } while (Swap);

	    //
	    // Draw items
	    //
	    for (i = 0; i < pPara->NumItems; i++)
	    {
#if 0
	    	RECT rc;
	    	const BITMAP *bm;

	    	bm =pPara->pBitmapItem[pPara->pItemInfo[i].Index].pBitmap;

	    	x =pPara->pItemInfo[i].xPos;
	    	y =pPara->pItemInfo[i].yPos;

	    	x += ((int)ITEM_W-(int)bm->Width)/2;
	    	y += ((int)ITEM_H-(int)bm->Height)/2;
	    	DrawBitmap(hdc,x,y,(pPara->pBitmapItem + pPara->pItemInfo[i].Index)->pBitmap,NULL);

		    rc.x =pPara->pItemInfo[i].xPos;
		    rc.y =pPara->pItemInfo[i].yPos;
		    rc.w =ITEM_W;
		    rc.h =ITEM_H;

		    SetPenColor(hdc,MapRGB(hdc,50,50,100));
		    //DrawRect(hdc,&rc);
#else

        if (pPara->pBitmapItem[pPara->pItemInfo[i].Index].bmp != NULL)
        {
          RECT rc;
          const BITMAP *bm;

          bm = pPara->pBitmapItem[pPara->pItemInfo[i].Index].pBitmap;

          x = pPara->pItemInfo[i].xPos;
          y = pPara->pItemInfo[i].yPos;

          x += ((int)ITEM_W - (int)bm->Width) / 2;
          y += ((int)ITEM_H - (int)bm->Height) / 2;
          DrawBitmap(hdc, x, y, (pPara->pBitmapItem + pPara->pItemInfo[i].Index)->pBitmap, NULL);

          rc.x = pPara->pItemInfo[i].xPos;
          rc.y = pPara->pItemInfo[i].yPos;
          rc.w = ITEM_W;
          rc.h = ITEM_H;

          SetPenColor(hdc, MapRGB(hdc, 50, 50, 100));
          //DrawRect(hdc,&rc);
        }
        else
        {
          RECT rc;
          const BITMAP *bm;
          const WCHAR  *icon = pPara->pBitmapItem[pPara->pItemInfo[i].Index].icon;
          u32 icon_color = pPara->pBitmapItem[pPara->pItemInfo[i].Index].color;

          /* ��ʾAPP��Ӧ������ͼ�� */
          SetFont(hdc, iconFont);

          bm = pPara->pBitmapItem[pPara->pItemInfo[i].Index].pBitmap;

          x = pPara->pItemInfo[i].xPos;
          y = pPara->pItemInfo[i].yPos;

          //x += ((int)ITEM_W - (int)bm->Width) / 2;
          //y += ((int)ITEM_H - (int)bm->Height) / 2;
          //DrawBitmap(hdc, x, y, (pPara->pBitmapItem + pPara->pItemInfo[i].Index)->pBitmap, NULL);

          rc.x = pPara->pItemInfo[i].xPos;
          rc.y = pPara->pItemInfo[i].yPos;
          rc.w = ITEM_W;
          rc.h = ITEM_H;

          if (i != (pPara->NumItems - 1))
          {
            SetTextColor(hdc, MapXRGB8888(hdc, icon_color));
            DrawText(hdc, (LPCWSTR)icon, -1, &rc, DT_VCENTER | DT_CENTER);

          }
          else
          {
#if 1
            /* ��ǰ���ͼ�꣬�Ŵ� */
            HDC f_hdc;
            RECT rc0;

            f_hdc = CreateMemoryDC(COLOR_FORMAT_XRGB8888,rc.w,rc.h);
            ClrDisplay(f_hdc,NULL,0x0000ff); 
            
            SetTextColor(f_hdc, MapXRGB8888(f_hdc, 0xff0000));
            /* ��ʾAPP��Ӧ������ͼ�� */
            SetFont(f_hdc, iconFont);

            rc0.x = 0;
            rc0.y = 0;
            rc0.w = rc.w;
            rc0.h = rc.h;

            DrawText(f_hdc, (LPCWSTR)icon, -1, &rc0, DT_VCENTER | DT_CENTER);

            //BitBlt(hdc, rc.x, rc.y, rc.w, rc.h,
            //    f_hdc,0,0, SRCCOPY);
            StretchBlt(hdc,rc.x,rc.y,rc.w*2.0,rc.h*2.0,
              f_hdc,0,0,rc.w,rc.h, SRCCOPY);
            
            DeleteDC(f_hdc);

#else
            SetTextColor(hdc, MapXRGB8888(hdc, 0xFF0000));
            DrawText(hdc, (LPCWSTR)icon, -1, &rc, DT_VCENTER | DT_CENTER);

#endif

          }

          
          /* �ָ�Ĭ������ */
          SetFont(hdc, defaultFont);


          //SetPenColor(hdc, MapRGB(hdc, 50, 50, 100));
          //DrawRect(hdc,&rc);

        }
#endif


	    }

	    //
	    // Determine the position to draw the frame and the explanation at
	    //
	    if ((_xPosRect == 0) || (_yPosRect == 0))
	    {
	      for (i = 0; i < pPara->NumItems; i++)
	      {
	        if (pPara->pItemInfo[i].yPos > _yPosRect)
	        {
	          _xPosRect = pPara->pItemInfo[i].xPos;
	          _yPosRect = pPara->pItemInfo[i].yPos;
	        }
	      }
	    }

	    //
	    // Draw item text only after final move
	    //
	   // if (pPara->FinalMove)
	    {

	    	const WCHAR *pText;
	    	RECT rc;


	    	int i;

        
	    	i =pPara->NumItems - 1;

	    	pText =(pPara->pBitmapItem + pPara->pItemInfo[i].Index)->pName;

	    	rc.x =pPara->pItemInfo[i].xPos;
	    	rc.y =pPara->pItemInfo[i].yPos;
	    	rc.w =ITEM_W;
	    	rc.h =ITEM_H;



	    	SetPenColor(hdc,MapRGB(hdc,250,0,0));
	    	//DrawRect(hdc,&rc); 


	    	GetClientRect(hwnd,&rc);
	    	SetTextColor(hdc,MapRGB(hdc,250,20,20));

	    	//rc.h =30;
	    	//rc.y -=rc.h;
	    	//DrawText(hdc,pText,-1,&rc,DT_VCENTER|DT_CENTER);
        
			//TextOut(hdc,_xPosRect,_yPosRect,pText,-1);
        TextOut(hdc, pPara->rx, pPara->ry*3/2, pText, -1);
        //printf("\r\nw = %d,h=%d", pPara->xSizeWin / 2, pPara->ySizeWin / 2);

	      //GUI_DispStringHCenterAt((pPara->pBitmapItem + pPara->pItemInfo[pPara->NumItems - 1].Index)->pExplanation, _xPosRect, _yPosRect + _bmRectRed_60x60.YSize * 2 / 3);
	    }
	    //
	    // Draw frame surround the current item
	    //

	    //GUI_SetColor(GUI_RED);
	    //GUI_DrawBitmap(&_bmRectRed_60x60, _xPosRect - _bmRectRed_60x60.XSize / 2, _yPosRect - _bmRectRed_60x60.YSize / 2);



}

void	GUI_DEMO_Hello(void);


static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //���ڴ���ʱ,���Զ���������Ϣ,��������һЩ��ʼ���Ĳ����򴴽��Ӵ���.
		{
			GetClientRect(hwnd,&rc); //��ô��ڵĿͻ�������.
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL);

			InflateRectEx(&rc,-ITEM_W,-ITEM_H,-ITEM_W,-(ITEM_H+110));
			_Init(rc.x,rc.y,rc.w,rc.h);

		}
		return TRUE;
		////

		case WM_LBUTTONDOWN:
		{
			move_n=0;
			temp_x0 =GET_LPARAM_X(lParam);
			temp_y0 =GET_LPARAM_Y(lParam);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);

		case WM_LBUTTONUP:
		{
			int x,y;

			x =GET_LPARAM_X(lParam);
			y =GET_LPARAM_Y(lParam);

			OnClick(hwnd,x,y);
			InvalidateRect(hwnd,NULL,FALSE);

			move_n=0;
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);


		case WM_MOUSEMOVE:
		{
			int x,y;

			x =GET_LPARAM_X(lParam);
			y =GET_LPARAM_Y(lParam);

			if(move_n++ >= 3)
			{
				if(wParam&MK_LBUTTON)
				{
					pPara->FinalMove  = TRUE;
					pPara->Pos += x-temp_x0;

					if (pPara->Pos > pPara->NumItems * MOVE_STEP)
					{
						pPara->Pos -= pPara->NumItems * MOVE_STEP;
					}

					if (pPara->Pos < 0)
					{
						pPara->Pos += pPara->NumItems * MOVE_STEP;
					}
					InvalidateRect(hwnd,NULL,FALSE);
				}
				else
				{
					pPara->FinalMove  = FALSE;
				}

			}

			temp_x0 =x;
			temp_y0 =y;

		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case WM_NOTIFY: //WM_NOTIFY��Ϣ:wParam��16λΪ���͸���Ϣ�Ŀؼ�ID,��16λΪ֪ͨ��;lParamָ����һ��NMHDR�ṹ��.
		{
			u16 code,id;

			code =HIWORD(wParam); //���֪ͨ������.
			id   =LOWORD(wParam); //��ò�������Ϣ�Ŀؼ�ID.

			if(id==ID_OK && code==BN_CLICKED) // ��ť����������.
			{
				PostCloseMessage(hwnd); //ʹ����WM_CLOSE��Ϣ�رմ���.
			}
		}
		break;
		////

		case MSG_MENU_CLICKED: //
		{
			int menu_id;

			menu_id =wParam;
			printf("Menu Clicked: %08XH\r\n",menu_id);
      if (menu_id == 0x1001)
      {
        GUI_DEMO_Hello();
      }
		}
		break;
		/////

		case WM_ERASEBKGND:
		return TRUE;

		case WM_PAINT: //������Ҫ����ʱ�����Զ���������Ϣ.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			HDC hdc_mem;

			GetClientRect(hwnd,&rc);

			hdc =BeginPaint(hwnd,&ps); //��ʼ��ͼ

			////�û��Ļ�������...
			//TextOut(hdc,10,10,L"Hello",-1);
			hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);

			ClrDisplay(hdc_mem,NULL,MapRGB888(hdc_mem,BK_COLOR));
			_Draw(hdc_mem,hwnd);
			BitBlt(hdc,0,0,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			DeleteDC(hdc_mem);
			EndPaint(hwnd,&ps); //������ͼ
		}
		break;
		////

		case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{

			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		}
		break;
		////

		default: //�û������ĵ���Ϣ,��ϵͳ����.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

//static void	AppMain(void)
void	GUI_DEMO_RadiaMenu(void)
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
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//����������
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_Demo: RadiaMenu"), //��������
								WS_OVERLAPPEDWINDOW,
								0,0, GUI_XSIZE, GUI_YSIZE,    //����λ�úʹ�С
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
