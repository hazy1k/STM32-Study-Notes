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

/* �����ź��� */
GUI_SEM *Call_Sem;
TaskHandle_t* CallCallMonitorHandle;    // ������������ƿ�

/* ͨ����Ϣ */
typedef struct 
{
	u8 Flag;
	WCHAR Status[128];
  WCHAR PhoneNum[128];
}CallInfo_t;

static u16 id_tbl[]=
{
	L'1', L'2', L'3',
	L'4', L'5', L'6',
	L'7', L'8', L'9',
	L'*', L'0', L'#',

};

static const WCHAR *str_tbl[]=
{
	L"1", L"2", L"3",
	L"4", L"5", L"6",
	L"7", L"8", L"9",
	L"*", L"0", L"#",

};

static WCHAR phone_num[12] = {L'1',L'1',L'2'};

#define	NUM_BTN_COUNT	(sizeof(id_tbl)/sizeof(id_tbl[0]))
  
/* �ؼ� ID ���� */
enum
{   
  /* ��ť ID */
  eID_Phone_EXIT  = 0,
  eID_Phone_CONN,
  eID_Phone_DEL,
  eID_Phone_INNUM,
  eID_Call_EXIT,
  eID_Call_EXIT1,    // ����ӵ绰��İ�ť
  eID_Call_WIN,
  eID_Call_ANSWER,
  eID_Call_STATUS,
  eID_Call_TIME,
  eID_Call_PHONENUM,

};

uint8_t CallFlag = 0;    // ͨ����־

