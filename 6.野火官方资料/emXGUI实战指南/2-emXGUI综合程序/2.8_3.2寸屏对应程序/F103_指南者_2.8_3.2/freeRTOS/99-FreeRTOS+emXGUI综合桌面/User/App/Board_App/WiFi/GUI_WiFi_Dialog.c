#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "./WiFi/GUI_WiFi_Dialog.h"
#include ".\WiFi\ESP8266\bsp_esp8266.h"
#include "bsp_ov7725.h"

int		number_input_box(int x, int y, int w, int h,
							const WCHAR *pCaption,
							WCHAR *pOut,
							int MaxCount,
							HWND hwndParent);

TaskHandle_t wifi_task_handle;

static HWND Receive_Handle;
              
uint8_t network_start_flag=0;

extern struct netif gnetif;
extern __IO uint8_t EthLinkStatus;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint16_t bsp_result=0;

static uint8_t IsConnect=0;

uint8_t ucTcpClosedFlag;
static uint8_t IsChange=1;
static char ssid[21]={"A304"};
static char pwd[21]={"wildfire"};
static char host_ip[16]={"192.168.1.6"};
static char host_port[6]={"5000"};
static uint8_t wifistat=0;

#define TRYCOUNT					10

void wifi_dispose_task(HWND hwnd) 
{
  IsConnect=0;//Ĭ�ϲ���������
	IsChange=1;
	wifistat=0;
  uint16_t i=0;
  uint32_t timecount=0;
  uint8_t ucStatus;
  char *receive;
  
  while(1)
  {
		if((wifistat==0)&&IsConnect)
		{			
			if(IsChange)
			{
//				WM_DisableWindow(hFrameClose);
				SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"������...");
        ESP8266_Init();
        macESP8266_CH_ENABLE();
        GUI_msleep(1000);
        //����ATָ��
        if(ESP8266_AT_Test()==0)	
          wifistat|=0x01;
        //ѡ��STAģʽ
        if(ESP8266_Net_Mode_Choose(STA)==0)		
          wifistat|=0x02;   
        
				//����wifi
				i=0;
				while((wifistat==0)&&(i<TRYCOUNT))
				{
          WCHAR wbuf[21];
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_SSID), wbuf, 21);
          x_wcstombs(ssid, wbuf, 21);
          
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_CIPHER), wbuf, 21);
          x_wcstombs(pwd, wbuf, 21);
          
					if(ESP8266_JoinAP(ssid, pwd))
            break;
          
					if(IsConnect == 0)
            wifistat=0xff;
//					++i;
				}
				if(i==TRYCOUNT)		wifistat|=0x04;
        
				//ʹ�õ�����ģʽ
				i=0;
				while((wifistat==0)&&(i<TRYCOUNT))
				{
					if(ESP8266_Enable_MultipleId(DISABLE))
            break;
					if((IsConnect==0))
            wifistat=0xff;
					++i;					
				}
				if(i==TRYCOUNT)	
          wifistat|=0x08;		
        
				//���ӷ�����
				i=0;
				while((wifistat==0)&&(i<TRYCOUNT))
				{
          WCHAR wbuf[21];
          WCHAR wlbuf[4][6];
          char  lbuf[4][6];
          
          /* �õ��ı� */
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP1), wlbuf[0], 6);
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP2), wlbuf[1], 6);
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP3), wlbuf[2], 6);
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP4), wlbuf[3], 6);
          x_wcstombs(lbuf[0], wlbuf[0], 6);
          x_wcstombs(lbuf[1], wlbuf[1], 6);
          x_wcstombs(lbuf[2], wlbuf[2], 6);
          x_wcstombs(lbuf[3], wlbuf[3], 6);
          
          /* ƴ����һ�� */
          x_sprintf(host_ip, "%s.%s.%s.%s", lbuf[0], lbuf[1], lbuf[2], lbuf[3]);
          
          GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), wbuf, 6);
          x_wcstombs(host_port, wbuf, 6);
          
					if(ESP8266_Link_Server(enumTCP,(char*)host_ip,(char*)host_port,Single_ID_0))
            break;
          
					if((IsConnect==0))
            wifistat=0xff;
          
					++i;					
				}
				if(i==TRYCOUNT)
          wifistat|=0x010;
        
				//ʹ��͸��ģʽ
				i=0;
				while((wifistat==0)&&(i<TRYCOUNT))
				{
					if(ESP8266_UnvarnishSend())
            break;	
          
					if((IsConnect==0))
            wifistat=0xff;
          
					++i;					
				}
				if(i==TRYCOUNT)	wifistat|=0x20;
				//printf("wifistat:%d\n",wifistat);
        
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;			
				IsChange=0;
        SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"������");
