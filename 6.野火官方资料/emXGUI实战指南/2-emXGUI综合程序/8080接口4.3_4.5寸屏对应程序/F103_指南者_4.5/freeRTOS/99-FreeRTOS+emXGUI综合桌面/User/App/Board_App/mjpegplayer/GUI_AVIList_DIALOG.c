#include "emXGUI.h"
#include "ff.h"
#include "emXGUI_JPEG.h"
#include "./mjpegplayer/GUI_AVIList_DIALOG.h"
#include "./mjpegplayer/GUI_AVIPLAYER_DIALOG.h"
#include "string.h"
#include	"CListMenu.h"
#include "x_libc.h"
#include <stdlib.h>
#include "GUI_AppDef.h"

#define ID_EXIT 0x3000
#define ID_HOME 0x3001
/**********************����****************************/
char avi_playlist[FILE_MAX_NUM][FILE_NAME_LEN];//����List
char lcdlist_wnd[FILE_MAX_NUM][FILE_NAME_LEN];//��ʾlist
static char lcdlist[FILE_MAX_NUM][FILE_NAME_LEN];//��ʾlist
uint8_t  avi_file_num = 0;//�ļ�����
uint8_t  file_nums = 0;
static char path[100]="0:";//�ļ���Ŀ??
COLORREF color_bg_list;
int flag = 0;//ֻɨ��һ���ļ�Ŀ¼
int Play_index = 0;
extern HWND	VideoPlayer_hwnd;
BOOL player_state = TRUE;
#define EmptyFile   WM_USER+1
extern int LIST_STATE;
int sw_flag;//�л���־
/**
  * @brief  scan_files �ݹ�ɨ��sd���ڵ���Ƶ��??
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���??
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//���ļ���֧��
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //��Ŀ¼
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//��ȡĿ¼�µ�����
     if (res != FR_OK || fno.fname[0] == 0) break; 	//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if(strstr(path,"recorder")!=NULL)continue;       //�ӹ�¼���ļ�
      if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							//Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼??
        res = scan_files(path);											//�ݹ���� 
        if (res != FR_OK) 
					break; 																		//��ʧ�ܣ�����ѭ??
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s%s\r\n", path, fn);								//����ļ�??
				if(strstr(fn,".avi")||strstr(fn,".AVI"))//�ж��Ƿ�AVI�ļ�
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(avi_file_num<FILE_MAX_NUM)&&flag == 0)
					{
						sprintf(file_name, "%s/%s", path, fn);
						memcpy(avi_playlist[avi_file_num],file_name,strlen(file_name));
            memcpy(lcdlist_wnd[avi_file_num],fn,strlen(fn));
						memcpy(lcdlist[avi_file_num],fn,strlen(fn));						
						//memcpy(lcdlist1[avi_file_num],fn,strlen(fn));lcdlist_wnd
					}
               avi_file_num++;//��¼�ļ�����
				}//if 
      }//else
     } //for
  } 
  return res; 
}






void Insert(char a, int pos, char *str)
{
    int i, L;
    L = (int)strlen(str);
    if (L < pos)
    {
        return;
    }
    else
    {
        for (i = L; i > pos; i --)
        {
            str[i] = str[i - 1];
        }
        str[pos] = a;
        str[L + 1] = '\0';
    }

}

BOOL Player_Init(void)
{
   int i = 0;
   scan_files(path);
   if(!flag){
      flag = 1;
      for(; i < avi_file_num; i++)
      {
         Insert('\0', 13, lcdlist[i]);
      }    
   }
   return 0;
}


//static HDC hdc_bk;//����ͼ��
static void button_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];
   
   //HDC hdc_tmp;
   
	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
   
   //hdc_tmp = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);


   SetBrushColor(hdc, MapRGB(hdc, 54,54,54));

   FillRect(hdc, &rc); //�þ�����䱳��
	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
	}
	else if(ds->State & BST_PUSHED)
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
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //�þ�����䱳��
	//	DrawRect(hdc,&rc); //���������
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //�þ�����䱳��FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //���������

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_72);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));
      
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//��������(���ж��뷽ʽ)

   //BitBlt(hdc, 0, 0, rc.w, rc.h, hdc_tmp, 0, 0, SRCCOPY);

   //DeleteDC(hdc_tmp);

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
	SetFont(hdc, controlFont_48);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//��������(���ж��뷽ʽ)
   rc.x = 35; 
//   rc.y = 20;
  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
   DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
}
static void home_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
   
   FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
	//FillRect(hdc, &rc); //�þ�����䱳��

   if (ds->State & BST_PUSHED)
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

	  /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, controlFont_64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
   rc.y = -10;
   rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)


  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);

}

LRESULT list_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static struct __obj_list *menu_list = NULL;
   static WCHAR (*wbuf)[128];
   static BOOL res;
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
         menu_list = (struct __obj_list *)GUI_VMEM_Alloc(sizeof(struct __obj_list)*(avi_file_num+1));
         wbuf = (WCHAR (*)[128])GUI_VMEM_Alloc(sizeof(WCHAR *)*avi_file_num);
         if(menu_list == NULL || wbuf == NULL)
          PostMessage(hwnd, EmptyFile, NULL, NULL); 
         for(;i < avi_file_num; i++){
//            printf("%s\n", lcdlist[i]);
            x_mbstowcs_cp936(wbuf[i], lcdlist[i], FILE_NAME_LEN);
            menu_list[i].pName = wbuf[i];
            menu_list[i].cbStartup = NULL;
            menu_list[i].icon = L"K";
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
         cfg.bg_color = 0x363636;
         wnd = CreateWindow(&wcex_ListMenu,
                      L"ListMenu1",
                      WS_VISIBLE | LMS_ICONFRAME|LMS_PAGEMOVE,
                      rc.x + 100, rc.y + 80, rc.w - 200, rc.h-80,
                      hwnd,
                      ID_LIST_1,
                      NULL,
                      &cfg);         
        SendMessage(wnd, MSG_SET_SEL, Play_index, 0); 
        wnd= CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                        0, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
         SetWindowFont(wnd, controlFont_48); 
	      wnd = CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW| WS_VISIBLE,
		        	rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
         SetWindowFont(wnd, controlFont_48);
         
//         CreateWindow(BUTTON, L"Q", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
//			10, 5, 70, 70, hwnd, ICON_VIEWER_ID_LIST, NULL, NULL);         
 
         CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_OWNERDRAW |WS_VISIBLE,
                           0, 0, 240, 80, hwnd, ID_EXIT, NULL, NULL);   

         #if 0
         u8 *jpeg_buf;
         u32 jpeg_size;
         JPG_DEC *dec;
         res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
         hdc_bk = CreateMemoryDC(SURF_SCREEN, 800, 480);
         if(res)
         {
            /* ����ͼƬ���ݴ���JPG_DEC��� */
            dec = JPG_Open(jpeg_buf, jpeg_size);

            /* �������ڴ���� */
            JPG_Draw(hdc_bk, 0, 0, dec);

            /* �ر�JPG_DEC��� */
            JPG_Close(dec);
         }
         /* �ͷ�ͼƬ���ݿռ� */
         RES_Release_Content((char **)&jpeg_buf);      
         #endif
         break;
      }
         
      case WM_ERASEBKGND:
      {
         HDC hdc = (HDC)wParam;
         RECT rc_top  = {0, 0, 800, 80};
         RECT rc_text = {200, 0, 400, 80};
         RECT rc_cli =*(RECT*)lParam;
         
         //hdc_mem = CreateMemoryDC(SURF_ARGB4444, rc_cli.w, rc_cli.h);
         SetBrushColor(hdc, MapRGB(hdc, 54, 54, 54));
         FillRect(hdc, &rc_cli);
//         if(res!=FALSE)
//            BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h,
//                   hdc_bk, rc_cli.x, rc_cli.y, SRCCOPY);         
         
         SetBrushColor(hdc, MapRGB(hdc,0, 0, 0));
         FillRect(hdc, &rc_top);
         SetFont(hdc, defaultFont);
         SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
         DrawText(hdc, L"�����б�", -1, &rc_text, DT_SINGLELINE| DT_CENTER | DT_VCENTER);

            
//         StretchBlt(hdc, 755, 12, 40, 40, 
//                    hdc_mem1, 0, 0, 72, 72, SRCCOPY);
         
         color_bg_list = GetPixel(hdc, 700, 0);
         
         break;
      }
      case EmptyFile:
      {
        int ret=1;
        const WCHAR *btn[] ={L"OK"};
        MSGBOX_OPTIONS mb;
        mb.Flag =MB_BTN_WIDTH(80)|MB_ICONWARNING;
        mb.pButtonText =btn;
        mb.ButtonCount=1;
        while(ret)
        {
          ret=MessageBox(hwnd,240,200,320,150,L"�Ҳ�����Ƶ�ļ�,\r\n����SD��������!",L"Warning",&mb);          
        }  
        player_state = FALSE;
        PostCloseMessage(hwnd);
        //MessageBox(hwnd,100,200,200,120,L"Hello!",L"MsgBox-1",NULL); 
        break;
      }
      case WM_DRAWITEM:
      {

         DRAWITEM_HDR *ds;

         ds = (DRAWITEM_HDR*)lParam;
         if(ds->ID == ID_EXIT)
            exit_owner_draw(ds); //ִ���Ի��ư�ť
         else if(ds->ID == ICON_VIEWER_ID_NEXT || ds->ID == ICON_VIEWER_ID_PREV)
            button_owner_draw(ds);
         else if(ds->ID == ID_HOME )
            home_owner_draw(ds);
         return TRUE;

      }
