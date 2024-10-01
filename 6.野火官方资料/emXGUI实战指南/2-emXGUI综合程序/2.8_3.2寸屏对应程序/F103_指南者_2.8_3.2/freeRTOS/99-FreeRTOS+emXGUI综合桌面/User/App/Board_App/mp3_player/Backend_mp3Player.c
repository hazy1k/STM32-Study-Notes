/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   VS1053�������ܲ���
******************************************************************************
* @attention
*
* ʵ��ƽ̨:Ұ��  STM32 F103 ������  
* ��̳    :http://www.firebbs.cn
* �Ա�    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include  "./mp3_player/vs1053/VS1053.h"
#include "ff.h" 
#include "./mp3_player/Backend_mp3Player.h"
#include "./mp3_player/Backend_musiclist.h"
#include "emXGUI.h"
#include "x_libc.h"
#include "flac.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include "./dwt_delay/core_delay.h" 

//#define Delay_ms GUI_msleep
void FileSystem_Test(void);
RECT rc_cli = {45, 183, 230, 20};//������
Music_State_t Music_State;

FRESULT result;
extern FIL AF_File_Handle;
UINT bw;
uint32_t File_HeaderTime=0;//����ʱ��ƫ��
extern uint8_t AF_Buf_REC_MUS[BUFSIZE];

void vs1053_player(uint8_t *filepath, uint8_t vol, HDC hdc)
{
	uint16_t playtime=0,time=0;
	static uint8_t lyriccount=0;
	uint16_t f_kbps=0xffff;
	uint16_t i=0;
	uint32_t val=0;
//  char lyrictext[MAX_LINE_LEN]={0};
  Music_State=STA_PLAYING;
	uint8_t type=0;
  WCHAR wbuf[128];
	
	printf("song:%s\n",filepath);
	VS_Restart_Play();  					// �������� 
	VS_Set_All();        					// ������������Ϣ 
  VS_Set_Vol(vol);              // ��������
	VS_Reset_DecodeTime();				// ��λ����ʱ�� 

  if (music_lcdlist[play_index][0] == '\0')
  {
    SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), L"���ֲ�����");
  }
  else
  {
    x_mbstowcs(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
    SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), wbuf);
  }
  
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
	{
//		VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
		GUI_msleep(1);//1ms
		type=2;
	}
	else	if(strstr((const char*)filepath,".wav")||strstr((const char*)filepath,".WAV"))
	{
		type=3;
	}
	else//if(strstr((const char*)filepath,".mp3")||strstr((const char*)filepath,".mp3"))
	{
		type=1;
	}
	
//	SLIDER_SetValue(hSchedule,0);
	lyriccount=0;
	lrc.oldtime=lrc.time_tbl[0];
	
	result=f_open(&AF_File_Handle,(const TCHAR*)filepath, FA_OPEN_EXISTING | FA_READ);//���ļ�	
	
	if(result!=FR_OK) return;
