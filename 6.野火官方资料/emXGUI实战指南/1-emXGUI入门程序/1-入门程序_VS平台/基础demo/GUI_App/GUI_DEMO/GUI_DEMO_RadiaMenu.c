
/*
 *GUI_DEMO_RadiaMenu.c
 *2018年12月26日下午8:16:38
 *
 */

#include <math.h>
#include "emXGUI.h"

extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

enum eMSG{
	MSG_MENU_CLICKED =WM_USER+1,

};
/*============================================================================*/

typedef struct {

  const int    id;
  const BITMAP *pBitmap;
  const WCHAR  *pText;

  /* 重构成类似CListMenu的表单 */
  /* APP下方的文字 */
  const WCHAR *pName;
  /* 图标的bmp数据内容，bmp为NULL时使用icon字符索引 */
  const void *bmp;
  /* 图标的字符索引，bmp非空时本配置无效 */
  void *icon;
  /* 图标使用的颜色 */
  u32 color;
  /* 图标对应的应用程序 */
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
*   应用程序的空回调函数
*/
static void dummy(HWND hwnd)
{

}

static const BITMAP_ITEM _aBitmapItem[] = {

    {0x1001, &bm_fish_0,  L"Hello emXGUI" ,      L"GUI应用",		NULL, 	L"A", 	0XFFFFFF,			dummy},
    {0x1002, &bm_fish_0,  L"Hello emXGUI" ,      L"按钮",		NULL, 	L"B", 	0XFFFFFF,			dummy},
    {0x1003, &bm_fish_0,  L"Hello emXGUI" ,      L"进度条",		NULL, 	L"C", 	0XFFFFFF,			dummy},
    {0x1004, &bm_fish_0,  L"Hello emXGUI" ,      L"显示汉字",		NULL, 	L"D", 	0XFFFFFF,			dummy},
    {0x1005, &bm_fish_5,  L"Hello emXGUI" ,      L"GUI应用",		NULL, 	L"E", 	0XFFFFFF,			dummy},
    {0x1006, &bm_fish_0,  L"Hello emXGUI" ,      L"GUI应用",		NULL, 	L"F", 	0XFFFFFF,			dummy},
    {0x1007, &bm_fish_0,  L"Hello emXGUI" ,      L"GUI应用",		NULL, 	L"G", 	0XFFFFFF,			dummy},
    {0x1008, &bm_fish_0,  L"Hello emXGUI" ,      L"GUI应用",		NULL, 	L"H", 	0XFFFFFF,			dummy},
    {0x1009, &bm_fish_0,  L"Hello emXGUI" ,      L"GUI应用",		NULL, 	L"I", 	0XFFFFFF,			dummy},


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
      /* 排序后保证背后的图标先绘制，最后绘制前面的图标*/
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

          /* 显示APP对应的字体图标 */
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
            /* 最前面的图标，放大 */
            HDC f_hdc;
            RECT rc0;

            f_hdc = CreateMemoryDC(COLOR_FORMAT_XRGB8888,rc.w,rc.h);
            ClrDisplay(f_hdc,NULL,0x0000ff); 
            
            SetTextColor(f_hdc, MapXRGB8888(f_hdc, 0xff0000));
            /* 显示APP对应的字体图标 */
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

          
          /* 恢复默认字体 */
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
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
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

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
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

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			HDC hdc_mem;

			GetClientRect(hwnd,&rc);

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);

			ClrDisplay(hdc_mem,NULL,MapRGB888(hdc_mem,BK_COLOR));
			_Draw(hdc_mem,hwnd);
			BitBlt(hdc,0,0,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			DeleteDC(hdc_mem);
			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
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
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_Demo: RadiaMenu"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								0,0, GUI_XSIZE, GUI_YSIZE,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
