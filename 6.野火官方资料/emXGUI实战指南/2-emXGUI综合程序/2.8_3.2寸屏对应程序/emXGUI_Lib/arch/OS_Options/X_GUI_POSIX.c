
#if 1
//#ifdef	X_GUI_USE_POSIX
/*===================================================================================*/

#include <semaphore.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>

#include	"emXGUI.h"
#include	"emXGUI_Arch.h"

/*===================================================================================*/
/*
��������: ����һ������(�û���������֧��Ƕ��ʹ��)
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{

	pthread_mutexattr_t attr;
	pthread_mutex_t *mutex = GUI_MEM_Alloc(sizeof(pthread_mutex_t));

	if(mutex!=NULL)
	{
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(mutex, &attr);
    
		pthread_mutexattr_destroy(&attr);
	}
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
	pthread_mutex_lock(hMutex);
	
	return	TRUE;
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
	pthread_mutex_unlock(hMutex);
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
	pthread_mutex_destroy(hMutex);
	
	GUI_MEM_Free(hMutex);
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
	sem_t *sem = GUI_MEM_Alloc(sizeof(sem_t));;
	
	if(sem!=NULL)
	{
		sem_init(sem, 0, init);
	}
	
	return	(GUI_SEM*)sem;
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
	sem_wait(hsem);
	
	return TRUE;
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
	sem_post(hsem);
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
	sem_destroy(hsem);
	
	GUI_MEM_Free(hsem);
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
	return	(HANDLE)pthread_self();
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
    struct timeval time;

    gettimeofday(&time, NULL);
    
	return	 time.tv_sec*1000+ time.tv_usec/1000;
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
	msleep(1);
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
	msleep(ms);
}


/*===================================================================================*/
#endif
