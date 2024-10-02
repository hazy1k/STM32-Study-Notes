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

extern GUI_MUTEX*	AVI_JPEG_MUTEX;    // ����ȷ��һ֡ͼ���ú��ͷ������˳��߳�

static volatile int frame=0;
static volatile int t0=0;
volatile int avi_fps=0;

volatile BOOL bDrawVideo=FALSE;
extern SURFACE *pSurf1;

//static u32 alltime = 0;		//��ʱ�� 
//u32 cur_time; 		//��ǰ����ʱ�� 
uint8_t tmp=0;	
u32 pos;//�ļ�ָ��λ��
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
		GUI_SemPost(Delete_VideoTask_Sem);//�ļ���ʧ��,����ǰ�ͷų�����Թرյ��ź���
    return;    
  }

  res=f_read(&fileR,pbuffer,20480,&BytesRD);


  avires=AVI_Parser(pbuffer);//����AVI�ļ���ʽ
  if(avires)
  {
    return;    
  }
  
  avires=Avih_Parser(pbuffer+32);//����avih���ݿ�
  if(avires)
  {
    return;    
  }
  //strl�б�
  avires=Strl_Parser(pbuffer+88);//����strh���ݿ�
  if(avires)
  {
    return;    
  }
  
  avires=Strf_Parser(pbuffer+164);//����strf���ݿ�
  if(res!=FR_OK)
  {
    return;    
  }
  
  mid=Search_Movi(pbuffer);//Ѱ��movi ID	�����ݿ飩	
  if(mid==0)
  {
    return;    
  }

  Strtype=MAKEWORD(pbuffer+mid+6);//�����ͣ�movi�����������ַ���
  Strsize=MAKEDWORD(pbuffer+mid+8);//����С
  if(Strsize%2)Strsize++;//������1
  f_lseek(&fileR,(mid+12));//������־ID  
  
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
  VideoDialog.alltime/=1000;//��λ����

  x_wsprintf(buff, L"�ֱ��ʣ�%d*%d", videoplayer_img_w, videoplayer_img_h);
  SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_RES), buff);

  x_mbstowcs_cp936(buff, lcdlist[VideoDialog.playindex], 200);
  SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_ITEM), buff);
  x_wsprintf(buff, L"%02d:%02d:%02d",
             VideoDialog.alltime/3600,(VideoDialog.alltime%3600)/60,VideoDialog.alltime%60);
  SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_ALLTIME), buff);

  while(1)//����ѭ��
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
	 if(Strtype==T_vids)//��ʾ֡
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
      

      res = f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//������֡+��һ������ID��Ϣ
      
      if(res != FR_OK)
      {
        GUI_DEBUG("E\n");
      }
         
			video_timeout=0;
		
			if(frame&1)
			{	


				HWND hwnd=VideoDialog.Video_Hwnd;
        
        GUI_MutexLock(AVI_JPEG_MUTEX,0xFFFFFFFF);    // ��ȡ������

				JPEG_Out(hdc1,0,0,Frame_buf,BytesRD);

        x_wsprintf(buff, L"֡�ʣ�%dFPS/s", avi_fps);
        if(!VideoDialog.LIST_STATE)
          SetWindowText(GetDlgItem(VideoDialog.Video_Hwnd, eID_TEXTBOX_FPS), buff);

        bDrawVideo=TRUE;

        InvalidateRect(hwnd,NULL,FALSE); //������Ч��...

        GUI_MutexUnlock(AVI_JPEG_MUTEX);              // ����������				


			}


        GUI_msleep(50);
  
	  bDrawVideo=FALSE;

    }//��ʾ֡
    else if(Strtype==T_auds)//��Ƶ���
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

      res = f_read(&fileR,Sound_buf[audiosavebuf],Strsize+8,&BytesRD);//������֡+��һ������ID��Ϣ
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
         //���ݽ�������������λ��				
         tmp=SendMessage(VideoDialog.SBN_TIMER_Hwnd, SBM_GETVALUE, NULL, NULL); 
         time_sum = fileR.fsize/VideoDialog.alltime*(tmp*VideoDialog.alltime/249-VideoDialog.curtime);//������������ ���㹫ʽ���ļ��ܴ�С/��Ҫ������������ = ��ʱ��/��ǰ��ʱ��
         //�����ǰ�ļ�ָ��δ�����
        	if(pos<fileR.fsize)pos+=time_sum; 
         //����ļ�ָ�뵽�����30K����
          if(pos>(fileR.fsize-1024*30))
          {
            pos=fileR.fsize-1024*30;
          }
         
          f_lseek(&fileR,pos);

         f_read(&fileR,Frame_buf,1024*30,&BytesRD);
         AVI_DEBUG("E\n");
         if(pos == 0)
            mid=Search_Movi(Frame_buf);//Ѱ��movi ID
         else 
            mid = 0;
         mid += Search_Fram(Frame_buf,1024*30);
         pbuffer = Frame_buf;
         Strtype=MAKEWORD(pbuffer+mid+2);//������
         Strsize=MAKEDWORD(pbuffer+mid+4);//����С

         
         if(Strsize%2)Strsize++;//������1
         f_lseek(&fileR,(pos+mid+8));//������־ID  
         AVI_DEBUG("S Strsize=%d\n",Strsize);

         f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//������֡+��һ������ID��Ϣ 
         
         VideoDialog.avi_chl = 0;  

     }
         //�ж���һ֡��֡���� 
         Strtype=MAKEWORD(pbuffer+Strsize+2);//������
         Strsize=MAKEDWORD(pbuffer+Strsize+4);//����С									
         if(Strsize%2)Strsize++;//������1
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
  HAL_TIM_Base_Stop_IT(&TIM3_Handle); //ֹͣ��ʱ��3�����ж�
  f_close(&fileR);

}


/**
  * @brief  ͨ�ö�ʱ��3�жϳ�ʼ��
  * @param  period : �Զ���װֵ��
  * @param  prescaler : ʱ��Ԥ��Ƶ��
  * @retval ��
  * @note   ��ʱ�����ʱ����㷽��:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=��ʱ������Ƶ��,ΪSystemCoreClock/2=90,��λ:Mhz
  */
  
void TIM3_Config(uint16_t period,uint16_t prescaler)
{	
  __TIM3_CLK_ENABLE();

  TIM3_Handle.Instance = TIM3;
  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM3_Handle.Init.Period = period;
  //��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
  //				PCLK1 = HCLK / 4 
  //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=200MHz
  // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=10000Hz
  TIM3_Handle.Init.Prescaler =  prescaler;
  TIM3_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM3_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  // ��ʼ����ʱ��TIM
  HAL_TIM_Base_Init(&TIM3_Handle);
  
  HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn); 
  // ������ʱ�������ж�
  HAL_TIM_Base_Start_IT(&TIM3_Handle);  
   
  
}

/**
  * @brief  ��ʱ��3�жϷ�����
  * @param  ��
  * @retval ��
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM3_Handle);
}
