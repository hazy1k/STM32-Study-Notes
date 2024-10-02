#include <stdio.h>
#include <string.h>
//#include "stm32f7xx.h"
//#include "ff.h"
#include "Backend_avifile.h"
#include "Backend_vidoplayer.h"
//#include "./sai/bsp_sai.h" 
#include "emXGUI.h"
#include "emXGUI_JPEG.h"
#include "GUI_VEDIOPLAYER_DIALOG.h"
#include "x_libc.h"
//#include "./mjpegplayer/GUI_AVIList_DIALOG.h"
//#include "./mjpegplayer/GUI_AVIPLAYER_DIALOG.h"
FIL       fileR ;
UINT      BytesRD;
#define   Frame_Buf_Size    (1024*30)
uint8_t   *Frame_buf;

static volatile uint8_t audiobufflag=0;
__align(4) uint8_t   Sound_buf[4][1024*5]	;//__attribute__((at(0xd1bd0000)));
//__align(4) uint8_t   Sound_buf[4][1024*5]	__EXRAM;

static uint8_t   *pbuffer;

uint32_t  mid;
uint32_t  Strsize;
uint16_t  Strtype;

TIM_HandleTypeDef TIM3_Handle;

volatile uint8_t video_timeout;
extern WAVEFORMAT*   wavinfo;
extern avih_TypeDef* avihChunk;
//extern HWND avi_wnd_time;
//extern int avi_chl;

extern GUI_MUTEX*	AVI_JPEG_MUTEX;    // 用于确保一帧图像用后被释放完在退出线程

static volatile int frame=0;
static volatile int t0=0;
volatile int avi_fps=0;

volatile BOOL bDrawVideo=FALSE;
extern SURFACE *pSurf1;

