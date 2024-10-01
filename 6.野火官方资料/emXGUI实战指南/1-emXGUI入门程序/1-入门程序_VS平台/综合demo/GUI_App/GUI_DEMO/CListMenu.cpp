

#include <stdio.h>
#include <stdlib.h>

#include "x_libc.h"
#include "x_obj.h"
//#include	"XFT_res.h"
#include	"CListMenu.h"


#include <emXGUI.h>
#include <GUI_Font_XFT.h>

#include "GUI_AppDef.h"


/*============================================================================*/


//#define TOP_BAR
#define	BOTTOM_BAR

/*============================================================================*/

enum eID
{
	ID_EXIT=0x1000,
	ID_OK,
	ID_PREV_PAGE,
	ID_NEXT_PAGE,
	ID_TMR_100,
//	ID_TMR_500,
	ID_PREV_OBJ,
	ID_NEXT_OBJ,

	ID_FOCUS_LEFT,
	ID_FOCUS_RIGHT,
	ID_FOCUS_UP,
	ID_FOCUS_DOWN,
	ID_FOCUS_PREV,
	ID_FOCUS_NEXT,

};


/*============================================================================*/


class	CListMenu{

public:
	RECT rc_list;

	LRESULT	OnCreate(HWND hwnd,list_menu_cfg_t *cfg);
	LRESULT	OnDestroy(HWND hwnd);
	LRESULT	OnPaint(HWND hwnd);
	LRESULT	OnLButtonDown(HWND hwnd,int x,int y);
	LRESULT	OnLButtonUp(HWND hwnd,int x,int y);
	LRESULT	OnMouseMove(HWND hwnd,int x,int y);
	LRESULT	OnNotify(HWND hwnd,u16 code,u16 id);
	LRESULT	OnKeyUp(HWND hwnd,int key_val);
	LRESULT	OnKeyDown(HWND hwnd,int key_val);
	LRESULT	OnTimer(HWND hwnd,int tmr_id);

	BOOL MoveToPrevPage(void);
	BOOL MoveToNextPage(void);

	void draw_icon_obj(HDC hdc, struct __x_obj_item *obj, u32 flag, u32 style);
	struct __x_obj_item *focus_list_obj;


private:
	//BOOL Init(int x,int y,int w,int h,int x_count,int y_count,const struct __obj_list *list_objs);

	void OffsetObjs(int dx,int dy);
	//void draw_icon_obj(HDC hdc,struct __x_obj_item *obj,u32 flag,u32 style);

	LRESULT DrawFrame(HDC hdc,HWND hwnd);

	void SetToPrevPage(void);
	void SetToNextPage(void);
	void SetToPage(int page);

	HWND hwndMain;

	int page_num,page_cur;
	int lbtn_down_x,lbtn_down_y;
	int x_move,x_moved,y_moved;
	int x_num,y_num;

	int	x_move_to,y_move_to;

	BOOL execu_obj;

	HFONT hFontSEG_32;

	//RECT rc_main,rc_title,rc_list;
	RECT rc_main,rc_title;

	const struct __obj_list *obj_tbl;

	//struct __x_obj_item *focus_list_obj;
	struct __x_obj_item *list_item;

};

/*============================================================================*/
#if 0
static WCHAR* LoadLanguage(WCHAR *buf,const WCHAR *str)
{
	x_wstrcpy(buf,str);
	return buf;
}
#endif

/*============================================================================*/
#define	OBJ_ACTIVE	(1<<0)


static BOOL is_ver_list(HWND hwnd)
{
	if(GetWindowLong(hwnd,GWL_STYLE)&LMS_VER)
	{
		return TRUE;
	}
	return FALSE;
}

