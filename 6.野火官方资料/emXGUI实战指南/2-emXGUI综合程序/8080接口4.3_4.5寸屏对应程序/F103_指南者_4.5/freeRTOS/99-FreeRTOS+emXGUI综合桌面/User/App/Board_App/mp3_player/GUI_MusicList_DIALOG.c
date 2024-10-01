#include "emXGUI.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./mp3_player/Backend_musiclist.h"
#include "x_libc.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"
#include <stdlib.h>
#include <string.h>
/******************����IDֵ********************/
#define ID_BUTTON_EXIT 0x2000  
/******************�б�IDֵ********************/
#define ID_LISTBOX1    0x2100
#define ID_LISTBOX2    0x2101

#define ID_LIST_1             0x2200
//#define ICON_VIEWER_ID_PREV   0x2201
//#define ICON_VIEWER_ID_NEXT   0x2202

#define ID_EXIT        0x3000

/**********************����****************************/
char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];//����List
//char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//��ʾlist
uint8_t  music_file_num = 0;//�ļ�����
int play_index = 0;   //���Ÿ����ı��ֵ


//static BITMAP bm;//λͼ�ṹ��

/*******************�ؼ��ػ����************************/
//����
static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[64];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
	FillRect(hdc, &rc); //�þ�����䱳��

	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}
  HFONT controlFont_48;
  controlFont_48 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_48);
	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_48);

	GetWindowText(ds->hwnd, wbuf, 64); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
	DeleteFont(controlFont_48);
}
static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[64];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

   
   
   
	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
   
   FillCircle(hdc, rc.x, rc.y, rc.w);
   SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
	FillRect(hdc, &rc); //�þ�����䱳��

	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}
  HFONT controlFont_48;
  controlFont_48 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_48);	
	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_48);

	GetWindowText(ds->hwnd, wbuf, 64); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)
   rc.x = 35; 
//   rc.y = 20;
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
   DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
	DeleteFont(controlFont_48);
}

HWND music_list_hwnd;
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

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	rc =ds->rc;

	SetBrushColor(hdc,MapRGB(hdc,83,98,181));
	FillRect(hdc,&ds->rc);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* �б�Ϊ�գ���ʾ��ʾ��ϢȻ��ֱ�ӷ��� */
    DrawText(hdc, L"û���ҵ������ļ�\r\n����SD����", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=play_index;

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}

		SetTextColor(hdc,MapRGB(hdc,50,10,10));

		if(i==cursel)
		{
			SetTextColor(hdc,MapRGB(hdc,2,167,240));
		}
		else
		{
			SetTextColor(hdc,MapRGB(hdc,255,255,255));
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
    
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);      // ��ʾ�绰����
    
    SetPenColor(hdc,MapRGB(hdc,10,10,10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                  // ��һ����

		i++;
	}
}

static LRESULT Win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_CREATE:
      {
         
         HWND wnd;
	   	   RECT rc;
         GetClientRect(hwnd, &rc);

         wnd = CreateWindow(LISTBOX,
                      L"ListMenu1",
                      WS_VISIBLE | WS_OWNERDRAW | LBS_NOTIFY,
                      10, 36, 359, 294,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      NULL);         
         SendMessage(wnd, LB_SETCURSEL, play_index, 0);
         WCHAR wbuf1[128];
         for (uint16_t xC=0; xC<music_file_num; xC++)
         {
           x_mbstowcs_cp936(wbuf1, &music_playlist[xC][11], FILE_NAME_LEN);
           SendMessage(wnd, LB_ADDSTRING, -1, (LPARAM)wbuf1);
         }       
     
         break;
      } 
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;
  
         if (ds->ID == ID_LIST_1)
         {
           listbox_owner_draw(ds);
           return TRUE;
         }
         
         if(ds->ID == ID_EXIT)
            exit_owner_draw(ds);
         else
            button_owner_draw(ds); //ִ���Ի��ư�ť
         return TRUE;

      }    
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;
         RECT rc;//���ڴ�С
         GetClientRect(hwnd, &rc); //��ÿͻ�������.
         hdc = BeginPaint(hwnd, &ps);
         //����
         SetBrushColor(hdc, MapRGB(hdc, 83,98,181));
         FillRoundRect(hdc, &rc, 10);
         rc.x += rc.w/2;
         rc.w /= 2;
         FillRect(hdc, &rc);
         //DrawBitmap(hdc,0,0,&bm_0,NULL);   
         rc.x = 0;
         rc.y = 0;
         rc.w = 379;
         rc.h = 35;
         SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
         DrawText(hdc, L"�����б�", -1, &rc, DT_VCENTER|DT_CENTER);

         EndPaint(hwnd, &ps);
         break;
      }  

      case WM_ERASEBKGND:
      {
        return FALSE;
      }

      case WM_NOTIFY:
      {
         u16 code, id;	
         code = HIWORD(wParam);
         id = LOWORD(wParam); 


         if (code == LBN_SELCHANGE)
         {
            switch (id)
            {
               case ID_LIST_1:
                 {
                     play_index = SendMessage(GetDlgItem(hwnd, ID_LIST_1), LB_GETCURSEL,0,0);               // ��õ�ǰѡ����;//�л�����һ��
										 Music_State = STA_SWITCH;   
               }

               break;
            }

         }

         break;
      }      
      case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{         
         enter_flag = 0;
         SetForegroundWindow(MusicPlayer_hwnd);
			return DestroyWindow(hwnd); //����DestroyWindow�������ٴ��ڣ��ú�����ʹ�����ڽ������˳���Ϣѭ��;���򴰿ڽ���������.
		} 
    //�رմ�����Ϣ����case
      case WM_DESTROY:
      {               
        return PostQuitMessage(hwnd);	    // �˳���Ϣѭ��
      }
    
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam); 
   }
   return WM_NULL;
} 

void GUI_MusicList_DIALOG(HWND hwnd)
{
	WNDCLASS	wcex;

	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	music_list_hwnd = CreateWindow(//WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//|WS_EX_LOCKZORDER
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
                         421, 60, 379, 335,
                         hwnd, NULL, NULL, NULL);
	//��ʾ������
}