//static u32 alltime = 0;		//总时长 
//u32 cur_time; 		//当前播放时间 
uint8_t tmp=0;	
u32 pos;//文件指针位置
s32 time_sum = 0;
void AVI_play(char *filename)
{	
  FRESULT  res;
  uint32_t offset;
  uint16_t audiosize;
  uint8_t avires=0;
  uint8_t audiosavebuf;
  WCHAR buff[128];

	HDC hdc1;

  bDrawVideo=FALSE;
  Frame_buf = (uint8_t *)GUI_VMEM_Alloc(Frame_Buf_Size);
  
  hdc1 =CreateDC(pSurf1,NULL);
  
  pbuffer=Frame_buf;

  res=f_open(&fileR,filename,FA_READ);
  if(res!=FR_OK)
  {
    GUI_VMEM_Free(Frame_buf);
    DeleteDC(hdc1);
		GUI_SemPost(Delete_VideoTask_Sem);//文件打开失败,返回前释放程序可以关闭的信号量
    return;    
  }

  res=f_read(&fileR,pbuffer,20480,&BytesRD);


  avires=AVI_Parser(pbuffer);//解析AVI文件格式
  if(avires)
  {
    return;    
  }
  
  avires=Avih_Parser(pbuffer+32);//解析avih数据块
  if(avires)
  {
    return;    
  }
  //strl列表
  avires=Strl_Parser(pbuffer+88);//解析strh数据块
  if(avires)
  {
    return;    
  }
  
  avires=Strf_Parser(pbuffer+164);//解析strf数据块
  if(res!=FR_OK)
  {
    return;    
  }
  
  mid=Search_Movi(pbuffer);//寻找movi ID	（数据块）	
  if(mid==0)
  {
    return;    
  }

  Strtype=MAKEWORD(pbuffer+mid+6);//流类型（movi后面有两个字符）
  Strsize=MAKEDWORD(pbuffer+mid+8);//流大小
  if(Strsize%2)Strsize++;//奇数加1
  f_lseek(&fileR,(mid+12));//跳过标志ID  
  
  offset=Search_Auds(pbuffer);
  if(offset==0)
  {
    return;    
  }  
  audiosize=*(uint8_t *)(pbuffer+offset+4)+256*(*(uint8_t *)(pbuffer+offset+5));
  if(audiosize==0)
  {
    offset=(uint32_t)pbuffer+offset+4;
    mid=Search_Auds((uint8_t *)offset);
    if(mid==0)
    {
      return;    
    }
    audiosize=*(uint8_t *)(mid+offset+4)+256*(*(uint8_t *)(mid+offset+5));
  }


  audiobufflag=0;	    
  video_timeout=0;
  audiosavebuf=0;
  audiobufflag=0;

	
	t0= GUI_GetTickCount();

  
  VideoDialog.alltime=(avihChunk->SecPerFrame/1000)*avihChunk->TotalFrame;
  VideoDialog.alltime/=1000;//单位是秒

  x_wsprintf(buff, L"分辨率：%d*%d", videoplayer_img_w, videoplayer_img_h);
  SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_RES), buff);

  x_mbstowcs_cp936(buff, lcdlist[VideoDialog.playindex], 200);
  SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_ITEM), buff);
  x_wsprintf(buff, L"%02d:%02d:%02d",
             VideoDialog.alltime/3600,(VideoDialog.alltime%3600)/60,VideoDialog.alltime%60);
  SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_ALLTIME), buff);

  while(1)//播放循环
  {

      GUI_msleep(10);

		int t1;
    if(1)
    {

        
  
   VideoDialog.curtime=((double)fileR.fptr/fileR.fsize)*VideoDialog.alltime;

   if(!(SendMessage(VideoDialog.SBN_TIMER_Hwnd, SBM_GETSTATE,0,0)&SST_THUMBTRACK))
    SendMessage(VideoDialog.SBN_TIMER_Hwnd, SBM_SETVALUE, TRUE, VideoDialog.curtime*255/VideoDialog.alltime);     
   
   x_wsprintf(buff, L"%02d:%02d:%02d",
              VideoDialog.curtime/3600,(VideoDialog.curtime%3600)/60,VideoDialog.curtime%60); 
   if(!VideoDialog.LIST_STATE)
    SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_CURTIME), buff);    
	 if(Strtype==T_vids)//显示帧
    {    	
			frame++;
			t1 =GUI_GetTickCount();
			if((t1 - t0) >= 1000)
			{
				
				avi_fps =frame;
				t0 =t1;
				frame =0;
			}

      pbuffer=Frame_buf;
      

      res = f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息
      
      if(res != FR_OK)
      {
        GUI_DEBUG("E\n");
      }
         
			video_timeout=0;
		
			if(frame&1)
			{	


				HWND hwnd=VideoDialog.Video_Hwnd;
        
        GUI_MutexLock(AVI_JPEG_MUTEX,0xFFFFFFFF);    // 获取互斥量

				JPEG_Out(hdc1,0,0,Frame_buf,BytesRD);

        x_wsprintf(buff, L"帧率：%dFPS/s", avi_fps);
        if(!VideoDialog.LIST_STATE)
          SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_FPS), buff);

        bDrawVideo=TRUE;

        InvalidateRect(hwnd,NULL,FALSE); //产生无效区...

        GUI_MutexUnlock(AVI_JPEG_MUTEX);              // 解锁互斥量				


			}


        GUI_msleep(50);
  
	  bDrawVideo=FALSE;

    }//显示帧
    else if(Strtype==T_auds)//音频输出
    { 
      uint8_t i;
      audiosavebuf++;
      if(audiosavebuf>3)
			{
				audiosavebuf=0;
			}	
      do
      {
        i=audiobufflag;
        if(i)
					i--;
        else 
					i=3; 

      }while(audiobufflag==i);
      AVI_DEBUG("S\n");

      res = f_read(&fileR,Sound_buf[audiosavebuf],Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息
      if(res != FR_OK)
      {
        GUI_DEBUG("E\n");
      }
      
      pbuffer=Sound_buf[audiosavebuf];      
      
    }
    else 
		{
			break;
//			Strtype=T_vids;
//			continue;
		}
					   	
    }
    else
    {
         pos = fileR.fptr;
         //根据进度条调整播放位置				
         tmp=SendMessage(VideoDialog.SBN_TIMER_Hwnd, SBM_GETVALUE, NULL, NULL); 
         time_sum = fileR.fsize/VideoDialog.alltime*(tmp*VideoDialog.alltime/249-VideoDialog.curtime);//跳过多少数据 计算公式：文件总大小/需要跳过的数据量 = 总时间/当前的时间
         //如果当前文件指针未到最后
        	if(pos<fileR.fsize)pos+=time_sum; 
         //如果文件指针到了最后30K内容
          if(pos>(fileR.fsize-1024*30))
          {
            pos=fileR.fsize-1024*30;
          }
         
          f_lseek(&fileR,pos);

         f_read(&fileR,Frame_buf,1024*30,&BytesRD);
         AVI_DEBUG("E\n");
         if(pos == 0)
            mid=Search_Movi(Frame_buf);//寻找movi ID
         else 
            mid = 0;
         mid += Search_Fram(Frame_buf,1024*30);
         pbuffer = Frame_buf;
         Strtype=MAKEWORD(pbuffer+mid+2);//流类型
         Strsize=MAKEDWORD(pbuffer+mid+4);//流大小

         
         if(Strsize%2)Strsize++;//奇数加1
         f_lseek(&fileR,(pos+mid+8));//跳过标志ID  
         AVI_DEBUG("S Strsize=%d\n",Strsize);

         f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息 
         
         VideoDialog.avi_chl = 0;  

     }
         //判断下一帧的帧内容 
         Strtype=MAKEWORD(pbuffer+Strsize+2);//流类型
         Strsize=MAKEDWORD(pbuffer+Strsize+4);//流大小									
         if(Strsize%2)Strsize++;//奇数加1
     }


	GUI_VMEM_Free(Frame_buf);
  DeleteDC(hdc1);
  if(!VideoDialog.SWITCH_STATE)
  {
    VideoDialog.playindex++;
    if(VideoDialog.playindex > VideoDialog.avi_file_num - 1) 
      VideoDialog.playindex = 0;
  }
  else
    VideoDialog.SWITCH_STATE = 0;
  HAL_TIM_Base_Stop_IT(&TIM3_Handle); //停止定时器3更新中断
  f_close(&fileR);

}


/**
  * @brief  通用定时器3中断初始化
  * @param  period : 自动重装值。
  * @param  prescaler : 时钟预分频数
  * @retval 无
  * @note   定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=定时器工作频率,为SystemCoreClock/2=90,单位:Mhz
  */
  
void TIM3_Config(uint16_t period,uint16_t prescaler)
{	
  __TIM3_CLK_ENABLE();

  TIM3_Handle.Instance = TIM3;
  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM3_Handle.Init.Period = period;
  //定时器时钟源TIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=200MHz
  // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM3_Handle.Init.Prescaler =  prescaler;
  TIM3_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM3_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  // 初始化定时器TIM
  HAL_TIM_Base_Init(&TIM3_Handle);
  
  HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn); 
  // 开启定时器更新中断
  HAL_TIM_Base_Start_IT(&TIM3_Handle);  
   
  
}

/**
  * @brief  定时器3中断服务函数
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM3_Handle);
}
