#include <stdint.h>
#include <string.h>
#include "VS1053.h"
#include "recorder.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "ff.h"

/*------------------------------------------------------------*/
//��ʾ�Ӽ�ʹ��DWT�����ʱֵ��Ϊ���¼���ļ�����
#define NAMESIZE 30
static char last_record_pathname[NAMESIZE]="0:Test_0001.wav";
void enable_dwt(void)
{
	#define  DWT_CR      *(__IO uint32_t *)0xE0001000
	#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
	#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC
	#define  DEM_CR_TRCENA                   (1 << 24)
	#define  DWT_CR_CYCCNTENA                (1 <<  0)	
  /* ʹ��DWT���� */
  DEM_CR |= (uint32_t)DEM_CR_TRCENA;                
  /* DWT CYCCNT�Ĵ���������0 */
  DWT_CYCCNT = (uint32_t)0u;
  /* ʹ��Cortex-M DWT CYCCNT�Ĵ��� */
  DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}
void make_new_pathname(char* pathstr)
{
	memset(last_record_pathname,0,NAMESIZE);
	sprintf(pathstr,"0:Test_%d.wav" ,((uint32_t)DWT_CYCCNT) % 9999);
}
/*-------------------------------------------------------------*/


#define TEMPDATA_SIZE   0x1000	//4096B
#define RECORD_BUFSIZE	512
#define PLAY_BUFSIZE    1024*4

static FRESULT result; 
static FIL 		 file;		 /* file objects */
static UINT 	 bw;   		/* File R/W count */

static __WaveHeader rechead;
static _recorder_obj recset;
static enumRecorder recorder_statue = STATE_WAIT;

void Rest_Statue(void)
{
	/* Ӳ��λMP3 */
	VS_HD_Reset();				
	/* ��λVS10XX */
	VS_Soft_Reset();
	recorder_statue = STATE_WAIT;
}