//      case WM_LBUTTONDOWN:
//      {
//         POINT pt;
//         pt.x =GET_LPARAM_X(lParam); //���X����
//         pt.y =GET_LPARAM_Y(lParam); //���Y����
//         RECT rc = {0, 0, 80, 80};
//         if(PtInRect(&rc, &pt))
//         {
//            PostCloseMessage(hwnd);
//            //����WM_CLOSE��Ϣ�ر�������
//         }
//         break;         
//      }
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
                  
                  Play_index = nm->idx;
                  sw_flag = 1;
                  PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
                  //menu_list_1[nm->idx].cbStartup(hwnd);
               }
               case ICON_VIEWER_ID_LIST:
               {
                  PostCloseMessage(hwnd); //����WM_CLOSE��Ϣ�ر�������
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
         if (code == BN_CLICKED && (id == ID_EXIT || id == ID_HOME))
         {
            PostCloseMessage(hwnd);
         }

         break;
      }
      case WM_CLOSE:
      {
         GUI_VMEM_Free(menu_list);
         GUI_VMEM_Free(wbuf);
         file_nums = avi_file_num;
         res = FALSE;
         LIST_STATE = 0;
         //player_state = TRUE;
         //DeleteDC(hdc_bk);
         SetForegroundWindow(VideoPlayer_hwnd);//����ǰ̨����ΪMusicPlayer_hwnd������Ļ��ᴥ���ػ�
         //DestroyWindow(hwnd);
         return DestroyWindow(hwnd);	
      } 
      default:
	   	return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return WM_NULL;
}




void GUI_AVIList_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = list_win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
  // Player_Init();
	
	hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                         &wcex,
                         L"GUI_MusicList_DIALOG",
                         WS_CLIPSIBLINGS,
                         0, 0, GUI_XSIZE, GUI_YSIZE,
                         NULL, NULL, NULL, NULL);
	//��ʾ����??
	ShowWindow(hwnd, SW_SHOW);
	//��ʼ������Ϣѭ??���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)??
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
