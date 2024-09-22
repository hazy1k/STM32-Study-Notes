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
	OSRunning =  OS_STATE_OS_STOPPED;
	
	OSTCBCurPtr = (OS_TCB *)0;
	OSTCBHighRdyPtr = (OS_TCB *)0;
	
	OS_RdyListInit();
	
	*p_err = OS_ERR_NONE;
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
void OSSched (void)
{
	if( OSTCBCurPtr == OSRdyList[0].HeadPtr )
	{
		OSTCBHighRdyPtr = OSRdyList[1].HeadPtr;
	}
	else
	{
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
	}
	
	OS_TASK_SW();
}