void CListMenu::draw_icon_obj(HDC hdc,struct __x_obj_item *obj,u32 flag,u32 style)
{

//	WCHAR wstr[64],wbuf[64+12];
	RECT rc,rc0;
	const void *bmp, *icon;
	BITMAPINFO info;
	int x,y;
	//HDC hdc_ico;

	//hdc_ico =CreateMemoryDC(BM_DEVICE,obj->rc.w,obj->rc.h);
	rc =obj->rc;

	if(flag&OBJ_ACTIVE)
	{
	#if 1 /* 矩形背景 */
		SetBrushColor(hdc,MapRGB(hdc,160,100,100));
    InflateRect(&rc,-20,0);

		FillRect(hdc,&rc);
	#endif
		////
    /* 矩形外框 */
		SetPenColor(hdc,MapRGB(hdc,255,0,0));
//		InflateRect(&rc,-10,0);
    DrawRect(hdc,&rc);
    
		SetPenColor(hdc,MapRGB(hdc,250,100,100));
		InflateRect(&rc,-1,-1);

		DrawRect(hdc,&rc);
	}
	else
	{
		//rc =obj->rc;
		//rc.x =0;
		//rc.y =0;

		//ClrDisplay(hdc_ico,&rc,MapRGB(hdc_ico,0,50,60));
		if(style& LMS_ICONFRAME)
		{
			SetPenColor(hdc,MapRGB(hdc,255,255,255));
			InflateRect(&rc,-20,0);
      DrawRect(hdc,&rc);
      
			SetPenColor(hdc,MapRGB(hdc,255,255,255));
      InflateRect(&rc,-1,-1);
			DrawRect(hdc,&rc);
		}

	} 

  /* 若bmp非空，优先使用bmp作为图标 */
  bmp =obj_tbl[obj->id].bmp;
  icon =obj_tbl[obj->id].icon;
  
  if(bmp != NULL)
  {
    /* 显示APP对应的bmp图标 */
    BMP_GetInfo(&info,bmp);

    x = rc.x+(((int)rc.w-(int)info.Width)/2);
    y = rc.y+(((int)rc.h-(int)info.Height)/2);
    BMP_Draw(hdc,x,y,bmp,NULL);
  }
  else if(icon != NULL)
  {   
    /* 显示APP对应的字体图标 */  
    SetFont(hdc, iconFont); 
		
    rc0.w = rc.w;
    rc0.h = rc.h*2/3;
    rc0.x = rc.x;
    rc0.y = rc.y ;
    
    SetTextColor(hdc,MapRGB(hdc,255,255,255));

    DrawText(hdc,(LPCWSTR)icon,-1,&rc0,DT_VCENTER|DT_CENTER);
    SetFont(hdc, defaultFont); 
    
    //矩形外框，图标字体宽度为100*100，所以减去它们的宽度除以2
    InflateRect(&rc0,-(rc0.w-100)/2,-(rc0.h-100)/2);

    SetPenColor(hdc,MapRGB(hdc,255,255,255));
    DrawRect(hdc,&rc0);
    
    InflateRect(&rc0,-1,-1);
    DrawRect(hdc,&rc0);
  }
	/////
	SetTextColor(hdc,MapRGB(hdc,255,255,255));

	rc0.w =rc.w;
  rc0.h = rc.h*1/3;
	rc0.x =rc.x;
	rc0.y =rc.y+rc.h-rc0.h-6;
	DrawText(hdc,obj->pszText,-1,&rc0,DT_VCENTER|DT_CENTER);
#if 0
	if(1)
	{
		BLENDFUNCTION bf;
		RECT rc0;
		int x,size,a;

		rc =obj->rc;

		size =rc.w;

		if(rc.x < rc_main.x)
		{
			x =rc_main.x-rc.x;
			a =(255*x)/size;
			a =255-a;

			a =(a*12)>>3;


		}
		else if((rc.x+rc.w) > (rc_main.x+rc_main.w))
		{
			x =(rc_main.x+rc_main.w)-rc.x;
			a =(255*x)/size;

			a =(a*12)>>3;

		}
		else
		{
			a =255;
		}

		if(a!=255)
		{


			a =MIN(a,255);
			a =MAX(a,0);
			bf.AlphaFormat =0;
			bf.BlendFlags =AC_DST_NO_ALPHA|AC_SRC_NO_ALPHA;
			bf.SourceConstantAlpha =a;

			rc0 =rc;
			rc0.w =(rc0.w*a)>>8;
			rc0.h =(rc0.h*a)>>8;

			//InflateRect(&rc0,-((rc0.w*(255-a))>>8),-((rc0.h*(255-a))>>8));

			AlphaBlend(hdc,rc0.x,rc0.y,rc0.w,rc0.h,hdc_ico,0,0,obj->rc.w,obj->rc.h,bf);
		}
		else
		{
			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_ico,0,0,SRCCOPY);
		}

	}
#else
	//rc =obj->rc;
	//BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_ico,0,0,SRCCOPY);
#endif

	//DeleteDC(hdc_ico);
}


BOOL  CListMenu::MoveToPrevPage(void)
{
	struct __x_obj_item *obj=NULL;

	obj =x_obj_get_first(list_item);
  
	x_move_to = ((obj->rc.x+(obj->rc.w>>1))/obj->rc.w)*obj->rc.w;

	if(x_move_to > 0)
	{
		x_move_to =0;
	}
	//////////////

	if(page_cur > 0)
	{
		page_cur--;

		x_move = -(page_cur*rc_main.w);

		//x_move = MAX(x_move,-(rc_main.w*page_num));
		//x_move = MIN(x_move,4);

	}
  
   /* 到达最前一个图标时返回TRUE */
  if(obj->rc.x  >= -obj->rc.w)
    return TRUE;
  else
    return FALSE;  
}

BOOL  CListMenu::MoveToNextPage(void)
{
	struct __x_obj_item *obj=NULL;

	obj =x_obj_get_first(list_item);  
 
	x_move_to = ((obj->rc.x-(obj->rc.w))/obj->rc.w)*obj->rc.w;

	if(x_move_to < (-(page_num*rc_list.w)))
	{
		x_move_to =-(page_num*rc_list.w);
	}

	/////////////

	if(page_cur < (page_num-1))
	{
		page_cur++;
	}     
  
  /* 到达最后一个图标时返回TRUE */
  if(obj->rc.x  <= - (page_num*x_num*obj->rc.w-2*obj->rc.w) )
    return TRUE;
  else
    return FALSE;

}

void  CListMenu::SetToPrevPage(void)
{
	struct __x_obj_item *obj=NULL;
	int x,x_end;

	if(page_cur > 0)
	{
		page_cur--;

		obj =x_obj_get_first(list_item);
		x =obj->rc.x;
		x_end =-(page_cur*rc_list.w)+rc_list.x;

		if(x > x_end)
		{
			x =x-x_end;
			OffsetObjs(-x,0);
			InvalidateRect(hwndMain,NULL,FALSE);

		}
		else if(x < x_end)
		{
			x =x_end-x;
			OffsetObjs(x,0);
			InvalidateRect(hwndMain,NULL,FALSE);
		}

	}
}

