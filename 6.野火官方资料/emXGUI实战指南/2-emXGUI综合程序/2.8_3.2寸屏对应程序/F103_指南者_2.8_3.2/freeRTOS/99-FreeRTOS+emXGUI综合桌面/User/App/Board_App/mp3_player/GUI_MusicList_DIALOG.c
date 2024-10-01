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
char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//��ʾlist
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
	WCHAR wbuf[128];

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
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //�þ�����䱳��
	//	DrawRect(hdc,&rc); //���������
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //�þ�����䱳��FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //���������

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_32);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}
static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

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
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //�������ɫ(BrushColor��������Fill���͵Ļ�ͼ����)
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //���û���ɫ(PenColor��������Draw���͵Ļ�ͼ����)
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //��������ɫ
	}
	else
	{ //��ť�ǵ���״̬
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //�þ�����䱳��
	//	DrawRect(hdc,&rc); //���������
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //�þ�����䱳��FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //���������

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_16);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)
   rc.x = 35; 
//   rc.y = 20;
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
   DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
}
//LIST


static LRESULT Win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static struct __obj_list *menu_list = NULL;
	static WCHAR (*wbuf)[20];//��������
   switch(msg)
   {
      case WM_CREATE:
      {
         
         HWND wnd;
         int i = 0;
         list_menu_cfg_t cfg;
	   	   RECT rc;
         GetClientRect(hwnd, &rc);
         /* ��Ҫ����N+1����һ��Ϊ�� */
        GUI_DEBUG("����%d\n", music_file_num);
         menu_list = (struct __obj_list *)GUI_VMEM_Alloc(sizeof(struct __obj_list)*(music_file_num+1));
				wbuf = (WCHAR (*)[20])GUI_VMEM_Alloc(sizeof(WCHAR) * music_file_num * 36);//��Ÿ�����������
         if(menu_list == NULL) 
            return 0;
         for(;i < music_file_num; i++){
            //printf("%s\n", lcdlist[i]);
            
            
         char p[128] ;
         strcpy(p, music_lcdlist[i]);
         printf("%s\n",p);
         int t, L;
         L = (int)strlen(p);
         if (L > 13)
         {
            for (t = L; t > 13; t --)
            {
               p[t] = p[t - 1];
            }
            p[13] = '\0';
            p[L + 1] = '\0';
         }            
            
            
            x_mbstowcs(wbuf[i], p, FILE_NAME_LEN);
            menu_list[i].pName = wbuf[i];
            menu_list[i].cbStartup = NULL;
            menu_list[i].icon = L"a";
            menu_list[i].bmp = NULL;
            menu_list[i].color = RGB_WHITE;
         } 
          /* ���һ��Ϊ�� */
          menu_list[i].pName = NULL;
          menu_list[i].cbStartup = NULL;
          menu_list[i].icon = NULL;
          menu_list[i].bmp = NULL;
          menu_list[i].color = NULL;         
         
         cfg.list_objs = menu_list; 
         cfg.x_num = 3;
         cfg.y_num = 2; 
         cfg.bg_color = 0;
         wnd = CreateWindow(&wcex_ListMenu,
                      L"ListMenu1",
                      WS_VISIBLE | LMS_ICONFRAME|LMS_PAGEMOVE,
                      rc.x + 30, rc.y + 20, rc.w - 60, rc.h-20,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      &cfg);         
         SendMessage(wnd, MSG_SET_SEL, play_index, 0);
         wnd= CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                       2, (rc.h) / 2, 30, 30, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
         SetWindowFont(wnd, controlFont_48); 
	      wnd = CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
        rc.w - 30, (rc.h ) / 2, 30, 30, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
         SetWindowFont(wnd, controlFont_16);    
         
         CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_OWNERDRAW |WS_VISIBLE,
                        0, 0, 35, 30, hwnd, ID_EXIT, NULL, NULL);         
         
         
         break;
      } 
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;
  
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
         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
         FillRect(hdc, &rc);  
         //DrawBitmap(hdc,0,0,&bm_0,NULL);   
         rc.x = 0;
         rc.y = 0;
         rc.w = 300;
         rc.h = 20;
         SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
         DrawText(hdc, L"�����б�", -1, &rc, DT_VCENTER|DT_CENTER);
         EndPaint(hwnd, &ps);
         break;
      }  
      case WM_NOTIFY:
      {
         u16 code, id;	
         LM_NMHDR *nm;
         code = HIWORD(wParam);
         id = LOWORD(wParam); 

         nm = (LM_NMHDR*)lParam;

         if (code == LMN_CLICKED)
         {
            switch (id)
            {
               case ID_LIST_1:{
                     play_index = nm->idx;//�л�����һ��
                     Music_State = STA_SWITCH;	                  
//                  Play_index = ;
//                  sw_flag = 1;
                  //PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
                  //menu_list_1[nm->idx].cbStartup(hwnd);
               }

               break;
            }

         }

      
         if (code == BN_CLICKED && id == ICON_VIEWER_ID_PREV)
         {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_PREV, TRUE, 0);
         }
         ////
         if (code == BN_CLICKED && id == ICON_VIEWER_ID_NEXT)
         {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_NEXT, TRUE, 0);
         }         
         if (code == BN_CLICKED && id == ID_EXIT)
         {
           
            PostCloseMessage(hwnd);
         }   
         break;
      }      
      case WM_CLOSE: //���ڹر�ʱ�����Զ���������Ϣ.
		{         
         GUI_VMEM_Free(menu_list);
         GUI_VMEM_Free(wbuf);
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


void GUI_MusicList_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
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
