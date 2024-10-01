/*
******************************************************************************
* @file    Backend_Recorder.c
* @author  fire
* @version V1.0
* @date    2019-xx-xx
* @brief   ¼�����ܲ���
******************************************************************************
* @attention
* 
* ʵ��ƽ̨:Ұ�� STM32 F103 V2 �Ե� ������  
* ��̳    :http://www.firebbs.cn
* �Ա�    :https://fire-stm32.taobao.com
*
******************************************************************************
*/

#include "bsp/key/bsp_key.h" 
#include "ff.h" 
#include "Backend_Recorder.h"
#include "GUI_RECORDER_DIALOG.h"
#include  "VS1053.h"

extern FIL	AF_File_Handle;																							//�ļ����
extern FRESULT result;																				//�ļ�ϵͳAPI��������ֵ
extern UINT bw;																								//�Ѷ�����д���ֽ���

uint8_t Replay=0, RecordFlag=0;
static TCHAR recfilename[25]={"1:/recorder/rec001.wav"};
static __WaveHeader rechead;
uint8_t AF_Buf_REC_MUS[BUFSIZE];

static char tempstr[12]={"rec001.wav"};
_recorder_obj recset;
extern uint8_t chgsch;      // ������������־λ
extern int  power_rec;    // ����ֵ

void recorder(const TCHAR *recfile)
{
	uint32_t sectorsize=0;
	uint16_t recsec=0;					//¼��ʱ��
	uint16_t idx=0;
	uint16_t regval=0;
	uint16_t i;	
	i=0;
	while(i<BUFSIZE)   
	{
		AF_Buf_REC_MUS[i]='\0';
		++i;		
	}


	recoder_enter_rec_mode(&recset);
	
	while(VS_RD_Reg(SPI_HDAT1)>>8);			//�ȵ�buf ��Ϊ�����ٿ�ʼ

	rechead.riff.ChunkID=0X46464952;	//"RIFF"
	rechead.riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
	rechead.riff.Format=0X45564157; 	//"WAVE"
	rechead.fmt.ChunkID=0X20746D66; 	//"fmt "
	rechead.fmt.ChunkSize=16; 			//��СΪ16���ֽ�
	rechead.fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
	rechead.fmt.NumOfChannels=1;		//������	
 	rechead.fmt.SampleRate=recset.samplerate*8000;		//��������
 	rechead.fmt.ByteRate=rechead.fmt.SampleRate*2;//16λ,��2���ֽ�
 	rechead.fmt.BlockAlign=2;			//���С,2���ֽ�Ϊһ����
 	rechead.fmt.BitsPerSample=16;		//16λPCM
  rechead.data.ChunkID=0X61746164;	//"data"
 	rechead.data.ChunkSize=0;			//���ݴ�С,����Ҫ����  

	result=f_open(&AF_File_Handle,recfile, FA_CREATE_ALWAYS | FA_WRITE); 
	if(result != FR_OK)
		return;
	else
		result=f_write(&AF_File_Handle,&rechead,sizeof(__WaveHeader),&bw);//д��ͷ����
	//printf("write result:%d,num:%d\n",result,bw);
	while(1)
	{
		if(RecordFlag==2)
		{
			rechead.riff.ChunkSize=sectorsize*BUFSIZE+36;	//�����ļ��Ĵ�С-8;
			rechead.data.ChunkSize=sectorsize*BUFSIZE;		//���ݴ�С
			f_lseek(&AF_File_Handle,0);							//ƫ�Ƶ��ļ�ͷ.
			result=f_write(&AF_File_Handle,&rechead,sizeof(__WaveHeader),&bw);//д��ͷ����
      if(result)
			{
				printf("err:%d\n",result);  
			}
			//printf("%s->write sector:%d,result:%d,num:%d\n",recfile,sectorsize,result,bw);
			f_close(&AF_File_Handle);
			VS_HD_Reset();		   	//Ӳ��λ
			VS_Soft_Reset();  		//��λ 
			sectorsize=0;
			RecordFlag=0;
			return;//break;
		}
		regval=VS_RD_Reg(SPI_HDAT1);	
		if((regval>=256)&&(regval<896))
		{
			idx=0;
			while(idx<BUFSIZE) 	//һ�ζ�ȡBUFSIZE�ֽ�
			{	 
				regval=VS_RD_Reg(SPI_HDAT0);
				AF_Buf_REC_MUS[idx]=regval&0XFF;
				AF_Buf_REC_MUS[idx+1]=regval>>8;
				idx+=2;
			}
			f_lseek(&AF_File_Handle,44+sectorsize*BUFSIZE);
			result=f_write(&AF_File_Handle,AF_Buf_REC_MUS,BUFSIZE,&bw);//д���ļ�
			if(result)
			{
				printf("err:%d bw:%d\n",result,bw);
				break;//д�����.	  
			}
			sectorsize++;//����������1
		}
//		if(recsec!=(sectorsize*4/125/recset.samplerate))//¼��ʱ����ʾ
//		{
//			recsec=sectorsize*4/125/recset.samplerate;
//			sprintf(tempstr,"%02d:%02d",recsec/60,recsec%60);
//			tempstr[5]='\0';
//			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),tempstr);
//		}
//		WM_Exec();//GUI_Delay(1);//
	}
	//f_close(&sdfile);
}

