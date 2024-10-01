#include <emXGUI.h>
#include "GUI_AppDef.h" 
#include "Widget.h"
/*
 * @brief  ���������水ť
 * @param  hwnd:   �������ľ��ֵ
 * @retval NONE
*/
void home_owner_draw(DRAWITEM_HDR *ds) 
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
  EnableAntiAlias(hdc, TRUE);
	

  EnableAntiAlias(hdc, FALSE);   
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
   //��ť����״̬
   if (ds->State & BST_PUSHED)
	{ 
		SetBrushColor(hdc, MapRGB(hdc, 105,105,105));
	}
	else//��ť����״̬
	{ 
		SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
	}
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
   /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_32);
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
   rc.y = -10;
   rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
}

/*
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  ScrollBar_parm �������Ĳ���
 * @retval NONE
*/
void draw_scrollbar(HWND hwnd, HDC hdc, ScrollBar_S ScrollBar_parm)
{
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* ���� */
	SetBrushColor(hdc, ScrollBar_parm.back_c);
	FillRect(hdc, &rc);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, ScrollBar_parm.page_c));
	FillRect(hdc, &rc_scrollbar);

	/* ���� */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));

	/* �߿� */

	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.w / 2, rc.w / 2 - 1);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, ScrollBar_parm.fore_c));
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.w / 2, rc.w / 2 - 1);
}
/*
 * @brief  �Զ��廬�������ƺ���
 * @param  ds:	�Զ�����ƽṹ��
 * @param  ScrollBar_parm1,ScrollBar_parm2:�������Ĳ���
 * @retval NONE
*/
void scrollbar_owner_draw(DRAWITEM_HDR *ds, ScrollBar_S ScrollBar_parm1, ScrollBar_S ScrollBar_parm2)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;
	//	int i;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   
         
   	
	//���ư�ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc_mem1, ScrollBar_parm1);
	//������ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc_mem, ScrollBar_parm2);
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//��
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//��
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w) , rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//���ƻ���
	if (ds->State & SST_THUMBTRACK)//����
	{
      BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}
	else//δѡ��
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//�ͷ��ڴ�MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

