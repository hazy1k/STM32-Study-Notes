#include "emXGUI.h"
#include "./recorder/GUI_RECORDER_DIALOG.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "./recorder/Backend_Recorder.h"

//ͼ���������
recorder_icon_t record_icon[] = {

   {L"A",        { 25, 180, 26, 24},   ID_RECORD_bPOWER},    // 0. ����
   {L"Q",        { 68, 180, 24, 24},   ID_RECORD_BUGLE},     // 1. ���Ȱ�ť
   {L"S",        {164, 205, 24, 24},   ID_RECORD_BACK},      // 2. ��һ��
   {L"T",        {203, 201, 32, 32},   ID_RECORD_PLAY},      // 3. ����
   {L"V",        {250, 204, 24, 24},   ID_RECORD_NEXT},      // 4. ��һ��
   {L"U",        { 25, 146, 24, 24},   ID_RECORD_STOP},      // 5. ֹͣ¼��
   {L"U",        { 68, 146, 24, 24},   ID_RECORD_START},     // 6. ��ʼ¼��
   {L"U",        { 68, 146, 24, 24},   ID_RECORD_PADNC},     // 7. ��ͣ����
   {L"O",        {286,   8, 23, 23},   ID_RECORD_EXIT},      // 8. �˳�
 
   {L"¼����",   {30,   41, 57, 27},   ID_RECORD_STATE},     // 9. ����¼��
   {L"00:00",    {38,   86, 42, 27},   ID_RECORD_TIME},      // 10. ¼��ʱ��
   {L"00:00",    {119, 177, 31, 18},   ID_PLAY_TIME},        // 11. ����ʱ��
   {L"00:00",    {287, 177, 32, 18},   ID_PLAY_TOTAL_TIME},  // 12. ¼����ʱ��
  
   {L" ",        {135, 26, 158,138},   ID_RECORD_LIST},      // 13. �����б�
   {L" ",        {152, 179,130, 16},   ID_PLAY_PROGRESS},    // 14. ���Ž�����
   {L" ",        { 24, 211, 69, 16},   ID_RECORD_sPOWER},    // 15. ����������
  
};

#define RECORDER_BACK_COLOR     95, 137, 159//188, 213, 253

//���ֲ��������
HWND	Recorer_hwnd;
TaskHandle_t h_record;    // ����������ƾ��
TaskHandle_t h_play_record;

/* ������ MUSIC �ж���ģ�����ֱ���ã������¶��壬�Խ�Լ�ڴ� */
extern char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];  // ����List
extern char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];  // ��ʾlist
extern uint8_t  music_file_num;                   // �ļ�����
extern char music_name[FILE_NAME_LEN];    // ����������
extern int play_index;      // ���Ÿ����ı��ֵ
extern HWND music_wnd_time; // �������������ھ��
extern uint8_t chgsch;      // ������������־λ

static char path[100];     // �ļ���Ŀ¼
int  power_rec = 220;    // ����ֵ

/**
  * @brief  scan_files �ݹ�ɨ��sd���ڵ�¼���ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_record_files(char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];
	WCHAR wbuf[128];	
	
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
     if (res != FR_OK || fno.fname[0] == 0)
     {
       f_closedir(&dir);
       break; 	//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
     }
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //if(strstr(path,"recorder")!=NULL)continue;      // �ӹ�¼���ļ�
      if (*fn == '.') continue; 											  // ���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							  // Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							    // �ϳ�����Ŀ¼��
        res = scan_record_files(path);									// �ݹ���� 
        if (res != FR_OK) 
        {
          f_closedir(&dir);
					break; 																	     	// ��ʧ�ܣ�����ѭ��
        }
        path[i] = 0; 
      } 
      else 
      { 
//				printf("%s/%s\r\n", path, fn);								// ����ļ���
				if(strstr(fn,".wav")||strstr(fn,".WAV"))      // �ж��Ƿ�wav�ļ�
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(music_file_num<MUSIC_MAX_NUM))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(music_playlist[music_file_num], file_name, strlen(file_name) + 1);
                  printf("%s\r\n", music_playlist[music_file_num]);
						//memcpy(music_lcdlist[music_file_num],fn,strlen(fn));
						x_mbstowcs(wbuf, fn, sizeof(wbuf));	    // ��Ansi�ַ�ת����GUI��unicode�ַ�.
						//��Listbox������һ��Item���¼�ļ������ļ�����.
						i = SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_GETCOUNT, 0, 0);
						SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_ADDSTRING, i, (LPARAM)wbuf);
						music_file_num++;//��¼�ļ�����
					}
				}//if mp3||wav
      }//else
     } //for
  } 
  return res; 
}

/**
  * @brief  ����¼������
  * @param  hwnd:�����ھ��
  * @retval ��
  */