void  CListMenu::SetToNextPage(void)
{
	struct __x_obj_item *obj=NULL;
	int x,x_end;

	if(page_cur < (page_num-1))
	{
		page_cur++;

		obj =x_obj_get_first(list_item);
		x =obj->rc.x;
		x_end =-(page_cur*rc_list.w)+rc_list.x;

		if(x > x_end)
		{
			x =x-x_end;
			OffsetObjs(-x,0);
			InvalidateRect(hwndMain,NULL,FALSE);

		}
		else if(x < x_end)
		{
			x =x_end-x;
			OffsetObjs(x,0);
			InvalidateRect(hwndMain,NULL,FALSE);
		}

	}
}

void  CListMenu::SetToPage(int page)
{
	struct __x_obj_item *obj=NULL;
	int x,x_end;

	if(page < 0)
	{
		return;
	}

	if(page >= page_num)
	{
		return;
	}

	if(page < page_num)
	{
		page_cur =page;

		obj =x_obj_get_first(list_item);
		x =obj->rc.y;
		x_end =-(page_cur*rc_list.w)+rc_list.x;

		if(x > x_end)
		{
			x =x-x_end;
			OffsetObjs(-x,0);
			InvalidateRect(hwndMain,NULL,FALSE);

		}
		else if(x < x_end)
		{
			x =x_end-x;
			OffsetObjs(x,0);
			InvalidateRect(hwndMain,NULL,FALSE);
		}

	}
}

/*============================================================================*/

extern const char bkgnd_bmp[];

LRESULT CListMenu::DrawFrame(HDC hdc,HWND hwnd)
{
//	int i,a,x,y,style;
  int style;
	WCHAR wbuf[128];
	RECT rc;
	struct __x_obj_item *obj;
	////////

	style=GetWindowLong(hwnd,GWL_STYLE);


	////backgroup
	//StretchBlt(hdc,0,0,rc_main.w,rc_main.h,hdc_bkgnd,0,0,bkgnd_w,bkgnd_h,SRCCOPY);
	//BitBlt(hdc,0,0,rc_main.w,rc_main.h,hdc_bkgnd,0,0,SRCCOPY);

	ClrDisplay(hdc,NULL,MapRGB(hdc,COLOR_DESKTOP_BACK_GROUND));
	//BMP_Draw(hdc,0,0,bkgnd_bmp,NULL);

#if 0
	////list_item
	SetFont(hdc,NULL);
	obj =x_obj_get_first(list_item);
	while(obj != NULL)
	{
		if(IsIntersectRect(&rc_list,&obj->rc))
		{	//只绘制在可视区内的对象.
			if(focus_list_obj == obj)
			{
				draw_icon_obj(hdc,obj,OBJ_ACTIVE,style);
			}
			else
			{
				draw_icon_obj(hdc,obj,0,style);
			}
		}

		obj = x_obj_get_next(obj);
	}
#else
	x_obj_draw(hdc, list_item);
#endif
#if 0
	////button_item
	//SetFont(hdc,hFont24);
	obj =x_obj_get_first(button_item);
	while(obj != NULL)
	{
		u32 text_flag;

		if(focus_btn_obj == obj)
		{

			draw_button_obj(hdc,obj,MapRGB888(hdc,cr_btn_text[1]),MapRGB888(hdc,cr_btn_border[1]),MapRGB888(hdc,cr_btn_bkgnd[1]),OBJ_ACTIVE);
		}
		else
		{
			draw_button_obj(hdc,obj,MapRGB888(hdc,cr_btn_text[0]),MapRGB888(hdc,cr_btn_border[0]),MapRGB888(hdc,cr_btn_bkgnd[0]),0);
		}


		obj = x_obj_get_next(obj);
	}
#endif

	///////////////////
#if 0
	rc.x =0;
	rc.y =0;
	rc.w =rc_main.w;
	rc.h =32;
	ClrDisplay(hdc_argb,&rc,ARGB4444(8,2,2,2));
	BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_argb,0,0,SRCCOPY);

	SetFont(hdc,hFont16);
	rc =rc_main;
	rc.h =32;
	//SetBrushColor(hdc,MapRGB(hdc,0,0,0));
	//FillRect(hdc,&rc);
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
//	x_wsprintf(wbuf,L"%s:(%d/%d),%d",wszName,page_cur+1,page_num,x_move);
	x_wsprintf(wbuf,L"Vbat:%dmV,CPU:%d%%",pwr.BatteryVoltage,mk_cpu_usage(0));
	//DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
#endif

