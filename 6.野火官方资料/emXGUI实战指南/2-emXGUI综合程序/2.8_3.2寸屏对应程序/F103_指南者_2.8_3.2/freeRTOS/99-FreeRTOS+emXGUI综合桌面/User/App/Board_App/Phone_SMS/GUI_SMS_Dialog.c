#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "x_libc.h"
				   
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./Phone_SMS/sim900a/bsp_sim900a.h"
#include "./beep/bsp_beep.h"
#include "./dwt_delay/core_delay.h"   

#define COLOR_SMS_BACK_GROUND         50,50,50
extern int SelectDialogBox(HWND hwndParent, RECT rc, const WCHAR *pText, const WCHAR *pCaption, const MSGBOX_OPTIONS *ops);
extern int number_input_box(int x, int y, int w, int h,
							              const WCHAR *pCaption,
							              WCHAR *pOut,
							              int MaxCount,
							              HWND hwndParent);
  
/* �ؼ� ID ���� */
enum
{   
  /* ��ť ID */
  eID_SMS_EXIT  = 0,
  eID_SMS_CLEAR,
  eID_SMS_SEND,
  eID_SMS_DEL,
  eID_SMS_CONTENT,
  eID_SMS_NUMBER,
  eID_SMS_LIST,

};

/* �û��Զ�����Ϣ */
enum eMSG 
{
	eMSG_READ_TEXT = WM_USER + 1,
	eMSG_INIT_ERROR,
};

typedef struct
{
	WCHAR	cTime[20];
	WCHAR cState[2];
  uint16_t	Address;
}ListEXData_t;

/*
 * @brief  ����������
 * @param  hListWnd: �б����
 * @retval NONE
*/
void Read_Text(HWND hListWnd)
{
	u32 i = 0;
	u32 xC = 1;
	WCHAR wbuf[40];
  char messagename[40];
//  char messagename1[20]={'��'};
  char *wNumber;
  char *wTime;
  uint16_t ListData = 0;    // ���λ 0:����1:�գ���15λΪ���ŵ�ַ
  uint8_t Temp = 0;
  
  sim900a_tx_printf("AT+CNMI=2,1\r");
  SIM900A_DELAY(100);  
  sim900a_tx_printf("AT+CMGF=1\r");           //�ı�ģʽ
  SIM900A_DELAY(100); 
  sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");     //"GSM"�ַ���
  SIM900A_DELAY(100); 
  
  wNumber = (char *)GUI_VMEM_Alloc(100);
  wTime = (char *)GUI_VMEM_Alloc(256);

  SIM900A_CLEAN_RX();
  Temp = ReadMessageInfo(xC, wNumber, wTime);

	while((Temp) != 0)
	{
    hexuni2gbk(wNumber, messagename);	
    strcat(messagename, wTime);                             // ƴ����ʱ��
    
//    GUI_DEBUG("number->(%s)\n",messagename);
    
    x_mbstowcs(wbuf, messagename, sizeof(wbuf));	    // ��Ansi�ַ�ת����GUI��unicode�ַ�.
    
		//��Listbox������һ��Item���¼�ļ������ļ�����.
		i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
		SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
    
		/* ͨ������Item���DATAֵ����¼���ŵ�����(���������Ƿ��͵Ļ��ǽ��յ�). */
    ListData = xC++;        // ������ŵ�ַ
    if (Temp != 3)
    {
      ListData |= 1 << 15;    // ���Ϊ��
    }
    else
    {
      ListData &= ~(1 << 15);    // ���Ϊ��
    }
		SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)ListData);
    
    SIM900A_CLEAN_RX();
    
    Temp = ReadMessageInfo(xC, wNumber, wTime);
	}
  
  GUI_VMEM_Free(wNumber);
  GUI_VMEM_Free(wTime);
}

