#ifndef __MP3PLAYER_H__
#define __MP3PLAYER_H__

#include <inttypes.h>
#include "emXGUI.h"


#define RECBUFFER_SIZE  1024*8
#define FMT_COUNT	6		/* ��Ƶ��ʽ����Ԫ�ظ��� */
/* ״̬ */
typedef enum 
{
	STA_IDLE = 0,	/* ����״̬ */
	STA_PLAYING,	/* ����״̬ */
   STA_SWITCH,   /* �и�״̬ */
	STA_ERR,			/*  error  */
}Music_State_t;

typedef struct
{
	uint8_t ucVolume;			/* ��ǰ�������� */
	uint8_t ucStatus;			/* ״̬��0��ʾ������1��ʾ�����У�2 ���� */	
	uint32_t ucFreq;			/* ����Ƶ�� */
     uint32_t ucbps;            /* ������ ��ÿ�봫�Ͷ��ٸ�λ */
}MP3_TYPE;	

typedef struct
{
	uint8_t ucInput;			/* ����Դ��0:MIC, 1:������ */
	uint8_t ucFmtIdx;			/* ��Ƶ��ʽ����׼��λ��������Ƶ�� */
	uint8_t ucVolume;			/* ��ǰ�������� */
	uint8_t ucGain;			  /* ��ǰ���� */	
	uint8_t ucStatus;			/* ¼����״̬��0��ʾ������1��ʾ¼���У�2��ʾ������ */	
}REC_TYPE;	

/* WAV�ļ�ͷ��ʽ */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*///4
	uint32_t	size_8;						/* ���¸���ַ��ʼ���ļ�β�����ֽ���	*///4
	uint32_t	wave;							/* = "WAVE" 0x45564157*///4
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*///4
	uint32_t	fmtSize;					/* ��һ���ṹ��Ĵ�С(һ��Ϊ16) *///4	 
	uint16_t	wFormatTag;				/* ���뷽ʽ,һ��Ϊ1	*///2
	uint16_t	wChannels;				/* ͨ������������Ϊ1��������Ϊ2 *///2
	uint32_t	dwSamplesPerSec;	/* ������ *///4
	uint32_t	dwAvgBytesPerSec;	/* ÿ���ֽ���(= ������ �� ÿ���������ֽ���) *///4
	uint16_t	wBlockAlign;			/* ÿ���������ֽ���(=����������/8*ͨ����) *///2
	uint16_t	wBitsPerSample;		/* ����������(ÿ��������Ҫ��bit��) *///2
																			   
	uint32_t	data;							/* = "data" 0x61746164*///4
	uint32_t	datasize;					/* �����ݳ��� *///4
} WavHead;
extern MP3_TYPE mp3player;         /* mp3�����豸 */
void mp3PlayerDemo(const char *mp3file, uint8_t vol, HDC hdc);
void wavplayer(const char *wavfile, uint8_t vol, HDC hdc);
extern int time2exit;
extern GUI_SEM *exit_sem;
extern uint32_t File_HeaderTime;//����ʱ��ƫ��

extern Music_State_t Music_State;
void vs1053_player(uint8_t *filepath, uint8_t vol, HDC hdc);


#endif  /* __MP3PLAYER_H__   */

