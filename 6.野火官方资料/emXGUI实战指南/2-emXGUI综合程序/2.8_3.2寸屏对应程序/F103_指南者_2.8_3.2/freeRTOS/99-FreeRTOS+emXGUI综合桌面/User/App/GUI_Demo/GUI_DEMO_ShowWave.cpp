

/*============================================================================*/
#include <math.h>


#include "emXGUI.h"
#include "GUI_Font_XFT.h"
#include "x_libc.h"
#include "x_obj.h"

#include "GUI_AppDef.h"

/*============================================================================*/

extern "C" const char ASCII_20_4BPP[];
extern "C" const char ASCII_24_4BPP[];

/*============================================================================*/
#define	BK_W	rc_main.w
#define	BK_H	rc_main.h

//#define	LEFT_OFFSET	 30
//#define	TOP_OFFSET	 15
//#define	RIGHT_OFFSET  (15)
//#define BOTTOM_OFFSET (15)

//#define	CUR_BOX_SIZE 16

static int LEFT_OFFSET	= 30;
static int TOP_OFFSET	= 15;
static int RIGHT_OFFSET = (15);
static int BOTTOM_OFFSET =(17);

static int CUR_BOX_SIZE =(15);



//static POINT pt_wav[800];
static POINT *pt_wav;
/* �Ƿ�Ҫ����������������� */
static uint8_t scatter_init = 0;
         

//static int x_cur,y_cur;

//static SURFACE *pSurfTop=NULL;
//static HDC hdcTop=NULL;

//static HFONT hFont20,hFont24;

static RECT rc_main,rc_wav,rc_button,rc_label,rc_x1_cur,rc_x2_cur,rc_y1_cur,rc_y2_cur;
static int wave_freq,wave_amp;

enum WAVE_FORMAT{
  SINE_WAVE,
  SQUARE_WAVE,
  TRIANGULAR_WAVE,
  SCATTER,
};

static WAVE_FORMAT wave_format = SINE_WAVE;
// static int x_step;

#define	Y_STEP_NUM	9
#define	X_STEP_NUM	9

static int y_step_cur=0;
static int x_step_cur=0;

// static const float y_step[Y_STEP_NUM]={0.1,0.2,0.5,1.0,2.0,5.0,10.0,20.0,50.0};

// static const WCHAR *y_step_str[Y_STEP_NUM]={
// 	L"0.1mV",
// 	L"0.2mV",
// 	L"0.5mV",
// 	L"1mV",
// 	L"2mV",
// 	L"5mV",
// 	L"10mV",
// 	L"20mV",
// 	L"50mV",

// };

static const float y_step[Y_STEP_NUM]={1.1,1.2,1.3,1.4,1.5,2.0,5.0,6.0,7.0};

static const WCHAR *y_step_str[Y_STEP_NUM]={
	L"1.1mV",
	L"1.2mV",
	L"1.3mV",
	L"1.4mV",
	L"1.5mV",
	L"2.0mV",
	L"5.0mV",
	L"6.0mV",
	L"7.0mV",

};

static const int x_step[X_STEP_NUM]={50,60,70,80,90,100,110,120,130};

static const WCHAR *x_step_str[X_STEP_NUM]={
	L"50mS",
	L"60mS",
	L"70mS",
	L"80mS",
  L"90mS",
	L"100mS",
  L"110mS",	
	L"120mS",
  L"130mS",
};


// static const int x_step[X_STEP_NUM]={1000,2000};

// static const WCHAR *x_step_str[X_STEP_NUM]={
// 	L"1000mS",
// 	L"2000mS",

// };



#define	CUR_X1	1
#define	CUR_X2	2
#define	CUR_Y1	3
#define	CUR_Y2	4
static int focus_cur=0;

static int x1_cur,x2_cur,y1_cur,y2_cur;


/*============================================================================*/

struct	__alco_cfg{
	char tag[8]; //"ALCO.CFG"
	u32 B2_ref;
	u32 B10_ref;
	u32 B2_max;
	u32 B10_max;
};


/*============================================================================*/
enum eMSG{
	MY_MSG_FISRT	=WM_USER,

	MSG_CUR_X1_CHANGE, //X1 ���λ���б䶯.
	MSG_CUR_X2_CHANGE, //X2 ���λ���б䶯.
	MSG_CUR_Y1_CHANGE, //Y1 ���λ���б䶯.
	MSG_CUR_Y2_CHANGE, //Y2 ���λ���б䶯.

};

enum eID{

	ID_BTN_BEGIN,////////////

	ID_EXIT =1,
	ID_START,
	ID_SENSOR_ON,
	ID_SENSOR_OFF,
	ID_Y_STEP,
  ID_X_STEP,
	ID_PUMP,

	ID_BTN_END,//////////////

	ID_LISTBOX,

	ID_X1_CUR,
	ID_X2_CUR,
	ID_Y1_CUR,
	ID_Y2_CUR,

	ID_Y_STEP_SUB,
	ID_Y_STEP_ADD,

  ID_X_STEP_SUB,
	ID_X_STEP_ADD,

	ID_TEST,
  
  ID_X_STR,
	ID_Y_STR,

	ID_SCROLL1,
	ID_SCROLL2,

  ID_RB1,
  ID_RB2,  
  ID_RB3,  
  ID_RB4,

  ID_TEXT1,
  ID_TEXT2,
  ID_TEXT3,
  ID_TEXT4,
  ID_TEXT5,
  ID_TEXT6,
  ID_TEXT7,
  ID_TEXT8,  
  	ID_WAVE,

};



static struct __x_obj_item *focus_obj =NULL;
static struct __x_obj_item *button_item =NULL;
//static struct __x_obj_item *text_item =NULL;

static struct __x_obj_item *x1_cur_obj =NULL;
static struct __x_obj_item *x2_cur_obj =NULL;
static struct __x_obj_item *y1_cur_obj =NULL;
static struct __x_obj_item *y2_cur_obj =NULL;

/*============================================================================*/
/**
  * @brief  ������
  * @param  back_c ������ɫ
  * @param  Page_c ��������ɫ
  * @param  fore_c ������ɫ  * 
  * @retval ��
  */