void Recorder_Run()
{
	uint32_t sectorsize=0;
	uint16_t recsec=0;					//¼��ʱ��
	uint16_t idx=0;
	uint16_t regval=0;
	uint16_t i=0;
	uint8_t  buffer[PLAY_BUFSIZE]={0};
  int8_t   tempdata[TEMPDATA_SIZE]={0};
	
	enable_dwt();
	
	LED_RGBOFF;
	
	while(1)
	{
		switch(recorder_statue)
		{
			case STATE_WAIT:
					
				LED_YELLOW
			
				if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
				{
					printf("��ʼ¼��\r\n");					
					recorder_statue = STATE_RECORDING;
					Delay_us(10000);							
				} 	
				
				if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
				{
					printf("�������һ��¼��\r\n");
					recorder_statue = STATE_PLAYING;
					Delay_us(10000);									
				} 
				
			break;
			
			case STATE_RECORDING:
						
				LED_RGBOFF;
			
				memset(tempdata,0,TEMPDATA_SIZE);
	
				/*---------------------------------------------------*/
				vsset.mvol=240;			//������С
				vsset.bflimit=6;		//��������
				vsset.bass=15;			//����Ч��
				vsset.tflimit =10;	//��������
				vsset.treble =15;		//����Ч��
				vsset.effect =0;		//�޶���Ч��
				vsset.speed =1;			//�����ٶ�			
			 	VS_Set_All();    
				/*---------------------------------------------------*/	
					
			  /*---------------------------------------------------*/
				recset.input =0;			//MIC
				recset.samplerate =1;	//8K
				recset.channel =0;		//������
				recset.agc = 6;			//����
				recoder_enter_rec_mode(&recset);	
				/*---------------------------------------------------*/
			
				while(VS_RD_Reg(SPI_HDAT1)>>8);	//�ȵ�buf ��Ϊ�����ٿ�ʼ
						
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

        make_new_pathname(last_record_pathname);
				result=f_open(&file,last_record_pathname, FA_CREATE_ALWAYS | FA_WRITE);						
				if(result != 0 )
				{
					printf("f_open error : %d",result);
					Rest_Statue();
					return;
				}		
				else
				{
					result=f_write(&file,&rechead,sizeof(__WaveHeader),&bw);//Ԥд��ͷ����		
				}
					
				while(1)
				{							
					LED3_TOGGLE;
					regval=VS_RD_Reg(SPI_HDAT1);	
					if((regval>=256)&&(regval<896))
					{
						idx=0;				   	 
						while(idx<RECORD_BUFSIZE) 	//һ�ζ�ȡBUFSIZE�ֽ�
						{	 
							regval=VS_RD_Reg(SPI_HDAT0);
							tempdata[idx]=regval&0XFF;
							tempdata[idx+1]=regval>>8;
							idx+=2;
						}	 
						f_lseek(&file,44+sectorsize*RECORD_BUFSIZE);
						result=f_write(&file,tempdata,RECORD_BUFSIZE,&bw);//д���ļ�
						if(result)
						{
							printf("err:%d bw:%d\n",result,bw);
							f_close(&file);
							Rest_Statue();
							return;//д�����.	  
						}
						sectorsize++;//����������1
					}
												
					if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
					{
						rechead.riff.ChunkSize=sectorsize*RECORD_BUFSIZE+36;	//�����ļ��Ĵ�С-8;
						rechead.data.ChunkSize=sectorsize*RECORD_BUFSIZE;		//���ݴ�С
						f_lseek(&file,0);							//ƫ�Ƶ��ļ�ͷ.
						result=f_write(&file,&rechead,sizeof(__WaveHeader),&bw);//��д��ͷ����
						printf("%s->write sector:%d,result:%d,num:%d\n",last_record_pathname,sectorsize,result,bw);
						f_close(&file);
						VS_HD_Reset();		   	//Ӳ��λ
						VS_Soft_Reset();  		//��λ 
						
						recsec=sectorsize*4/125/recset.samplerate;				
						char  tempstr[12]={0};
						sprintf(tempstr,"%02d:%02d",recsec/60,recsec%60);
						tempstr[5]='\0';
						sectorsize=0;
						
						printf("¼������\r\n¼��ʱ��: %s \r\n",tempstr);							
						recorder_statue = STATE_WAIT;
						return;	
					}
				}
															
			break;
			
			case STATE_PLAYING:
				
				LED_RGBOFF
				
				/*---------------------------------------------------*/
				vsset.mvol=240;			//������С
				vsset.bflimit=6;		//��������
				vsset.bass=15;			//����Ч��
				vsset.tflimit =10;	//��������
				vsset.treble =15;		//����Ч��
				vsset.effect =0;		//�޶���Ч��
				vsset.speed =1;			//�����ٶ�			
			
				VS_Restart_Play();  					
				VS_Set_All();        							 
				VS_Reset_DecodeTime();
				VS_SPI_SpeedHigh();	
				/*---------------------------------------------------*/
		
				result=f_open(&file,last_record_pathname,FA_READ);	
				if(result != 0)
				{
					printf("f_open error : %d",result);
					Rest_Statue();
					return;
				}	
																
				while(1)
				{		
					LED2_TOGGLE;						
					result=f_read(&file,buffer,PLAY_BUFSIZE,(UINT*)&bw);
					if(result !=0 )
					{
						f_close(&file);
						printf("f_read error: %d",result);
						printf("ֹͣ����\r\n");
						Rest_Statue();	
						return;	 				
					}
						
					i=0;					
					do
					{  	
						if(VS_Send_MusicData(buffer+i)==0)
						{
							i+=32;
						}			
				
						if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
						{
							f_close(&file);
							printf("ֹͣ����\r\n");
							recorder_statue = STATE_WAIT;
							return;	 					
						} 
						
					}while(i<bw);
					
					if(bw!=PLAY_BUFSIZE||result!=0)
					{
						f_close(&file);
						printf("���Ž���\r\n");
						recorder_statue = STATE_WAIT;
						return;	  
					}									
				}
									
			break;
		
			default:
			break;
		}	
	}
	
}
