/**
  *********************************************************************
  * @file    GUI_DEMO_FileExpoler_Port.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   文件浏览器范例要使用的中间件
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */
#ifndef	__GUI_DEMO_FILEEXPOLER_PORT_H__
#define	__GUI_DEMO_FILEEXPOLER_PORT_H__

#include "def.h"

/* 相关的宏封装 */
#define FILE_ATTRIBUTE_DIRECTORY_PORT	 0x00000010

/* 本应用只关心两个结构体成员值，
*  可自行根据需要添加
*/
typedef struct _EXPOLER_DATA {
	ULONG	dwFileAttributes;
	WCHAR  cFileName[260];

}EXPOLER_DATA, *PEXPOLER_DATA;


/* 读取目录信息的接口 */
void *FindFirstFile_Port(char *lpFileName, PEXPOLER_DATA lpExpolerData);
BOOL FindNextFile_Port(void *hFindFile, PEXPOLER_DATA lpExpolerData);
BOOL FindClose_Port(void *hFindFile);

#endif	/*__GUI_DEMO_FILEEXPOLER_PORT_H__*/