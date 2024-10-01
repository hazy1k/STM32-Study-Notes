#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "ff.h"
#include "./mjpegplayer/Backend_avifile.h"
#include "./mjpegplayer/Backend_vidoplayer.h"
#include "./Bsp/lcd/bsp_lcd.h"
#include "./Bsp/wm8978/bsp_wm8978.h"  
#include "emXGUI.h"
#include "emXGUI_JPEG.h"
#include "x_libc.h"
#include "./mjpegplayer/GUI_AVIList_DIALOG.h"
#include "./mjpegplayer/GUI_AVIPLAYER_DIALOG.h"
FIL       fileR ;
UINT      BytesRD;
__align(8) uint8_t   Frame_buf[1024*30] __EXRAM;

static volatile uint8_t audiobufflag=0;
__align(8) uint8_t   Sound_buf[4][1024*5] __EXRAM={0};

static uint8_t   *pbuffer;

uint32_t  mid;
uint32_t  Strsize;
uint16_t  Strtype;

extern int LIST_STATE;

static volatile uint8_t timeout;
extern WAVEFORMAT*   wavinfo;
extern avih_TypeDef* avihChunk;
extern HWND avi_wnd_time;
extern int avi_chl;
void MUSIC_I2S_DMA_TX_Callback(void);
extern void mjpegdraw(uint8_t *mjpegbuffer,uint32_t size);
static void TIM3_Config(uint16_t period,uint16_t prescaler);
extern void App_DecodeMusic(HWND hwnd, const void *dat, int cbSize, JPG_DEC *dec);
extern char tiimg[];
extern unsigned int timgsize(void);
//extern HDC hdc_AVI;
extern HWND hwnd_AVI;
extern volatile int win_fps;
extern GUI_MUTEX*	AVI_JPEG_MUTEX;    // ����ȷ��һ֡ͼ���ú��ͷ������˳��߳�

static volatile int frame=0;
static volatile int t0=0;
volatile int avi_fps=0;


static u32 alltime = 0;		//��ʱ�� 
u32 cur_time; 		//��ǰ����ʱ�� 
uint8_t temp11=0;	
u32 pos;//�ļ�ָ��λ��
s32 time_sum = 0;
void AVI_play(char *filename, HWND hwnd, int vol)
{
  FRESULT  res;
  uint32_t offset;
  uint16_t audiosize;
  uint8_t avires=0;
  uint8_t audiosavebuf;

  pbuffer=Frame_buf;
  res=f_open(&fileR,filename,FA_READ);
  if(res!=FR_OK)
  {
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
  f_lseek(&fileR,mid+12);//������־ID  
  
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
  
   I2S_Stop();			/* ֹͣI2S¼���ͷ��� */
	wm8978_Reset();		/* ��λWM8978����λ״̬ */	
  	/* ����WM8978оƬ������ΪDAC�����Ϊ���� */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* ����������������ͬ���� */
	wm8978_SetOUT1Volume(vol);
   if(vol == 0)
      wm8978_OutMute(1);//����
   else
      wm8978_OutMute(0);
	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
  I2S_GPIO_Config();
  I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, wavinfo->SampleRate);
  I2S_DMA_TX_Callback=MUSIC_I2S_DMA_TX_Callback;			//�ص�����ָwav_i2s_dma_callback
  I2S_Play_Stop();
  I2Sx_TX_DMA_Init((uint16_t *)Sound_buf[1],(uint16_t *)Sound_buf[2],audiosize/2);
  audiobufflag=0;	    
  timeout=0;
  audiosavebuf=0;
  audiobufflag=0;
  TIM3_Config((avihChunk->SecPerFrame/100)-1,9000-1);
  I2S_Play_Start();  
	
	t0= GUI_GetTickCount();

   //�����ܳ���=ÿһ֡��Ҫ��ʱ�䣨s��*֡����
   alltime=(avihChunk->SecPerFrame/1000)*avihChunk->TotalFrame;
   alltime/=1000;//��λ����
  WCHAR buff[128];
  //char *str = NULL;
 // RECT rc0 = {0, 367,120,30};//��ǰʱ��
  x_wsprintf(buff, L"�ֱ��ʣ�%d*%d", img_w, img_h);
  SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB2), buff);

