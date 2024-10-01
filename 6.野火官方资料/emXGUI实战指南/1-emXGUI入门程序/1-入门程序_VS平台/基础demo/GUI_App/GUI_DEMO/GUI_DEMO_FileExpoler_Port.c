
/*
 *CDlg_FileExpoler.c
 *2018年3月5日下午2:14:42
 *
 */

//#include "emXGUI.h"
//#include "x_libc.h"
#include <windows.h>
#include	<stdio.h>
#include "GUI_DEMO_FileExpoler_Port.h"

/*============================================================================*/
#if 0

////搜索指定目录下的文件，并增加到Listbox中.
void scan_file(const char *path, HWND hListWnd)
{
#if 0
	int i;
	DIR dir;
	FRESULT res;
	FILINFO fno;
	char *fn;        // 文件名	
	char nbuf[_MAX_LFN];
	WCHAR wbuf[_MAX_LFN * 2];

#if _USE_LFN 
	/* 长文件名支持 */
	fno.lfname = nbuf;
	fno.lfsize = sizeof(nbuf);
#endif   
	//打开目录
	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		/* 读取目录信息至fno */
		while (f_readdir(&dir, &fno) == FR_OK)
		{
			/* 若为0表示读完 */
			if (fno.fname[0] == 0)
			{
				break;
			}

			/* 使用长文件名需要进行如下处理 */
#if _USE_LFN 
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else 
			fn = fno.fname;
#endif       

			//x_sprintf(buf,"%s/%s",path,nbuf);	//格式化成完整的目录字符串
			x_mbstowcs_cp936(wbuf, fn, sizeof(wbuf));	//将Ansi字符转换成GUI的unicode字符.

			////在Listbox中增加一个Item项，记录文件名和文件属性.
			i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
			SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
			/* 通过设置Item项的DATA值来记录文件的属性(用于区分是文件夹和文件). */
			SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)fno.fattrib);

		}
		f_closedir(&dir);
	}
#else
	HANDLE hFind;
	WIN32_FIND_DATA pNextInfo;  //搜索得到的文件信息将储存在pNextInfo中; 
	int i;

	//FRESULT res;
	//FILINFO fno;
	//char *fn;        // 文件名	
	char nbuf[256];
	WCHAR wbuf[256 * 2];

	hFind = FindFirstFile(path, &pNextInfo);

	do
	{
		if (pNextInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			printf(" <DIR> %s", pNextInfo.cFileName);
		}
		else
		{
			printf(" <FILE> %s", pNextInfo.cFileName);

			////x_sprintf(buf,"%s/%s",path,nbuf);	//格式化成完整的目录字符串
			//x_mbstowcs_cp936(wbuf, nbuf, 512);	//将Ansi字符转换成GUI的unicode字符.

			//////在Listbox中增加一个Item项，记录文件名和文件属性.
			//i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
			//SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
			////通过设置Item项的DATA值来记录文件的属性(用于区分是文件夹和文件).
			//SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)pNextInfo.dwFileAttributes); 
		}
	} while (FindNextFile(hFind, &pNextInfo));

	FindClose(hFind);
	//
	//
	//	//打开目录
	//	res = f_opendir(&dir, path);
	//	if (res == FR_OK)
	//	{
	//		/* 读取目录信息至fno */
	//		while (f_readdir(&dir, &fno) == FR_OK)
	//		{
	//			/* 若为0表示读完 */
	//			if (fno.fname[0] == 0)
	//			{
	//				break;
	//			}
	//
	//			/* 使用长文件名需要进行如下处理 */
	//#if _USE_LFN 
	//			fn = *fno.lfname ? fno.lfname : fno.fname;
	//#else 
	//			fn = fno.fname;
	//#endif       
	//
	//			//x_sprintf(buf,"%s/%s",path,nbuf);	//格式化成完整的目录字符串
	//			x_mbstowcs_cp936(wbuf, fn, sizeof(wbuf));	//将Ansi字符转换成GUI的unicode字符.
	//
	//			////在Listbox中增加一个Item项，记录文件名和文件属性.
	//			i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
	//			SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
	//			/* 通过设置Item项的DATA值来记录文件的属性(用于区分是文件夹和文件). */
	//			SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)fno.fattrib);
	//
	//		}
	//		f_closedir(&dir);
	//	}
#endif
}
/*============================================================================*/
#endif

void* GUI_VMEM_Alloc(unsigned int size);
void GUI_VMEM_Free(void *p);

WIN32_FIND_DATA FindFileData;

/* windows读取目录的接口 */
void *FindFirstFile_Port(char *lpFileName, PEXPOLER_DATA lpExpolerData)
{
	void* handle;
	//*lpFindFileData =  GUI_VMEM_Alloc(sizeof(WIN32_FIND_DATA));
	//WIN32_FIND_DATA FindFileData;

	////PEXPOLER_DATA lpExpolerData = GUI_VMEM_Alloc(sizeof(EXPOLER_DATA));
	handle = FindFirstFile(lpFileName, &FindFileData);
	lpExpolerData->dwFileAttributes = FindFileData.dwFileAttributes;
	//lpExpolerData->cFileName = FindFileData.cFileName;
	strcpy(lpExpolerData->cFileName, FindFileData.cFileName);

	return handle;
}

BOOL FindNextFile_Port(void *hFindFile, PEXPOLER_DATA lpExpolerData)
{
	BOOL result;
	//WIN32_FIND_DATA FindFileData;

	result =  FindNextFile(hFindFile, &FindFileData);

	lpExpolerData->dwFileAttributes = FindFileData.dwFileAttributes;
	//lpExpolerData->cFileName = FindFileData.cFileName;
	strcpy(lpExpolerData->cFileName, FindFileData.cFileName);
	return result;
}

BOOL FindClose_Port(void *hFindFile)
{
	//GUI_VMEM_Free(lpFindFileData);
	return FindClose(hFindFile);
}


/*============================================================================*/