//				WM_EnableWindow(hFrameClose);
			}
			else if(timecount>=20)
			{
//				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "ͨ����");
//				WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
				//���յ�����
				if(strEsp8266_Fram_Record.InfBit.FramFinishFlag)
				{
					strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
					receive=strEsp8266_Fram_Record .Data_RX_BUF;
          
          uint32_t WinTexeLen = 0;
          WCHAR *wbuf;
          WCHAR *wbuf_rec;
          
          WinTexeLen = GetWindowTextLength(Receive_Handle) + 1;                       // ��ȡ�ı�����
          wbuf = (WCHAR *)GUI_VMEM_Alloc(WinTexeLen*sizeof(WCHAR) + strEsp8266_Fram_Record.InfBit.FramLength*sizeof(WCHAR));    // �����ı����� + ����Ϣ���ȵ��ڴ�
          wbuf_rec =  (WCHAR *)GUI_VMEM_Alloc((strEsp8266_Fram_Record.InfBit.FramLength + 1)*sizeof(WCHAR));    // �����ı����� + ����Ϣ���ȵ��ڴ�

          if (wbuf != NULL)
          {
            if (wbuf_rec != NULL)
            {
              GetWindowText(Receive_Handle, wbuf, WinTexeLen+1);                      // �õ�ԭ�ı�
              x_mbstowcs(wbuf_rec, receive, strEsp8266_Fram_Record.InfBit.FramLength*sizeof(WCHAR));
              x_wstrcat(wbuf, wbuf_rec);                                              // ׷�����ı�
              SetWindowText(Receive_Handle, wbuf);                                    // ���ý��մ��ڵ��ı�
              GUI_VMEM_Free(wbuf_rec);                                                // �ͷ�������ڴ�
            }
            
            GUI_VMEM_Free(wbuf);                                                // �ͷ�������ڴ�
          }
              
					// printf("text data:%s\n", receive);
					//�Զ����		
				}
				if(ucTcpClosedFlag)                                           //����Ƿ�ʧȥ����    
				{
					ESP8266_ExitUnvarnishSend ();                               //�˳�͸��ģʽ      
					do ucStatus = ESP8266_Get_LinkStatus ();                    //��ȡ����״̬      
					while(!ucStatus);
					if(ucStatus==4)                                             //ȷ��ʧȥ���Ӻ�����
					{	
						IsConnect=0;
						IsChange=0;
//						WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
//						WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
//						WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
//						WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
//						WM_DisableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));						
//						BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "δ����");	
            SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"����");  
            ucTcpClosedFlag = 0;
            wifistat=0;
					}				
				}
				strEsp8266_Fram_Record .InfBit .FramLength = 0;
				strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
				timecount=0;
			}
			if(wifistat)
			{
//				WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT3));
//				WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT2));
//				WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT1));
//				WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_EDIT0));
//				WM_DisableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));						
//				BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0), "δ����");	
				IsConnect=0;
				IsChange=0;
				if(wifistat!=0xff)
        {
            SetTimer(hwnd, 10, 10, TMR_START | TMR_SINGLE, NULL);    // ������ʱ����ʾ����ʧ��
				    IsChange=1;
        }
        
        SetWindowText(GetDlgItem(hwnd, eID_BTN_STATE), L"����");
        wifistat=0;
			}
		}
		timecount++;

		GUI_msleep(10); 
  }
}

