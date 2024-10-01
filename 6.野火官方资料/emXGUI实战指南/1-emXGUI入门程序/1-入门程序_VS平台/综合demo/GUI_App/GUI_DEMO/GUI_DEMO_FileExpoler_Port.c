
/*
 *CDlg_FileExpoler.c
 *2018��3��5������2:14:42
 *
 */

//#include "emXGUI.h"
//#include "x_libc.h"
#include <windows.h>
#include	<stdio.h>
#include "GUI_DEMO_FileExpoler_Port.h"

/*============================================================================*/
#if 0

////����ָ��Ŀ¼�µ��ļ��������ӵ�Listbox��.
void scan_file(const char *path, HWND hListWnd)
{
#if 0
	int i;
	DIR dir;
	FRESULT res;
	FILINFO fno;
	char *fn;        // �ļ���	
	char nbuf[_MAX_LFN];
	WCHAR wbuf[_MAX_LFN * 2];

#if _USE_LFN 
	/* ���ļ���֧�� */
	fno.lfname = nbuf;
	fno.lfsize = sizeof(nbuf);
#endif   
	//��Ŀ¼
	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		/* ��ȡĿ¼��Ϣ��fno */
		while (f_readdir(&dir, &fno) == FR_OK)
		{
			/* ��Ϊ0��ʾ���� */
			if (fno.fname[0] == 0)
			{
				break;
			}

			/* ʹ�ó��ļ�����Ҫ�������´��� */
#if _USE_LFN 
			fn = *fno.lfname ? fno.lfname : fno.fname;
#else 
			fn = fno.fname;
#endif       

			//x_sprintf(buf,"%s/%s",path,nbuf);	//��ʽ����������Ŀ¼�ַ���
			x_mbstowcs_cp936(wbuf, fn, sizeof(wbuf));	//��Ansi�ַ�ת����GUI��unicode�ַ�.

			////��Listbox������һ��Item���¼�ļ������ļ�����.
			i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
			SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
			/* ͨ������Item���DATAֵ����¼�ļ�������(�����������ļ��к��ļ�). */
			SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)fno.fattrib);

		}
		f_closedir(&dir);
	}
#else
	HANDLE hFind;
	WIN32_FIND_DATA pNextInfo;  //�����õ����ļ���Ϣ��������pNextInfo��; 
	int i;

	//FRESULT res;
	//FILINFO fno;
	//char *fn;        // �ļ���	
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

			////x_sprintf(buf,"%s/%s",path,nbuf);	//��ʽ����������Ŀ¼�ַ���
			//x_mbstowcs_cp936(wbuf, nbuf, 512);	//��Ansi�ַ�ת����GUI��unicode�ַ�.

			//////��Listbox������һ��Item���¼�ļ������ļ�����.
			//i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
			//SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
			////ͨ������Item���DATAֵ����¼�ļ�������(�����������ļ��к��ļ�).
			//SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)pNextInfo.dwFileAttributes); 
		}
	} while (FindNextFile(hFind, &pNextInfo));

	FindClose(hFind);
	//
	//
	//	//��Ŀ¼
	//	res = f_opendir(&dir, path);
	//	if (res == FR_OK)
	//	{
	//		/* ��ȡĿ¼��Ϣ��fno */
	//		while (f_readdir(&dir, &fno) == FR_OK)
	//		{
	//			/* ��Ϊ0��ʾ���� */
	//			if (fno.fname[0] == 0)
	//			{
	//				break;
	//			}
	//
	//			/* ʹ�ó��ļ�����Ҫ�������´��� */
	//#if _USE_LFN 
	//			fn = *fno.lfname ? fno.lfname : fno.fname;
	//#else 
	//			fn = fno.fname;
	//#endif       
	//
	//			//x_sprintf(buf,"%s/%s",path,nbuf);	//��ʽ����������Ŀ¼�ַ���
	//			x_mbstowcs_cp936(wbuf, fn, sizeof(wbuf));	//��Ansi�ַ�ת����GUI��unicode�ַ�.
	//
	//			////��Listbox������һ��Item���¼�ļ������ļ�����.
	//			i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
	//			SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
	//			/* ͨ������Item���DATAֵ����¼�ļ�������(�����������ļ��к��ļ�). */
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

/* windows��ȡĿ¼�Ľӿ� */
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