/* �� Backend_Recorder.c �ж��� */
extern uint8_t Isread;              /* DMA������ɱ�־ */
extern uint8_t bufflag;             /* ���ݻ�����ѡ���־ */
extern uint32_t wavsize;            /* wav��Ƶ���ݴ�С */
extern FIL AF_File_Handle;			/* file objects */
char recfilename[25]; 
//extern WavHead rec_wav;             /* WAV�豸  */
extern FRESULT result; 
extern UINT bw;            					/* File R/W count */
extern REC_TYPE Recorder;           /* ¼���豸 */

static void App_Record(void *p)
{
  char *name = p;
//  vTaskSuspend(h_record);    // �����߳�
  uint8_t app = 1;
  while(app) //�߳��Ѵ�����
  {     
    recorder(name);
    app = 0;    // ֻ����һ��
  }
  printf("¼������\n\r");
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

/**
  * @brief  ����¼������
  * @param  hwnd:�����ھ��
  * @retval ��
  */
static int thread=0;
static void App_PlayRecord(HWND hwnd)
{                          
	
  thread =1;
  vTaskSuspend(h_play_record);    // �����߳�
  
	while(thread) //�߳��Ѵ�����
	{     
    int i = 0;      
     
    i = 0;
     //�õ�������Ŀ���ļ���
    while(music_playlist[play_index][i]!='\0')
    {
      music_name[i]=music_playlist[play_index][i];
      i++;
    }
    music_name[i]='\0';
    if (play_index != SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL, 0, 0))     // ��鵱ǰҪ���ŵ���Ŀ�ǲ����Ѿ�ѡ��
    {
      SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, play_index, 0);            // ���õ�ǰ���ŵĸ���Ϊѡ��״̬
    }

    if(strstr(music_name,".wav")||strstr(music_name,".WAV"))
    {
      record_play((uint8_t *)music_name, hwnd);
    }

    printf("���Ž���\n");

    //�����������
    GUI_msleep(20);
	   
   }
   thread = 1;
   printf("¼�������߳��˳�\n");
   GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

static void exit_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
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
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //���û���ɫ
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

static void stop_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  EnableAntiAlias(hdc, TRUE);
   
  if (ds->Style & WS_DISABLED)    // �����ǽ�ֹ��
  {
    SetBrushColor(hdc, MapRGB(hdc, 200, 200, 200));
    SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //��ť�ǰ���״̬
      SetPenColor(hdc, MapRGB(hdc, 185, 185, 185));
      SetBrushColor(hdc, MapRGB(hdc, 185, 185, 185));      //��������ɫ
    }
    else
    { //��ť�ǵ���״̬
      SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
    }
  }
  
  /* ��Բ�� */
//  SetPenSize(hdc, 2);
  DrawCircle(hdc, rc.x+rc.w/2, rc.y+rc.h/2, rc.w/2);

  /* ����Բ�Ǿ��� */
	InflateRect(&rc, -5, -5);
  FillRoundRect(hdc, &rc, 4);
  
  EnableAntiAlias(hdc, FALSE);
}

static void start_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  EnableAntiAlias(hdc, TRUE);
 
  //���ð�������ɫ
	if (ds->Style & WS_DISABLED)    // �����ǽ�ֹ��
  {
    SetBrushColor(hdc, MapRGB(hdc, 180, 20, 20));      //��������ɫ
    SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //��ť�ǰ���״̬
      SetBrushColor(hdc, MapRGB(hdc, 180, 20, 20));      //���ñ���ɫ
      SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
    }
    else
    { //��ť�ǵ���״̬
      SetBrushColor(hdc, MapRGB(hdc, 240, 10, 10));
      SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
    }
  }
  
  /* ��Բ�� */