/*
 * @brief  �˳���ť�ػ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Phone_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
	RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

//	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 26, 197, 93));
	}
  
  SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
  FillRect(hdc, &rc);

  SetPenSize(hdc, 1);

  InflateRect(&rc, -8, -11);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.x + 21);
    rc.y += 5;
  }
}

/*
 * @brief  Բ�ΰ�ť�ػ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Phone_CircleButton_OwnerDraw(DRAWITEM_HDR *ds, COLOR_RGB32 bk_colous)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	//	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB888(hdc, bk_colous));
	FillCircle(hdc, rc.w / 2, rc.h / 2, rc.h / 2);
  
  HFONT controlFont_32;
	
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
  SetFont(hdc, controlFont_32);
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.x -= 2;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

  SetFont(hdc, defaultFont);    // �ָ�Ĭ������
  DeleteFont(controlFont_32);
}

/*
 * @brief  Բ�ΰ�ť�ػ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Phone_DelButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	//	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬

		SetTextColor(hdc, MapRGB(hdc, 20, 20, 20));
	}

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);
  
  HFONT controlFont_32;
	
  controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
  SetFont(hdc, controlFont_32);
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.y += 4;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

  SetFont(hdc, defaultFont);    // �ָ�Ĭ������
  DeleteFont(controlFont_32);
}

/*
 * @brief  �ػ�绰������ı�
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

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
	FillRoundRect(hdc, &rc, 10);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)
}

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
//  rc.w -= 45;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}

static LRESULT	DialWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static uint32_t TimeCount = 0;       // ͨ��ʱ����ʱ
	switch (msg)
	{
    case WM_CREATE:
    {
      RECT rc;
      CallInfo_t	*CallInfo;

      CallInfo = (CallInfo_t *)lParam;

      GetClientRect(hwnd, &rc);

      if (CallInfo->Flag == 1)    // ����
      {
        CreateWindow(BUTTON, L"N", WS_VISIBLE | WS_OWNERDRAW, 141, 152, 38, 38, hwnd, eID_Call_EXIT,   NULL, NULL);
      }
      else if (CallInfo->Flag == 2)    // ����
      {
        CreateWindow(BUTTON, L"N", WS_OWNERDRAW, 141, 152, 38, 38, hwnd, eID_Call_EXIT1, NULL, NULL);
        CreateWindow(BUTTON, L"N", WS_VISIBLE | WS_OWNERDRAW, 107, 152, 38, 38, hwnd, eID_Call_EXIT,   NULL, NULL);
        CreateWindow(BUTTON, L"N", WS_VISIBLE | WS_OWNERDRAW, 183, 152, 38, 38, hwnd, eID_Call_ANSWER, NULL, NULL);
        SetTimer(hwnd, 1, 300, TMR_START, NULL);    // ����������ʱ��
      }
      
      CreateWindow(TEXTBOX, CallInfo->Status, WS_VISIBLE | WS_OWNERDRAW, 115, 23, 91, 30, hwnd, eID_Call_STATUS, NULL, NULL);
      CreateWindow(TEXTBOX, CallInfo->PhoneNum, WS_VISIBLE | WS_OWNERDRAW, 68, 61, 184, 26, hwnd, eID_Call_PHONENUM, NULL, NULL);
      CreateWindow(TEXTBOX, L"00:00", WS_VISIBLE | WS_OWNERDRAW, 136, 87, 49, 26, hwnd, eID_Call_TIME, NULL, NULL);

      SetTimer(hwnd, 0, 1000, TMR_START, NULL);

      break;
    }
    
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // ��ʱ�� ID

      if (tmr_id == 0)    // ÿ�붨ʱ��
      {
        if(IsNOCARRIER() == SIM900A_TRUE)    // �Է��Ҷ��˵绰
        {
          CallFlag = 0;
          SIM900A_HANGOFF();         // �ҵ绰
          SIM900A_CLEAN_RX();        // ������ջ���
          BEEP_OFF;                  // �رշ�����
          PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
        }
        
        if(sim900a_cmd_check("\r\nNO ANSWER") == SIM900A_TRUE)    // �Է���ʱû�ӵ绰
        {
          CallFlag = 0;
          SIM900A_HANGOFF();         // �ҵ绰
          SIM900A_CLEAN_RX();        // ������ջ���
          BEEP_OFF;                  // �رշ�����
          PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
        }
        
        if(sim900a_cmd_check("\r\n+COLP:") == SIM900A_TRUE)    // �Է������˵绰(����ֻʹ���˲��ֲ���)
        {
          SIM900A_CLEAN_RX();    // ������ջ���
          TimeCount = 0;         // �Է������˵绰�����¿�ʼ��ʱ
        }
        
        TimeCount++;
        
        WCHAR wbuf[20];
        
        x_wsprintf(wbuf, L"%02d:%02d", TimeCount/60, TimeCount%60);
        SetWindowText(GetDlgItem(hwnd, eID_Call_TIME), wbuf);
      }
      else if (tmr_id == 1)    // ������ʱ��
      {
        BEEP_TOGGLE;    // ����������״̬
      }
      break;
    }

    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
      FillRect(hdc, &rc);
      return FALSE;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);

      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      switch (ds->ID)
      {
        case eID_Call_EXIT1:
        case eID_Call_EXIT:
        {
          Phone_CircleButton_OwnerDraw(ds, RGB888(240, 0, 0));
          return TRUE;
        }

        case eID_Call_ANSWER:
        {
          Phone_CircleButton_OwnerDraw(ds, RGB888(26, 197, 93));
          return TRUE;
        }

        case eID_Call_STATUS:
        case eID_Call_TIME:
        case eID_Call_PHONENUM:
        {
          Brigh_Textbox_OwnerDraw(ds);
          return TRUE;
        }
      }

      break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id = LOWORD(wParam);//��ȡ��Ϣ��ID��
      code = HIWORD(wParam);//��ȡ��Ϣ������    

      switch (code)
      {
        case BN_CLICKED:
        {
          switch (id)
          {
            /* �˳���ť���� */
            case eID_Call_EXIT:
            case eID_Call_EXIT1:
            {
              CallFlag = 0;
              SIM900A_HANGOFF();         // �ҵ绰
              SIM900A_CLEAN_RX();        // ������ջ���
              BEEP_OFF;                  // �رշ�����
              PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
            }
            break;
            
            /* ������ť���� */
            case eID_Call_ANSWER:
            {
              TimeCount = 0;    // �ӵ绰
              SIM900A_HANGON();      // ��������
              SIM900A_CLEAN_RX();    // ������ջ���
              KillTimer(hwnd, 1);    // ɾ������������ʱ��
              BEEP_OFF;              // �رշ�����

              ShowWindow(GetDlgItem(hwnd, eID_Call_ANSWER), SW_HIDE);    // ���ؽ�����ť
              ShowWindow(GetDlgItem(hwnd, eID_Call_EXIT),  SW_HIDE);     // ���ؿ����˳���ť
              ShowWindow(GetDlgItem(hwnd, eID_Call_EXIT1), SW_SHOW);     // ��ʾ�м��˳���ť
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
      TimeCount = 0;    // ����ͨ��ʱ��
      BEEP_OFF;         // �رշ�����
      return PostQuitMessage(hwnd);
    }

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return WM_NULL;

}

extern int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);