//FileSystem_Test();
	VS_SPI_SpeedHigh();	//����						   
	while(Music_State==STA_PLAYING)
	{
		if (Music_State==STA_IDLE)
		{			
			GUI_msleep(5);
			continue;											//��ͣ��ʱ��������ѭ��
		}		
		if(chgsch==1&&((VS_RD_Reg(SPI_STATUS)&0x8000)==0))//���������
		{     
			uint32_t sch=0;
			uint16_t numb=0;
			uint8_t 	dat[32]={0};
			VS_Set_Vol(10);
			numb=VS_Get_EndFillByte() & 0xff;
			for(i=0;i<32;i++)
        dat[i]=numb;
      
			for(i=0;i<2048+32;)
			{
				if(VS_Send_MusicData(dat)==0)
        {
          i+=32;
        }
			}
      //���ݽ�������������λ��
      numb=SendMessage(music_wnd_time, SBM_GETVALUE, NULL, NULL);     //��ȡ��ǰ��������λ��
			sch = numb*AF_File_Handle.fsize/0xff;
//			printf("chgsch:%d,sch:%d\n",chgsch,sch);
			f_lseek(&AF_File_Handle,sch);
			VS_Set_Vol(vol);
			chgsch=0;
      lrc.oldtime=0;
      lyriccount=0;
		}
		
		result=f_read(&AF_File_Handle,AF_Buf_REC_MUS,BUFSIZE,(UINT*)&bw);//����2048���ֽ�	
		i=0;
    /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
//    taskENTER_CRITICAL();
		while(i<bw)
		{
			if(VS_Send_MusicData(AF_Buf_REC_MUS+i)==0)//��VS10XX������Ƶ����
			{
				i+=32;
			}
		}
//    taskEXIT_CRITICAL();	
		
		if(bw!=BUFSIZE||result!=FR_OK)
		{
      printf("%s��%d��", result != FR_OK ? "��ʧ��" : "������", result);
			VS_Soft_Reset();
			GUI_msleep(10);
			if(play_index<music_file_num-1)		//�Զ���ʼ��һ�׸���
				play_index++;
			else
				play_index = 0;
			Music_State = STA_SWITCH;
			f_close(&AF_File_Handle);
      SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, 0);    // ���������������õ�ǰֵ
			printf("song end!!\n");
			return;//������.		  
		}
		GUI_msleep(1);
		if((Music_State == STA_SWITCH))break;

		if(f_kbps==0xffff)//ֻ����һ��
		{
			uint8_t tmp[4]={0};
			uint32_t dattmp=0;
			uint16_t time=0;
			switch (type)
			{
				case 2://flac
					tmp[0]=AF_Buf_REC_MUS[22];//��λ��flac�ļ��ܲ�������ŵ�ȷʵλ��
					tmp[1]=AF_Buf_REC_MUS[23];//�ܲ�������������36λ�洢�ģ�������ֻ����32λ��һ���ļ�������ô������
					tmp[2]=AF_Buf_REC_MUS[24];//����ʹ�þ��Զ�λ���洢��ַ�����ܲ����ļ�����ƫ�Ϊ��ȷʹ�ÿ�ʹ�ò��Ҷ�λ
					tmp[3]=AF_Buf_REC_MUS[25];
					dattmp=(tmp[0]<<24)+(tmp[1]<<16)+(tmp[2]<<8)+tmp[3];//flac�ļ��ܲ�����
					time=dattmp/44100;//flac�ļ��ܲ���ʱ��
					f_kbps=AF_File_Handle.fsize/125/time;//flac�ļ�������
					break;
				case 3://wav
					tmp[0]=AF_Buf_REC_MUS[24];//���Զ�λ��wav�ļ��洢����Ƶ�ʵ�ַ
					tmp[1]=AF_Buf_REC_MUS[25];				
					dattmp=(tmp[1]*256+tmp[0])*32/1000;				
					f_kbps=(uint16_t)dattmp;
					//printf("tmp:%02X %02X,dattmp:%d,f_kbps:%d\n",tmp[0],tmp[1],dattmp,f_kbps);
					break;
				default:
					f_kbps=VS_Get_HeadInfo();
					break;
			}	

        //�Ҳ�������ļ�
        if (lrc.flag == 0)
        {
           
           SetWindowText(wnd_lrc3,L"����SDCard������Ӧ�ĸ���ļ�(*.lrc)");
           SetWindowText(wnd_lrc1,L"");
           SetWindowText(wnd_lrc2,L"");
           SetWindowText(wnd_lrc4,L"");
           SetWindowText(wnd_lrc5,L"");
        }
		}
		time++;
		if((time>=(25*type+playtime))&&(chgsch==0))
		{
			if(!File_HeaderTime)
			{
				File_HeaderTime=f_tell(&AF_File_Handle)/f_kbps/125;//ƫ���ļ�ͷ���ֵ�ʱ��
			}
			playtime=time;	
			if((type==1)&&((f_kbps>448)||(f_kbps<32)))f_kbps=VS_Get_HeadInfo();
			val=AF_File_Handle.fsize/f_kbps/125-File_HeaderTime;
			x_wsprintf(wbuf, L"%02d:%02d", val/60,val%60);
      SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB1), wbuf);
			
			lrc.curtime=f_tell(&AF_File_Handle)/f_kbps/125-File_HeaderTime;
			
			x_wsprintf(wbuf, L"%02d:%02d", lrc.curtime/60,lrc.curtime%60);
      SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB2), wbuf);
      SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, lrc.curtime*255/val);
			
//			musicrate=lrc.curtime*255/val;

      if(lrc.flag == 1)
      {
        //+100����ǰ��ʾ����ʾ��Ҫ����һ��ʱ��
        if((lrc.oldtime <= lrc.curtime*100)&&(lrc.indexsize>7))//+100
        {
          //��ʾ��ǰ�еĸ��
          x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount]-1], LYRIC_MAX_SIZE);
          SetWindowText(wnd_lrc3,wbuf);
          //��ʾ��i-1�еĸ�ʣ�ǰһ�У�
          if(lyriccount>0)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-1]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc2,wbuf);
          }
          else
             SetWindowText(wnd_lrc2,L" ");
          //��ʾ��i-2�еĸ�ʣ�ǰ���У�
          if(lyriccount>0)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-2]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc1,wbuf);
          }
          else
             SetWindowText(wnd_lrc1,L" ");
          //��ʾ��i+1�еĸ�ʣ���һ�У�   
          if(lyriccount < lrc.indexsize-1)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+1]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc4,wbuf);                    
          }
          else
             SetWindowText(wnd_lrc4,L" ");
          //��ʾ��i+2�еĸ�ʣ�����У�   
          if(lyriccount < lrc.indexsize-2)
          {
             x_mbstowcs(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+2]-1], LYRIC_MAX_SIZE);
             SetWindowText(wnd_lrc5,wbuf);                    
          }
          else
             SetWindowText(wnd_lrc5,L" ");
                    
          do{
          lyriccount++;					
          if(lyriccount>=lrc.indexsize)
          {
             lrc.oldtime=0xffffff;
             break;
          }
          lrc.oldtime=lrc.time_tbl[lyriccount];
          }while(lrc.oldtime<=(lrc.curtime*100));
        }                  
             
        }
        
		}
	}
  
	lyriccount=0;
	f_close(&AF_File_Handle);
  if(time2exit == 1)
  {
    f_close(&AF_File_Handle);	 
    GUI_SemPost(exit_sem);
  }
	File_HeaderTime = 0;
}

/***************************** (END OF FILE) *********************************/