void record_play(uint8_t *recfile,HWND hWin)
{
	uint16_t i=0,time=0,oldtime=0;
	uint16_t f_k=0xffff;    // �������ݴ������ʣ�ÿ��ƽ���ֽ�����
  WCHAR wbuf[10];
	//if(strstr((const char *)recfile,"recorder")==0)return 0xff;
	i=0;
	while(i<BUFSIZE)
	{
		AF_Buf_REC_MUS[i]='\0';
		++i;		
	}
  Replay = 1;
	VS_Restart_Play();  					//�������� 
	VS_Set_All();        	//������������Ϣ
  VS_Set_Vol(power_rec);     // ����vs1053������
	VS_Reset_DecodeTime();	//��λ����ʱ�� 	 
	result=f_open(&AF_File_Handle,(const TCHAR*)recfile,FA_READ);
	if(result==0)
	{ 
			VS_SPI_SpeedHigh();		
    
			while(Replay)
			{
				i=0;	
				result=f_read(&AF_File_Handle,AF_Buf_REC_MUS,BUFSIZE,(UINT*)&bw);	
				if(f_k==0xffff)
				{
//          f_kbps=(uint16_t)((recbuf[25]*256+recbuf[24])*32/1000);
//          time = f_size(&file)/f_kbps/125;
          f_k=((((AF_Buf_REC_MUS[25] << 8) | AF_Buf_REC_MUS[24]) * AF_Buf_REC_MUS[22] * 16)/8);
          time = (f_size(&AF_File_Handle) - sizeof(rechead))/f_k;
          x_wsprintf(wbuf, L"%02d:%02d", time/60, time%60);
          SetWindowText(GetDlgItem(hWin, ID_PLAY_TOTAL_TIME), wbuf);
          time = time ? time : 1;
          SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_SETRANGE, 0, time);
          time = 0;
				}
//				OSSchedLock(&err);
				while(i<bw)
			  {  	
					if(VS_Send_MusicData(AF_Buf_REC_MUS+i)==0)
					{
						i+=32;
					}
				}
//				OSSchedUnlock(&err);
        if(result != FR_OK)
        {
          printf("���ļ�����-->(%d)\r\n", result);
          break;
        }
          
				if((bw!=BUFSIZE))
				{		
          printf("������\r\n");
					break;
				}
				GUI_msleep(1);
        
        if (chgsch == 0)
        {
          oldtime = (f_tell(&AF_File_Handle) - sizeof(rechead))/f_k;
          if((time+1)<=oldtime)
          {
            time=oldtime;
            x_wsprintf(wbuf, L"%02d:%02d", time/60, time%60);
            SetWindowText(GetDlgItem(hWin, ID_PLAY_TIME), wbuf);
            SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_SETVALUE, TRUE, time);
          }
        }
        else if (chgsch == 1)
        {
           uint8_t temp=0;
           uint32_t pos;
          
           temp=SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_GETVALUE, NULL, NULL);  
           pos = temp*f_k;
//           pos=file.fsize/255*temp;
           if(pos<sizeof(rechead))
             pos=sizeof(rechead);
           if(rechead.fmt.BitsPerSample==24)
             temp=12;
           else 
             temp=8;
           if((pos-sizeof(rechead))%temp)
           {
             pos+=temp-(pos-sizeof(rechead))%temp;
           }
           f_lseek(&AF_File_Handle,pos);
           oldtime=0;
           time = 0;
           chgsch=0;         
         }
			}
		}

    SetWindowText(GetDlgItem(hWin, ID_PLAY_TIME), L"00:00");
    SendMessage(GetDlgItem(hWin, ID_PLAY_PROGRESS), SBM_SETVALUE, TRUE, 0);
    GUI_msleep(2);
		f_close(&AF_File_Handle);
}

//�������ݴ������ʣ�ÿ��ƽ���ֽ����� = ����Ƶ�� �� ��Ƶͨ���� �� ÿ�β����õ�������λ�� / 8

//�����ʣ�kbps�� = �������ݴ������� �� 8 / 1000

//WAV�ļ���ռ��С���ֽ�) = �������ݴ������� �� ��Ƶ�ļ�ʱ��

//��Ƶ�ļ�ʱ�����룩 = WAV�ļ���ռ���� / �������ݴ�������

/***************************** (END OF FILE) *********************************/