/*
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  hdc:    ��ͼ������
 * @param  back_c��������ɫ
 * @param  Page_c: ������Page������ɫ
 * @param  fore_c���������������ɫ
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* ���� */
	SetBrushColor(hdc, back_c);
	FillRect(hdc, &rc);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2-1;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* ���� */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	//rc.y += (rc.h >> 2) >> 1;
	//rc.h -= (rc.h >> 2);
	/* �߿� */
	//FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
   //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
}
static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
  HDC hdc;
  RECT rc;
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;

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
static void RB_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
   
   //
	FillRect(hdc, &rc); //�þ�����䱳��

   if (ds->State & BST_CHECKED)
	{ //��ť��ѡ��״̬
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
    SetBrushColor(hdc,MapRGB(hdc,220,200,200)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
    SetPenColor(hdc,MapRGB(hdc,30,30,230));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)

    FillCircle(hdc, rc.x+rc.w/2, rc.y+rc.w/2, rc.w/2);
    SetBrushColor(hdc,MapRGB(hdc,90,174,22));
    FillCircle(hdc, rc.x+rc.w/2, rc.y+rc.w/2, rc.w/2-3);
	}
	else
	{ 
		SetBrushColor(hdc,MapRGB(hdc,200,220,200));
		SetPenColor(hdc,MapRGB(hdc,50,50,50));

    FillCircle(hdc, rc.x+rc.w/2, rc.y+rc.w/2, rc.w/2);
	}



}
/*
 * @brief  �Զ��廬�������ƺ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	RECT rc_cli;
	//	int i;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);         
   	
	//���ư�ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc, RGB888(0, 0, 0), RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//������ɫ���͵Ĺ�����

	//���ƻ���
	if (ds->State & SST_THUMBTRACK)//����
	{
		draw_scrollbar(hwnd, hdc, RGB888(0, 0, 0), RGB888(	50, 205, 50), RGB888(50, 205, 50));
	}
	else//δѡ��
	{
	}
}

static	void btn_draw(HDC hdc,struct __x_obj_item * obj)
{
//	HFONT hFont;

//	hFont =GetFont(hdc);

	if(obj->rc.w > 60 && obj->rc.h > 20)
	{
//		SetFont(hdc,hFont20);
	}


	if(obj == focus_obj)
	{
		SetTextColor(hdc,MapRGB(hdc,255,0,255));
		SetPenColor(hdc,MapRGB(hdc,200,0,200));
		SetBrushColor(hdc,MapRGB(hdc,100,0,100));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

	}
	else
	{
		// RECT rc;
    /* ��Ӱ */
		// CopyRect(&rc,&obj->rc);
		// OffsetRect(&rc,2,2);
		// SetBrushColor(hdc,MapRGB(hdc,20,20,20));
		// FillRect(hdc,&rc);

		SetTextColor(hdc,MapRGB(hdc,255,255,255));
	  SetPenColor(hdc,MapRGB(hdc,255,255,255));
	  SetBrushColor(hdc,MapRGB(hdc,90,174,22));
		DrawText(hdc,obj->pszText,-1,&obj->rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
	}

//	SetFont(hdc,hFont);

}


/**
  * @brief  ����ʾ�����м�Ĵ���
  */

static void DrawFrame(HDC hdc,const POINT *pt,int count,COLORREF color)
{
	int i,x,y;
	WCHAR wbuf[128];
	RECT rc;

	ClrDisplay(hdc,NULL,MapRGB(hdc,0,0,0));

	ClrDisplay(hdc,&rc_wav,MapRGB(hdc,0,40,40));

	////Draw Y
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	i=0;

	x =rc_wav.x;
	y =rc_wav.y+rc_wav.h-1;

	rc.x =2;
	rc.y =y-8;
	rc.w =LEFT_OFFSET-2;
	rc.h =40;

	while(y >= rc_wav.y)
	{
    /* ������һ���ߣ����������ÿ� */
    if(i!=0)  
		  HLine(hdc,x,y,x+rc_wav.w);

		if(i>0)
		{
			x_wsprintf(wbuf,L"%d",(int)(i*y_step[y_step_cur]*40));//.1f
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_TOP|DT_RIGHT);
		}


		i++;
		y -= 20;
		rc.y -= 20;

	}

	////Draw X
	SetPenColor(hdc,MapRGB(hdc,0,60,80));
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
	i=0;
	x=rc_wav.x;
	y=rc_wav.y;

	rc.x =x-8;
	rc.y =rc_wav.y+rc_wav.h-4;//���κ�������ֵλ��
	rc.w =40;
	rc.h =20;

	while(x < (rc_wav.x+rc_wav.w))
	{
     /* ������һ���ߣ����������ÿ� */
    if(i!=0)
		  VLine(hdc,x,y,y+rc_wav.h);

		if(i>0)
		{
			x_wsprintf(wbuf,L"%d",i*x_step[x_step_cur]*40/1000);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}

		i++;
		x+=20;
		rc.x+=20;
	}

	/////DrawFrame
	if(count>0)
	{
		int x1,y1,x2,y2;
		SetPenColor(hdc,color);

		for(i=1;i<count;i++)
		{
			x1 = pt[i-1].x + rc_wav.x;
			y1 =-pt[i-1].y + rc_wav.y+rc_wav.h-1;
			x1 =MAX(x1,rc_wav.x);
			x1 =MIN(x1,rc_wav.x+rc_wav.w-1);
			y1 =MAX(y1,rc_wav.y);
			y1 =MIN(y1,rc_wav.y+rc_wav.h-1);

			x2 = pt[i].x + rc_wav.x;
			y2 =-pt[i].y + rc_wav.y+rc_wav.h-1;
			x2 =MAX(x2,rc_wav.x);
			x2 =MIN(x2,rc_wav.x+rc_wav.w-1);
			y2 =MAX(y2,rc_wav.y);
			y2 =MIN(y2,rc_wav.y+rc_wav.h-1);

			Line(hdc,x1,y1,x2,y2);
			// AA_DrawLine(hdc,x1,y1,x2,y2);
			// AA_DrawBoldLine(hdc,x1,y1,x2,y2);
		}
	}

	////Cursor X1
  SetPenColor(hdc,MapRGB(hdc,90,174,22));

	x=x1_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor X2
	x=x2_cur+rc_wav.x;
	VLine(hdc,x,rc_wav.y-2,rc_wav.y+rc_wav.h-1);

	////Cursor Y1
  SetPenColor(hdc,MapRGB(hdc,238,206,31));

	y=rc_wav.y+rc_wav.h-y1_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);

	////Cursor Y2
	y=rc_wav.y+rc_wav.h-y2_cur;
	HLine(hdc,rc_wav.x,y,rc_wav.x+rc_wav.w-1+2);

	////button obj
	x_obj_draw(hdc,button_item);
	// x_obj_draw(hdc,text_item);

}

