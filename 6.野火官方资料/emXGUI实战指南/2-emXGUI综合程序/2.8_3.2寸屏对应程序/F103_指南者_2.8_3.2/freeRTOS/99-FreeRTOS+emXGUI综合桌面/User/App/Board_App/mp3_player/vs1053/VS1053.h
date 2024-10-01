#ifndef __VS1053_H__
#define __VS1053_H__

#include "stm32f10x.h"

//////////////////////////////////////////////////////////////
/*����SPI2��ΪVS1053��Ӳ���ӿ�*/
#define VS_XCS								GPIO_Pin_12								/*����VS1053��Ƭѡ�ܽ�*/
#define VS_SCLK								GPIO_Pin_13								/*����VS1053��ʱ�ӹܽ�*/
#define VS_MISO								GPIO_Pin_14								/*����VS1053��MISO�ܽ�*/
#define VS_MOSI								GPIO_Pin_15								/*����VS1053��MOSI�ܽ�*/
#define VS_SPIGPIO_PORT				GPIOB											/* GPIO�˿� */
#define VS_SPIGPIO_CLK				RCC_APB2Periph_GPIOB			/* GPIO�˿�ʱ�� */
#define VS_SPI								SPI2
#define VS_SPI_CLK						RCC_APB1Periph_SPI2

#define VS_XDCS								GPIO_Pin_9								/*����VS1053��Ƭѡ�ܽ�*/
#define VS_GPIO_XDCS_PORT			GPIOB											/* GPIO�˿� */
#define VS_GPIO_XDCS_CLK			RCC_APB2Periph_GPIOB			/* GPIO�˿�ʱ�� */

#define VS_GPIO_RST_PORT			GPIOC											/* GPIO�˿� */
#define VS_GPIO_RST_CLK				RCC_APB2Periph_GPIOC					/* GPIO�˿�ʱ�� */
#define VS_RST								GPIO_Pin_7								/*����VS1053��RST�ܽ�*/

//#define VS_GPIO_HORN_PORT			GPIOC											/* GPIO�˿� */
//#define VS_GPIO_HORN_CLK		  RCC_APB2Periph_GPIOC			/* GPIO�˿�ʱ�� */
//#define VS_HORN								GPIO_Pin_6								/*����VS1053�����ȿ��ƹܽ�*/

#define VS_GPIO_DREQ_PORT			GPIOC											/* GPIO�˿� */
#define VS_GPIO_DREQ_CLK			RCC_APB2Periph_GPIOC			/* GPIO�˿�ʱ�� */
#define VS_DREQ								GPIO_Pin_6								/*����VS1053��DREQ�ܽ�*/

#define VS_DREQ_IN						GPIO_ReadInputDataBit(VS_GPIO_DREQ_PORT,VS_DREQ)

#define VS_XDCS_SET						GPIO_SetBits(VS_GPIO_XDCS_PORT,VS_XDCS)
#define VS_XDCS_CLR						GPIO_ResetBits(VS_GPIO_XDCS_PORT,VS_XDCS)

#define VS_XCS_SET						GPIO_SetBits(VS_SPIGPIO_PORT,VS_XCS)
#define VS_XCS_CLR						GPIO_ResetBits(VS_SPIGPIO_PORT,VS_XCS)

#define VS_RST_SET						GPIO_SetBits(VS_GPIO_RST_PORT,VS_RST)
#define VS_RST_CLR						GPIO_ResetBits(VS_GPIO_RST_PORT,VS_RST)
//////////////////////////////////////////////////////////////

__packed typedef struct 
{							  
	u8 mvol;		//������,��Χ:0~254
	u8 bflimit;		//����Ƶ���޶�,��Χ:2~15(��λ:10Hz)
	u8 bass;		//����,��Χ:0~15.0��ʾ�ر�.(��λ:1dB)
	u8 tflimit;		//����Ƶ���޶�,��Χ:1~15(��λ:Khz)
	u8 treble;		//����,��Χ:0~15(��λ:1.5dB)(ԭ����Χ��:-8~7,ͨ�������޸���);
	u8 effect;		//�ռ�Ч������.0,�ر�;1,��С;2,�е�;3,���.
  u8 speed;			//�����ٶȣ�0��1:�������ţ�2:2���ٶȣ�3:3���ٶ�...
//	u8 saveflag; 	//�����־,0X0A,�������;����,����δ����	   
}_vs1053_obj;

__packed typedef struct
{
	u8 input;						//����ͨ��ѡ��.0��MICP��1��LINE1
	u8 samplerate;			//��������ѡ��x*8K����1��8K��2��16K��3��24K...
	u8 channel;					//������1��˫������2����������3��������
	u8 agc;							//���棺1~64
}_recorder_obj;


