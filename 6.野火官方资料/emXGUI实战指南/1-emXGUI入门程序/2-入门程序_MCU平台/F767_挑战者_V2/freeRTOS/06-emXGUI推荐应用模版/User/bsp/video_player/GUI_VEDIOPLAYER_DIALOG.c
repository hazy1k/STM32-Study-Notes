#include "emXGUI.h"
#include "GUI_VEDIOPLAYER_DIALOG.h"
#include "emXGUI_JPEG.h"
#include "GUI_AppDef.h"
#include	"CListMenu.h"
#include "x_libc.h"
#include <string.h>
#include "Backend_vidoplayer.h"
#include "Backend_avifile.h"
//#include "./sai/bsp_sai.h" 
uint8_t chgsch_TouchUp;
GUI_SEM *Delete_VideoTask_Sem;//������ͬ��,����������ǰ�ȹرղ�������
TaskHandle_t VideoTask_Handle;
extern volatile uint8_t video_timeout;//��Ƶ��������
VIDEO_DIALOG_Typedef VideoDialog;
char avi_playlist[20][100];//����List
char lcdlist[20][100];//��ʾlist
//static char lcdlist[20][100];//��ʾlist
static char path[512]="0:";//�ļ���Ŀ??
uint8_t hide_flag = 0;
//ͼ���������

VIDEO_DIALOG_Typedef VideoDialog ={
  .power = 20,
  
};

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
  char file_name[100];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (0x81 ? 2 : 1) + 1]; 	//���ļ���֧��
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
      if(strstr(path,"recorder")!=NULL)continue;       //�ӹ�¼���ļ�
      if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							//Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼??
        res = scan_files(path);											//�ݹ���� 
        if (res != FR_OK) 
			{
			    f_closedir(&dir);
					break; 																		//��ʧ�ܣ�����ѭ??
			}
        path[i] = 0; 
      } 
      else 
		{ 
											//����ļ�??
				if(strstr(fn,".avi")||strstr(fn,".AVI"))//�ж��Ƿ�AVI�ļ�
				{
					if ((strlen(path)+strlen(fn)<100)&&(VideoDialog.avi_file_num<20))
					{
						sprintf(file_name, "%s/%s", path, fn);
            printf("%s%s\r\n", path, fn);	
						memcpy(avi_playlist[VideoDialog.avi_file_num],file_name,strlen(file_name));
            memcpy(lcdlist[VideoDialog.avi_file_num],fn,strlen(fn));
//						memcpy(lcdlist[VideoDialog.avi_file_num],fn,strlen(fn));						
						//memcpy(lcdlist1[avi_file_num],fn,strlen(fn));lcdlist_wnd
					}
               VideoDialog.avi_file_num++;//��¼�ļ�����
				}//if 
      }//else
     } //for
  } 
  return res; 
}
static int thread_PlayVideo = 0;

static void App_PlayVideo(void *param)
{
	int app=0;

	while(1) //�߳��Ѵ�����
	{
		if(app==0)
		{
			app=1;    
      AVI_play(avi_playlist[VideoDialog.playindex]);
      app = 0;
			vTaskDelay(1);
		}
  }
	while(1)//�������,��������ִ��
	{
		vTaskDelay(200);
	}
}

extern uint8_t *Frame_buf;
extern UINT BytesRD;
extern volatile BOOL bDrawVideo;
GUI_MUTEX*	AVI_JPEG_MUTEX = NULL;    // ����ȷ��һ֡ͼ���ú��ͷ������˳��߳�
SURFACE *pSurf1;
u16 *vbuf;
static LRESULT video_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    { 
			 Delete_VideoTask_Sem = GUI_SemCreate(0, 1);
			
			 AVI_JPEG_MUTEX = GUI_MutexCreate();    // ����һ�������ź���
			
       u8 *jpeg_buf;
       u32 jpeg_size;
       JPG_DEC *dec;

       vbuf =GUI_GRAM_Alloc(GUI_YSIZE*GUI_XSIZE*2);
       pSurf1 =CreateSurface(SURF_SCREEN,GUI_XSIZE,GUI_YSIZE,480*2,vbuf);
			 pSurf1->GL->FillArea(pSurf1,0,0,GUI_XSIZE,GUI_YSIZE,pSurf1->CC->MapRGB(0,0,0)); 

			BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
			/* ������Ƶ�������� */
			xReturn = xTaskCreate((TaskFunction_t )App_PlayVideo,  /* ������ں��� */
														(const char*    )"App_PlayVideo",/* �������� */
														(uint16_t       )3*1024,  /* ����ջ��С */
														(void*          )NULL,/* ������ں������� */
														(UBaseType_t    )5, /* ��������ȼ� */
														(TaskHandle_t*  )&VideoTask_Handle);/* ������ƿ�ָ�� */ 
       
			if(pdPASS != xReturn)
			{
				GUI_ERROR("VIDEOPLAYER Start Failed!Play Reset\r\n");
				while(1);//ֹͣ����
			}else
			{
				thread_PlayVideo = 1;
			}
			
      break;
    }
    case WM_NOTIFY:
    {
       break;
    }     
    case WM_DRAWITEM:
    {
          return TRUE;
    }  
		
    case WM_PAINT:
    {
    	PAINTSTRUCT ps;
    	HDC hdc;
			HDC hdc_avi;
    	hdc =BeginPaint(hwnd,&ps);
			
    	hdc_avi =CreateDC(pSurf1,NULL);
			
			RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
			BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_avi , rc.x, rc.y, SRCCOPY);

			DeleteDC(hdc_avi);	 
    	EndPaint(hwnd,&ps);
    	return TRUE;
    }

    case WM_DESTROY:
    {
			thread_PlayVideo = 0;  //�������ֲ����߳�
			VideoDialog.SWITCH_STATE = 0xff;//�л�״̬,�˳�ѭ������
      video_timeout =1;//�ָ�����
			
			GUI_SemWait(Delete_VideoTask_Sem,0xFFFFFFFF);//����,ͬ�����������߳�
			
			/* �ر�Ӳ�� */
			vTaskDelete(VideoTask_Handle);
			
			/* �����λ */
      VideoDialog.playindex = 0;
			VideoDialog.SWITCH_STATE = 0;
			
      DeleteDC(VideoDialog.hdc_bk);
      GUI_GRAM_Free(vbuf);
      DeleteSurface(pSurf1);
			
			GUI_MutexDelete(AVI_JPEG_MUTEX);//ɾ��������
			GUI_SemDelete(Delete_VideoTask_Sem);//ɾ���ź���
			
      return PostQuitMessage(hwnd);	
    }  
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_VideoPlayer_DIALOG(void*param)
{	
	WNDCLASS	wcex;
	MSG msg;
	VideoDialog.avi_file_num = 0;
  scan_files(path);

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = video_win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	VideoDialog.Video_Hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                                    &wcex,
                                    L"GUI_MUSICPLAYER_DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(VideoDialog.Video_Hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, VideoDialog.Video_Hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
