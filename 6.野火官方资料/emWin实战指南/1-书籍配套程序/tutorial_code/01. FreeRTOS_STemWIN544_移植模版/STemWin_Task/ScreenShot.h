#ifndef __SCREEN_SHOT_H
#define __SCREEN_SHOT_H

#include "GUI.h"

/******************* �����õ���ȫ�ֱ��� *******************/
extern FATFS   fs;			/* FatFs�ļ�ϵͳ���� */
extern FIL     file;		/* file objects */
extern UINT    bw;     /* File R/W count */
extern FRESULT result; 
extern FILINFO fno;
extern DIR dir;

void FS_Init(void);
void ScreenShot(void);
void _WriteByte2File(U8 Data, void *p);

#endif /* __SCREEN_SHOT_H */