//	SetPenSize(hdc, 2);
  DrawCircle(hdc, rc.x+rc.w/2, rc.y+rc.h/2, rc.w/2);

  /* ����Բ�Ǿ��� */
				 
  FillCircle(hdc, rc.x+rc.w/2, rc.x+rc.w/2, 7);
  
  EnableAntiAlias(hdc, FALSE);
}

/***********************�ؼ��ػ溯��********************************/
/**
  * @brief  button_owner_draw ��ť�ؼ����ػ���
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @retval NULL
  */

static void button_owner_draw(DRAWITEM_HDR *ds)
{
  HDC hdc; //�ؼ�����HDC
	
  HWND hwnd; //�ؼ���� 
  RECT rc_cli;//�ؼ���λ�ô�С����
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 
													
  //��ȡ�ؼ���λ�ô�С��Ϣ
  GetClientRect(hwnd, &rc_cli);

  GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������  
  GetClientRect(hwnd, &rc_cli);//�õ��ؼ���λ��

  //���ż�ʹ��100*100������
  if(ds->ID == ID_RECORD_PLAY)
  {
    SetFont(hdc, controlFont_32);
  }
  else
  {
    SetFont(hdc, controlFont_24);
  }

  //���ð�������ɫ
  if (ds->Style & WS_DISABLED)    // �����ǽ�ֹ��
  {
    SetTextColor(hdc, MapARGB(hdc, 250, 220, 220, 220));      //��������ɫ
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //��ť�ǰ���״̬
      SetTextColor(hdc, MapARGB(hdc, 250,105,105,105));      //��������ɫ
    }
    else
    { //��ť�ǵ���״̬
      SetTextColor(hdc, MapARGB(hdc, 250,250,250,250));
    }
  }													   

  DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER);//��������(���ж��뷽ʽ)
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
   RECT rc,rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.

	/* ���� */
   GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
   GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
   
//  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
//	FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}

/*
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  hdc:    ��ͼ������
 * @param  back_c��������ɫ
 * @param  Page_c: ������Page������ɫ
 * @param  fore_c���������������ɫ
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
  RECT rc;
  RECT rc_scrollbar;

  /* ���� */
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��

  SetBrushColor(hdc,MapARGB(hdc, 0, RECORDER_BACK_COLOR));
  FillRect(hdc, &rc);

  rc_scrollbar.x = rc.x;
  rc_scrollbar.y = rc.h/2-1;
  rc_scrollbar.w = rc.w;
  rc_scrollbar.h = 2;

  SetBrushColor(hdc, MapRGB888(hdc, Page_c));
  FillRect(hdc, &rc_scrollbar);

  /* ���� */
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

  SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
  //rc.y += (rc.h >> 2) >> 1;
  //rc.h -= (rc.h >> 2);
  /* �߿� */
  //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
  InflateRect(&rc, -2, -2);

  SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
}

/*
 * @brief  �Զ��廬�������ƺ���
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	RECT rc_cli;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	//������ɫ���͵Ĺ�����
	draw_scrollbar(hwnd, hdc, 0, RGB888(	50, 205, 50), RGB888(50, 205, 50));
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   
}

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

  /* ���� */																			 
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��

  SetBrushColor(hdc, MapRGB(hdc, RECORDER_BACK_COLOR));
  FillRect(hdc, &rc);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* �б�Ϊ�գ���ʾ��ʾ��ϢȻ��ֱ�ӷ��� */
    DrawText(hdc, L"��û��¼���ļ���", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}

    SetTextColor(hdc, MapRGB(hdc,50,10,10));

    if(i==cursel)
    {
      SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
    }
    else
    {
      SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
    }

    /* ��ǰ���ͼ�� */
    // SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
    // EnableAntiAlias(hdc, TRUE);
    // FillPolygon(hdc, 0, 0, pt, 5);
    // EnableAntiAlias(hdc, FALSE);

    SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);

    DrawText(hdc, wbuf, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);      // ��ʾ�б��ı�
    
    SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                         // ��һ����

    i++; 
  }
}

