
#include	"Include.h"

//#include "bg.c"
//#include "0.c"
//#include "1.c"
//#include "5.c"
//extern const unsigned short bg0[];
extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

/*============================================================================*/
static int disp_w,disp_h;
//ֱ��д��
#define FISHNUM   30
#define ScreenW   disp_w
#define ScreenH   disp_h

static 	POINT	FishPos[FISHNUM];
static 	int		FishSpeed[FISHNUM];
static 	int		FishWay[FISHNUM];
static 	int		FishType[FISHNUM];
static 	int		FishDead[FISHNUM];

static 	RECT rect;
static 	BITMAP bm_bg;
static 	BITMAP bm_fish_0;
static 	BITMAP bm_fish_1;
static 	BITMAP bm_fish_5;

static 	HDC hdc_mem;
static 	SURFACE *pSurf;


static void GetRect(RECT* rect, long left, long top, long width, long height)
{
	rect->x	 = left;
	rect->y	 = top;
	//rect->h	 = left+width;
	//rect->w = top+height;
	rect->w	 = width;
	rect->h = height;
	return;
}

void FishInit(void)
{
	disp_w =GUI_XSIZE;
	disp_h =GUI_YSIZE;

	bm_bg.Format	= BM_RGB565;
	bm_bg.Width = 800;
	bm_bg.Height = 480;
	bm_bg.WidthBytes =800*2;
	bm_bg.LUT =NULL;
	bm_bg.Bits =(void*)gImage_0;//bg0;

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

}

void FishExecu(HDC hdc)
{
	int i;

	ClrDisplay(hdc,NULL,RGB565(0,20,10));

    for(i=0; i<FISHNUM; i++)
    {
	      if( FishPos[i].x <= -40 || FishPos[i].x>=ScreenW ||
		      FishPos[i].y <= 0   || FishPos[i].y>=ScreenH )
	      {
		        FishDead[i] = 0;
	      }

          if(0 != FishDead[i])
	      {
		       switch (FishType[i])
	  	       {
	                 	case 0:
								GetRect( &rect, FishPos[i].x, FishPos[i].y, 50, 57);

								DrawBitmap(hdc,rect.x,rect.y,&bm_fish_0,NULL);
								//BMP_DrawFromMemory(hdc_mem,rect.x,rect.y,bmp_0);
								//gdevRotateBitmap_ARGB8888(pSurf,rect.x,rect.y,&bm_fish_0,rotate++);
								FishPos[i].y+=FishSpeed[i]+(1);
								FishPos[i].x+=FishWay[i];

			                    break;

						case 1:
								GetRect( &rect, FishPos[i].x, FishPos[i].y, 50, 57);

								DrawBitmap(hdc,rect.x,rect.y,&bm_fish_1,NULL);
								//BMP_DrawFromMemory(hdc_mem,rect.x,rect.y,bmp_1);
								//gdevRotateBitmap_ARGB8888(pSurf,rect.x,rect.y,&bm_fish_1,rotate++);
								FishPos[i].y+=FishSpeed[i]+(1);
								FishPos[i].x+=FishWay[i];

								break;


						case 2:
						default:
								GetRect( &rect, FishPos[i].x, FishPos[i].y, 50, 57);

								DrawBitmap(hdc,rect.x,rect.y,&bm_fish_5,NULL);
								//BMP_DrawFromMemory(hdc_mem,rect.x,rect.y,bmp_5);
								//gdevRotateBitmap_ARGB8888(pSurf,rect.x,rect.y,&bm_fish_5,rotate++);

								FishPos[i].y+=FishSpeed[i]+(1);
								FishPos[i].x+=FishWay[i];

								break;

	  	         }

	      }

          if ( FishDead[i]==0 )
          {
				/*
				FishPos[i].x=(int)(ScreenW*((long)rand()/32767.0))+50;
				FishPos[i].y=(int)(35*((long)rand()/32767.0));
				FishSpeed[i]=(int)(4*((long)rand()/32767.0));
				FishWay[i]=(int)(3*((long)rand()/32767.0))-1;//-1,0,1

				FishType[i]=(int)(3*((long)rand()/32767.0));//0,1,2,3
				FishDead[i]=1;
				*/
				FishPos[i].x=x_rand()%ScreenW;
				FishPos[i].y=2;
				FishSpeed[i]=3+(((int)x_rand()%8));
				FishWay[i]=(int)((int)x_rand()%3)-(int)1;//(int)(3*((long)rand()/32767.0))-1;//-1,0,1

				FishType[i]=(unsigned int)x_rand()%4;//(int)(3*((long)rand()/32767.0));//0,1,2,3
				FishDead[i]=1;

          }
    }

}
		
void DrawBitmap_Test_Fish(void)	
{
		
	HWND hwnd;
	HDC hdc;
	int t0,t1,frame,fps;
	WCHAR wbuf[128];
	int lbtn=0;
	RECT rc;
	////

	frame=0;
	fps=0;

	FishInit();

	pSurf =CreateSurface(BM_RGB565,ScreenW,ScreenH,-1,NULL);
	hdc_mem =CreateDC(pSurf,NULL); //�����ڴ�DC
			
	hwnd = GetDesktopWindow();
	hdc =GetDC(hwnd);	//�����ĻDC
			
	t0 = GetTickCount();

	while(1)
	{
          FishExecu(hdc_mem);

          t1 =GetTickCount();
          if((t1-t0) >= 1000)
			{
				fps = frame;
				
				frame =0;
				t0 =t1;
			}
			else
			{
				frame++;
			}
			
			x_wsprintf(wbuf,L"fps:%d  ",fps);
			SetTextColor(hdc_mem,MapRGB(hdc_mem,200,0,0));
			TextOut(hdc_mem,20,8,wbuf,-1);
			BitBlt(hdc,0,0,ScreenW,ScreenH,hdc_mem,0,0,SRCCOPY);

//			Sleep(10);
			
			if(GetKeyState(VK_LBUTTON))	//������(���������),���˳�
			{
				//if(lbtn++ > (1000/10))
				if(lbtn++ > 5)
				{
					break;	
				}
			
			}
			else
			{
				
				lbtn =0;
			}
			
	}
		
	DeleteDC(hdc_mem);
	DeleteSurface(pSurf);
	ReleaseDC(hwnd,hdc);	//�ͷ���ĻDC
		
	InvalidateRect(hwnd,NULL,TRUE);
	ShowWindow(hwnd,SW_SHOW);
}
	
/*============================================================================*/