#ifdef	 TOP_BAR
	#if(1)
	{
		SetTextColor(hdc,MapRGB(hdc,200,250,250));
		u32 mem_used,mem_total;

		rc =rc_top_bar;
		GetMemInfo(&mem_used,&mem_total);
		x_wsprintf(wbuf,L"CPU:占用率%d%%; MEM:已使用:%d;总数:%d",GetCPUUsage(),mem_used,mem_total);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
	#endif
#endif
	////draw top icons.
#if 0
	{

		//// battery icon:48x28
		rc.w =48;
		rc.h =28;
		rc.x =rc_main.w-rc.w-4;
		rc.y =rc_main.y+2;
		if(pwr.BatteryFlag&BATTERY_FLAG_CHARGING)
		{
			BMP_Draw(hdc,rc.x,rc.y,batt_charging_bmp,NULL);
		}
		else
		{
			if(pwr.BatteryLifePercent > 10)
			{
				draw_battery_life(hdc,rc.x,rc.y,pwr.BatteryLifePercent);
			}
			else
			{
				if(tmr_500ms_count&(1<<0))
				{
					draw_battery_life(hdc,rc.x,rc.y,pwr.BatteryLifePercent);
				}
			}
		}

		//// gps icon:32x32
		rc.w =32;
		rc.h =32;
		if(GPS_Hard_IsRdy())
		{
			if(Get_GPS_State() == 'A')
			{
				OffsetRect(&rc,-(rc.w+2),0);
				BMP_Draw(hdc,rc.x,rc.y,gps_logo_bmp,NULL);
			}
			else
			{
				if(tmr_500ms_count&(1<<0))
				{
					OffsetRect(&rc,-(rc.w+2),0);
					BMP_Draw(hdc,rc.x,rc.y,gps_logo_bmp,NULL);
				}

			}
		}

		////

	}
#endif

#if 1
	//// draw title
	//SetFont(hdc,hFont24);
	if(style&LMS_ICONTITLE)
	{
		if(focus_list_obj!=NULL)
		{

			//rc =rc_title;
			//rc.x =0;
			//rc.y =0;
			//ClrDisplay(hdc_argb,&rc,ARGB4444(8,0,0,0));
			//SetPenColor(hdc_argb,ARGB4444(13,0,0,0));
			//DrawRect(hdc_argb,&rc);

			rc =rc_title;
			//BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_argb,0,0,SRCCOPY);
			SetTextColor(hdc,MapRGB(hdc,10,250,10));
			SetBrushColor(hdc,MapRGB(hdc,0,30,30));
			DrawText(hdc,focus_list_obj->pszText,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND);

			SetTextColor(hdc,MapRGB(hdc,200,250,250));
			x_wsprintf(wbuf,L" %d/%d",page_cur+1,page_num);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
	}

#endif

#if 1
	/*
	//// draw page label
	//rc.x =0;
	//rc.y =0;
	//rc.w =rc_page.w;
	//rc.h =rc_page.h;
	//ClrDisplay(hdc_argb,&rc,ARGB4444(8,0,0,0));

	rc =rc_page;
	//BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_argb,0,0,SRCCOPY);
	SetTextColor(hdc,MapRGB(hdc,250,250,250));
	SetBrushColor(hdc,MapRGB(hdc,10,45,55));
	x_wsprintf(wbuf,L"%d/%d",page_cur+1,page_num);
	DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_BKGND);
	*/
	if(1)
	{
//		int x,y,i;
    int i;

		RECT m_rc[2],rc;


		obj =x_obj_get_first(list_item);

		if(obj->rc.x != x_move_to)
		{
			i=MIN(0-obj->rc.x,page_num*rc_list.w);
			
			rc.w =150;
			rc.h =20;
			rc.x =(rc_main.w-rc.w)>>1;
			rc.y =rc_main.h-rc.h-15;
			MakeProgressRect(m_rc,&rc,page_num*rc_list.w,i,PB_ORG_LEFT);

			SetPenColor(hdc,MapRGB(hdc,250,220,220));
			SetBrushColor(hdc,MapRGB(hdc,250,250,250));
      //太小的矩形不画
      if(m_rc[0].w > 10)
      {      
        FillRoundRect(hdc,&m_rc[0],10);
      }
      DrawRoundRect(hdc,&rc,10);
		}
	/*

		x =(rc_main.w-(page_num*24))>>1;
		y =rc_main.h-28;
		SetPenColor(hdc,MapRGB(hdc,250,220,220));
		SetBrushColor(hdc,MapRGB(hdc,250,250,250));
		
		for(i=0;i<page_num;i++)
		{
			rc.x =x+i*24;
			rc.y =y;
			rc.w =20;
			rc.h =20;
			
			if(i== page_cur)
			{
				FillRect(hdc,&rc);
			}
			else
			{
				DrawRect(hdc,&rc);
			}	
		}
		*/
	}
#endif

#if 0
	//// draw bottom bar
	{
		rc =rc_bottom_bar;

		//rc.x =0;
		//rc.y =0;

		//ClrDisplay(hdc,&rc,ARGB4444(8,0,0,0));

		if(tmr_500ms_count > (5000/500))
		{
			SetFont(hdc,hFontSEG_32);
			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			SetBrushColor(hdc,MapRGB(hdc,30,40,50));
			x_wsprintf(wbuf,L"%04d-%d-%d    %02d:%02d:%02d",tm.year,tm.mon,tm.day,tm.hour,tm.min,tm.sec);
			DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);
		}
		else if(tmr_500ms_count > (3000/500))
		{
#if 0
			SetFont(hdc_argb,NULL);
			SetTextColor(hdc_argb,ARGB4444(15,15,15,15));
			x_wsprintf(wbuf,L"SN:%08X-%08X; SW Ver:%s",SN[0],SN[1],__VERSION);
			DrawText(hdc_argb,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
			DrawText(hdc_argb,wbuf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
#endif
		}
		else
		{
			SetFont(hdc,NULL);
			SetTextColor(hdc,MapRGB(hdc,10,250,10));
			SetBrushColor(hdc,MapRGB(hdc,20,50,20));
			DrawText(hdc,wszName,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_BKGND);
		}

		//rc =rc_bottom_bar;
		//BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_argb,0,0,SRCCOPY);


	}
#endif

	////end

	return TRUE;
}


/*============================================================================*/

void CListMenu::OffsetObjs(int dx,int dy)
{
	struct __x_obj_item *obj;

	////list_item
	obj =x_obj_get_first(list_item);
	while(obj != NULL)
	{

		OffsetRect(&obj->rc,dx,dy);

		obj = x_obj_get_next(obj);
	}

}


/*============================================================================*/
static void ListItemDraw(HDC hdc, struct __x_obj_item * obj)
{
	HWND hwnd;
	CListMenu *pApp;
	int style;

	////list_item
	SetFont(hdc, NULL);
	
	/* 类的回调函数扩展参数，hwnd句柄 */
	hwnd = *(HWND *)x_obj_get_extra_ptr(obj->parent);
	pApp = (CListMenu*)GetWindowLong(hwnd, GWL_USERDATA);

	style = GetWindowLong(hwnd, GWL_STYLE);	

	if (IsIntersectRect(&pApp->rc_list, &obj->rc))
	{	//只绘制在可视区内的对象.
		if (pApp->focus_list_obj == obj)
		{
			pApp->draw_icon_obj(hdc, obj, OBJ_ACTIVE, style);
		}
		else
		{
			pApp->draw_icon_obj(hdc, obj, 0, style);
		}
	}
}

LRESULT CListMenu::OnCreate(HWND hwnd,list_menu_cfg_t *cfg)
{
	int i,j,page,style;
	RECT rc,*m_rc;
	__x_obj_item *obj;
	HWND *pextra;


	hwndMain =hwnd;
//	hFont16 =NULL;
//	hFont24 =NULL;
	//focus_btn_obj =NULL;
	focus_list_obj =NULL;
	execu_obj =FALSE;
//	tmr_500ms_count =0;
//	btn_down_count =0;

//	SYS_get_local_time(&tm);
//	SYS_kernel_ioctl(KERNEL_IOCTL_GET_SYSTEM_POWER_STATE,&pwr,sizeof(pwr));

	style =GetWindowLong(hwnd,GWL_STYLE);
	GetClientRect(hwnd,&rc_main);


	x_move_to =0;
	y_move_to =0;

	rc =rc_main;
	x_num =cfg->x_num;
	y_num =cfg->y_num;
	obj_tbl =cfg->list_objs;

	m_rc =new RECT[x_num*y_num];


	if(style&LMS_ICONTITLE)
	{
		rc.x =0;
		rc.y =0;
		rc.w =rc_main.w;
		rc.h =32;
	}
	else
	{
		rc.x=0;
		rc.y=0;
		rc.w=rc_main.w;
		rc.h=0;
	}
	rc_title =rc;

	OffsetRect(&rc,0,rc.h);
	rc.h =rc_main.h-rc.y-30;
	InflateRectEx(&rc,0,0,0,-28);
	rc_list =rc;


	//list_item =x_obj_create(L"list_item", 0xFFFFFFFF, &rc_list,X_OBJ_VISIBLE,0,NULL);
	list_item =x_obj_create_class(L"list_item", 0xFFFFFFFF, &rc_list,X_OBJ_VISIBLE, sizeof(HWND), ListItemDraw);
	pextra = (HWND *)x_obj_get_extra_ptr(list_item);
	*pextra = hwnd;

	rc =rc_list;

	MakeMatrixRect(m_rc,&rc_list,0,0,x_num,y_num);
	if(is_ver_list(hwnd))
	{
		for(i=0;obj_tbl[i].pName!=NULL;i++)
		{
			const WCHAR *pText;
			page =i/(x_num*y_num);
			j    =i%(x_num*y_num);

			rc = m_rc[j];
			rc.y += page*rc_list.h;
			pText =obj_tbl[i].pName;

		//	InflateRect(&rc,-6,-8);
			obj = x_obj_create(pText,	i,	&rc,	X_OBJ_VISIBLE, 0,list_item);
			//pextra = (HWND *)x_obj_get_extra_ptr(obj);
			//*pextra = hwnd;
		}
	}
	else
	{
		for(i=0;obj_tbl[i].pName!=NULL;i++)
		{
			const WCHAR *pText;
			page =i/(x_num*y_num);
			j    =i%(x_num*y_num);

			rc = m_rc[j];
			rc.x += page*rc_list.w;
			pText =obj_tbl[i].pName;

		//	InflateRect(&rc,-6,-8);
			obj = x_obj_create(pText,	i,	&rc,	X_OBJ_VISIBLE, 0,list_item);
			//pextra = (HWND *)x_obj_get_extra_ptr(obj);
			//*pextra = hwnd;
		}
	}

	x_move =m_rc[0].x;
	page_num =page+1;
	page_cur =0;
	focus_list_obj =x_obj_get_from_id(list_item,0);
	////



//	hFontSEG_32 =XFT_CreateFont(SEG_NUM_32);


	SetTimer(hwnd,ID_TMR_100,20,TMR_START,NULL);
//	SetTimer(hwnd,ID_TMR_500,500,TMR_START,NULL);
	delete m_rc;

	return TRUE;
}

LRESULT CListMenu::OnDestroy(HWND hwnd)
{
//	int i;

	x_obj_del(list_item);
	//x_obj_del(button_item);
	//DeleteDC(hdc_mem);
	//DeleteDC(hdc_argb);
	//DeleteDC(hdc_ico);

	DeleteFont(hFontSEG_32);



	PostQuitMessage(hwnd);
	return TRUE;
}

LRESULT	CListMenu::OnPaint(HWND hwnd)
{
	HDC hdc,hdc_mem;
	PAINTSTRUCT ps;
	RECT rc;

	GetClientRect(hwnd,&rc);
	hdc_mem =CreateMemoryDC(SURF_SCREEN,rc.w,rc.h);
	DrawFrame(hdc_mem,hwnd);


	hdc =BeginPaint(hwnd,&ps);
	BitBlt(hdc,0,0,rc_main.w,rc_main.h,hdc_mem,0,0,SRCCOPY);
	EndPaint(hwnd,&ps);

	DeleteDC(hdc_mem);
	return TRUE;
}

LRESULT CListMenu::OnNotify(HWND hwnd,u16 code,u16 id)
{
//	int i;
	struct __x_obj_item *obj;

	if(code!=BN_CLICKED)
	{
		return FALSE;
	}

	switch(id)
	{
	case ID_FOCUS_PREV:
		obj =x_obj_get_prev(focus_list_obj); //find prev obj.
		if(obj != NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }			
      }
		}
		else
		{
			focus_list_obj =x_obj_get_prev(list_item);
			SetToPage(page_num-1);
		}
		break;
		////

	case ID_FOCUS_NEXT:
		obj =x_obj_get_next(focus_list_obj); //find next obj.
		if(obj!=NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }
			}
		}
		else
		{
			focus_list_obj =x_obj_get_next(list_item);
			SetToPage(0);
		}
		break;
		////

	case ID_FOCUS_LEFT:
		obj =x_obj_get_left(focus_list_obj);
		if(obj != NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }
			}
		}
		else
		{
			focus_list_obj =x_obj_get_prev(list_item);
			SetToPage(page_num-1);
		}
		break;
		////