extern int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static uint8_t  BUGLE_STATE = 0;     // ����״̬
	 static uint32_t Record_Timer = 0;    // ¼����ʱ��
  
   switch(msg){
      case WM_CREATE:
      {
         vsset.mvol=140;			//������С
         vsset.bflimit=6;		//��������
         vsset.bass=15;			//����Ч��
         vsset.tflimit =10;	//��������
         vsset.treble =15;		//����Ч��
         vsset.effect =0;		//�޶���Ч��
         vsset.speed =1;			//�����ٶ�
          
         recset.input =0;			//MIC
         recset.samplerate =1;	//8K
         recset.channel =0;		//������
         recset.agc = 2;				//ʵ������Ϊ1
        
         VS_Init();
         GUI_msleep(100);
        
         if (VS_HD_Reset())
         {
           GUI_DEBUG("vs1053Ӳ����λʧ��");
         }
        
         VS_Soft_Reset();   
		  
         for (uint8_t xC=0; xC<9; xC++)     // 0~7 ����ť
         {
            /* ѭ��������ť */
            CreateWindow(BUTTON, record_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_TRANSPARENT,
                         record_icon[xC].rc.x, record_icon[xC].rc.y,
                         record_icon[xC].rc.w,record_icon[xC].rc.h,
                         hwnd, record_icon[xC].id, NULL, NULL); 
         }
				 ShowWindow(GetDlgItem(hwnd, ID_RECORD_BUGLE), SW_HIDE);     // ����������ȵİ�ť
         ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_HIDE);     // �������ؼ�������ͣ�İ�ť
         EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), DISABLE);    // ����ֹͣ��ť

         for (uint8_t xC=9; xC<13; xC++)    // 8~11 ���ı���
         {
            /* ѭ�������ı��� */
            CreateWindow(TEXTBOX, record_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_TRANSPARENT,
                         record_icon[xC].rc.x, record_icon[xC].rc.y,
                         record_icon[xC].rc.w,record_icon[xC].rc.h,
                         hwnd, record_icon[xC].id, NULL, NULL); 
         }

         /*********************����������******************/
         SCROLLINFO sif;
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 255;
         sif.nValue = 0;//��ʼֵ
         sif.TrackSize = 15;//����ֵ
         sif.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������          
         music_wnd_time = CreateWindow(SCROLLBAR, record_icon[14].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                           record_icon[14].rc.x, record_icon[14].rc.y, record_icon[14].rc.w,
                           record_icon[14].rc.h, hwnd, record_icon[14].id, NULL, NULL);
         SendMessage(music_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         

         /*********************����ֵ������******************/
         SCROLLINFO sif_power;
         sif_power.cbSize = sizeof(sif_power);
         sif_power.fMask = SIF_ALL;
         sif_power.nMin = 0;
         sif_power.nMax = 254;//�������ֵΪ63
         sif_power.nValue = 220;//��ʼ����ֵ
         sif_power.TrackSize = 15;//����ֵ
         sif_power.ArrowSize = 0;//���˿��Ϊ0��ˮƽ��������
         
         HWND wnd;
         wnd = CreateWindow(SCROLLBAR, record_icon[15].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                           record_icon[15].rc.x, record_icon[15].rc.y, record_icon[15].rc.w,
                           record_icon[15].rc.h, hwnd, record_icon[15].id, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_power);
    
         BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
         xReturn = xTaskCreate((TaskFunction_t )(void(*)(void*))App_PlayRecord,  /* ������ں��� */
                            (const char*    )"App_PlayMusic",          /* �������� */
                            (uint16_t       )2*1024/4,                   /* ����ջ��СFreeRTOS������ջ����Ϊ��λ */
                            (void*          )hwnd,                     /* ������ں������� */
                            (UBaseType_t    )5,                        /* ��������ȼ� */
                            (TaskHandle_t  )&h_play_record);           /* ������ƿ�ָ�� */

				if(xReturn != pdPASS )
				{
					GUI_ERROR("GUI rec play Thread Create failed");
				} 
        
         /* ����¼���ļ��б� */
         CreateWindow(LISTBOX, record_icon[13].icon_name, WS_OWNERDRAW |  WS_VISIBLE | LBS_NOTIFY,
                           record_icon[13].rc.x, record_icon[13].rc.y, record_icon[13].rc.w,
                           record_icon[13].rc.h, hwnd, record_icon[13].id, NULL, NULL);
         
        PostAsyncMessage(hwnd, eMSG_SCAN_FILE, 0, 0);     // ִ��һ���ļ�ɨ��(ʹ���첽�ķ�ʽ������Ϣ)
				InvalidateRect(hwnd,NULL,TRUE);
        break;
      }

      case WM_TIMER:
      {
        int tmr_id;
				WCHAR wbuf[20];

				tmr_id = wParam;    // ��ʱ�� ID

				if (tmr_id == ID_Record_Timer)    // ÿ��¼����ʱ��ʱ��
				{
					Record_Timer++;
					x_wsprintf(wbuf, L"%02d:%02d", Record_Timer / 60, Record_Timer % 60);
          SetWindowText(GetDlgItem(hwnd, ID_RECORD_TIME), wbuf);
				}
      }  
			break;     
      
      case eMSG_SCAN_FILE:
      {
        HWND wnd = GetDlgItem(hwnd, ID_RECORD_LIST);
               
        SendMessage(wnd, LB_RESETCONTENT, 0, 0);
        memset(music_playlist, '\0', sizeof(music_playlist));
				printf("sizeof(music_lcdlist) : %d\r\n",sizeof(music_lcdlist));
//        memset(music_lcdlist, '\0', sizeof(music_lcdlist));
        scan_record_files(path);
        SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, 0, 0);    // Ĭ��ѡ�е�һ��
        InvalidateRect(wnd, NULL, TRUE);
      }
      break;
      
      case WM_NOTIFY:
      {
         u16 code,  id, ctr_id;;
         id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
         code=HIWORD(wParam);//��ȡ��Ϣ������
         ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID. 
         
         NMHDR *nr;        

         //���͵���
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* �˳���ť���� */
            case ID_RECORD_EXIT:
            {
              PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
            }
            break;
            
            /* ��һ�� */
            case ID_RECORD_BACK:
            {
              int i, num;
              WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
              i = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);     // ��õ�ǰѡ����
              num = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCOUNT,0,0);    // ���������
              SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, i == 0 ? num-1 : --i, 0);     // ������õ�ǰѡ����
            
              GetWindowText(wnd, wbuf, 3);
              if (wbuf[0] == L'T')
              {
                vTaskResume(h_play_record);            // ��ǰ�ǹ���ģ��ָ�����
                SetWindowText(wnd, L"U");
                EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // ���ÿ�ʼ¼����ť
              }
                