#define PI (float)(3.14)
/*
 ���ǲ����������Һ������ƣ���׼����Ϊ2��,ռ�ձ�p ������ʱ������ڣ���ʾ���ǲ������½��εı���
 p=0.5Ϊ�����½��ԳƵġ���[0,2�С�p] ��Ϊ�����أ�
 ��2�С�p,2��,2������Ϊ�½��ء�
*/
static float trianglewav(float t,float p)
{
  //���ǲ�������p��0��1ռ�ձ�
  //trianglewav(t,p)������Ϊ2pi
  //����ֵ��-1��+1��
  float y;
  //��t�黯��0��2pi����
  if(t>=0)
    t=(t/(2*PI)-(int)(t/(2*PI)))*2*PI;
  else
    t=2*PI+(t/(2*PI)-(int)(t/(2*PI)))*2*PI;
  //���ռ�ձȲ�����Χ�Ƿ�Ϸ�
  if(p<0 || p>1){return 0;}
  p=p*2*PI;
  //��pΪ0��2*piʱ�Ľ��ƴ���
  p=(p-2*PI==0)?2*PI-1e-10:p;
  p=(p==0)?1e-10:p;
  
  if(t<p)
    {
      y=2*t/p-1;//����������
    }
  else
    {
      y=-2*t/(2*PI-p)+(2*PI+p)/(2*PI-p);//�½���
    }
  return y;
}

/*
 �����������������Һ������ƣ���׼����Ϊ2��,ռ�ձ�p 
 p=0.5Ϊ�����½��ԳƵġ���[0,2�С�p] ��Ϊ�ߵ�ƽ��
 ��2�С�p,2��,2������Ϊ�͵�ƽ��
*/
static int squarewav(float t ,float p)
{
 //����������p��0��1ռ�ձ�
  //squarewav(t,p)������Ϊ2pi
  //����ֵ��-1��+1��
  int y;
  //��t�黯��0��2pi����
  if(t>=0)
    t=(t/(2*PI)-(int)(t/(2*PI)))*2*PI;
  else
    t=2*PI+(t/(2*PI)-(int)(t/(2*PI)))*2*PI;
  //���ռ�ձȲ�����Χ�Ƿ�Ϸ�
  if(p<0 || p>1){return 0;}
  p=p*2*PI;
  //��pΪ0��2*piʱ�Ľ��ƴ���
  p=(p-2*PI==0)?2*PI-1e-10:p;
  p=(p==0)?1e-10:p;
  
  if(t<p)
    {
      y=1;//�ߵ�ƽ
    }
  else
    {
      y=-1;//�͵�ƽ
    }
  return y;

}



static void wave_owner_draw(HDC hdc)
{
	int x_cur,y_cur;
  
  switch(wave_format)
  {
    case SINE_WAVE:
      for(x_cur=0;x_cur < rc_wav.w;x_cur++)
      {
        float mV;
    
        /* 
        *  y = A sin(2*pi*f*t + p) + k 
        *  ����
        *   A����ֵ
        *   pi��Բ����
        *   f��Ƶ��
        *   t��ʱ��
        *   p����λ
        *   k��ƫ��
        */
        /* ��ʱ�䰴��ʵ���̼���Ƚ��鷳����ʾ������Ƶ��Ҫ��Ӧ�ض����̣�������չʾ */
        // mV = wave_amp*(sin(2*3.14*(1.0*x_cur*x_step[x_step_cur]/1000)*wave_freq)+1);
        float t;
        /* 800�Ǵ�Ű����ֳ�800���� */
        t=-1+1.0/800*x_cur;
        // t=-1+1.0/800*x_cur*x_step[x_step_cur]/1000;

        /* ֱ�������ص��ʾ */
        mV = wave_amp*(sin(2*3.14*(t)*wave_freq)+1);
        
        y_cur =(int)(mV/y_step[y_step_cur]);

        pt_wav[x_cur].x =x_cur;
        pt_wav[x_cur].y =y_cur;
      }
    break;

    case TRIANGULAR_WAVE:
      for(x_cur=0;x_cur < rc_wav.w;x_cur++)
      {
        float mV;
    
        /* 
        *  y = A trianglewav(2*pi*f*t + p) + k 
        *  ����
        *   A����ֵ
        *   pi��Բ����
        *   f��Ƶ��
        *   t��ʱ��
        *   p����λ
        *   k��ƫ��
        */
        /* ��ʱ�䰴��ʵ���̼���Ƚ��鷳����ʾ������Ƶ��Ҫ��Ӧ�ض����̣�������չʾ */
        // mV = wave_amp*(sin(2*3.14*(1.0*x_cur*x_step[x_step_cur]/1000)*wave_freq)+1);
        
        float t;
        
        t=-1+1.0/800*x_cur;
        // t=-1+1.0/800*x_cur*x_step[x_step_cur]/1000;

        /* ֱ�������ص��ʾ */
        mV = wave_amp*(trianglewav(2*3.14*(t)*wave_freq,0.5)+1);
        
        y_cur =(int)(mV/y_step[y_step_cur]);

        pt_wav[x_cur].x =x_cur;
        pt_wav[x_cur].y =y_cur;
      }

    break;

    case SQUARE_WAVE:      
      for(x_cur=0;x_cur < rc_wav.w;x_cur++)
      {
        float mV;
    
        /* 
        *  y = A squarewav(2*pi*f*t + p) + k 
        *  ����
        *   A����ֵ
        *   pi��Բ����
        *   f��Ƶ��
        *   t��ʱ��
        *   p����λ
        *   k��ƫ��
        */
        /* ��ʱ�䰴��ʵ���̼���Ƚ��鷳����ʾ������Ƶ��Ҫ��Ӧ�ض����̣�������չʾ */
        // mV = wave_amp*(sin(2*3.14*(1.0*x_cur*x_step[x_step_cur]/1000)*wave_freq)+1);
        
        float t;
        
        t=-1+1.0/800*x_cur;
        // t=-1+1.0/800*x_cur*x_step[x_step_cur]/1000;

        /* ֱ�������ص��ʾ */
        mV = wave_amp*(squarewav(2*3.14*(t)*wave_freq,0.5)+1);
        
        y_cur =(int)(mV/y_step[y_step_cur]);

        pt_wav[x_cur].x =x_cur;
        pt_wav[x_cur].y =y_cur;
      }
    break;

    case SCATTER:
    {    
      /* �Ƿ�Ҫ������������� */
      if(scatter_init == 0)
      {
        for(x_cur=0;x_cur < rc_wav.w;x_cur++)
        {
          y_cur = x_rand()%200;
          
          pt_wav[x_cur].x =x_cur;
          pt_wav[x_cur].y =y_cur;
        }
        
        scatter_init =1;
      }
      
//      for(x_cur=0;x_cur < rc_wav.w;x_cur++)
//      {
//          pt_wav[x_cur].x =pt_wav2[x_cur].x;
//          pt_wav[x_cur].y =pt_wav2[x_cur].y;   
//      }
    }
    break;
    
    default:
    break;

  }	

	DrawFrame(hdc,pt_wav,rc_wav.w,MapRGB(hdc,255,255,255));
}

