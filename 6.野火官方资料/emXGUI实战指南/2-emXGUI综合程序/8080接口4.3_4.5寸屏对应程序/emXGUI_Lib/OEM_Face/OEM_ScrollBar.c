
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
void	*hBmpArrowUp(void);     //���ϼ�ͷ����BMP���ݡ�
void	*hBmpArrowDown(void);   //���¼�ͷ����BMP���ݡ�
void	*hBmpArrowLeft(void);   //�����ͷ����BMP���ݡ�
void	*hBmpArrowRight(void);  //���Ҽ�ͷ����BMP���ݡ�

/*=========================================================================================*/

#define	SCROLLBAR_TEXTCOLOR		XRGB8888(0,0,0)
#define	SCROLLBAR_BACKCOLOR		XRGB8888(200,205,220)
#define	SCROLLBAR_BODERCOLOR	XRGB8888(120,130,140)
#define	SCROLLBAR_FORECOLOR		XRGB8888(100,150,240)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{

	cr->TextColor =SCROLLBAR_TEXTCOLOR;
	cr->BackColor =SCROLLBAR_BACKCOLOR;
	cr->BorderColor =SCROLLBAR_BODERCOLOR;
	cr->ForeColor  =SCROLLBAR_FORECOLOR;


	if(style&WS_DISABLED)
	{
		cr->TextColor =RGB32_Offset(cr->TextColor,-10,-10,-10);
		cr->BackColor =RGB32_Offset(cr->BackColor,-10,-10,-10);
		cr->BorderColor =RGB32_Offset(cr->BorderColor,-10,-10,-10);
		cr->ForeColor =RGB32_Offset(cr->ForeColor,-10,-10,-10);

		cr->TextColor =RGB32_AVG(cr->TextColor,RGB888(120,120,128));
		cr->BackColor =RGB32_AVG(cr->BackColor,RGB888(120,120,128));
		cr->BorderColor =RGB32_AVG(cr->BorderColor,RGB888(120,120,128));
		cr->ForeColor =RGB32_AVG(cr->ForeColor,RGB888(120,120,128));

	}

}

static void draw_btn(HDC hdc,CONST RECT *prc,BOOL pushed,CTLCOLOR *cr)
{//���Ƽ�ͷ(��ť)����.

	RECT rc =*prc;
	COLOR_RGB32 c;

	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&rc);

	if(pushed)
	{

		c =RGB32_AVG(cr->BackColor,cr->ForeColor);
		SetBrushColor(hdc,MapRGB888(hdc,c));
		FillRoundRect(hdc,&rc,4);

		SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
		DrawRoundRect(hdc,&rc,4);
	}
	else
	{
		SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
		FillRoundRect(hdc,&rc,3);

		SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
		DrawRoundRect(hdc,&rc,3);
	}

}

static void draw_page(HDC hdc,const RECT *prc,BOOL act,CTLCOLOR *cr)
{//���ƻ�������.

	COLOR_RGB32 c;

	////DrawPage
	if(act)
	{
		c =RGB32_AVG(cr->BackColor,cr->ForeColor);
		SetBrushColor(hdc,MapRGB888(hdc,c));
		FillRect(hdc,prc);
	}
	else
	{
		c =cr->BackColor;
		SetBrushColor(hdc,MapRGB888(hdc,c));
		FillRect(hdc,prc);
	}

}

static void draw_track(HDC hdc,const RECT *prc,BOOL act,BOOL is_vert,CTLCOLOR *cr)
{//���ƻ�������.

	RECT rc;
	COLOR_RGB32 c;

	rc =*prc;

	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&rc);


	if(act)
	{
		c =cr->ForeColor;
	}
	else
	{
		c =RGB32_Offset(cr->BorderColor,-20,-20,-20);

	}
	//SetBrushColor(hdc,MapRGB888(hdc,c));
	//FillRoundRect(hdc,&rc,MIN(rc.w>>1,rc.h>>1));


	EnableAntiAlias(hdc,TRUE);
	InflateRect(&rc,-1,-1);
	//c=RGB32_AVG(c,RGB888(10,20,30));
	SetPenColor(hdc,MapRGB888(hdc,c));
	DrawRoundRect(hdc,&rc,MIN(rc.w>>1,rc.h>>1));
	EnableAntiAlias(hdc,FALSE);


	if(act)
	{
		c =RGB32_AVG(cr->BorderColor,cr->ForeColor);
	}
	else
	{
		c =cr->BorderColor;
	}
	SetPenColor(hdc,MapRGB888(hdc,c));

	if(is_vert)
	{
		int x,y,end;

		if(rc.w >= 8 && rc.h >= 8)
		{
			x =rc.x+3;
			y =rc.y+(rc.h>>1);
			end =x+(rc.w-3*2)-1;

			HLine(hdc,x,y-3,end);
			HLine(hdc,x,y-0,end);
			HLine(hdc,x,y+3,end);
		}
	}
	else
	{
		int x,y,end;

		if(rc.w >= 8 && rc.h >= 8)
		{
			x =rc.x+(rc.w>>1);
			y =rc.y+3;
			end =y+(rc.h-3*2)-1;

			VLine(hdc,x-3,y,end);
			VLine(hdc,x+0,y,end);
			VLine(hdc,x+3,y,end);
		}

	}

}