//              I2S_Play_Start();
              play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // ��õ�ǰѡ����
              Replay = 0;
            }
            break;
            
            /* ��һ�� */
            case ID_RECORD_NEXT:
            {
              int i, num;
              WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
              i = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);     // ��õ�ǰѡ����
              num = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCOUNT,0,0);    // ���������
              SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, i==(num-1) ? 0 : ++i, 0);     // ������õ�ǰѡ����
              
              GetWindowText(wnd, wbuf, 3);
              if (wbuf[0] == L'T')
              {
                vTaskResume(h_play_record);            // ��ǰ�ǹ���ģ��ָ�����
                SetWindowText(wnd, L"U");
                EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // ���ÿ�ʼ¼����ť
              }
                
//              I2S_Play_Start();
              play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // ��õ�ǰѡ����
              Replay = 0;
            }
            break;
            
            /* ����¼�� */
            case ID_RECORD_PLAY:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
							GetWindowText(wnd, wbuf, 3);
              
							if (wbuf[0] == L'U')
							{
                vTaskSuspend(h_play_record);
								SetWindowText(wnd, L"T");
								EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), ENABLE);      // ʹ�ܿ�ʼ¼����ť
							}
							else
							{
                vTaskResume(h_play_record);
//                Replay = 1;
								SetWindowText(wnd, L"U");
								EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // ���ÿ�ʼ¼����ť
							}
            }
            break;
            
            /* ��ʼ¼�� */
            case ID_RECORD_START:
						{
              int i = 0;
              DIR dir;
              
              /*  ���·�������ڣ������ļ���  */
              result = f_opendir(&dir,RECORDERDIR);
              while(result != FR_OK)
              {
                i++;
                if (i > 5)
                {
                  /* ��ʼ��ʧ�� */
                  RECT RC;
                  MSGBOX_OPTIONS ops;
                  const WCHAR *btn[] = { L"ȷ��",L"ȡ��" };      //�Ի����ڰ�ť������

                  ops.Flag = MB_ICONERROR;
                  ops.pButtonText = btn;
                  ops.ButtonCount = 2;
                  RC.w = 180;
                  RC.h = 120;
										  
                  RC.x = (GUI_XSIZE - RC.w) >> 1;
                  RC.y = (GUI_YSIZE - RC.h) >> 1;
                  SelectDialogBox(hwnd, RC, L"û�м�⵽SD��\n��ȷ��SD�Ѳ��롣", L"����", &ops);    // ��ʾ������ʾ��
                  break;
                }
                f_mkdir(RECORDERDIR);
                result = f_opendir(&dir,RECORDERDIR);
              }
              
              if (i >= 5)break;
              
              /* Ѱ�Һ����ļ��� */
              for(i=1;i<0xff;++i)
              {
                sprintf(recfilename,"0:/recorder/rec%03d.wav",i);
                result=f_open(&AF_File_Handle,(const TCHAR *)recfilename,FA_READ);
                if(result==FR_NO_FILE)break;					
              }
              f_close(&AF_File_Handle);
              
              if(i==0xff)
              {
                break;
              }
              /* ��ʼ¼�� ����¼������ */
              RecordFlag = 0;
              BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
              xReturn = xTaskCreate((TaskFunction_t )(void(*)(void*))App_Record,  /* ������ں��� */
                            (const char*    )"Record Task",       /* �������� */
                            (uint16_t       )(2*1024)/4,            /* ����ջ��СFreeRTOS������ջ����Ϊ��λ */
                            (void*          )recfilename,         /* ������ں������� */
                            (UBaseType_t    )4,                   /* ��������ȼ� */
                            (TaskHandle_t  )&h_record);           /* ������ƿ�ָ�� */

              if(xReturn != pdPASS )
              {
                GUI_ERROR("GUI rec thread create failed");
              }

							SetTimer(hwnd, ID_Record_Timer, 1000, TMR_START, NULL);                    // ����¼����ʱ��ʱ��
							/* ��ť���� */
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_BUGLE), L"Q");
							SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"����¼��");             // ����¼����״̬
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_START), SW_HIDE);                    // �������ؿ�ʼ¼���İ�ť
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_SHOW);                    // ������ʾ��������ͣ�İ�ť
							SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_LOCKCURSEL, TRUE, 0);     // �����б��������޸�ѡ�е���
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_PLAY), DISABLE);                   // ���ò��Ű�ť
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_BACK), DISABLE);                   // ������һ�װ�ť
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_NEXT), DISABLE);                   // ������һ�װ�ť
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), ENABLE);                    // ʹ��ֹͣ¼����ť
            }
            break;

            case ID_RECORD_STOP:
            {
              RecordFlag = 2;
              vTaskResume(h_record);                  // �ָ�¼������
              KillTimer(hwnd, ID_Record_Timer);       // ɾ��¼����ʱ��ʱ��
              Record_Timer = 0;
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_TIME), L"00:00");
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_PADNC), L"U");         // ����¼����״̬

              /* �ļ���¼���� */		
              char *cbuf = NULL;
              WCHAR wbuf[128];
              int i = 0;
              memcpy(music_playlist[music_file_num], recfilename, strlen(recfilename) + 1);
              printf("%s\r\n", music_playlist[music_file_num]);
              cbuf = strstr(recfilename, "/");    // ������һ�� / 
              cbuf = strstr(cbuf+1, "/");         // �����ڶ��� /
              if (cbuf != NULL)
              { 
              }
              x_mbstowcs(wbuf, cbuf + 1, sizeof(wbuf));
              //��Listbox������һ��Item��
              i = SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_GETCOUNT, 0, 0);
              SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_ADDSTRING, i, (LPARAM)wbuf);
              music_file_num++;//��¼�ļ�����
              InvalidateRect(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), NULL, TRUE);    // ��Ч���б�ʹ���ػ�
              
              /* ֹͣ¼�� */
							SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"¼����");                // ����¼����״̬
							SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_LOCKCURSEL, FALSE, 0);     // �����б���������޸�ѡ�е���
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_START), SW_SHOW);                     // ��ʾ��ʼ¼���İ�ť
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_HIDE);                     // ���ؼ�������ͣ�İ�ť
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_BACK), ENABLE);                     // ʹ����һ�װ�ť
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_NEXT), ENABLE);                     // ʹ����һ�װ�ť
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_PLAY), ENABLE);                     // ʹ�ܲ��Ű�ť
							EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), DISABLE);                    // ����ֹͣ¼����ť
            }
            break;

						/* ��ͣ����¼����ť */
            case ID_RECORD_PADNC:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PADNC);
              
							GetWindowText(wnd, wbuf, 3);
							if (wbuf[0] == L'U')
							{
								KillTimer(hwnd, ID_Record_Timer);       // ɾ��¼����ʱ��ʱ��
                vTaskSuspend(h_record);    // ����¼������
//                I2Sxext_Recorde_Stop();    // ֹͣ¼��
								SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"����ͣ");          // ����¼����״̬
								SetWindowText(wnd, L"T");
							}
							else
							{
								SetTimer(hwnd, ID_Record_Timer, 1000, TMR_START, NULL);               // ����¼����ʱ��ʱ��
                vTaskResume(h_record);      // �ָ�¼������
//                I2Sxext_Recorde_Start();    // ����¼��
								SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"����¼��");         // ����¼����״̬
								SetWindowText(wnd, L"U");
							}
            }
            break;