/*
 * @brief  �˳���ť�ػ�
 * @param  ds: �Զ�����ƽṹ��
 * @retval NONE
*/
static void SMS_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
#if 0
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc); //�þ�����䱳��

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	FillCircle(hdc, rc.x + rc.w, rc.y, rc.w);

	if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	/* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_64);

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	rc.y = -10;
	rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)

  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
#else
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //���û���ɫ
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
#endif
}

// Բ�ǰ�ť�ػ���
static void SMS_FilletButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 22, 155, 213));
	EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc, &rc, 5);
	EnableAntiAlias(hdc, FALSE);

	if (ds->State & BST_PUSHED)
	{
		/* ��ť�ǰ���״̬ */
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));       //��������ɫ
		OffsetRect(&rc, 1, 1);
	}
	else
	{
		/* ��ť�ǵ���״̬ */
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}

	GetWindowText(hwnd, wbuf, 128);                          //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);    //��������(���ж��뷽ʽ)
}

/*
 * @brief  �ػ���ʾ���ȵ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Fillet_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	GetClientRect(hwnd, &rc);//�õ��ؼ���λ��

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
	EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc, &rc, 8);
	EnableAntiAlias(hdc, FALSE);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)
}

#if 0
/*
 * @brief  �ػ���ʾ���ȵ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	GetClientRect(hwnd, &rc);//�õ��ؼ���λ��

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	rc.w -= 45;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}
#endif
/*
 * @brief  �ػ��б�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];
  WCHAR Time[40];
  WCHAR *Temp;
	POINT pt;
  uint16_t ListData = 0;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

//	hdc =CreateMemoryDC(SURF_ARGB4444,ds->rc.w,ds->rc.h); //����һ���ڴ�DC����ͼ.

	rc =ds->rc;

	SetBrushColor(hdc,MapRGB(hdc,220,220,240));
	FillRect(hdc,&ds->rc);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* �б�Ϊ�գ���ʾ��ʾ��ϢȻ��ֱ�ӷ��� */
    DrawText(hdc, L"�б�����У�\r\n��û����Ϣ��", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
    ListData =SendMessage(hwnd,LB_GETITEMDATA,i,0);     // ���Item���DATAֵ(��������Ŀ��ʱ���õ�DATAֵ...).
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}
		
    SetBrushColor(hdc,MapRGB(hdc,22,155,213));
    InflateRectEx(&rc, -1, 0, -1, 0);
    FillCircle(hdc, rc.x+rc.h/2, rc.y+rc.h/2, rc.h/2);

		SetTextColor(hdc,MapRGB(hdc,50,10,10));

		if(i==cursel)
		{
			SetTextColor(hdc,MapRGB(hdc,250,10,10));

			SetBrushColor(hdc,MapRGB(hdc,250,250,250));
			FillRect(hdc,&rc);

		}
		else
		{
			SetTextColor(hdc,MapRGB(hdc,10,10,10));
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
    if ((ListData >> 15) & 1)
    {
      DrawText(hdc, L"��",-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);    // ��ʾ��
    }
    else
    {
      DrawText(hdc, L"��",-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);    // ��ʾ��
    }
    rc.x += rc.h;
    rc.w -= rc.h;
    
    Temp = x_wstrstr(wbuf, L"/");
    if (Temp != NULL)
    { 
      /* ��ʱ��,�ѵ绰�����ʱ��� */
      x_wstrcpy(Time, Temp+1);
      *Temp = '\0';
//      DrawText(hdc,Time,-1,&rc,DT_SINGLELINE|DT_RIGHT|DT_VCENTER);     // ��ʾʱ��
    }
    
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);      // ��ʾ�绰����
    
    SetPenColor(hdc,MapRGB(hdc,10,10,10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                  // ��һ����

		i++;
	}

//	BitBlt(ds->hDC,0,0,ds->rc.w,ds->rc.h,hdc,0,0,SRCCOPY); //���ڴ�DC���������������(DC)��.
//	DeleteDC(hdc);
}