//  char *ss;
//  int length1=strlen(filename);
//  int length2=strlen(File_Path);
//  if(strncpy(filename,File_Path,length2))//�Ƚ�ǰn���ַ���������strcpy
//  {
//    ss = filename + length2;
//  }
  x_mbstowcs_cp936(buff, lcdlist_wnd[Play_index], 200);
  SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB1), buff);
  
  x_wsprintf(buff, L"%02d:%02d:%02d",
             alltime/3600,(alltime%3600)/60,alltime%60);
  SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB4), buff);
  
  while(1&&!sw_flag)//����ѭ��
  {					
		int t1;
     if(!avi_chl){

        
        
   //fptr������ļ�ָ���λ�ã�fsize���ļ����ܴ�С������֮��ı����͵�ǰʱ������ʱ���ı�����ͬ��fptr/fsize = cur/all��     
   cur_time=((double)fileR.fptr/fileR.fsize)*alltime;
   //���½�����
   if(!LIST_STATE)
    InvalidateRect(avi_wnd_time, NULL, FALSE);  
   if(!LIST_STATE)   
    SendMessage(avi_wnd_time, SBM_SETVALUE, TRUE, cur_time*255/alltime);    
   
    x_wsprintf(buff, L"%02d:%02d:%02d",///%02d:%02d:%02d alltime/3600,(alltime%3600)/60,alltime%60
             cur_time/3600,(cur_time%3600)/60,cur_time%60); 		
   
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

      //HDC hdc_mem,hdc;
      pbuffer=Frame_buf;
      AVI_DEBUG("S\n"); 
      f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//������֡+��һ������ID��Ϣ
      AVI_DEBUG("E\n");   
			timeout=0;
		
			if(frame&!LIST_STATE)
			{	
#if 1		//ֱ��д�����ڷ�ʽ.	
				HDC hdc;
        GUI_MutexLock(AVI_JPEG_MUTEX,0xFFFFFFFF);    // ��ȡ������
//				printf("1\n");
				hdc =GetDC(hwnd_AVI);
//        hdc_AVI = GetDC(hwnd);
				JPEG_Out(hdc,160,89,Frame_buf,BytesRD);
        ReleaseDC(hwnd_AVI,hdc);
//        printf("2\n");
        
        
//            ClrDisplay(hdc, &rc0, MapRGB(hdc, 0,0,0));
//            SetTextColor(hdc, MapRGB(hdc,255,255,255));
//            DrawText(hdc, buff,-1,&rc0,DT_VCENTER|DT_CENTER);
            
//       if(!LIST_STATE)
        SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB5), buff);
        x_wsprintf(buff, L"֡�ʣ�%dFPS/s", avi_fps);
//        if(!LIST_STATE)
        SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB3), buff);
        GUI_MutexUnlock(AVI_JPEG_MUTEX);              // ����������

