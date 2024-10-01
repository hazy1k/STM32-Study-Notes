#ifndef __AVIFILE_H
#define __AVIFILE_H

#include "stm32f4xx.h"
#include "ff.h"

//#define DEBUGINFO	//��Ϣ��ӡ����

#define RIFF_ID		0x52494646
#define AVI_ID		0x41564920
#define LIST_ID		0x4c495354
#define hdrl_ID		0x6864726c
#define avih_ID		0x61766968
#define strl_ID		0x7374726c
#define strh_ID		0x73747268
#define strf_ID		0x73747266
#define movi_ID		0x6d6f7669

typedef struct
{	
  DWORD  RIFFchunksize;
  DWORD  LISTchunksize;
	DWORD  avihsize;
	DWORD  strlsize;
	DWORD  strhsize;
}AVI_TypeDef;

typedef struct
{	
	DWORD  SecPerFrame;	//��ʾÿ֡��ʱ��
	DWORD  MaxByteSec; 	//������ݴ�����
	DWORD  ChunkBase; 	//�鳤��Ϊ��ֵ�ı�����ͨ��Ϊ2048
	DWORD  SpecProp;	  //AVI�ļ�����������
	DWORD  TotalFrame;	//�ļ���֡��
	DWORD  InitFrames;  //��ʼ����ǰ�ĳ�ʼ��֡��
	DWORD  Streams;		  //����������������
	DWORD  RefBufSize;  //����ʹ�õĻ�������С��ͨ��Ϊ�洢һ֡ͼ����ͬ����Ƶ����֮��
	DWORD  Width;		    //ͼ���
	DWORD  Height;		  //ͼ���
	DWORD  Reserved;	  //����
}avih_TypeDef;

typedef struct
{	
	uint8_t StreamType[4];//���������࣬vids(0x73646976):��Ƶ,auds:��Ƶ
	uint8_t Handler[4];	  //���ݽ�ѹ���������������
	DWORD   StreamFlag;   //����������
	WORD    Priority;	    //�����������ŵ����ȼ�
	WORD    Language;	    //��Ƶ�����Դ���
	DWORD   InitFrames;   //��ʼ����ǰ�ĳ�ʼ��֡��
	DWORD   Scale;		    //��Ƶÿ֡�Ĵ�С����Ƶ�Ĳ�����
	DWORD   Rate; 		    //Scale/Rate=ÿ�������
	DWORD   Start;		    //��������ʼ���ŵ�λ�ã���λΪScale
	DWORD   Length;		    //������������������λΪScale
 	DWORD   RefBufSize;   //����ʹ�õĻ�������С
  DWORD   Quality;		  //��ѹ������������ֵԽ������Խ��
	DWORD   SampleSize;	  //��Ƶ��������С
	struct{				
	  short int  Left;
		short int  Top;
		short int  Right;
		short int  Bottom;
	}Frame;				        //��Ƶ֡��ռ�ľ��� 
}strh_TypeDef;

typedef struct tagBMPHEADER
{
  DWORD  	Size;		      //���ṹ��ռ���ֽ���
	long   	Width;		    //ͼ���
	long   	Height;		    //ͼ���
	WORD   	Planes;		    //ƽ����������Ϊ1
	WORD   	BitCount;	    //����λ��
	u8  	Compression[4];	//ѹ�����ͣ�MJPEG
	DWORD  	SizeImage;	  //ͼ���С
	long   	XpixPerMeter; //ˮƽ�ֱ���
	long   	YpixPerMeter; //��ֱ�ֱ���
	DWORD  	ClrUsed;		  //ʵ��ʹ���˵�ɫ���е���ɫ��,ѹ����ʽ�в�ʹ��
	DWORD  	ClrImportant; //��Ҫ����ɫ
}BMPHEADER;             //�ýṹռ��40�ֽ�

typedef struct tagRGBQUAD 
{
	BYTE  rgbBlue;	      //��ɫ������(ֵ��ΧΪ0-255)
	BYTE  rgbGreen;       //��ɫ������(ֵ��ΧΪ0-255)
	BYTE  rgbRed; 	      //��ɫ������(ֵ��ΧΪ0-255)
	BYTE  rgbReserved;    //����������Ϊ0
}RGBQUAD;               //��ɫ��

typedef struct tagBMPINFO 
{
	BMPHEADER  bmiHeader;  //λͼ��Ϣͷ
	RGBQUAD    bmColors[1];//��ɫ��
}BMPINFO;  

typedef struct 
{
  WORD  FormatTag;
	WORD  Channels;	  //������
	DWORD SampleRate; //������
	DWORD BaudRate;   //������
	WORD  BlockAlign; //���ݿ�����־
	WORD  Size;       //�ýṹ��С
}WAVEFORMAT;

#define	 MAKEWORD(ptr)	(WORD)(((WORD)*((BYTE*)(ptr))<<8)|(WORD)*(BYTE*)((ptr)+1))
#define  MAKEDWORD(ptr)	(DWORD)(((WORD)*(BYTE*)(ptr)|(((WORD)*(BYTE*)(ptr+1))<<8)|\
						(((WORD)*(BYTE*)(ptr+2))<<16)|(((WORD)*(BYTE*)(ptr+3))<<24))) 


uint8_t  AVI_Parser(uint8_t *buffer);
uint8_t  Avih_Parser(uint8_t *buffer);
uint8_t  Strl_Parser(uint8_t *buffer);
uint8_t  Strf_Parser(uint8_t *buffer);
uint16_t Search_Movi(uint8_t* buffer);
uint16_t Search_Fram(uint8_t* buffer,uint32_t size);
uint16_t Search_Auds(uint8_t* buffer);
uint32_t ReadUnit(uint8_t *buffer,uint8_t index,uint8_t Bytes,uint8_t Format);
extern int img_w, img_h;
#endif