/*============================================================================*/

//������ʾ�Ĵ��ڹ���.
static	LRESULT	WaveWinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
	case WM_CREATE:

			x1_cur=15;
			x2_cur=50;
			y1_cur=30;
			y2_cur=70;
			// x_step=50; //50ms
			y_step_cur=0;
      x_step_cur=0;


			focus_obj =NULL;

			GetClientRect(hwnd,&rc);

			GetClientRect(hwnd,&rc_main);
			CopyRect(&rc_wav,&rc_main);
			InflateRectEx(&rc_wav,-LEFT_OFFSET,-TOP_OFFSET,-RIGHT_OFFSET,-BOTTOM_OFFSET);

			rc_x1_cur.x =rc_wav.x+x1_cur-CUR_BOX_SIZE/2;
			rc_x1_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x1_cur.w =CUR_BOX_SIZE;
			rc_x1_cur.h =CUR_BOX_SIZE;

			rc_x2_cur.x =rc_wav.x+x2_cur-CUR_BOX_SIZE/2;
			rc_x2_cur.y =rc_wav.y-CUR_BOX_SIZE;
			rc_x2_cur.w =CUR_BOX_SIZE;
			rc_x2_cur.h =CUR_BOX_SIZE;

			rc_y1_cur.x =rc_wav.x+rc_wav.w;
			rc_y1_cur.y =rc_wav.y+rc_wav.h-y1_cur-CUR_BOX_SIZE/2;
			rc_y1_cur.w =CUR_BOX_SIZE;
			rc_y1_cur.h =CUR_BOX_SIZE;

			rc_y2_cur.x =rc_wav.x+rc_wav.w;
			rc_y2_cur.y =rc_wav.y+rc_wav.h-y2_cur-CUR_BOX_SIZE/2;
			rc_y2_cur.w =CUR_BOX_SIZE;
			rc_y2_cur.h =CUR_BOX_SIZE;
      
  		if(1)
			{
        /* ��� */

				x1_cur_obj = x_obj_create(L"X1", ID_X1_CUR, &rc_x1_cur,	X_OBJ_VISIBLE,0,button_item);
				x2_cur_obj = x_obj_create(L"X2", ID_X2_CUR, &rc_x2_cur,	X_OBJ_VISIBLE,0,button_item);
				y1_cur_obj = x_obj_create(L"Y1", ID_Y1_CUR, &rc_y1_cur,	X_OBJ_VISIBLE,0,button_item);
				y2_cur_obj = x_obj_create(L"Y2", ID_Y2_CUR, &rc_y2_cur,	X_OBJ_VISIBLE,0,button_item);
			}
			return TRUE;
			////

	case	WM_NOTIFY:
	{
		// U16 code,id;
    // NMHDR *nr;

		// code =HIWORD(wParam);
		// id   =LOWORD(wParam);
    // nr =(NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
	/////

	case    WM_LBUTTONDOWN: //
			{
				int x,y;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				focus_obj =x_obj_get_from_pos(button_item,x,y); //��x,y����ֵ��� x_obj����.
				if(focus_obj==NULL)
				{ //û��
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}

				if(focus_obj->id == ID_X1_CUR)
				{
					focus_cur =CUR_X1;
					InvalidateRect(hwnd,&x1_cur_obj->rc,FALSE);
				}

				if(focus_obj->id == ID_X2_CUR)
				{
					focus_cur =CUR_X2;
					InvalidateRect(hwnd,&x2_cur_obj->rc,FALSE);
				}

				if(focus_obj->id == ID_Y1_CUR)
				{
					focus_cur =CUR_Y1;
					InvalidateRect(hwnd,&y1_cur_obj->rc,FALSE);
				}

				if(focus_obj->id == ID_Y2_CUR)
				{
					focus_cur =CUR_Y2;
					InvalidateRect(hwnd,&y2_cur_obj->rc,FALSE);
				}

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////
	case WM_MOUSELEAVE:
	case WM_LBUTTONUP:
			{

				if(focus_obj!=NULL)
				{
					InvalidateRect(hwnd,&focus_obj->rc,FALSE);
				}

				/////
				focus_cur =0;
				focus_obj =NULL;
			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////
     

	case	WM_MOUSEMOVE:
			{
				int x,y;

				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);


				x =x-rc_wav.x;
				x=MAX(x,0);
				x=MIN(x,rc_wav.w-1);

				y =y-rc_wav.y;
				y=MAX(y,0);
				y=MIN(y,rc_wav.h-1);

				if(focus_cur == CUR_X1) // X1���.
				{

					////�ƶ�ǰ��λ��ˢ��.
					rc.x =x1_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x1_cur_obj->rc,FALSE);

					////�ƶ����µ�λ��.
					x1_cur =x;
					x_obj_move_to(x1_cur_obj,(x1_cur+rc_wav.x)-(x1_cur_obj->rc.w/2),x1_cur_obj->rc.y);

					//�ƶ����λ��ˢ��.
					rc.x =x1_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x1_cur_obj->rc,FALSE);

					////�������ڷ� MSG_CUR_X1_CAHNGLE ��Ϣ.
					PostMessage(GetParent(hwnd),MSG_CUR_X1_CHANGE,x1_cur,0);
				}

				if(focus_cur == CUR_X2) // X1���.
				{
					rc.x =x2_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x2_cur_obj->rc,FALSE);

					x2_cur =x;
					x_obj_move_to(x2_cur_obj,(x+rc_wav.x)-(x2_cur_obj->rc.w/2),x2_cur_obj->rc.y);

					rc.x =x2_cur + rc_wav.x;
					rc.y =rc_wav.y;
					rc.w =1;
					rc.h =rc_wav.h;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&x2_cur_obj->rc,FALSE);

					////
					PostMessage(GetParent(hwnd),MSG_CUR_X2_CHANGE,x2_cur,0);

				}

				if(focus_cur == CUR_Y1)
				{
					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y1_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y1_cur_obj->rc,FALSE);

					y1_cur =rc_wav.h-y;
					x_obj_move_to(y1_cur_obj,y1_cur_obj->rc.x,(y+rc_wav.y)-(y1_cur_obj->rc.h/2));

					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y1_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y1_cur_obj->rc,FALSE);

					////
					PostMessage(GetParent(hwnd),MSG_CUR_Y1_CHANGE,y1_cur,0);
				}

				if(focus_cur == CUR_Y2)
				{
					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y2_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y2_cur_obj->rc,FALSE);

					y2_cur =rc_wav.h-y;
					x_obj_move_to(y2_cur_obj,y2_cur_obj->rc.x,(y+rc_wav.y)-(y2_cur_obj->rc.h/2));

					rc.x =rc_wav.x;
					rc.y =rc_wav.y+rc_wav.h-y2_cur;
					rc.w =rc_wav.w;
					rc.h =1;
					InvalidateRect(hwnd,&rc,FALSE);
					InvalidateRect(hwnd,&y2_cur_obj->rc,FALSE);

					////
					PostMessage(GetParent(hwnd),MSG_CUR_Y2_CHANGE,y2_cur,0);

				}

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

		case	WM_ERASEBKGND:
		{
			// HDC hdc=(HDC)wParam;
			// RECT rc;

			// GetClientRect(hwnd,&rc);
			// SetBrushColor(hdc,MapRGB(hdc,0,0,0));
			// FillRect(hdc,&rc);
			return TRUE;
		}
   case WM_CLOSE:
   {
      return DestroyWindow(hwnd);
   }

	////

	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;

		hdc =BeginPaint(hwnd,&ps);

		///////
	  wave_owner_draw(hdc);
		EndPaint(hwnd,&ps);
	}
	break;
	////

	case WM_DESTROY:
	{
		// x_obj_del(button_item);
    // x_obj_del(text_item);

		// DeleteDC(hdcTop);
		// DeleteSurface(pSurfTop);
		// DeleteFont(hFont20);
		// DeleteFont(hFont24);

		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	////

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;
}



