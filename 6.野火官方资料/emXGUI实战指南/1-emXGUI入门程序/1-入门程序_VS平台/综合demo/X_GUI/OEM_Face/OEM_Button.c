
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
////���ļ�ʵ��"��ť"�ؼ�����ۻ���.


#define	PUSHBTN_TEXTCOLOR_0		XRGB8888(0,0,0)
#define	PUSHBTN_TEXTCOLOR_1		XRGB8888(0,20,30)

#define	PUSHBTN_BACKCOLOR_0		XRGB8888(230,230,235)
#define	PUSHBTN_BACKCOLOR_1		XRGB8888(150,175,210)

#define	PUSHBTN_BODERCOLOR_0	XRGB8888(20,30,50)
#define	PUSHBTN_BODERCOLOR_1	XRGB8888(10,80,120)


static void __draw_backgnd(HDC hdc,RECT *prc,CTLCOLOR *cr,u32 state)
{
	RECT rc;
	COLOR_RGB32 c1,c2,c;

	rc = *prc;

	if(state&BST_PUSHED)
	{
		c1 =RGB32_AVG(cr->BorderColor,cr->BackColor);
		c1 =RGB32_Offset(c1,-15,-15,-15);
		c2 =cr->BackColor;
		SetPenColor(hdc,MapRGB888(hdc,c1));
		DrawRect(hdc,&rc);

		InflateRect(&rc,-1,-1);
		SetBrushColor(hdc,MapRGB888(hdc,c2));
		FillRect(hdc,&rc);

	}
	else
	{
		c =cr->BackColor;

		c1 =RGB32_AVG(cr->BorderColor,cr->BackColor);
		c1 =RGB32_Offset(c1,-5,-5,-5);
		SetPenColor(hdc,MapRGB888(hdc,c1));
		DrawRect(hdc,&rc);

		InflateRect(&rc,-1,-1);
		c1 =RGB32_Offset(cr->BackColor,+55,+55,+55);
		SetPenColor(hdc,MapRGB888(hdc,c1));
		DrawRect(hdc,&rc);

		InflateRect(&rc,-1,-1);
		rc.h = (rc.h*7)>>4;
		c1 =RGB32_Offset(c,+15,+15,+15);
		c2 =RGB32_Offset(c,-3,-3,-3);
		GradientFillRect(hdc,&rc,MapRGB888(hdc,c1),MapRGB888(hdc,c2),TRUE);

		OffsetRect(&rc,0,rc.h);
		rc.h =prc->y+prc->h-rc.y-2;

		c1 =RGB32_Offset(c,-7,-7,-7);
		c2 =RGB32_Offset(c,-12,-12,-12);
		SetBrushColor(hdc,MapRGB888(hdc,c1));
		FillRect(hdc,&rc);

	}

}

static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{ //Button�ؼ�����ɫ����(ע��CTLCOLOR����Ա���ñ�׼RGB32��ʽ,��RGB888��ARGB8888).

	if(state&BST_PUSHED)
	{//�ǰ���״̬.
		cr->TextColor =PUSHBTN_TEXTCOLOR_1;     //������ɫ.
		cr->BackColor =PUSHBTN_BACKCOLOR_1;     //������ɫ.
		cr->BorderColor =PUSHBTN_BODERCOLOR_1;  //������ɫ.
	}
	else
	{//�ǵ���״̬.
		cr->TextColor =PUSHBTN_TEXTCOLOR_0;
		cr->BackColor =PUSHBTN_BACKCOLOR_0;
		cr->BorderColor =PUSHBTN_BODERCOLOR_0;
	}

	if(style&9)
	{//����ǡ���ֹ������,�����ʰ���ɫ.

		//�ȰѸ���ɫRGB����ͬ������(ʹԭɫ�䰵).
		cr->TextColor =RGB32_Offset(cr->TextColor,-10,-10,-10);
		cr->BackColor =RGB32_Offset(cr->BackColor,-10,-10,-10);
		cr->BorderColor =RGB32_Offset(cr->BorderColor,-10,-10,-10);
		cr->ForeColor =RGB32_Offset(cr->ForeColor,-10,-10,-10);

		//�ٰѸ���ɫ���ɫ��ƽ������(ʹԭɫ���).
		cr->TextColor =RGB32_AVG(cr->TextColor,RGB888(120,120,128));
		cr->BackColor =RGB32_AVG(cr->BackColor,RGB888(120,120,128));
		cr->BorderColor =RGB32_AVG(cr->BorderColor,RGB888(120,120,128));
		cr->ForeColor =RGB32_AVG(cr->ForeColor,RGB888(120,120,128));

	}
}

static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{ //��Ĭ�Ϸ��İ�ť(�û��ɰ��Լ���ϲ��ʵ��...).

	HDC hdc =di->hDC;
	RECT rc=di->rc;
	int state =di->State;

	//����������(��״ֵ̬).
	__draw_backgnd(hdc,&rc,cr,state);

	//���ư�ť�ϵ�����.
	if(state&BST_PUSHED) //�жϰ�ť״̬.
	{//����״̬.
		OffsetRect(&rc,+1,+1);
		SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER);
	}
	else
	{//����״̬.

		SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER);
	}

}

static void _Paint_FLAT(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{ //����ƽ����İ�ť.

	HDC hdc =di->hDC;
	RECT rc=di->rc;
	u32 state =di->State;

	SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
	SetPenColor(hdc,MapXRGB8888(hdc,cr->BorderColor));
	SetBrushColor(hdc,MapXRGB8888(hdc,cr->BackColor));

	if(state&BST_PUSHED)
	{//����״̬.
		OffsetRect(&rc,+1,+1); //����ʱ,ʹ��ť�ϵ����������·���ƫ��һ���,����ȥ���³�����ʽ.

		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND|DT_BORDER);
	}
	else
	{//����״̬.

		SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,DT_CENTER|DT_VCENTER|DT_BKGND|DT_BORDER);
	}

}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

static win_oem_t oem_flat=
{
	_SetColor,
	_Paint_FLAT,
};

win_oem_t* GetButtonOEM(void)
{
	return &oem;
	//return &oem_flat;
}