static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc, m_rc[12];
      
      GetClientRect(hwnd, &rc);

      /* ��ʼ�� GSM ģ�� */
      if (sim900a_init() != SIM900A_TRUE)//
      {
        SetTimer(hwnd, 2, 1, TMR_START|TMR_SINGLE, NULL);    // ��ʼ��ʧ�ܿ���������ʾ��ʱ��
      }

      //InflateRectEx(&rc, -3, -112, -3, -101);
      //MakeMatrixRect(m_rc, &rc, 0, 0, 3, 4);
	    
      CreateWindow(BUTTON, L"O",	WS_VISIBLE|WS_OWNERDRAW|WS_TRANSPARENT, 295, 10, 23, 23, hwnd, eID_SMS_EXIT, NULL, NULL);
      CreateWindow(BUTTON, L"���",	WS_VISIBLE|WS_OWNERDRAW, 247, 215, 33, 22, hwnd, eID_SMS_CLEAR, NULL, NULL);
      CreateWindow(BUTTON, L"ɾ��ȫ������",	WS_VISIBLE|WS_OWNERDRAW, 164, 215, 77, 22, hwnd, eID_SMS_DEL,  NULL, NULL);
      CreateWindow(BUTTON, L"����", WS_VISIBLE|WS_OWNERDRAW, 285, 215, 33, 22, hwnd, eID_SMS_SEND, NULL, NULL);
      CreateWindow(TEXTBOX, L"������ʾ����", WS_VISIBLE, 164, 35, 153, 177, hwnd, eID_SMS_CONTENT, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SMS_CONTENT), TBM_SET_TEXTFLAG, 0, DT_TOP | DT_LEFT | DT_BKGND | DT_WORDBREAK);
      CreateWindow(TEXTBOX, L"", WS_VISIBLE | WS_OWNERDRAW,191, 7, 100, 22, hwnd, eID_SMS_NUMBER, NULL, NULL);
      CreateWindow(LISTBOX, L"SMS LIST", WS_VISIBLE | WS_BORDER | WS_OWNERDRAW, 3, 35, 160, 202, hwnd, eID_SMS_LIST, NULL, NULL);
//      SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_LOCKCURSEL, TRUE, 0);
      SetTimer(hwnd, 0, 1, TMR_START|TMR_SINGLE, NULL);
      SetTimer(hwnd, 1, 1000, TMR_START, NULL);

