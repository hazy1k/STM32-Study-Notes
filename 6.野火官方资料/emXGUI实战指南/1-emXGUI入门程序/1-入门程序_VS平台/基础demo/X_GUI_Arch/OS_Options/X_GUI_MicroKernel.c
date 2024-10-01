#if 1
//#ifdef	X_GUI_USE_MICROKernel

#include	<stddef.h>
#include	"emXGUI_Arch.h"
#include 	"MicroKernel.h"

/*===================================================================================*/
/*
��������: ����һ������
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{
	mk_mutex_t *mutex;
	
	mutex =GUI_MEM_Alloc(sizeof(mk_mutex_t));
	mk_mutex_init(mutex,NULL);
	return	(HANDLE)mutex;
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
	if(	mk_mutex_lock(hMutex,time)==MK_ERR_NONE)
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
	mk_mutex_unlock(hMutex);
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
	mk_mutex_destroy(hMutex);
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
	mk_sem_t *sem;
	
	sem =GUI_MEM_Alloc(sizeof(mk_sem_t));
	mk_sem_init(sem,init,max,NULL);
	return	(HANDLE)sem;
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
	if(mk_sem_wait(hsem,time) == MK_ERR_NONE)
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
	mk_sem_post(hsem);
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
	mk_sem_destroy(hsem);
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
	return	(HANDLE)mk_thread_self();
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
	return	mk_get_time_count(NULL);
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
	mk_sleep(1);
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
	mk_sleep(ms);
}

/*===================================================================================*/

#endif