#if 0
            /* ��Ƶ���ѡ��ť */
            case ID_RECORD_BUGLE:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_BUGLE);
              
							GetWindowText(wnd, wbuf, 3);
							if (wbuf[0] == L'P')
							{
								SetWindowText(wnd, L"Q");
                VS_Horn_Set(DISABLE);
							}
							else
							{
								SetWindowText(wnd, L"P");
                VS_Horn_Set(ENABLE);
							}
            }
            break;
#endif
            /* �������ڰ�ť������ */
            case ID_RECORD_bPOWER:
            {
							if (BUGLE_STATE == 1)
							{
                BUGLE_STATE = 0;
								ShowWindow(GetDlgItem(hwnd, ID_RECORD_sPOWER), SW_HIDE); //��������
							}
							else
							{
                BUGLE_STATE = 1;
								ShowWindow(GetDlgItem(hwnd, ID_RECORD_sPOWER), SW_SHOW); //������ʾ
							}
            }
            break;
            
          }
        }
        else if (LBN_SELCHANGE == code)   // �б�ѡ����ı�
        {
          if (id == ID_RECORD_LIST)
          {
            
            WCHAR wbuf[3];
            HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
            
            GetWindowText(wnd, wbuf, 3);
            if (wbuf[0] == L'T')
            {
              vTaskResume(h_play_record);            // ��ǰ�ǹ���ģ��ָ�����
              SetWindowText(wnd, L"U");
						  EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // ���ÿ�ʼ¼����ť
            }
              
//            I2S_Play_Start();
            play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // ��õ�ǰѡ����
//            mp3player.ucStatus = STA_SWITCH;
            Replay = 0;
          }
        }
         
         /* ���������� */
          nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
          //����������case
          if (ctr_id == ID_RECORD_sPOWER)
          {
             NM_SCROLLBAR *sb_nr;
             sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
             static int ttt = 0;
             switch (nr->code)
             {
                case SBN_THUMBTRACK: //R�����ƶ�
                {
                   power_rec = sb_nr->nTrackValue; //�õ���ǰ������ֵ
                   if(power_rec == 0) 
                   {
//                      wm8978_OutMute(1);//����
                      SetWindowText(GetDlgItem(hwnd, ID_RECORD_bPOWER), L"J");
                      ttt = 1;
                   }
                   else
                   {
                      if(ttt == 1)
                      {
                         ttt = 0;
                         SetWindowText(GetDlgItem(hwnd, ID_RECORD_bPOWER), L"A");
                      }
                     
                   } 
                   SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power_rec); //����SBM_SETVALUE����������ֵ
                }
                break;
                
               case SBN_CLICKED:
               {
                 VS_Set_Vol(power_rec);     // ����vs1053������
               }
               break;
             }
          }

          //����������case
          if (ctr_id == ID_PLAY_PROGRESS)
          {
             NM_SCROLLBAR *sb_nr;
             int i = 0;
             sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
             switch (nr->code)
             {
                case SBN_THUMBTRACK: //R�����ƶ�
                {
                   i = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ                
                   SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //���ý���ֵ
                   chgsch = 2;
                }
                break;
                
               case SBN_CLICKED:
               {
                 //��λ���������λ��
                   chgsch = 1;
               }
               break;
             }
          }         
      
      break;
    }

      //�ػ��ƺ�����Ϣ
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_RECORD_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_STOP)
         {
            stop_owner_draw(ds);    // �ػ�ֹͣ¼����ť
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_START)
         {
            start_owner_draw(ds);    // �ػ濪ʼ¼����ť
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_sPOWER || ds->ID == ID_PLAY_PROGRESS)   
         {
            scrollbar_owner_draw(ds);    // �������ػ�
            return TRUE;
         }
         else if (ds->ID >= ID_RECORD_bPOWER && ds->ID<= ID_RECORD_PADNC)
         {
            button_owner_draw(ds);    // ��ť�ػ�
            return TRUE;
         }
         else if (ds->ID >= ID_RECORD_STATE && ds->ID<= ID_PLAY_TIME)
         {
            Brigh_Textbox_OwnerDraw(ds);    // �ػ�͸���ı�
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_LIST)
         {
            listbox_owner_draw(ds);    // �ػ��б�
            return TRUE;
         }
         return FALSE;
      }     
      
      //���ƴ��ڽ�����Ϣ
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc;//��Ļhdc
        RECT rc = {181, 3, 75, 20};

        //��ʼ����
        hdc = BeginPaint(hwnd, &ps); 

				DrawText(hdc, L"¼���ļ�", -1, &rc, NULL);//��������(���ж��뷽ʽ)

        EndPaint(hwnd, &ps);

        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
        SetBrushColor(hdc, MapRGB(hdc, 95, 137, 159));//130, 195, 213
        FillRect(hdc, &rc);
				