static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  
  


	switch(msg)
	{
	case WM_CREATE:
  
			x1_cur=15;
			x2_cur=50;
			y1_cur=30;
			y2_cur=70;
			// x_step=50; //50ms
			y_step_cur=2;
      x_step_cur=0;


			focus_obj =NULL;

//			hFont20 =XFT_CreateFont(ASCII_20_4BPP);
//			hFont24 =XFT_CreateFont(ASCII_24_4BPP);

			GetClientRect(hwnd,&rc);
			// pSurfTop =CreateSurface(SURF_ARGB4444,rc.w,rc.h,NULL,0);
			// hdcTop =CreateDC(pSurfTop,NULL);
			// ClrDisplay(hdcTop,NULL,MapARGB(hdcTop,0,0,0,0));


			GetClientRect(hwnd,&rc_main);
			CopyRect(&rc_wav,&rc_main);
			InflateRectEx(&rc_wav,-LEFT_OFFSET,-TOP_OFFSET,-RIGHT_OFFSET,-BOTTOM_OFFSET);
      /* ����������Ҫʹ�õĿռ� */
      pt_wav = (POINT*)GUI_VMEM_Alloc(sizeof(POINT)*rc_wav.w);

      /* �ײ��ı�ǩ�� */
			rc_label.x =0;
      rc_label.h =36;
			rc_label.y =rc_main.h-rc_label.h+1;
			rc_label.w =300-LEFT_OFFSET-RIGHT_OFFSET+16;

			rc_button.w =73;
			rc_button.h =rc_main.h-20;
			rc_button.x =rc_main.w-RIGHT_OFFSET;
			rc_button.y =0;
	

			if(1)
			{
				RECT rc,m_rc[8],cb_rc;
				HWND wnd;

        rc.w =rc_main.w-60*2+50;
        rc.h =20;
        rc.x = 30;
        rc.y =0;  

        CreateWindow(TEXTBOX,L"������ʾ",TBS_FLAT|TBS_CENTER|TBS_TOP|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT5,NULL,NULL);
        
        rc.w = 22;
        rc.h = 22;
        rc.x = 286;
        rc.y = 5;  
             
        /* �رհ�ť */  
        wnd=CreateWindow(BUTTON,L"O",	BS_FLAT|WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_EXIT,NULL,NULL); //����һ����ť.
        SetWindowFont(wnd,controlFont_24); //���ÿؼ���������.

				MakeMatrixRect(m_rc,&rc_button,3,13,1,7);
        
				// text_item =x_obj_create_class(L"TextItem",	0xFFFFFFFF,	&rc_button,X_OBJ_VISIBLE,0,text_draw);

				rc =m_rc[0];
				//OffsetRect(&rc,-100,0);

				rc.x =rc_main.w-rc.w;
            rc.y += 28;
				CreateWindow(TEXTBOX,L"ģ������",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT1,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(TEXTBOX,L"Ƶ�ʣ���λHz",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT2,NULL,NULL);

				OffsetRect(&rc,0,rc.h);
        {
            SCROLLINFO sif;
            sif.cbSize	=sizeof(sif);
            sif.fMask		=SIF_ALL;
            sif.nMin		=5;
            sif.nMax		=30;
            wave_freq = sif.nValue =15;
            sif.TrackSize		= rc.h;
            sif.ArrowSize		=0;//20;

            wnd = CreateWindow(SCROLLBAR,L"HScroll1",WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SCROLL1,NULL,NULL);
            SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
			
            sif.nMin		=5;
            sif.nMax		=250;
            wave_amp = sif.nValue =100;

          	OffsetRect(&rc,0,rc.h+10);
				    CreateWindow(TEXTBOX,L"��ֵ����λmV",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEXT3,NULL,NULL);

				    OffsetRect(&rc,0,rc.h);
            wnd = CreateWindow(SCROLLBAR,L"HScroll2",WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_SCROLL2,NULL,NULL);
            SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
        }
				
        OffsetRect(&rc,0,rc.h+5);
        CopyRect(&cb_rc,&rc);
        cb_rc.x -= 2;
        cb_rc.h -= 5;
        cb_rc.y += 10;
        cb_rc.w = cb_rc.h;

        //������ѡ��ť(GroupID��Ϊ1).
        CreateWindow(BUTTON,L"",WS_OWNERDRAW|BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,ID_RB1,NULL,NULL);

        OffsetRect(&cb_rc,cb_rc.w+7,0);
        CreateWindow(BUTTON,L"",WS_OWNERDRAW|BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,ID_RB2,NULL,NULL);

        OffsetRect(&cb_rc,cb_rc.w+7,0);
        CreateWindow(BUTTON,L"",WS_OWNERDRAW|BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,ID_RB3,NULL,NULL);
        
        OffsetRect(&cb_rc,cb_rc.w+7,0);
        CreateWindow(BUTTON,L"",WS_OWNERDRAW|BS_RADIOBOX|WS_VISIBLE,cb_rc.x,cb_rc.y,cb_rc.w,cb_rc.h,hwnd,ID_RB4,NULL,NULL);
	  
        /* ����Ĭ�ϰ�ť i g*/
    		SendMessage(GetDlgItem(hwnd,ID_RB1),BM_SETSTATE,BST_CHECKED,0);

        OffsetRect(&rc,-2,rc.h+5);
				wnd = CreateWindow(TEXTBOX,L"j",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,cb_rc.w,rc.h,hwnd,ID_TEXT4,NULL,NULL);
        SetWindowFont(wnd,controlFont_16);
        
        OffsetRect(&rc,cb_rc.w+7,0);
        wnd = CreateWindow(TEXTBOX,L"h",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,cb_rc.w,rc.h,hwnd,ID_TEXT6,NULL,NULL);
        SetWindowFont(wnd,controlFont_16);
        
        OffsetRect(&rc,cb_rc.w+7,0);
				wnd = CreateWindow(TEXTBOX,L"i",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,cb_rc.w,rc.h,hwnd,ID_TEXT7,NULL,NULL);
        SetWindowFont(wnd,controlFont_16);
        
        OffsetRect(&rc,cb_rc.w+7,0);
        wnd = CreateWindow(TEXTBOX,L"g",TBS_FLAT|TBS_CENTER|WS_VISIBLE,rc.x,rc.y,cb_rc.w,rc.h,hwnd,ID_TEXT8,NULL,NULL);
        SetWindowFont(wnd,controlFont_16);            
            
        /* ��ť */
				button_item =x_obj_create_class(L"ButtonItem",	0xFFFFFFFF,	&rc_button,X_OBJ_VISIBLE,0,btn_draw);

				rc.w =rc_main.w-RIGHT_OFFSET-170;
				rc.h =15;
				rc.x =10;
				rc.y =rc_main.h - 2*rc.h-4;


        ////����һ�� TEXTBOX.

		    // x_wsprintf(wbuf,L"X1: % 5dmS, X2: % 5dmS, X2-X1: % 5dmS",x1_cur*x_step[x_step_cur],x2_cur*x_step[x_step_cur],(x2_cur-x1_cur)*x_step[x_step_cur]);
        // CreateWindow(TEXTBOX,wbuf,WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_X_STR,NULL,NULL);

        // ////����һ�� TEXTBOX.
        // OffsetRect(&rc,0,rc.h+2);
        // x_wsprintf(wbuf,L"Y1: %6.1fmV, Y2: %6.1fmV, Y2-Y1: %6.1fmV",y1_cur*y_step[y_step_cur],y2_cur*y_step[y_step_cur],(y2_cur-y1_cur)*y_step[y_step_cur]);

        // CreateWindow(TEXTBOX,wbuf,WS_BORDER|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_Y_STR,NULL,NULL);


				rc.w =16;
				rc.h =16;
				rc.x =rc_main.w-rc.w;
				rc.y =rc_main.h - 2*rc.h-4;
				x_obj_create(L"+",	ID_Y_STEP_ADD,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =40;
				OffsetRect(&rc,-(rc.w+2),0);
				x_obj_create(y_step_str[y_step_cur],ID_Y_STEP,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =16;
				OffsetRect(&rc,-(rc.w+2),0);
				x_obj_create(L"-",	ID_Y_STEP_SUB,	&rc,	X_OBJ_VISIBLE,0,button_item);
				
				rc.w =16;
				rc.h =16;
				rc.x =rc_main.w-rc.w;
				rc.y =rc_main.h - 2*rc.h-4;
        OffsetRect(&rc,0,rc.h+2);
        x_obj_create(L"+",	ID_X_STEP_ADD,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =40;
				OffsetRect(&rc,-(rc.w+2),0);
				x_obj_create(x_step_str[x_step_cur],ID_X_STEP,	&rc,	X_OBJ_VISIBLE,0,button_item);

				rc.w =16;
				OffsetRect(&rc,-(rc.w+2),0);
				x_obj_create(L"-",	ID_X_STEP_SUB,	&rc,	X_OBJ_VISIBLE,0,button_item);
				OffsetRect(&rc,-(rc.w+2),0);

			}

      if(1)
				{
					WNDCLASS wcex;

					wcex.Tag	 		= WNDCLASS_TAG;

					wcex.Style			= CS_HREDRAW | CS_VREDRAW;
					wcex.lpfnWndProc	= (WNDPROC)WaveWinProc;
					wcex.cbClsExtra		= 0;
					wcex.cbWndExtra		= 0;
					wcex.hInstance		= NULL;
					wcex.hIcon			= NULL;
					wcex.hCursor		= NULL;
					
          rc.x =0;
					rc.y =20;
					rc.w =245;
					rc.h =rc_main.h-rc.y-35;
					////����"������ʾ"�Ŀؼ�.
					CreateWindow(&wcex,L"---",WS_CLIPCHILDREN|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_WAVE,NULL,NULL);

					// ////����һ�� TEXTBOX.
					// OffsetRect(&rc,0,rc.h+4);
					// rc.h =24;
					// CreateWindow(TEXTBOX,L"---",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_X_STR,NULL,NULL);

					// ////����һ�� TEXTBOX.
					// OffsetRect(&rc,0,rc.h);
					// CreateWindow(TEXTBOX,L"---",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_Y_STR,NULL,NULL);
				}
			return TRUE;
			////

  case WM_CTLCOLOR:
		{
			u16 id;

		id =LOWORD(wParam);			
      if(id >= ID_TEXT1 && id <= ID_TEXT8)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        cr->TextColor =RGB888(255,255,255);
        cr->BackColor =RGB888(0,0,0);
        cr->BorderColor =RGB888(0,0,0);
         
				return TRUE;
      }
//      else if(id >= ID_RB1 && id <= ID_RB4)
//      {
//        CTLCOLOR *cr;
//        cr =(CTLCOLOR*)lParam;

//				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_CHECKED)
//				{
//					cr->TextColor =RGB888(0,0,0);      //������ɫ��RGB32��ɫ��ʽ)
//					cr->BackColor =RGB888(220,200,200);    //������ɫ��RGB32��ɫ��ʽ)
//					cr->BorderColor =RGB888(30,30,230);  //�߿���ɫ��RGB32��ɫ��ʽ)
//					cr->ForeColor =RGB888(90,174,22);  //ǰ����ɫ��RGB32��ɫ��ʽ)
//				}
//				else
//				{
//					cr->TextColor =RGB888(0,0,0);
//					cr->BackColor =RGB888(200,220,200);
//					cr->BorderColor =RGB888(50,50,50);
//					cr->ForeColor =RGB888(180,200,230);
//				}
//         
//				return TRUE;
//      }
      else if(id == ID_X_STR || id == ID_Y_STR)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        cr->TextColor =RGB888(255,255,255);      //������ɫ��RGB32��ɫ��ʽ)
        cr->BackColor =RGB888(90,174,22);    //������ɫ��RGB32��ɫ��ʽ)
        cr->BorderColor =RGB888(255,255,255);  //�߿���ɫ��RGB32��ɫ��ʽ)
        // cr->ForeColor =RGB888(90,174,22);  //ǰ����ɫ��RGB32��ɫ��ʽ)

				return TRUE;
      }

			else
			{
				//������ťʹ��ϵͳĬ�ϵ���ɫ���л��ƣ�����ֱ�ӷ���FALSE.
				return FALSE;
			}
		}	

	case    WM_LBUTTONDOWN: //
			{
				int x,y;
				x=GET_LPARAM_X(lParam);
				y=GET_LPARAM_Y(lParam);

				focus_obj =x_obj_get_from_pos(button_item,x,y); //��x,y����ֵ��� x_obj����.

				if(focus_obj==NULL)
				{ //û��
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}
        
       			else if(focus_obj->id == ID_Y_STEP_SUB)
				{
					InvalidateRect(hwnd,&focus_obj->rc,FALSE);

					if(y_step_cur > 0)
					{
						y_step_cur--;
						x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
						InvalidateRect(hwnd,&x_obj_get_from_id(button_item,ID_Y_STEP)->rc,FALSE);
						InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
						// InvalidateRect(GetDlgItem(hwnd,ID_Y_STR),NULL,FALSE);
						//�� MSG_CUR_Y_CAHNGLE ��Ϣ.
						PostMessage(hwnd,MSG_CUR_Y1_CHANGE,x1_cur,0);

					}
				}

				else if(focus_obj->id == ID_Y_STEP_ADD)
				{
					InvalidateRect(hwnd,&focus_obj->rc,FALSE);

					if(y_step_cur < (Y_STEP_NUM-1))
					{
						y_step_cur++;
						x_obj_set_text(x_obj_get_from_id(button_item,ID_Y_STEP),y_step_str[y_step_cur]);
						InvalidateRect(hwnd,&x_obj_get_from_id(button_item,ID_Y_STEP)->rc,FALSE);
						InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
						// InvalidateRect(GetDlgItem(hwnd,ID_Y_STR),NULL,FALSE);
						PostMessage(hwnd,MSG_CUR_Y1_CHANGE,x1_cur,0);
					}
				}

        else if(focus_obj->id == ID_X_STEP_SUB)
				{
          InvalidateRect(hwnd,&focus_obj->rc,FALSE);

          if(x_step_cur > 0)
          {
            x_step_cur--;
            x_obj_set_text(x_obj_get_from_id(button_item,ID_X_STEP),x_step_str[x_step_cur]);
            InvalidateRect(hwnd,&x_obj_get_from_id(button_item,ID_X_STEP)->rc,FALSE);
            InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
            // InvalidateRect(GetDlgItem(hwnd,ID_X_STR),NULL,FALSE);
            //�� MSG_CUR_X1_CAHNGLE ��Ϣ.
					  PostMessage(hwnd,MSG_CUR_X1_CHANGE,x1_cur,0);

          }

				}

				else if(focus_obj->id == ID_X_STEP_ADD)
				{
          InvalidateRect(hwnd,&focus_obj->rc,FALSE);

          if(x_step_cur < (X_STEP_NUM-1))
          {
            x_step_cur++;
            x_obj_set_text(x_obj_get_from_id(button_item,ID_X_STEP),x_step_str[x_step_cur]);
            InvalidateRect(hwnd,&x_obj_get_from_id(button_item,ID_X_STEP)->rc,FALSE);
            InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
            // InvalidateRect(GetDlgItem(hwnd,ID_X_STR),NULL,FALSE);
            //�� MSG_CUR_X1_CAHNGLE ��Ϣ.
					  PostMessage(hwnd,MSG_CUR_X1_CHANGE,x1_cur,0);

          }

				} 

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////

	case WM_MOUSELEAVE:
	case WM_LBUTTONUP:
			{
				/////
				if(focus_obj!=NULL)
				{
					InvalidateRect(hwnd,&focus_obj->rc,FALSE);
				}
				focus_cur =0;
				focus_obj =NULL;
        				// InvalidateRect(hwnd,NULL,TRUE);

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);
			////
  
	case	WM_NOTIFY:
	{
		U16 code,id;
    NMHDR *nr;

		code =HIWORD(wParam);
		id   =LOWORD(wParam);
    nr =(NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.

		if(id==ID_EXIT && code==BN_CLICKED)
		{
      		/* ����������α�־ */
			scatter_init = 0;
			PostCloseMessage(GetDlgItem(hwnd,ID_WAVE));
			PostCloseMessage(hwnd);
		}
    else if(id == ID_SCROLL1 || id == ID_SCROLL2 )
    {
      NM_SCROLLBAR *sb_nr;
      int i;

      sb_nr =(NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
      switch(nr->code)
      {

        case SBN_THUMBTRACK: //�����ƶ�
        {

          i =sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
          if(id == ID_SCROLL1)
            wave_freq = i;
          else if(id == ID_SCROLL2)
            wave_amp = i;


          SendMessage(nr->hwndFrom,SBM_SETVALUE,TRUE,i); //����λ��ֵ
          // InvalidateRect();
          // SendMessage(GetDlgItem(hwnd,ID_WAVE),WM_PAINT,NULL,NULL); //����λ��ֵ
          // if(wave_format != SCATTER)
            InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);

        /* ����������α�־ */
        scatter_init = 0;
 
        }
        break;
        default:
          break;
      }
    }
    else if(id >= ID_RB1 && id<= ID_RB4)
		{
			if(code == BN_CLICKED) //�������
			{

        /* ����������α�־ */
        scatter_init = 0;
        switch(id)
        {
          case ID_RB1:
            wave_format = SINE_WAVE;
            break;
          case ID_RB2:
            wave_format = SQUARE_WAVE;
            break;
          case ID_RB3:
            wave_format = TRIANGULAR_WAVE;
            break;
          case ID_RB4:
            wave_format = SCATTER;
            break;
          default:
            wave_format = SINE_WAVE;
            break;
        }	
        InvalidateRect(GetDlgItem(hwnd,ID_WAVE),NULL,FALSE);
			}
		}
 
	}
	break;
	/////
	case MSG_CUR_X1_CHANGE:
	case MSG_CUR_X2_CHANGE:
	{

    InvalidateRect(hwnd,&rc_label,TRUE);

		// WCHAR wbuf[128];

		// x_wsprintf(wbuf,L"X1: % 5dmS, X2: % 5dmS, X2-X1: % 5dmS",x1_cur*x_step[x_step_cur],x2_cur*x_step[x_step_cur],(x2_cur-x1_cur)*x_step[x_step_cur]);
		// SetWindowText(GetDlgItem(hwnd,ID_X_STR),wbuf);
	}
	break;
	////

	case MSG_CUR_Y1_CHANGE:
	case MSG_CUR_Y2_CHANGE:
	{
    	InvalidateRect(hwnd,&rc_label,TRUE);

		// WCHAR wbuf[128];

		// x_wsprintf(wbuf,L"Y1: %6.1fmV, Y2: %6.1fmV, Y2-Y1: %6.1fmV",y1_cur*y_step[y_step_cur],y2_cur*y_step[y_step_cur],(y2_cur-y1_cur)*y_step[y_step_cur]);
		// SetWindowText(GetDlgItem(hwnd,ID_Y_STR),wbuf);
	}
	break;
  

		case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0,0,0));
			FillRect(hdc,&rc);
			return TRUE;
		}

   case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;
         if(ds->ID == ID_SCROLL1 || ds->ID == ID_SCROLL2)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
         if(ds->ID == ID_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;            
         }
         if(ds->ID >= ID_RB1 && ds->ID <= ID_RB4)
         {
            RB_owner_draw(ds);
            return TRUE;               
         }
		}
    break;
	////

	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		RECT m_rc[8];
		WCHAR wbuf[128];

		hdc =BeginPaint(hwnd,&ps);
	  x_obj_draw(hdc,button_item);

#if 1
    {
      MakeMatrixRect(m_rc,&rc_label,1,1,3,2);

      ////X Text
      SetTextColor(hdc,MapRGB(hdc,255,255,255));
      SetPenColor(hdc,MapRGB(hdc,255,255,255));
      SetBrushColor(hdc,MapRGB(hdc,90,174,22));

      m_rc[0].w -= 20; 
      x_wsprintf(wbuf,L"X1:%dmS",x1_cur*x_step[x_step_cur]);
      DrawText(hdc,wbuf,-1,&m_rc[0],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);//|DT_VCENTER
      
      m_rc[1].w -= 20; 
      m_rc[1].x -= 20; 
      x_wsprintf(wbuf,L"X2:%dmS",x2_cur*x_step[x_step_cur]);
      DrawText(hdc,wbuf,-1,&m_rc[1],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

      m_rc[2].x -= 40; 
      x_wsprintf(wbuf,L"X2-X1:%dmS",(x2_cur-x1_cur)*x_step[x_step_cur]);
      DrawText(hdc,wbuf,-1,&m_rc[2],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

      ////Y Text
      SetTextColor(hdc,MapRGB(hdc,255,255,255));
      SetPenColor(hdc,MapRGB(hdc,255,255,255));
      SetBrushColor(hdc,MapRGB(hdc,90,174,22));

      m_rc[3].w -= 20; 
      x_wsprintf(wbuf,L"Y1:%.1fmV",y1_cur*y_step[y_step_cur]);
      DrawText(hdc,wbuf,-1,&m_rc[3],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

      m_rc[4].w -= 20; 
      m_rc[4].x -= 20; 
      x_wsprintf(wbuf,L"Y2:%.1fmV",y2_cur*y_step[y_step_cur]);
      DrawText(hdc,wbuf,-1,&m_rc[4],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

      m_rc[5].x -= 40; 
      x_wsprintf(wbuf,L"Y2-Y1:%.1fmV",(y2_cur-y1_cur)*y_step[y_step_cur]);
      DrawText(hdc,wbuf,-1,&m_rc[5],DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);
    }
#endif
		EndPaint(hwnd,&ps);
	}
	break;
	////
  
  case WM_CLOSE:
  {

    return DestroyWindow(hwnd);
  }
  
	case WM_DESTROY:
	{
		x_obj_del(button_item);

//		DeleteFont(hFont20);
//		DeleteFont(hFont24);
    GUI_VMEM_Free(pt_wav);

    /* ����������α�־ */
    wave_format = SINE_WAVE;
		return PostQuitMessage(hwnd);
	}

  default:
    return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;
}

/*============================================================================*/

static void ADS1120_Init(void)
{
//	sensor_enable();
}

//static u16 ADS1120_GetData(void)
//{
//	//return get_alco_data();
//	return x_rand()%0x7FFF;
//}

//static float ADS1120_GetVoltage_mV(u16 addata)
//{
//	float val;

//	addata=ADS1120_GetData();
//	val=(float)addata*(float)2048/32768;
//	return val;
//}


extern "C" void	GUI_DEMO_ShowWave(void)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;
	RECT rc;
	////

	ShowCursor(FALSE);
	ADS1120_Init();

	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	rc.x =0;
	rc.y =0;
	rc.w =GUI_XSIZE;
	rc.h =GUI_YSIZE;

	hwnd	=CreateWindowEx(	WS_EX_NODRAG|WS_EX_FRAMEBUFFER,&wcex,
							L"Show Wave",
							WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
							rc.x,rc.y,rc.w,rc.h,
							NULL,0,NULL,NULL);

	ShowWindow(hwnd,SW_SHOW);
	

	ShowWindow(hwnd,SW_SHOW);
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
  UpdateWindow( GetDesktopWindow());
	ShowCursor(TRUE);
}
