
#if 1
//#ifdef	X_GUI_USE_UCOSII

/*===================================================================================*/
#include	<stddef.h>

#include "ucos_ii.h"

#include	"emXGUI.h"
#include	"emXGUI_Arch.h"


struct tagGUI_MUTEX
{
	OS_EVENT *event;
	OS_TCB *tcb;
	U32 hold;
};

/*===================================================================================*/

static U32 ms_to_tick(U32 ms)
{
	return (ms*OS_TICKS_PER_SEC)/1000;
}

/*===================================================================================*/
/*
��������: ����һ������(�û���������֧��Ƕ��ʹ��)
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{

	GUI_MUTEX *m;
	
	m =(GUI_MUTEX*)GUI_MEM_Alloc(sizeof(GUI_MUTEX));

	m->event =OSSemCreate(1);
	m->tcb =OSTCBCur;
	m->hold  =0;
	
	return	m;
	
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
	INT8U err=OS_ERR_NONE;
	
	if(time==0UL)
	{
		time=0xFFFFFFFE;
	}
	else if(time==(U32)0xFFFFFFFF)
	{
		time =0;
	}
	else
	{
		time =ms_to_tick(time);
	}
	
	if(OSTCBCur == hMutex->tcb)
	{	
		if(hMutex->hold++ == 0)
		{
			OSSemPend(hMutex->event,time,&err);
		}
	}
	else
	{
		OSSemPend(hMutex->event,time,&err);
		hMutex->tcb =OSTCBCur;
		hMutex->hold =1;
	}

	return TRUE;

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
	if(OSTCBCur == hMutex->tcb)
	{
		if(hMutex->hold > 0)
		{
			if(--hMutex->hold == 0)
			{
				OSSemPost(hMutex->event);
			}
		}
	}
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
	INT8U err;
	OSSemDel(hMutex->event,OS_DEL_ALWAYS,&err);
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
	OS_EVENT *sem;
	
	sem =OSSemCreate(init);
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
	INT8U err;
	
	if(time==0UL)
	{
		time=0xFFFFFFFE;
	}
	else if(time==(U32)0xFFFFFFFF)
	{
		time =0;
	}
	else
	{
		time =ms_to_tick(time);
	}

	OSSemPend((OS_EVENT*)hsem,time,&err);

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
	OSSemPost((OS_EVENT*)hsem);
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
	INT8U err;
	OSSemDel((OS_EVENT*)hsem,OS_DEL_ALWAYS,&err);
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
	return	(HANDLE)OSTCBCur;
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
	return (OSTime*1000)/OS_TICKS_PER_SEC;
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
	OSTimeDly(1);
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
	if(ms==0)
	{
		ms=1;
	}
	OSTimeDlyHMSM(0,0,0,ms);
}


/*===================================================================================*/
#endif
