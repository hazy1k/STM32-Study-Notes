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
GUI_SEM *Delete_VideoTask_Sem;//做任务同步,结束播放器前先关闭播放任务
TaskHandle_t VideoTask_Handle;
extern volatile uint8_t video_timeout;//视频播放引入
VIDEO_DIALOG_Typedef VideoDialog;
char avi_playlist[20][100];//播放List
char lcdlist[20][100];//显示list
//static char lcdlist[20][100];//显示list
static char path[512]="0:";//文件根目??
uint8_t hide_flag = 0;
//图标管理数组

VIDEO_DIALOG_Typedef VideoDialog ={
  .power = 20,
  
};

/**
  * @brief  scan_files 递归扫描sd卡内的视频文??
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回??
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[100];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (0x81 ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
     if (res != FR_OK || fno.fname[0] == 0)
		 {
			 f_closedir(&dir);
			 break; 	//为空时表示所有项目读取完毕，跳出
		 }
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if(strstr(path,"recorder")!=NULL)continue;       //逃过录音文件
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录??
        res = scan_files(path);											//递归遍历 
        if (res != FR_OK) 
			{
			    f_closedir(&dir);
					break; 																		//打开失败，跳出循??
			}
        path[i] = 0; 
      } 
      else 
		{ 
											//输出文件??
				if(strstr(fn,".avi")||strstr(fn,".AVI"))//判断是否AVI文件
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
               VideoDialog.avi_file_num++;//记录文件个数
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

	while(1) //线程已创建了
	{
		if(app==0)
		{
			app=1;    
      AVI_play(avi_playlist[VideoDialog.playindex]);
      app = 0;
			vTaskDelay(1);
		}
  }
	while(1)//任务结束,程序在这执行
	{
		vTaskDelay(200);
	}
}

extern uint8_t *Frame_buf;
extern UINT BytesRD;
extern volatile BOOL bDrawVideo;
GUI_MUTEX*	AVI_JPEG_MUTEX = NULL;    // 用于确保一帧图像用后被释放完在退出线程
SURFACE *pSurf1;
u16 *vbuf;
static LRESULT video_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    { 
			 Delete_VideoTask_Sem = GUI_SemCreate(0, 1);
			
			 AVI_JPEG_MUTEX = GUI_MutexCreate();    // 创建一个互斥信号量
			
       u8 *jpeg_buf;
       u32 jpeg_size;
       JPG_DEC *dec;

       vbuf =GUI_GRAM_Alloc(GUI_YSIZE*GUI_XSIZE*2);
       pSurf1 =CreateSurface(SURF_SCREEN,GUI_XSIZE,GUI_YSIZE,480*2,vbuf);
			 pSurf1->GL->FillArea(pSurf1,0,0,GUI_XSIZE,GUI_YSIZE,pSurf1->CC->MapRGB(0,0,0)); 

			BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
			/* 创建视频播放任务 */
			xReturn = xTaskCreate((TaskFunction_t )App_PlayVideo,  /* 任务入口函数 */
														(const char*    )"App_PlayVideo",/* 任务名字 */
														(uint16_t       )3*1024,  /* 任务栈大小 */
														(void*          )NULL,/* 任务入口函数参数 */
														(UBaseType_t    )5, /* 任务的优先级 */
														(TaskHandle_t*  )&VideoTask_Handle);/* 任务控制块指针 */ 
       
			if(pdPASS != xReturn)
			{
				GUI_ERROR("VIDEOPLAYER Start Failed!Play Reset\r\n");
				while(1);//停止工作
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
			thread_PlayVideo = 0;  //结束音乐播放线程
			VideoDialog.SWITCH_STATE = 0xff;//切换状态,退出循环播放
      video_timeout =1;//恢复任务
			
			GUI_SemWait(Delete_VideoTask_Sem,0xFFFFFFFF);//死等,同步结束播放线程
			
			/* 关闭硬件 */
			vTaskDelete(VideoTask_Handle);
			
			/* 软件复位 */
      VideoDialog.playindex = 0;
			VideoDialog.SWITCH_STATE = 0;
			
      DeleteDC(VideoDialog.hdc_bk);
      GUI_GRAM_Free(vbuf);
      DeleteSurface(pSurf1);
			
			GUI_MutexDelete(AVI_JPEG_MUTEX);//删除互斥量
			GUI_SemDelete(Delete_VideoTask_Sem);//删除信号量
			
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
	wcex.lpfnWndProc = video_win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	VideoDialog.Video_Hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                                    &wcex,
                                    L"GUI_MUSICPLAYER_DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(VideoDialog.Video_Hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, VideoDialog.Video_Hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