//		break;
		////

	case ID_FOCUS_RIGHT:
		obj =x_obj_get_right(focus_list_obj);
		if(obj!=NULL)
		{
			focus_list_obj =obj;
			if(obj->rc.x < rc_list.x)
			{
        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
			}

			if(obj->rc.x > (rc_list.x+rc_list.w-1))
			{
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }
			}
		}
		else
		{
			focus_list_obj =x_obj_get_next(list_item);
			SetToPage(0);
		}
		break;
	}

	return TRUE;
}

LRESULT	CListMenu::OnKeyUp(HWND hwnd,int key_val)
{
	if(key_val == VK_LEFT)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_LEFT|(BN_CLICKED<<16),0);
	}

	if(key_val == VK_RIGHT)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_RIGHT|(BN_CLICKED<<16),0);

	}

	if(key_val == VK_UP)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_UP|(BN_CLICKED<<16),0);
	}

	if(key_val == VK_DOWN)
	{
		PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_DOWN|(BN_CLICKED<<16),0);
	}
/*
	if(key_val == VK_RETURN)
	{
		int id;

		id = focus_list_obj->id;
		if(id >=0 & id<= 256)
		{
			if(execu_obj!=FALSE)
			{
				obj_tbl[id].cbStartup(hwnd);
			}

		}
	}
*/
	return TRUE;
}

	

