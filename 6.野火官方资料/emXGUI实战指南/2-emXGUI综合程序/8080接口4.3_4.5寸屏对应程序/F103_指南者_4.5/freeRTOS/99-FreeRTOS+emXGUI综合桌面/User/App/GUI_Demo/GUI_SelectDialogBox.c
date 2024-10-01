
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/


static uint8_t ButtonNum = 0;    // �����û����µİ���
static const WCHAR *pTextInt;
static const WCHAR *pCaptionInt;
static const MSGBOX_OPTIONS *opsInt;

static void DialogBoxButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
  
  if(ds->State&BST_PUSHED)
	{
    /* ������������ */
    SetBrushColor(hdc,MapRGB(hdc, 200, 200, 200));
	}
	else
	{
    /* ������������ */
    SetBrushColor(hdc,MapRGB(hdc, 250, 250, 250));
	}
  
  FillRoundRect(hdc, &rc, 7);
    
  SetPenColor(hdc,MapRGB(hdc, 200, 200, 200));    /* ����ɫ���� */
  HLine(hdc, rc.x, rc.y, rc.x+rc.w);
  if (ds->ID < opsInt->ButtonCount - 1)    /* ���һ����ť�����ұߵ�һ���� */
  {
    VLine(hdc, rc.x+rc.w-1, rc.y, rc.y+rc.h);
  }
  
  SetPenColor(hdc,MapRGB(hdc, 105, 105, 105));           // ���û��ʵ���ɫ
  GetWindowText(ds->hwnd, wbuf, 128);                    // ��ð�ť�ؼ�������
//  OffsetRect(&rc, 0, -3);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
}

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    /* ���ڴ��� */
    case WM_CREATE:
    {
      RECT rc;
      RECT rc_button;
//      int FontH;
//      FontH = GetFontAveHeight(defaultFont);
      GetClientRect(hwnd, &rc);

      for(uint8_t xC=0; xC<opsInt->ButtonCount; xC++)
      {
        /* ���㰴��λ�� ���߶�Ϊ���ڵ��ķ�֮һ�� */
        rc_button.w = rc.w / opsInt->ButtonCount;    // ��ť�Ŀ��
        rc_button.h = rc.h / 4;                      // ���ڸ߶ȵ� 1/4
        rc_button.x = rc_button.w * xC;
        rc_button.y = rc.h - rc_button.h;
        
        /* ����һ����ť */
        CreateWindow(BUTTON, opsInt->pButtonText[xC], BS_FLAT|WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, //
                     rc_button.x, rc_button.y, rc_button.w, rc_button.h, hwnd, xC, NULL, NULL);
      }
      
      break; 
    }
    
    case WM_ERASEBKGND:
    {
      return TRUE;    // ֱ�ӷ��� TRUE ��ⲻ�ửĬ�ϱ���
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      RECT rc;
      RECT rc_Text;

      GetClientRect(hwnd,&rc);
      hdc = BeginPaint(hwnd,&ps);
      
//      SetBrushColor(hdc,MapRGB(hdc,50,0,0));
//      FillRect(hdc, &rc);
      SetBrushColor(hdc, MapRGB(hdc,250,250,250));
      FillRoundRect(hdc, &rc, 7);
      
      /* ������Ϣ���͵���ʾλ�� */
      rc_Text.w = rc.w;
      rc_Text.h = rc.h / 4;
      rc_Text.x = rc.x;
      rc_Text.y = rc.y;
      
      /* ��ʾ��Ϣ���� */
      DrawText(hdc, pCaptionInt, -1, &rc_Text, DT_VCENTER|DT_CENTER);
      
      /* ������Ϣ���͵���ʾλ�� */
      rc_Text.w = rc.w;
      rc_Text.y = rc.y + rc_Text.h;
      rc_Text.h = rc.h / 4 * 2;
      rc_Text.x = rc.x;
      
      /* ��ʾ��Ϣ���� */
      DrawText(hdc, pTextInt, -1, &rc_Text, DT_VCENTER|DT_CENTER);
      
      EndPaint(hwnd,&ps);
      break;
    }
    
    case WM_NOTIFY: 
    {
      u16 code,  id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������

      if(code == BN_CLICKED)
      {
        ButtonNum =  id;
        
        PostCloseMessage(hwnd);
      }
      break;
     }
    
     case WM_DRAWITEM:
     {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       
       DialogBoxButton_OwnerDraw(ds);
       
       return TRUE;
     }
    
    case WM_DESTROY:
    {
      return PostQuitMessage(hwnd);
    }
    
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  
  return WM_NULL;
}

int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops)
{
  WNDCLASS	wcex;

  if(hwndParent==NULL)
	{
		hwndParent =GetDesktopWindow();
	}
  
  /* ��ʼ���ڲ����� */
  ButtonNum = 0;
  pTextInt = pText;
  pCaptionInt = pCaption;
  opsInt = ops;
  
	wcex.Tag = WNDCLASS_TAG;  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	DialogWindow(	&wcex,	pCaption,
//							WS_OVERLAPPED|WS_BORDER|WS_DLGFRAME|WS_CLIPCHILDREN,
                WS_CLIPCHILDREN,//WS_CAPTIONWS_BORDER|
							rc.x, rc.y, rc.w, rc.h,
							hwndParent, 0x0000, NULL, NULL);//hInst

// 	//��ʾ������
// 	ShowWindow(hwnd, SW_SHOW);
  
  return ButtonNum;
}