static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc, m_rc[12];
      
      GetClientRect(hwnd, &rc);
      Beep_Init();
      /* ��ʼ�� GSM ģ�� */
      if(sim900a_init()!= SIM900A_TRUE)
      {  
        SetTimer(hwnd, 0, 0, TMR_START|TMR_SINGLE, NULL);        
      }
      else
      {
        SetTimer(hwnd, 1, 500, TMR_START, NULL); 
      }
      SIM900A_CLEAN_RX();//������ջ���

      InflateRectEx(&rc, -2, -56, -2, -49);
      MakeMatrixRect(m_rc, &rc, 0, 0, 3, 4);

      for(int i=0; i<NUM_BTN_COUNT; i++)
      {
        CreateWindow(BUTTON, str_tbl[i], WS_VISIBLE,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,hwnd,id_tbl[i],NULL,NULL);
      }
	    
      CreateWindow(BUTTON, L"����",	WS_VISIBLE|WS_OWNERDRAW, 35,  195, 38, 38, hwnd, eID_Phone_EXIT, NULL, NULL);
      CreateWindow(BUTTON, L"N",	WS_VISIBLE|WS_OWNERDRAW, 141, 196, 38, 38, hwnd, eID_Phone_CONN, NULL, NULL);
      CreateWindow(BUTTON, L"M",	WS_VISIBLE|WS_OWNERDRAW, 248, 195, 38, 38, hwnd, eID_Phone_DEL,  NULL, NULL);
      CreateWindow(TEXTBOX, L"112", WS_VISIBLE|WS_OWNERDRAW, 2, 9, 317, 38, hwnd, eID_Phone_INNUM, NULL, NULL);
      
      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // ��ʱ�� ID

      if (tmr_id == 0)    // GSM ģ���ʼ������
      {
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] ={L"ȷ��",L"ȡ��"};      //�Ի����ڰ�ť������

        ops.Flag =MB_ICONERROR;
        ops.pButtonText =btn;
        ops.ButtonCount =2;
        RC.w = 140;
        RC.h = 100;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"û�м�⵽GSMģ��\n�����¼�����ӡ�", L"����", &ops);    // ��ʾ������ʾ��
        PostCloseMessage(hwnd);                                                              // ���͹رմ��ڵ���Ϣ
      }
      else if(tmr_id == 1)    // ��������������
      {
        char num[20]={0};
        
        if(IsRing(num)== SIM900A_TRUE && CallFlag == 0)
        {
          CallFlag = 1;     // �������
          WNDCLASS wcex;
          RECT rc;
          CallInfo_t CallInfo;
          WCHAR buf[128];

          /* ����绰���� */
          x_mbstowcs(buf, num, 40);
          x_wstrcpy(CallInfo.PhoneNum, buf);

          CallInfo.Flag = 2;                            // ��������
          x_wstrcpy(CallInfo.Status, L"��������");

          wcex.Tag = WNDCLASS_TAG;
          wcex.Style = CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc = (WNDPROC)DialWinProc;
          wcex.cbClsExtra = 0;
          wcex.cbWndExtra = 0;
          wcex.hInstance = NULL;
          wcex.hIcon = NULL;
          wcex.hCursor = NULL;

          rc.x = 0;
          rc.y = 0;
          rc.w = GUI_XSIZE;
          rc.h = GUI_YSIZE;

          CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, eID_Call_WIN, NULL, &CallInfo);
        }
      }

      break;
    }
    
    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
      FillRect(hdc, &rc);
      return FALSE;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      
      EndPaint(hwnd, &ps);
      break;
    }
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Phone_EXIT:
          {
            Phone_ExitButton_OwnerDraw(ds);
            return TRUE;             
          } 
          
          case eID_Phone_DEL:
          {
            Phone_DelButton_OwnerDraw(ds);    // �ػ��˸�ť
            return TRUE;  
          }
          
          case eID_Phone_CONN:
          {
            Phone_CircleButton_OwnerDraw(ds, RGB888(26, 197, 93));   // �ػ沦�Ű�ť
            return TRUE;             
          }    

          case eID_Phone_INNUM:
          {
            Fillet_Textbox_OwnerDraw(ds);
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
					case eID_Phone_EXIT:
					{
						PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
					}
					break;

					/* ��ͨ��ť���� */
					case eID_Phone_CONN:
					{
						WNDCLASS wcex;
						RECT rc;
						CallInfo_t CallInfo;
            char TextBuf[12];
            
            CallFlag = 1;     // ��־ͨ����
            
            /* ����绰���� */
            GetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num, 12);
            x_wcstombs(TextBuf ,phone_num, 12);
            
            if (*phone_num == NULL)    // ��һ������Ϊ�գ���˵��û�к���
            {
              break;
            }
//            sim900a_tx_printf("AT+COLP=1\r");    // �����������˳�ʼ����
            sim900a_call(TextBuf);

						CallInfo.Flag = 1;    // ����
						x_wstrcpy(CallInfo.Status, L"���ڲ���");
            x_wstrcpy(CallInfo.PhoneNum, phone_num);

						wcex.Tag = WNDCLASS_TAG;
						wcex.Style = CS_HREDRAW | CS_VREDRAW;
						wcex.lpfnWndProc = (WNDPROC)DialWinProc;
						wcex.cbClsExtra = 0;
						wcex.cbWndExtra = 0;
						wcex.hInstance = NULL;
						wcex.hIcon = NULL;
						wcex.hCursor = NULL;

						rc.x = 0;
						rc.y = 0;
						rc.w = GUI_XSIZE;
						rc.h = GUI_YSIZE;

						CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE, 
                         rc.x, rc.y, rc.w, rc.h, hwnd, eID_Call_WIN, NULL, &CallInfo);
					}
					break;

					/* �˸�ť���� */
					case eID_Phone_DEL:
					{
						u8 Len = 0;
						Len = GetWindowTextLength(GetDlgItem(hwnd, eID_Phone_INNUM));
						if (Len >= 1)
						{
							GetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num, 11);
							*(phone_num + Len - 1) = L'\0';
							SetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num);
						}
						
					}
					break;

					/* ���Ӱ�ť���� */
					case	L'0':
					case	L'1':
					case	L'2':
					case	L'3':
					case	L'4':
					case	L'5':
					case	L'6':
					case	L'7':
					case	L'8':
					case	L'9':
					case	L'*':
					case	L'#':
					{
						WCHAR wbuf[2] = {id, '\0'};

						if (GetWindowTextLength(GetDlgItem(hwnd, eID_Phone_INNUM)) < 11)
						{
							GetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num, 11);        // �õ�ԭ�ı�
							x_wstrcat(phone_num, wbuf);                                             // ׷�����ı�
							SetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num);            // ���ô��ڵ��ı�
						}
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
      // USART_ITConfig(GSM_USART, USART_IT_RXNE, DISABLE);	
      // USART_Cmd(GSM_USART, DISABLE);
      // GSM_USART_CLKCMD(GSM_USART_CLK, DISABLE);
      SIM900A_CLEAN_RX();
      CallFlag = 0;
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

