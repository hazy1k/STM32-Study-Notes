/**
  *********************************************************************
  * @file    GUI_DEMO_FileExpoler_Port.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �ļ����������Ҫʹ�õ��м��
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */
#ifndef	__GUI_DEMO_FILEEXPOLER_PORT_H__
#define	__GUI_DEMO_FILEEXPOLER_PORT_H__

#include "def.h"

/* ��صĺ��װ */
#define FILE_ATTRIBUTE_DIRECTORY_PORT	 0x00000010

/* ��Ӧ��ֻ���������ṹ���Աֵ��
*  �����и�����Ҫ���
*/
typedef struct _EXPOLER_DATA {
	ULONG	dwFileAttributes;
	WCHAR  cFileName[260];

}EXPOLER_DATA, *PEXPOLER_DATA;


/* ��ȡĿ¼��Ϣ�Ľӿ� */
void *FindFirstFile_Port(char *lpFileName, PEXPOLER_DATA lpExpolerData);
BOOL FindNextFile_Port(void *hFindFile, PEXPOLER_DATA lpExpolerData);
BOOL FindClose_Port(void *hFindFile);

#endif	/*__GUI_DEMO_FILEEXPOLER_PORT_H__*/