#endif
			}
			
      while(timeout==0)
      {   
				//rt_thread_delay(1); //��Ҫ���ȣ�������ź���.				
        GUI_msleep(1);
      }      
      
      //DeleteDC(hdc_mem);
      //ReleaseDC(hwnd, hdc);
      timeout=0;
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
				//rt_thread_delay(1); 
        i=audiobufflag;
        if(i)
					i--;
        else 
					i=3; 

      }while(audiobufflag==i);
      AVI_DEBUG("S\n");
      f_read(&fileR,Sound_buf[audiosavebuf],Strsize+8,&BytesRD);//������֡+��һ������ID��Ϣ
      AVI_DEBUG("E\n");
      pbuffer=Sound_buf[audiosavebuf];      
    }
    else 
    {
      GUI_INFO("֡����\n");
//      GUI_INFO("%lX    %X    %d    %lX",fileR.fptr,Strtype,Strsize,&fileR.fptr-Strsize);
      break;
    }
					   	
  }
  else
  {
         pos = fileR.fptr;
//         //���ݽ�������������λ��				
         temp11=SendMessage(avi_wnd_time, SBM_GETVALUE, NULL, NULL); 
         time_sum = fileR.fsize/alltime*(temp11*alltime/255-cur_time);//������������ ���㹫ʽ���ļ��ܴ�С/��Ҫ������������ = ��ʱ��/��ǰ��ʱ��
         //�����ǰ�ļ�ָ��δ�����
        	if(pos<fileR.fsize)pos+=time_sum; 
         //����ļ�ָ�뵽�����30K����
          if(pos>(fileR.fsize-1024*30))
          {
            pos=fileR.fsize-1024*30;
          }
         
          f_lseek(&fileR,pos);
      
      #if 0
         if(pos == 0)
            mid=Search_Movi(Frame_buf);//Ѱ��movi ID  �ж��Լ��ǲ��ǻ������ݶ�
         else 
            mid = 0;  
        int iiii= 0;//����ƫ����
         while(1)
         {
            //ÿ�ζ�512���ֽڣ�ֱ���ҵ�����֡��֡ͷ
            u16 temptt = 0;//��������֡��λ��
            AVI_DEBUG("S\n");

            f_read(&fileR,Frame_buf,512,&BytesRD);
            AVI_DEBUG("E\n");

            temptt = Search_Fram(Frame_buf,BytesRD);
            iiii++;
            if(temptt)
            {            
               AVI_DEBUG("S temptt =%d\n",temptt);
               AVI_DEBUG("S Frame_buf[temptt] =%c %c %c %c\n",
                                      Frame_buf[temptt],
                                      Frame_buf[temptt+1],
                                      Frame_buf[temptt+2],
                                      Frame_buf[temptt+3]);
               /* ���ȡ512���ݣ���ֹ��־�ڱ߽�ʱ���� */
               f_read(&fileR,(u8*)Frame_buf+BytesRD,512,&BytesRD);
               AVI_DEBUG("E\n");
                pbuffer = Frame_buf;
               Strtype=MAKEWORD(pbuffer+temptt+2);//������
               Strsize=MAKEDWORD(pbuffer+temptt+4);//����С
               mid += temptt + 512*iiii-512;//����ƫ����
//               if(temptt == 16)
//                  continue;
               break;
            }

         }
         #else
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
         #endif
         
         if(Strsize%2)Strsize++;//������1
         f_lseek(&fileR,pos+mid+8);//������־ID  
         AVI_DEBUG("S Strsize=%d\n",Strsize);

         f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//������֡+��һ������ID��Ϣ 
         AVI_DEBUG("E\n");
         
         avi_chl = 0;    
     }
     
    
         //�ж���һ֡��֡���� 
         Strtype=MAKEWORD(pbuffer+Strsize+2);//������
         Strsize=MAKEDWORD(pbuffer+Strsize+4);//����С									
         if(Strsize%2)Strsize++;//������1		  
//      GUI_INFO("%lX    %X    %X    %lX    %X    %X",fileR.fptr,Strtype,Strsize,fileR.fptr-Strsize,Strsize+8,BytesRD);
     }
  
 

  sw_flag = 0;
  I2S_Play_Stop();
  I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
	wm8978_Reset();	/* ��λWM8978����λ״̬ */
  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,DISABLE); //ʹ�ܶ�ʱ��3
  f_close(&fileR);
}

void MUSIC_I2S_DMA_TX_Callback(void)
{
  audiobufflag++;
  if(audiobufflag>3)
	{
		audiobufflag=0;
	}
	
	if(DMA1_Stream4->CR&(1<<19)) //��ǰ��ȡMemory1����
	{
		DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)Sound_buf[audiobufflag], DMA_Memory_0);
	}
	else
	{
		DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)Sound_buf[audiobufflag], DMA_Memory_1); 
	}
}

/**
  * @brief  ͨ�ö�ʱ��3�жϳ�ʼ��
  * @param  period : �Զ���װֵ��
  * @param  prescaler : ʱ��Ԥ��Ƶ��
  * @retval ��
  * @note   ��ʱ�����ʱ����㷽��:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=��ʱ������Ƶ��,ΪSystemCoreClock/2=90,��λ:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=period;   //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  ��ʱ��3�жϷ�����
  * @param  ��
  * @retval ��
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{    
    timeout=1;
	}
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ	
}
