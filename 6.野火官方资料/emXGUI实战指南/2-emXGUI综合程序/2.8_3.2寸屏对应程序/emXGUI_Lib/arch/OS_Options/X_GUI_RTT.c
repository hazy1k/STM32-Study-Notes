/**
  *********************************************************************
  * @file    X_GUI_RTT.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI��RT-Thread����ϵͳ�ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
/* �ڹ���ѡ���ж�������꣬������ʹ�õĲ���ϵͳ */ 
#ifdef	X_GUI_USE_RTTHREAD

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

/**
 * @breif: �����߳�
 * @param name �߳���
 * @param entry �߳���ں���
 * @param parameter �̲߳���
 * @param stack_size �߳�ջ��С����λ�ֽڣ�ע�ⲿ��ϵͳ��Ҫ���е�λת����
 * @param priority �߳����ȼ�
 * @param tick ʱ��Ƭ��ͬ���ȼ������ʱ��Ƭ��ת��
 * @return �Ƿ񴴽��ɹ�
*/
BOOL GUI_Thread_Create(void (*entry)(void *parameter),
                         const char *name,
                         u32  stack_size,
                         void *parameter,
                         u32  priority,
                         u32  tick)
{
  rt_thread_t h;

  h=rt_thread_create((const char *)name,           /* �������� */
                        (void (*)(void *))entry,    /* ������ں��� */
                        (void*          )parameter,  /* ������ں������� */
                        (rt_uint32_t)stack_size,    /* ����ջ��С */
                        (rt_uint8_t)priority,       /* ��������ȼ� */
                        (rt_uint32_t)tick);         /* ʱ��Ƭ���˴�Ĭ�Ϸ��� */
  
  rt_thread_startup(h);		
  
  if(h == RT_NULL)
  {
    GUI_ERROR("GUI Thread Create failed:%s",name);
    return FALSE;
  }
  else
    return TRUE;
}

/**
 * @breif: ɾ���̣߳���ͨ��GUI_GetCurThreadHandle��ȡ��ǰ��������Ϊ�������
 * @return ��
*/
void GUI_Thread_Delete(HANDLE thread)
{
  rt_thread_delete((rt_thread_t) thread);
}

#endif

/********************************END OF FILE****************************/