//				RECT rc1 = {120, 0, 200, GUI_YSIZE};
//        SetBrushColor(hdc, MapRGB(hdc, RECORDER_BACK_COLOR)); 
//        FillRect(hdc, &rc1);
        
				RECT rc_grad = {110, 0, 3, GUI_YSIZE};
        GradientFillRect(hdc, &rc_grad, MapRGB(hdc, 50, 50, 50), MapRGB(hdc, 95, 137, 159), FALSE);
        return TRUE;
      }

      //�رմ�����Ϣ����case
      case WM_CLOSE:
      {        
        Record_Timer = 0;     // ��λ¼����ʱ
//        mp3player.ucStatus = STA_IDLE;
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //�رմ�����Ϣ����case
      case WM_DESTROY:
      {
        vTaskResume(h_play_record);
        BUGLE_STATE = 0;
									
        chgsch = 0;
        RecordFlag = 2;
        Replay = 0;
        thread = 0;
        GUI_msleep(10);    // �����������
        while(!thread)
        {
          GUI_msleep(10);
        }    // �ȴ�����ɾ��
        
//        vTaskDelete(h_record);
//        vTaskDelete(h_play_record);
        music_file_num = 0;   // ��λ�ļ���¼

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_RECORDER_DIALOG(void)
{ 	
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	Recorer_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI RECORDER DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(Recorer_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, Recorer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