extern _vs1053_obj vsset;		//VS1053����

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
//VS1053�Ĵ�������
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f 

#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 		 

#define I2S_CONFIG			0XC040
#define GPIO_DDR			0XC017
#define GPIO_IDATA			0XC018
#define GPIO_ODATA			0XC019



u16  VS_RD_Reg(u8 address);				//���Ĵ���
u16  VS_WRAM_Read(u16 addr);	    	//��RAM
void VS_WRAM_Write(u16 addr,u16 val);	//дRAM
void VS_WR_Data(u8 data);				//д����
void VS_WR_Cmd(u8 address,u16 data);	//д����
u8   VS_HD_Reset(void);			    	//Ӳ��λ
void VS_Soft_Reset(void);           	//��λ
u16 VS_Ram_Test(void);             		//RAM����	    
void VS_Sine_Test(void);            	//���Ҳ���
												
void VS_SPI_SpeedHigh(void);
void VS_SPI_SpeedLow(void);
u8 	 VS_SPI_ReadWriteByte(u8 data);
void VS_Init(void);						//��ʼ��VS10XX	 
void VS_Set_Speed(u8 t);				//���ò����ٶ�
u16  VS_Get_HeadInfo(void);     		//�õ�������
u32 VS_Get_ByteRate(void);				//�õ��ֽ�����
u16 VS_Get_EndFillByte(void);			//�õ�����ֽ�
u8 	 VS_Send_MusicData(u8* buf);		//��VS10XX����32�ֽ�
void VS_Restart_Play(void);				//���¿�ʼ��һ�׸貥��	  
void VS_Reset_DecodeTime(void);			//�������ʱ��
u16  VS_Get_DecodeTime(void);   		//�õ�����ʱ��

void VS_Load_Patch(u16 *patch,u16 len);	//�����û�patch
//u8 	 VS_Get_Spec(u16 *p);       		//�õ���������	   
//void VS_Set_Bands(u16 *buf,u8 bands);	//��������Ƶ��
void VS_Set_Vol(u8 volx);				//����������   
void VS_Set_Bass(u8 bfreq,u8 bass,u8 tfreq,u8 treble);//���øߵ���
void VS_Set_Effect(u8 eft);				//������Ч
void VS_Set_All(void);
void VS_Horn_Set(FunctionalState State);

/*--------------  ������VS1053�������� -------------------------*/
/*--------------  ���濪ʼ�����ֲ��Ų��� -------------------------*/
#define BUFSIZE				(512)
void vs1053_player_song(uint8_t *filepath);


/*--------������¼������------------*/
 //RIFF��
typedef __packed struct
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"RIFF",��0X46464952
    u32 ChunkSize ;		   	//���ϴ�С;�ļ��ܴ�С-8
    u32 Format;	   			//��ʽ;WAVE,��0X45564157
}ChunkRIFF ;
//fmt��
typedef __packed struct
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"fmt ",��0X20746D66
    u32 ChunkSize ;		   	//�Ӽ��ϴ�С(������ID��Size);����Ϊ:20.
    u16 AudioFormat;	  	//��Ƶ��ʽ;0X10,��ʾ����PCM;0X11��ʾIMA ADPCM
	u16 NumOfChannels;		//ͨ������;1,��ʾ������;2,��ʾ˫����;
	u32 SampleRate;			//������;0X1F40,��ʾ8Khz
	u32 ByteRate;			//�ֽ�����; 
	u16 BlockAlign;			//�����(�ֽ�); 
	u16 BitsPerSample;		//�����������ݴ�С;4λADPCM,����Ϊ4
//	u16 ByteExtraData;		//���ӵ������ֽ�;2��; ����PCM,û���������
//	u16 ExtraData;			//���ӵ�����,�����������ݿ��С;0X1F9:505�ֽ�  ����PCM,û���������
}ChunkFMT;	   
//fact�� 
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"fact",��0X74636166;
    u32 ChunkSize ;		   	//�Ӽ��ϴ�С(������ID��Size);����Ϊ:4.
    u32 NumOfSamples;	  	//����������; 
}ChunkFACT;
//data�� 
typedef __packed struct 
{
    u32 ChunkID;		   	//chunk id;����̶�Ϊ"data",��0X61746164
    u32 ChunkSize ;		   	//�Ӽ��ϴ�С(������ID��Size);�ļ���С-60.
}ChunkDATA;

//wavͷ
typedef __packed struct
{ 
	ChunkRIFF riff;	//riff��
	ChunkFMT fmt;  	//fmt��
	//ChunkFACT fact;	//fact�� ����PCM,û������ṹ��	 
	ChunkDATA data;	//data��		 
}__WaveHeader; 

void recoder_enter_rec_mode(_recorder_obj *recset);
/*-----------  ����¼������ -------------------*/
#endif
