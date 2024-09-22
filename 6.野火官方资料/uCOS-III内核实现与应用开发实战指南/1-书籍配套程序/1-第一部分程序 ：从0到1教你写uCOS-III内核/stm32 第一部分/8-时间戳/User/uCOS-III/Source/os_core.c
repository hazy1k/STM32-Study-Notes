#include "os.h"

/* �����б��ʼ�� */
void OS_RdyListInit(void)
{
	OS_PRIO i;
	OS_RDY_LIST *p_rdy_list;
	
	for( i=0u; i<OS_CFG_PRIO_MAX; i++ )
	{
		p_rdy_list = &OSRdyList[i];
		p_rdy_list->HeadPtr = (OS_TCB *)0;
		p_rdy_list->TailPtr = (OS_TCB *)0;
	}
}

/* RTOS��ʼ��
** ��ʼ��ȫ�ֱ���
*/
void OSInit (OS_ERR *p_err)
{
	/* ����OS��ʼ״̬Ϊֹ̬ͣ */
	OSRunning =  OS_STATE_OS_STOPPED;
	
	/* ��ʼ������ȫ��TCB��������TCB���������л� */
	OSTCBCurPtr = (OS_TCB *)0;
	OSTCBHighRdyPtr = (OS_TCB *)0;
	
	/* ��ʼ�������б� */
	OS_RdyListInit();
	
	/* ��ʼ���������� */
	OS_IdleTaskInit(p_err);
	if (*p_err != OS_ERR_NONE) 
	{
        return;
    }
}

/* ����RTOS�������ٷ��� */
void OSStart (OS_ERR *p_err)
{	
	if( OSRunning == OS_STATE_OS_STOPPED )
	{
		/* �ֶ���������1������ */
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
		
		/* ���������л������᷵�� */
		OSStartHighRdy();
		
		/* �������е�������е������ʾ�����������Ĵ��� */
		*p_err = OS_ERR_FATAL_RETURN;
	}
	else
	{
		*p_err = OS_STATE_OS_RUNNING;
	}
}

/* �����л���ʵ�ʾ��Ǵ���PendSV�쳣��Ȼ����PendSV�쳣�н����������л� */
void OSSched(void)
{
#if 0	/* �ǳ��򵥵�������ȣ�������������ִ�� */
	if( OSTCBCurPtr == OSRdyList[0].HeadPtr )
	{
		OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
	}
	else
	{
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
	}
#endif
	
	/* �����ǰ�����ǿ���������ô��ȥ����ִ������1��������2���������ǵ���ʱʱ���Ƿ����
	   ����������ʱʱ���û�е��ڣ��Ǿͷ��ؼ���ִ�п������� */
	if( OSTCBCurPtr == &OSIdleTaskTCB )
	{
		if(OSRdyList[0].HeadPtr->TaskDelayTicks == 0)
		{
			OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
		}
		else if(OSRdyList[1].HeadPtr->TaskDelayTicks == 0)
		{
			OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
		}
		else
		{
			return;		/* ������ʱ��û�е����򷵻أ�����ִ�п������� */
		} 
	}
	else
	{
		/*�����task1����task2�Ļ������������һ������,����������������ʱ�У����л���������
        �����ж��µ�ǰ�����Ƿ�Ӧ�ý�����ʱ״̬������ǵĻ������л����������񡣷���Ͳ������κ��л� */
		if(OSTCBCurPtr == OSRdyList[0].HeadPtr)
		{
			if(OSRdyList[1].HeadPtr->TaskDelayTicks == 0)
			{
				OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
			}
			else if(OSTCBCurPtr->TaskDelayTicks != 0)
			{
				OSTCBHighRdyPtr = &OSIdleTaskTCB;
			}
			else 
			{
				return;		/* ���أ��������л�����Ϊ�������񶼴�����ʱ�� */
			}
		}
		else if(OSTCBCurPtr == OSRdyList[1].HeadPtr)
		{
			if(OSRdyList[0].HeadPtr->TaskDelayTicks == 0)
			{
				OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
			}
			else if(OSTCBCurPtr->TaskDelayTicks != 0)
			{
				OSTCBHighRdyPtr = &OSIdleTaskTCB;
			}
			else 
			{
				return;		/* ���أ��������л�����Ϊ�������񶼴�����ʱ�� */
			}
		}
	}
	
	/* �����л� */
	OS_TASK_SW();
}


/* �������� */
void  OS_IdleTask (void  *p_arg)
{
	p_arg = p_arg;
	
	/* ��������ʲô��������ֻ��ȫ�ֱ���OSIdleTaskCtr ++ ���� */
	for(;;)
	{
		OSIdleTaskCtr++;
	}
}

/* ���������ʼ�� */
void  OS_IdleTaskInit(OS_ERR  *p_err)
{	
	/* ��ʼ��������������� */
	OSIdleTaskCtr = (OS_IDLE_CTR)0;
	
	/* ������������ */
	OSTaskCreate( (OS_TCB     *)&OSIdleTaskTCB, 
			      (OS_TASK_PTR )OS_IdleTask, 
			      (void       *)0,
			      (CPU_STK    *)OSCfg_IdleTaskStkBasePtr,
			      (CPU_STK_SIZE)OSCfg_IdleTaskStkSize,
			      (OS_ERR     *)p_err );
}