LRESULT	CListMenu::OnKeyDown(HWND hwnd,int key_val)
{
/*
	////
	if(key_val == VK_NUMPAD0)
	{
		PostMessage(hwnd,WM_COMMAND,ID_FOCUS_PREV,0);
		InvalidateRect(hwnd,NULL,FALSE);
	}

	if(key_val == VK_NUMPAD1)
	{
		PostMessage(hwnd,WM_COMMAND,ID_FOCUS_NEXT,0);
		InvalidateRect(hwnd,NULL,FALSE);
	}

	if(key_val == VK_NUMPAD2)
	{
		MoveToPrevPage();
		InvalidateRect(hwnd,NULL,FALSE);
	}

	if(key_val == VK_NUMPAD3)
	{
		MoveToNextPage();
		InvalidateRect(hwnd,NULL,FALSE);
	}
*/
	return TRUE;
}


LRESULT	CListMenu::OnLButtonDown(HWND hwnd,int x,int y)
{
//	int id;
	struct __x_obj_item *obj=NULL;
//	RECT rc;

	execu_obj =TRUE;

	x_moved =x;
	y_moved =y;

	lbtn_down_x =x;
	lbtn_down_y =y;

	obj =x_obj_get_from_pos(list_item,x,y);
	if(obj!=NULL)
	{
		focus_list_obj =obj;
	}
#if 0
	obj =x_obj_get_from_pos(button_item,x,y);
	{
		focus_btn_obj =obj;
	}

	if(focus_btn_obj!=NULL)
	{
		btn_down_count =0;
		id =focus_btn_obj->id;

		switch(id)
		{
			case ID_PREV_PAGE:
			{
        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
			}
			break;

			case ID_NEXT_PAGE:
			{
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }
			}
			break;

			case ID_OK:
			case ID_EXIT:
			{
				break;
			}

			default:
				break;
		}
	}
