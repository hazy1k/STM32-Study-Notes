
#if 1
//#ifdef	X_GUI_USE_WIN32

#include	<stddef.h>
#include 	<windows.h>
#include 	<winbase.h>

/*===================================================================================*/

typedef	unsigned int U32;

typedef	struct tagGUI_MUTEX		GUI_MUTEX;
typedef	struct tagGUI_SEM		GUI_SEM;

/*===================================================================================*/
/*
��������: ����һ������(�û���������֧��Ƕ��ʹ��)
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{
	HANDLE mutex;

	mutex = CreateMutex(NULL,FALSE,NULL);
	return	(GUI_MUTEX*)mutex;
}

/*===================================================================================*/
/*
��������: ��������
����: hMutex(��GUI_MutexCreate���صľ��); 
      time ��ȴ�������,0���ȷ���,0xFFFFFFFF,һֱ�ȴ�
����: TRUE:�ɹ�;FALSE:ʧ�ܻ�ʱ
˵��: .
*/

BOOL	GUI_MutexLock(GUI_MUTEX *hMutex,U32 time)
{
	DWORD	nRet = WaitForSingleObject((HANDLE)hMutex,time);
	if(GetLastError() == WAIT_OBJECT_0)
	{
		return TRUE;
	}

	return	FALSE;
}

/*===================================================================================*/
/*
��������: �������
����: hMutex(��GUI_MutexCreate���صľ��);    
����: ��
˵��: .
*/

void	GUI_MutexUnlock(GUI_MUTEX *hMutex)
{
	ReleaseMutex((HANDLE)hMutex);
}

/*===================================================================================*/
/*
��������: ����ɾ��
����: hMutex(��GUI_MutexCreate���صľ��);    
����: ��
˵��: .
*/

void	GUI_MutexDelete(GUI_MUTEX *hMutex)
{
	CloseHandle((HANDLE)hMutex);
}

/*===================================================================================*/
/*
��������: ����һ���ź���
����: init: �ź�����ʼֵ; max: �ź������ֵ
����: �ź���������(Ψһ��ʶ)
˵��: �ź�����������ʵ��OS����,������ָ��,ID�ŵ�...
*/

GUI_SEM*	GUI_SemCreate(int init,int max)
{
	HANDLE	hSema = CreateSemaphore(NULL,init,max,NULL);
	return (GUI_SEM*)hSema;
}

/*===================================================================================*/
/*
��������: �ź����ȴ�
����: hsem(��GUI_SemCreate���صľ��); 
      time ��ȴ�������,0���ȷ���,0xFFFFFFFF,һֱ�ȴ�
����: TRUE:�ɹ�;FALSE:ʧ�ܻ�ʱ
˵��: .
*/

BOOL	GUI_SemWait(GUI_SEM *hsem,U32 time)
{
	DWORD	nRet = WaitForSingleObject(hsem,time);
	if(GetLastError() == WAIT_OBJECT_0)
	{
		return TRUE;
	}

	return FALSE;
}

/*===================================================================================*/
/*
��������: �ź�������
����: hsem(��GUI_SemCreate���صľ��);  
����: ��
˵��: .
*/

void	GUI_SemPost(GUI_SEM *hsem)
{
	ReleaseSemaphore(hsem,1,NULL);
}

/*===================================================================================*/
/*
��������: �ź���ɾ��
����: hsem(��GUI_SemCreate���صľ��);    
����: ��
˵��: .
*/

void	GUI_SemDelete(GUI_SEM *hsem)
{
	ReleaseSemaphore(hsem,1,NULL);
}

/*===================================================================================*/
/*
��������: ��õ�ǰ�߳̾��(Ψһ��ʶ)
����: ��  
����: ��ǰ�߳�Ψһ��ʶ,��ʵ��OS����,������ָ��,ID�ŵ�...
˵��: .
*/

HANDLE	GUI_GetCurThreadHandle(void)
{
	return GetCurrentThread();
}


/*===================================================================================*/
/*
��������: ��õ�ǰϵͳʱ��(��λ:����)
����: ��  
����: ��ǰϵͳʱ��
˵��: .
*/

U32	GUI_GetTickCount(void)
{
	return GetTickCount();
}

/*===================================================================================*/
/*
��������: ���ʱ�����ó�CPU
����: ��  
����: ��
˵��: ������OS�������,��򵥵ķ�����:OS Delay һ�� tick ����.
*/

void	GUI_Yield(void)
{
	Sleep(1);
}

/*===================================================================================*/
/*
��������: ��ʱ����
����: ms: ��ʱʱ��(��λ:����) 
����: ��
˵��: 
*/

void	GUI_msleep(u32 ms)
{
	Sleep(ms);
}


/*===================================================================================*/
#endif