//	  PostAsyncMessage(hwnd, eMSG_READ_TEXT, 0, 0);     // ִ��һ�ζ�����

      break;
    } 

    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // ��ʱ�� ID
      
      if (tmr_id == 0)
      {
        /* �����ŵ��б� */
        HWND wnd;
        wnd = GetDlgItem(hwnd, eID_SMS_LIST);

        SendMessage(wnd, LB_RESETCONTENT, 0, 0);

        Read_Text(wnd);
        InvalidateRect(wnd, NULL, TRUE);
      }
      else if (tmr_id == 1)    // ÿ�붨ʱ��
      {
        /* ����ǲ������¶��� */
        uint8_t newmessadd=0;
        newmessadd=IsReceiveMS();      
        if(newmessadd)
        {	
          SIM900A_DELAY(500); 
          sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");     //"GSM"�ַ���        
          SIM900A_DELAY(500);        
          BEEP_ON;
          SIM900A_CLEAN_RX();//������ջ���
          
          u32 i = 0;
          WCHAR wbuf[40];
          char messagename[40];
          char *wNumber;
          char *wTime;
          uint16_t ListData = 0;    // ���λ 0:����1:�գ���15λΪ���ŵ�ַ
          uint8_t Temp = 0;
                  
          HWND hListWnd = GetDlgItem(hwnd, eID_SMS_LIST);
          
          wNumber = (char *)GUI_VMEM_Alloc(100);
          wTime = (char *)GUI_VMEM_Alloc(256);

          SIM900A_CLEAN_RX();
          Temp = ReadMessageInfo(newmessadd, wNumber, wTime);
          BEEP_OFF;
          if (Temp != 0)
          {
            hexuni2gbk(wNumber, messagename);	
            strcat(messagename, wTime);                             // ƴ����ʱ��
            // GUI_DEBUG("number->(%s)\n",messagename);
            x_mbstowcs(wbuf, messagename, sizeof(wbuf));	    // ��Ansi�ַ�ת����GUI��unicode�ַ�.
            
            //��Listbox������һ��Item���¼�ļ������ļ�����.
            i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
            SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            
            /* ͨ������Item���DATAֵ����¼���ŵ�����(���������Ƿ��͵Ļ��ǽ��յ�). */
            ListData = newmessadd;
            ListData |= 1 << 15;    // ���Ϊ��

            SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)ListData);
            
            SIM900A_CLEAN_RX();
            InvalidateRect(hListWnd, NULL, TRUE);
          }
          
          GUI_VMEM_Free(wNumber);
          GUI_VMEM_Free(wTime);
        }
      }
      else if (tmr_id == 2)
      {
        /* ��ʼ��ʧ�� */
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] = { L"ȷ��",L"ȡ��" };      //�Ի����ڰ�ť������

        ops.Flag = MB_ICONERROR;
        ops.pButtonText = btn;
        ops.ButtonCount = 2;
        RC.w = 160;
        RC.h = 120;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"û�м�⵽GSMģ��\n�����¼�����ӡ�", L"����", &ops);    // ��ʾ������ʾ��
        PostCloseMessage(hwnd);
      }

      break;
    }

	case eMSG_INIT_ERROR:
	{
		// �ڶ�ʱ���������
		break;
	}
  
	case eMSG_READ_TEXT:
	{
		HWND wnd;
		int i;

		wnd = GetDlgItem(hwnd, eID_SMS_LIST);

		SendMessage(wnd, LB_RESETCONTENT, 0, 0);

		Read_Text(wnd);
		InvalidateRect(wnd, NULL, TRUE);

		break;
	}
	
    
    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
      FillRect(hdc, &rc);
      return TRUE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc = {146, 7, 40, 22};
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 22, 155, 213));
      EnableAntiAlias(hdc, TRUE);
      FillRoundRect(hdc, &rc, 5);
      EnableAntiAlias(hdc, FALSE);

      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      DrawText(hdc, L"��ϵ��", -1, &rc, DT_VCENTER | DT_CENTER);    //��������(���ж��뷽ʽ)

      rc.x = 57;
      rc.y = 7;
      rc.w = 37;
      rc.h = 19;
      DrawText(hdc, L"����", -1, &rc, DT_VCENTER | DT_CENTER);    //��������(���ж��뷽ʽ)
      
      EndPaint(hwnd, &ps);
      break;
    }
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_SMS_EXIT:
          {
            SMS_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

          case eID_SMS_CLEAR:
          case eID_SMS_DEL:
          case eID_SMS_SEND:
          {
            SMS_FilletButton_OwnerDraw(ds);
            return TRUE;
          }

          case eID_SMS_NUMBER:
          {
            Fillet_Textbox_OwnerDraw(ds);
            return TRUE;
          }
          
          case eID_SMS_LIST:
          {
            listbox_owner_draw(ds);
            return TRUE;
          }
       }
       break;
    }
    
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    

      switch (code)
      {
        case BN_CLICKED:
        {
          switch (id)
          {
            /* �˳���ť���� */
            case eID_SMS_EXIT:
            {
              PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
            }
            break;

            /* ɾ��ȫ�����Ű�ť���� */
            case eID_SMS_DEL:
            {
              RECT RC;
              MSGBOX_OPTIONS ops;
              const WCHAR *btn[] = { L"ȷ��",L"ȡ��" };      //�Ի����ڰ�ť������

              ops.Flag = MB_ICONERROR;
              ops.pButtonText = btn;
              ops.ButtonCount = 2;
              RC.w = 160;
              RC.h = 120;
              RC.x = (GUI_XSIZE - RC.w) >> 1;
              RC.y = (GUI_YSIZE - RC.h) >> 1;
              
              if (SelectDialogBox(hwnd, RC, L"����ɾ��ȫ�����շ����š�", L"ɾ��", &ops) == 0)    // ��ʾȷ����ʾ��
              {
                sim900a_tx_printf("AT+CMGDA=\"DEL ALL\"\r");                           // ɾ��ȫ������
                SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_RESETCONTENT, 0, 0);    // ����б�
                InvalidateRect(GetDlgItem(hwnd, eID_SMS_LIST), NULL, TRUE);            // �ػ��б�
              }
            }
            break;

            /* �����ť���� */
            case eID_SMS_CLEAR:
            {
              SetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), L"");
              SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), L"");
            }
            break;

            /* ���Ͱ�ť���� */
            case eID_SMS_SEND:
            {
              char *cNumber;
              char *cContent;
              WCHAR *wNumber;
              WCHAR *wContent;
              uint16_t Add;
              int i = 0;
              uint16_t ListData = 0;
              
              cNumber  = (char  *)GUI_VMEM_Alloc(100);
              cContent = (char  *)GUI_VMEM_Alloc(512);
              wNumber  = (WCHAR *)GUI_VMEM_Alloc(100*sizeof(WCHAR));
              wContent = (WCHAR *)GUI_VMEM_Alloc(256*sizeof(WCHAR));
              
              GetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), wNumber, 99);
              GetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), wContent, 2000);
              x_wcstombs(cNumber, wNumber, 99);
              x_wcstombs(cContent, wContent, 2000);
              
              Add = sim900a_save_sms(cNumber, cContent);
              if (Add)
              {
                //��Listbox������һ��Item���¼�ļ������ļ�����.
                i = SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_GETCOUNT, 0, 0);
                SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_ADDSTRING, i, (LPARAM)wNumber);
                
                /* ͨ������Item���DATAֵ����¼���ŵ�����(���������Ƿ��͵Ļ��ǽ��յ�). */
                ListData = Add;
                ListData &= ~(1 << 15);    // ���Ϊ��
                SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_SETITEMDATA, i, (LPARAM)ListData);
                
                InvalidateRect(GetDlgItem(hwnd, eID_SMS_LIST), NULL, TRUE);
              }
              
              GUI_VMEM_Free(cNumber);
              GUI_VMEM_Free(cContent);
              GUI_VMEM_Free(wNumber);
              GUI_VMEM_Free(wContent);
            }
            break;

            /* �绰�����ı��� */
            case eID_SMS_NUMBER:
            {
              WCHAR I[21] = {0};
              number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"NUMBRE", I, 20, hwnd);
              SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), I);
            }
            break;

            case eID_SMS_LIST:
            {
              uint16_t ListData = 0;
              uint8_t i;
              i = SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_GETCURSEL,0,0);               // ��õ�ǰѡ����
              ListData = SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_GETITEMDATA,i,0);      // ���Item���DATAֵ(��������Ŀ��ʱ���õ�DATAֵ...).

              char *cNumber;
              char *cContent;
              WCHAR *wNumber;
              WCHAR *wContent;
              cNumber  = (char  *)GUI_VMEM_Alloc(100);
              cContent = (char  *)GUI_VMEM_Alloc(256);
              wNumber  = (WCHAR *)GUI_VMEM_Alloc(100*sizeof(WCHAR));
              wContent = (WCHAR *)GUI_VMEM_Alloc(256*sizeof(WCHAR));
              
              /* ���������� */
              if (readmessage(ListData & 0x7FFF, cNumber, cContent))
              {
                hexuni2gbk((char *)cNumber, (char *)&cNumber[100]);	
                hexuni2gbk((char *)cContent, (char *)&cContent[2000]);
                
//                GUI_DEBUG("cNumber:(%s)", (char *)&cNumber[100]);
//                GUI_DEBUG("cContent:(%s)", (char *)&cContent[2000]);
                
                x_mbstowcs(wNumber, (char *)&cNumber[100], 100);	    // ��Ansi�ַ�ת����GUI��unicode�ַ�.
                x_mbstowcs(wContent, (char *)&cContent[2000], 2000);	// ��Ansi�ַ�ת����GUI��unicode�ַ�.

                SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), wNumber);       // ���õ绰����
                SetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), wContent);     // ���ö�������
              }

              GUI_VMEM_Free(cNumber);
              GUI_VMEM_Free(cContent);
              GUI_VMEM_Free(wNumber);
              GUI_VMEM_Free(wContent);

            }
            break;
          }
        }
        break;
      }
		break;
    } 

    case WM_DESTROY:
    {
      
      // DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_SMS_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"Short Messaging Service",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(MAIN_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