#endif
	InvalidateRect(hwnd,NULL,FALSE);
	return TRUE;
}

LRESULT	CListMenu::OnLButtonUp(HWND hwnd,int x,int y)
{
	int id;
	POINT pt;
	struct __x_obj_item *obj=NULL;
	////

	pt.x =x;
	pt.y =y;

	if(PtInRect(&rc_list,&pt))
	{
		if(is_ver_list(hwnd))
		{
			if((y-lbtn_down_y) < -50)
			{
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }
			}

			if((y-lbtn_down_y) > 50)
			{

        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
			}

			if(abs(y-lbtn_down_y) > 30)
			{
				execu_obj =FALSE; //拖动时，不执行obj，防止误操作
			}
		}
		else
		{
			//obj =x_obj_get_first(list_item);
			if((x-lbtn_down_x) < -50)
			{ //GoLEFT
                
        BOOL is_end;
				is_end = MoveToNextPage();
        
        /* 不管pre按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

        if(is_end)
        {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
        }
				//x_move_to -= 40;
				/*
				x_move_to = ((obj->rc.x-obj->rc.w)/obj->rc.w)*obj->rc.w;

				if(x_move_to < (-(page_num*rc_list.w)))
				{
					x_move_to =-(page_num*rc_list.w);
				}*/

			}

			if((x-lbtn_down_x) > 50)
			{//GoRIGHT

        BOOL is_head;
				is_head = MoveToPrevPage();
        
        /* 不管next按钮状态如何，使能之 */
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

        if(is_head)
        {
          EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
        }
				/*
				//x_move_to += 40;
				x_move_to = ((obj->rc.x+obj->rc.w)/obj->rc.w)*obj->rc.w;

				if(x_move_to > 0)
				{
					x_move_to =0;
				}*/

			}

			if(abs(x-lbtn_down_x) > 30)
			{
				execu_obj =FALSE; //拖动时，不执行obj，防止误操作
			}

		}
	}

	////////
	obj=NULL;
	//if(obj==NULL) obj =x_obj_get_from_pos(button_item,x,y);
	if(obj==NULL) obj =x_obj_get_from_pos(list_item,x,y);

	//btn_down_count =0;

	if(obj != NULL)
	{
		id =obj->id;

		if(id==ID_EXIT)
		{
			SendMessage(hwnd,WM_CLOSE,0,0);
		}

		if(id==ID_OK)
		{
			SendMessage(hwnd,WM_CLOSE,0,0);
		}

		if(id == ID_PREV_OBJ)
		{
			PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_PREV|(BN_CLICKED<<16),0);
		}
		if(id == ID_NEXT_OBJ)
		{
			PostMessage(hwnd,WM_NOTIFY,ID_FOCUS_NEXT|(BN_CLICKED<<16),0);
		}

		if(id >=0 & id<= 256)
		{
			if(execu_obj!=FALSE)
			if(id == focus_list_obj->id)
			{
				LM_NMHDR lmn;
				lmn.idx =id;

				NotifyParentEx(hwnd,LMN_CLICKED,(NMHDR*)&lmn);

			}
		}

	}
	/////
	execu_obj =TRUE;

//	focus_btn_obj =NULL;
	InvalidateRect(hwnd,NULL,FALSE);
	return 0;
}

LRESULT	CListMenu::OnMouseMove(HWND hwnd,int x,int y)
{
	int x_off,y_off;
	POINT pt;

	pt.x =x;
	pt.y =y;

	if(GetKeyState(VK_LBUTTON))
	if(PtInRect(&rc_list,&pt))
	{
		struct __x_obj_item *obj=NULL;
		x_off = x-x_moved;
		y_off = y-y_moved;

		if(is_ver_list(hwnd))
		{
			OffsetObjs(0,y_off);
		}
		else
		{
			//限制X方向移动范围.
			if(x_off > 0)
			{
				obj =x_obj_get_first(list_item);
				if(obj->rc.x > obj->rc.w)
				{
					x_off =0;
				}

			}
			else
			{
				obj =x_obj_get_last(list_item);
				if(obj->rc.x < 0)
				{
					x_off =0;
				}
			}
			OffsetObjs(x_off,0);
		}

		InvalidateRect(hwnd,NULL,FALSE);

		x_moved =x;
		y_moved =y;
	}
	return TRUE;

}