/*
 * @brief  �绰 APP ����
 * @param  NONE
 * @retval NONE
*/
void GUI_Phone_Dialog(void)
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
                              L"GUI Phone Dialog",
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

#if 0
/*
 * @brief  ������ʾ����
 * @param  num[]:����绰����
 * @retval NONE
*/
void GUI_PhoneCall_Dialog(char num[])
{

  CallFlag = 1;     // �������
  WNDCLASS wcex;
//  RECT rc;
  CallInfo_t CallInfo;
  WCHAR buf[128];

  /* ����绰���� */
  x_mbstowcs(buf, num, 40);
  x_wstrcpy(CallInfo.PhoneNum, buf);

  CallInfo.Flag = 2;                            // ��������
  x_wstrcpy(CallInfo.Status, L"��������");
	
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)DialWinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI PhoneCall Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, &CallInfo);
   //��ʾ������
	ShowWindow(MAIN_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}

/*
 * @brief  ��̨����������
 * @param  num[]:����绰����
 * @retval NONE
*/
void PhoneCallMonitorTask(void *p)
{
  char num[20]={0};

  Call_Sem = GUI_SemCreate(0, 1);    // ����һ����ֵ�ź���
  BEEP_GPIO_Config();                // ��ʼ��������

  /* ��ʼ�� GSM ģ�� */
  if(sim900a_init() != SIM900A_TRUE)           // ��ʼ�����ڲ���� GSM ģ��
  {  
    GUI_ERROR("û�м�⵽ GSM ģ��");
//     vTaskSuspend(GUI_GetCurThreadHandle());     // GSM ��ʼ��ʧ�ܹ���ǰ����
  }

  while(TRUE)
  {
    GUI_SemWait(Call_Sem, 0xFFFFFFFF);    // �ȴ��ź���

    if(IsRing(num)== SIM900A_TRUE && CallFlag == 0)    // �ж��Ƿ�������
    {
      CallFlag = 1;
      GUI_DEBUG("�������绰��");
      GUI_PhoneCall_Dialog(num);    // ����������ʾ����
      SIM900A_CLEAN_RX();           // ������ջ���
    }
    else
    {
      GUI_msleep(1200);             // �������磬��Щʱ����������������
      SIM900A_CLEAN_RX();           // ������ջ���
    }
  }
}
#endif

