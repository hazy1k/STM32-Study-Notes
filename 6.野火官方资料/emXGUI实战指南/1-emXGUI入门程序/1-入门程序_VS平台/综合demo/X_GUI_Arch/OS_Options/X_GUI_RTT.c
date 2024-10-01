
#if 1
//#ifdef	X_GUI_USE_RTTHREAD

#include	<stddef.h>
#include	"emXGUI_Arch.h"

#include  	"rtthread.h"


/*===================================================================================*/
/*
��������: ����һ������(�û���������֧��Ƕ��ʹ��)
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/


GUI_MUTEX*	GUI_MutexCreate(void)
{
	return (GUI_MUTEX*)rt_mutex_create(NULL,RT_IPC_FLAG_FIFO);
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
	
	if(rt_mutex_take((rt_mutex_t)hMutex,rt_tick_from_millisecond(time))==RT_EOK)
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
	rt_mutex_release((rt_mutex_t)hMutex);
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
	rt_mutex_delete((rt_mutex_t)hMutex);
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
	return (GUI_SEM*)rt_sem_create(NULL,init,RT_IPC_FLAG_FIFO);
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

	if(rt_sem_take((rt_sem_t)hsem,rt_tick_from_millisecond(time))== RT_EOK)
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
	rt_sem_release((rt_sem_t)hsem);
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
	rt_sem_delete((rt_sem_t)hsem);
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
	return	(HANDLE)rt_thread_self();
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
	U32 i;
	
	i=rt_tick_get();
	
	return (i*1000)/RT_TICK_PER_SECOND;

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
	//rt_thread_yield();
	rt_thread_delay(2);
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
	ms=rt_tick_from_millisecond(ms);
	rt_thread_delay(ms);
}

/*===================================================================================*/

#endif