LRESULT	CListMenu::OnTimer(HWND hwnd,int tmr_id)
{
	BOOL need_draw=FALSE;

	if(tmr_id==ID_TMR_100)
	{
		POINT pt;
		RECT rc;

		GetCursorPos(&pt);
		GetWindowRect(hwnd,&rc);
		if((GetKeyState(VK_LBUTTON)==0) || (PtInRect(&rc,&pt)==FALSE))
		{
			struct __x_obj_item *obj=NULL;

			obj =x_obj_get_first(list_item);
			if(obj!=NULL)
			{
//				int x,y,x_end,y_end;
        int x,y,y_end;

				////

				if(is_ver_list(hwnd))
				{
					y =obj->rc.y;
					y_end =-(page_cur*rc_list.h)+rc_list.y+2;

					if(y > y_end)
					{
						y =MIN(rc_list.h>>3,y-y_end);
						OffsetObjs(0,-y);
						need_draw =TRUE;
					}
					else if(y < y_end)
					{
						y =MIN(rc_list.h>>3,y_end-y);
						OffsetObjs(0,y);
						need_draw =TRUE;
					}

				}
				else
				{

					x =obj->rc.x;
					if(x > x_move_to)
					{

						//x =MIN(rc_list.w>>3,x-x_move_to);
						if((x-x_move_to) > 20)
						{
							x =MIN(obj->rc.w>>4,x-x_move_to);
						}
						else
						{
							x =MIN(1,x-x_move_to);
						}

						OffsetObjs(-x,0);
						need_draw =TRUE;
					}
					else if(x < x_move_to)
					{
						//x =MIN(rc_list.w>>3,x_move_to-x);
						if((x_move_to-x) > 20)
						{
							x =MIN(obj->rc.w>>4,x_move_to-x);
						}
						else
						{
							x =MIN(1,x_move_to-x);
						}
						OffsetObjs(x,0);
						need_draw =TRUE;
					}


				}


			}
		}
	}


	if(need_draw!=FALSE)
	{
		InvalidateRect(hwnd,NULL,FALSE);
	}

	return TRUE;
}

///////////////////////////////////////////////////////

static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	CListMenu *pApp =(CListMenu*)GetWindowLong(hwnd,GWL_USERDATA);

	switch(msg)
	{
		case WM_CREATE:
		{
//			RECT rc;
			list_menu_cfg_t *cfg =(list_menu_cfg_t*)lParam;

			pApp =new CListMenu;
			SetWindowLong(hwnd,GWL_USERDATA,(LONG)pApp);

			return pApp->OnCreate(hwnd,cfg);
		}
//		break;

		////

		case	MSG_MOVE_PREV:
    {
      BOOL is_head;
      
			is_head = pApp->MoveToPrevPage();
      
      /* 不管next按钮状态如何，使能之 */
      EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),TRUE); 

      /* 到达开头，禁止按钮 */
      if(is_head)
      {
        EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),FALSE); 
      }
      
			if(wParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}   
    }      

		break;
		////

		case	MSG_MOVE_NEXT:			
    {
      BOOL is_end;

      is_end = pApp->MoveToNextPage();
      
      /* 不管pre按钮状态如何，使能之 */
      EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_PREV),TRUE); 

      /* 到达末尾，禁止按钮 */
      if(is_end)
      {
         EnableWindow(GetDlgItem(GetParent(hwnd),ICON_VIEWER_ID_NEXT),FALSE); 
       }
      
			if(wParam)
			{
				InvalidateRect(hwnd,NULL,FALSE);
			}         
    } 
		break;
		////

		case    WM_NOTIFY:
		{
			return pApp->OnNotify(hwnd,HIWORD(wParam),HIWORD(wParam));
		}
//		return DefWindowProc(hwnd,msg,wParam,lParam);
		////
		case    WM_KEYUP:
		{
			int key;

			key=LOWORD(wParam);
			return pApp->OnKeyUp(hwnd,key);
		}
//		return DefWindowProc(hwnd,msg,wParam,lParam);
		////
		case    WM_KEYDOWN:
		{
			int key;

			key=LOWORD(wParam);
			return pApp->OnKeyDown(hwnd,key);
		}
//		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case    WM_LBUTTONDOWN:
		{
			int x,y;

			x=LOWORD(lParam);
			y=HIWORD(lParam);
			return pApp->OnLButtonDown(hwnd,x,y);
		}
//		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

//		case WM_MOUSELEAVE:
		case WM_LBUTTONUP:
		{
			int x,y;

			x=LOWORD(lParam);
			y=HIWORD(lParam);
			pApp->OnLButtonUp(hwnd,x,y);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case WM_MOUSEMOVE:
		{
			int x,y;

			x=LOWORD(lParam);
			y=HIWORD(lParam);
			pApp->OnMouseMove(hwnd,x,y);
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
		////

		case WM_TIMER:
		{
			int tmr_id;

			tmr_id =wParam;
			return pApp->OnTimer(hwnd,tmr_id);
		}
//		break;
		/////



		case WM_ERASEBKGND:
		{
			return TRUE;
		}
		////

		case WM_PAINT:
			return pApp->OnPaint(hwnd);
			//////

		case WM_DESTROY:
			{
				pApp->OnDestroy(hwnd);
				delete pApp;

			}
			return DefWindowProc(hwnd,msg,wParam,lParam);

		default:			return DefWindowProc(hwnd,msg,wParam,lParam);

	}

	return WM_NULL;
}
/*============================================================================*/

const WNDCLASS wcex_ListMenu={

		WNDCLASS_TAG,
		0,
		WinProc,
		0,		/* cbClsExtra */
		0,		/* cbWndExtra */
		NULL,	/* hInstance */
		NULL,	/* hIcon */
		NULL,	/* hCursor */

};

/*=========================================================================*/

/*=========================================================================*/