//�˳���ť�ػ���
static void Ent_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
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

}

// �ػ���ͨ��ť
static void Ent_Button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
  WCHAR wbuf[128];
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;

  SetBrushColor(hdc, MapRGB(hdc, 0, 202, 252));
  EnableAntiAlias(hdc, ENABLE);
  FillRoundRect(hdc, &rc, 5);
  EnableAntiAlias(hdc, DISABLE);

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
    OffsetRect(&rc, 1, 1);
		SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
	}
	else
	{ //��ť�ǵ���״̬

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));      //���û���ɫ
	}

  GetWindowText(hwnd, wbuf, 128);    // �õ�ԭ�ı�

  SetFont(hdc, defaultFont);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);     // ��������(������ʾ)
}

/*
 * @brief  �ػ���ʾ���ȵ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void edit_textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;
  
  SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
  FillRect(hdc, &rc);

  SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));

  GetWindowText(hwnd, wbuf, 128);    // �õ�ԭ�ı�

  SetFont(hdc, defaultFont);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);     // ��������( �������룬����룬���ַ��������α߽�ʱ,�Զ�����)
  HLine(hdc, rc.x, rc.y + rc.h - 1, rc.w);

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
	WCHAR *wbuf;
  uint32_t WinTexeLen = 0;

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  rc = ds->rc;

  SetTextColor(hdc, MapRGB(hdc, 170, 170, 170));

  if (ds->ID == ID_TEXTBOX_Receive)
  {
    DrawText(hdc, L"������", -1, &rc, DT_VCENTER|DT_CENTER);  // ��������
  }
  else
  {
    SetPenColor(hdc, MapRGB(hdc, 121, 121, 121));

    EnableAntiAlias(hdc, ENABLE);
    DrawRoundRect(hdc, &rc, 7);
    EnableAntiAlias(hdc, DISABLE);
    DrawText(hdc, L"������", -1, &rc, DT_VCENTER|DT_CENTER);  // ��������
  }
  
  OffsetRect(&rc, 2, 2);
  SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
  WinTexeLen = GetWindowTextLength(hwnd) + 1;               // ��ȡ�ı�����

  if (WinTexeLen > 0)
  {
    wbuf = (WCHAR *)GUI_VMEM_Alloc(WinTexeLen*sizeof(WCHAR));         // �����ı����� + ����Ϣ���ȵ��ڴ�
    
    if (wbuf != NULL)
    {
      GetWindowText(hwnd, wbuf, WinTexeLen+1);            // �õ�ԭ�ı�

      SetFont(hdc, defaultFont);
      DrawText(hdc, wbuf, -1, &rc, DT_TOP|DT_LEFT|DT_WORDBREAK);     // ��������( �������룬����룬���ַ��������α߽�ʱ,�Զ�����)
      
      GUI_VMEM_Free(wbuf);        // �ͷ�������ڴ�
    }
  }
}

extern void TCP_Echo_Init(void);
extern int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);
static WCHAR I[128];

static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      //����Camera
      DisableCamera();
      HWND Temp_Handle;
      
      xTaskCreate((TaskFunction_t )wifi_dispose_task,      /* ������ں��� */
                  (const char*    )"wifi dispose task",    /* �������� */
                  (uint16_t       )3*1024/4,               /* ����ջ��СFreeRTOS������ջ����Ϊ��λ */
                  (void*          )hwnd,                   /* ������ں������� */
                  (UBaseType_t    )5,                      /* ��������ȼ� */
                  (TaskHandle_t*  )&wifi_task_handle);     /* ������ƿ�ָ�� */
                      
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  286, 4, 23, 23, hwnd, eID_BTN_EXIT, NULL, NULL); 

      rc.x = 262;
      rc.y = 99;
      rc.w = 54;
      rc.h = 22;
      
      CreateWindow(BUTTON, L"����", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|WS_OWNERDRAW,
                  rc.x,rc.y,rc.w,rc.h, hwnd, eID_BTN_STATE, NULL, NULL);
      
      /* ���ݷ����ı����� */
      rc.w = 158;
      rc.h = 117;
      rc.x = 160;
      rc.y = 122;
      CreateWindow(TEXTBOX, L"���!������Ұ�𿪷���. �����޸�IP�Ȳ���,���ڳ������޸� �������Ÿ���,����������ͷģ���,�����Ӳ��ɹ������,���Ƴ�����ͷ����", WS_TRANSPARENT | WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Send, NULL, NULL);

      /* �������մ��� */
      rc.w = 157;
      rc.h = 194;
      rc.x = 1;
      rc.y = 26;
      Receive_Handle = CreateWindow(TEXTBOX, L"", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Receive, NULL, NULL);
      
      rc.x = 281;
      rc.y = 217;
      rc.w = 35;
      rc.h = 20;
      CreateWindow(BUTTON, L"����", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|WS_OWNERDRAW,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_BTN_SEND, NULL, NULL); 
                         
      rc.x = 101;
      rc.h = 20;
      rc.w = 55;
      rc.y = 217;
      CreateWindow(BUTTON, L"��ս���", WS_TRANSPARENT | BS_NOTIFY|WS_VISIBLE|WS_OWNERDRAW,
                         rc.x,rc.y,rc.w,rc.h, hwnd, eID_BTN_CLEAR, NULL, NULL); 
                         
      rc.w = 120;
      rc.h = 24;
      rc.x = 199;
      rc.y = 25;
      Temp_Handle = CreateWindow(TEXTBOX, L"embedfire2", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_SSID, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);
      
      OffsetRect(&rc, 0, rc.h);
      Temp_Handle = CreateWindow(TEXTBOX, L"wildfire", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_CIPHER, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      /* IP&�˿��ı����� */
      rc.w = 28;
      rc.h = 24;
      rc.x = 199;
      rc.y = 74;
      Temp_Handle = CreateWindow(TEXTBOX, L"192", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP1, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+3, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"168", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP2, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+3, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"000", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP3, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      OffsetRect(&rc, rc.w+3, 0);
      Temp_Handle = CreateWindow(TEXTBOX, L"130", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemoteIP4, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);

      rc.w = 55;
      rc.h = 22;
      rc.x = 199;
      rc.y = 99;
      Temp_Handle = CreateWindow(TEXTBOX, L"8080", WS_VISIBLE|WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_RemotePort, NULL, NULL);//
      SendMessage(Temp_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);



      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // ��ʱ�� ID

      if (tmr_id == 10)    // �����Ӵ���
      {
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] ={L"ȷ��",L"ȡ��"};      //�Ի����ڰ�ť������

        ops.Flag =MB_ICONERROR;
        ops.pButtonText =btn;
        ops.ButtonCount =2;
        RC.w = 160;
        RC.h = 120;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"����ʧ��\n�������ӡ�", L"����", &ops);    // ��ʾ������ʾ��                                                       // ���͹رմ��ڵ���Ϣ
      }
      
      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      
      RECT rc =  {0, 0, GUI_XSIZE, GUI_YSIZE};
      // hdc_pointer = CreateMemoryDC(SURF_SCREEN, PANEL_W, PANEL_H);
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
      FillRect(hdc, &rc);

      rc.h = 25;
      GradientFillRect(hdc, &rc, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);
      SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
      DrawText(hdc, L"Wi-Fi", -1, &rc, DT_VCENTER|DT_CENTER);

      SetPenColor(hdc, MapRGB(hdc, 121, 121, 121));

      rc.x = 1;
      rc.y = 26;
      rc.w = 157;
      rc.h = 213;
      EnableAntiAlias(hdc, ENABLE);
      DrawRoundRect(hdc, &rc, 5);     // ���ƽ����������
      EnableAntiAlias(hdc, DISABLE);
      
      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 0x16, 0x9B, 0xD5));
      
      rc.w = 38;
      rc.h = 24;
      rc.x = 160;
      rc.y = 26;
      DrawText(hdc, L"Wi-Fi:", -1, &rc, DT_RIGHT|DT_TOP);
      
      rc.y = 50;
      DrawText(hdc, L"����:", -1, &rc, DT_RIGHT|DT_TOP);
      

      rc.y = 74;
      DrawText(hdc, L"IP:", -1, &rc, DT_RIGHT|DT_TOP);

      rc.y = 98;
      DrawText(hdc, L"�˿�:", -1, &rc, DT_RIGHT|DT_TOP);
      
      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_BTN_EXIT:
          {
            Ent_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_TEXTBOX_Send:
          case ID_TEXTBOX_Receive:
          {
            Brigh_Textbox_OwnerDraw(ds);
            return TRUE;   
          }

          case eID_BTN_STATE:
          case eID_BTN_SEND:
          case eID_BTN_CLEAR:
          {
            Ent_Button_OwnerDraw(ds);
            return TRUE;   
          }
          
          case ID_TEXTBOX_SSID:
          case ID_TEXTBOX_CIPHER:
          case ID_TEXTBOX_RemoteIP1:
          case ID_TEXTBOX_RemoteIP2:
          case ID_TEXTBOX_RemoteIP3:
          case ID_TEXTBOX_RemoteIP4:
          case ID_TEXTBOX_RemotePort:
          {
            edit_textbox_OwnerDraw(ds);
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
      if(code == BN_CLICKED && id == eID_BTN_EXIT)    // �˳���ť����
      {
        PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
        break;
      }
      if(code == BN_CLICKED && id == eID_BTN_CLEAR)    // ��ս��յĴ��ڵİ�ť������
      {
        SetWindowText(Receive_Handle, L"");
        break;
      }
#if 0
      if(code == TBN_CLICKED && id == ID_TEXTBOX_SSID)    // IP1 �༭�򱻰���
      {
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"SSID", I, 15, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_SSID), I);
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_CIPHER)    // IP1 �༭�򱻰���
      {
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"CIPHER", I, 15, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_CIPHER), I);
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP1)    // IP1 �༭�򱻰���
      {
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP1", I, 3, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP1), I);
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP2){    // IP2 �༭�򱻰���
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP2", I, 3, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP2), I);
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP3){    // IP3 �༭�򱻰���
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP3", I, 3, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP3), I);
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemoteIP4){    // IP4 �༭�򱻰���
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"IP4", I, 3, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemoteIP4), I);
        break;
      }
      
      if(code == TBN_CLICKED && id == ID_TEXTBOX_RemotePort)    // �˿� �༭�򱻰���
      {
        number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"PORT", I, 5, hwnd);
        SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_RemotePort), I);
        break;
      }
#endif
      if(code == BN_CLICKED && id == eID_BTN_STATE)
      {
        if (!IsConnect)
          IsConnect = 1;
        else
          ucTcpClosedFlag = 1;
      }
      
      if(code == BN_CLICKED && id == eID_BTN_SEND)
      {
        WCHAR wbuf[128];
        char comdata[128];

        /* ��ȡ���ʹ��ڵ��ַ��� */
        GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_Send), wbuf, 128);
        x_wcstombs(comdata, wbuf, 128);
        
        ESP8266_SendString(ENABLE, comdata, 0, Single_ID_0 ); 
      }

      break;
    } 

    case WM_DESTROY:
    { 
      IsConnect=0;//�ر�����
      vTaskDelete(wifi_task_handle);
      ESP8266_stop();

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void gui_wifi_dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND hwnd;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"gui wifi dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);

   //��ʾ������
	ShowWindow(hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