////

static	void	vscroll_paint(DRAWITEM_HDR *di,CTLCOLOR *cr)
{//��ֱ���ScrollBar�Ļ���.

	RECT rc;
	SCROLLBAR_RECT rcSL;

	HWND hwnd =di->hwnd;
	HDC hdc =di->hDC;
	u32 State =di->State;


	//��ȡScrollBar����ؾ��β���.
	SendMessage(hwnd,SBM_GETRECT,0,(LPARAM)&rcSL);

	//��״̬�����ϲ������ˡ�.
	if(State&SST_PAGEUP)
	{//�ò����Ǳ����������״��.
		draw_page(hdc,&rcSL.V.TopPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.V.TopPage,FALSE,cr);
	}

	//��״̬�����²������ˡ�.
	if(State&SST_PAGEDOWN)
	{//�ò����Ǳ����������״��.
		draw_page(hdc,&rcSL.V.BottomPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.V.BottomPage,FALSE,cr);
	}

	//�ж��Ƿ��С���ͷ�����,������Ҫ����.
	if(!(di->Style&SBS_NOARROWS))
	{
		int x,y;
		BITMAPINFO info;

		//�����ϡ���ͷ������.
		rc =rcSL.V.TopArrow;
		if(State&SST_LINEUP)
		{//�ò����Ǳ����������״��.
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		//�����ϡ���ͷ��ǰ��ͼ��(�û�����������λͼ��ֱ�ӻ���ʵ��...)
		BMP_GetInfo(&info,hBmpArrowUp());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowUp(),NULL);
		////

		//�����¡���ͷ������.
		rc =rcSL.V.BottomArrow;
		if(State&SST_LINEDOWN)
		{//�ò����Ǳ����������״��.
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		//�����¡���ͷ��ǰ��ͼ��(�û�����������λͼ��ֱ�ӻ���ʵ��...)
		BMP_GetInfo(&info,hBmpArrowDown());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowDown(),NULL);
	}

	//���ƻ���.
	if(State&SST_THUMBTRACK)
	{//���ƻ���״̬�Ļ���.
		draw_track(hdc,&rcSL.V.Track,TRUE,TRUE,cr);
	}
	else
	{//���Ʒǻ���״̬�Ļ���.
		draw_track(hdc,&rcSL.V.Track,FALSE,TRUE,cr);
	}

}


static	void	hscroll_paint(DRAWITEM_HDR *di,CTLCOLOR *cr)
{//ˮƽ���ScrollBar�Ļ���.

	RECT rc;
	SCROLLBAR_RECT rcSL;

	HWND hwnd =di->hwnd;
	HDC hdc =di->hDC;
	u32 State =di->State;

	SendMessage(hwnd,SBM_GETRECT,0,(LPARAM)&rcSL);

	////DrawPage
	if(State&SST_PAGELEFT)
	{
		draw_page(hdc,&rcSL.H.LeftPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.H.LeftPage,FALSE,cr);
	}

	////
	if(State&SST_PAGERIGHT)
	{
		draw_page(hdc,&rcSL.H.RightPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.H.RightPage,FALSE,cr);
	}

	////DrawArrow
	if(!(di->Style&SBS_NOARROWS))
	{
		int x,y;
		BITMAPINFO info;

		rc =rcSL.H.LeftArrow;
		if(State&SST_LINELEFT)
		{
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		BMP_GetInfo(&info,hBmpArrowLeft());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowLeft(),NULL);
		////

		rc =rcSL.H.RightArrow;
		if(State&SST_LINERIGHT)
		{
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		BMP_GetInfo(&info,hBmpArrowRight());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowRight(),NULL);
	}

	////
	if(State&SST_THUMBTRACK)
	{
		draw_track(hdc,&rcSL.H.Track,TRUE,FALSE,cr);
	}
	else
	{
		draw_track(hdc,&rcSL.H.Track,FALSE,FALSE,cr);
	}

}


static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{
	if(di->Style & SBS_VERT)
	{//��ֱ����.
		vscroll_paint(di,cr);
	}
	else
	{//ˮƽ����.
		hscroll_paint(di,cr);
	}
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetScrollBarOEM(void)
{
	return &oem;
}
