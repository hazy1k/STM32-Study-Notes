#include "os.h"

/* ��ʼ��TCB */
void  OS_TaskInitTCB (OS_TCB  *p_tcb)
{
	p_tcb->StkPtr             = (CPU_STK       *)0;
	p_tcb->StkSize            = (CPU_STK_SIZE   )0u;	

	p_tcb->TaskDelayTicks     = (OS_TICK       )0u;
	
	p_tcb->Prio               = (OS_PRIO        )OS_PRIO_INIT;
	
	p_tcb->NextPtr            = (OS_TCB        *)0;
    p_tcb->PrevPtr            = (OS_TCB        *)0;
	
	p_tcb->TickNextPtr        = (OS_TCB        *)0;
    p_tcb->TickPrevPtr        = (OS_TCB        *)0;
    p_tcb->TickSpokePtr       = (OS_TICK_SPOKE *)0;
	
	p_tcb->TickCtrMatch       = (OS_TICK        )0u;
    p_tcb->TickRemain         = (OS_TICK        )0u;
	
	p_tcb->TimeQuanta         = (OS_TICK        )0u;
    p_tcb->TimeQuantaCtr      = (OS_TICK        )0u;	
}

/* �������� */
void OSTaskCreate (OS_TCB        *p_tcb, 
                   OS_TASK_PTR   p_task, 
                   void          *p_arg,
				   OS_PRIO       prio,
                   CPU_STK       *p_stk_base,
                   CPU_STK_SIZE  stk_size,
				   OS_TICK       time_quanta,
                   OS_ERR        *p_err)
{
	CPU_STK       *p_sp;
	CPU_SR_ALLOC();
	
	/* ��ʼ��TCBΪĬ��ֵ */
	OS_TaskInitTCB(p_tcb);

	/* ��ʼ����ջ */
	p_sp = OSTaskStkInit( p_task,
                          p_arg,
						  p_stk_base,
						  stk_size );
	
	p_tcb->Prio = prio;
	
	p_tcb->StkPtr = p_sp;
	p_tcb->StkSize = stk_size;

/* ʱ��Ƭ��س�ʼ�� */	
	p_tcb->TimeQuanta    = time_quanta;
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	p_tcb->TimeQuantaCtr = time_quanta;
#endif

	/* �����ٽ�� */
	OS_CRITICAL_ENTER();
	
	/* ��������ӵ������б� */
	OS_PrioInsert(p_tcb->Prio);		
    OS_RdyListInsertTail(p_tcb);
	
	/* �˳��ٽ�� */
	OS_CRITICAL_EXIT();
	
	*p_err = OS_ERR_NONE;
}


/*
*********************************************************************************************************
*                                           �������ͻָ�����
*********************************************************************************************************
*/
/* ����һ��ָ�������� */
#if OS_CFG_TASK_SUSPEND_EN > 0u
void   OSTaskSuspend (OS_TCB  *p_tcb,
                      OS_ERR  *p_err)
{
    CPU_SR_ALLOC();


#if 0 /* ���ο�ʼ */
#ifdef OS_SAFETY_CRITICAL
/* ��ȫ��飬OS_SAFETY_CRITICAL_EXCEPTION()������Ҫ�û����б�д */
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
/* ������ISR�����е��øú��� */
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {             
       *p_err = OS_ERR_TASK_SUSPEND_ISR;
        return;
    }
#endif

/* ���ܹ���������� */
    if (p_tcb == &OSIdleTaskTCB) {                         
        *p_err = OS_ERR_TASK_SUSPEND_IDLE;
        return;
    }

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
/* ���ܹ����жϴ������� */
    if (p_tcb == &OSIntQTaskTCB) {                          
        *p_err = OS_ERR_TASK_SUSPEND_INT_HANDLER;
        return;
    }
#endif

#endif /* ���ν��� */

    CPU_CRITICAL_ENTER();
	
	/* �Ƿ�����Լ� */
    if (p_tcb == (OS_TCB *)0) 
	{                             
        p_tcb = OSTCBCurPtr;
    }

    if (p_tcb == OSTCBCurPtr) 
	{
        /* �����������ס���ܹ����Լ� */
		if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) 
		{
            CPU_CRITICAL_EXIT();
            *p_err = OS_ERR_SCHED_LOCKED;
            return;
        }
    }

    *p_err = OS_ERR_NONE;
	
	/* ���������״̬����������Ķ��� */
    switch (p_tcb->TaskState) 
	{
        case OS_TASK_STATE_RDY:
             OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT();
             p_tcb->TaskState  =  OS_TASK_STATE_SUSPENDED;
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
             OS_RdyListRemove(p_tcb);
             OS_CRITICAL_EXIT_NO_SCHED();
             break;

        case OS_TASK_STATE_DLY:
             p_tcb->TaskState  = OS_TASK_STATE_DLY_SUSPENDED;
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
             CPU_CRITICAL_EXIT();
             break;

        case OS_TASK_STATE_PEND:
             p_tcb->TaskState  = OS_TASK_STATE_PEND_SUSPENDED;
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
             CPU_CRITICAL_EXIT();
             break;

        case OS_TASK_STATE_PEND_TIMEOUT:
             p_tcb->TaskState  = OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED;
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;
             CPU_CRITICAL_EXIT();
             break;

        case OS_TASK_STATE_SUSPENDED:
        case OS_TASK_STATE_DLY_SUSPENDED:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             p_tcb->SuspendCtr++;
             CPU_CRITICAL_EXIT();
             break;

        default:
             CPU_CRITICAL_EXIT();
             *p_err = OS_ERR_STATE_INVALID;
             return;
    }

    /* �����л� */
	OSSched();
}
#endif

#if OS_CFG_TASK_SUSPEND_EN > 0u
void  OSTaskResume (OS_TCB  *p_tcb,
                    OS_ERR  *p_err)
{
    CPU_SR_ALLOC();


#if 0 /* ���ο�ʼ */
#ifdef OS_SAFETY_CRITICAL
/* ��ȫ��飬OS_SAFETY_CRITICAL_EXCEPTION()������Ҫ�û����б�д */
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
/* ������ISR�����е��øú��� */
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {
       *p_err = OS_ERR_TASK_RESUME_ISR;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
#if OS_CFG_ARG_CHK_EN > 0u
/* �����Լ��ָ��Լ� */
    if ((p_tcb == (OS_TCB *)0) ||                         
        (p_tcb == OSTCBCurPtr)) {
        CPU_CRITICAL_EXIT();
        *p_err = OS_ERR_TASK_RESUME_SELF;
        return;
    }
#endif

#endif /* ���ν��� */

    *p_err  = OS_ERR_NONE;
	/* ���������״̬����������Ķ��� */
    switch (p_tcb->TaskState) 
	{
        case OS_TASK_STATE_RDY:
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_PEND:
        case OS_TASK_STATE_PEND_TIMEOUT:
             CPU_CRITICAL_EXIT();
             *p_err = OS_ERR_TASK_NOT_SUSPENDED;
             break;

        case OS_TASK_STATE_SUSPENDED:
             OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT();
             p_tcb->SuspendCtr--;
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                 p_tcb->TaskState = OS_TASK_STATE_RDY;
                 OS_TaskRdy(p_tcb);
             }
             OS_CRITICAL_EXIT_NO_SCHED();
             break;

        case OS_TASK_STATE_DLY_SUSPENDED:
             p_tcb->SuspendCtr--;
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                 p_tcb->TaskState = OS_TASK_STATE_DLY;
             }
             CPU_CRITICAL_EXIT();
             break;

        case OS_TASK_STATE_PEND_SUSPENDED:
             p_tcb->SuspendCtr--;
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                 p_tcb->TaskState = OS_TASK_STATE_PEND;
             }
             CPU_CRITICAL_EXIT();
             break;

        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             p_tcb->SuspendCtr--;
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {
                 p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;
             }
             CPU_CRITICAL_EXIT();
             break;

        default:
             CPU_CRITICAL_EXIT();
             *p_err = OS_ERR_STATE_INVALID;
             return;
    }

	/* �����л� */
    OSSched();
}
#endif

#if OS_CFG_TASK_DEL_EN > 0u
void  OSTaskDel (OS_TCB  *p_tcb,
                 OS_ERR  *p_err)
{
    CPU_SR_ALLOC();

	/* ������ɾ���������� */
	if (p_tcb == &OSIdleTaskTCB) 
	{                          
        *p_err = OS_ERR_TASK_DEL_IDLE;
        return;
    }
	
	/* ɾ���Լ� */
	if (p_tcb == (OS_TCB *)0) 
	{                             
        CPU_CRITICAL_ENTER();
        p_tcb  = OSTCBCurPtr;                               
        CPU_CRITICAL_EXIT();
    }

    OS_CRITICAL_ENTER();
	
	/* ���������״̬������ɾ���Ķ��� */
    switch (p_tcb->TaskState) 
	{
        case OS_TASK_STATE_RDY:
             OS_RdyListRemove(p_tcb);
             break;

        case OS_TASK_STATE_SUSPENDED:
             break;

        /* ����ֻ������ʱ����û�����κεȴ��б�*/
		case OS_TASK_STATE_DLY:        
        case OS_TASK_STATE_DLY_SUSPENDED:
             OS_TickListRemove(p_tcb);
             break;

        case OS_TASK_STATE_PEND:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             OS_TickListRemove(p_tcb);

#if 0 /* Ŀǰ���ǻ�û��ʵ�ֵȴ��б���ʱ�Ȱ��ⲿ�ִ���ע�� */		
             /* �����ڵȴ�ʲô */
			 switch (p_tcb->PendOn) 
			 {                       
                 case OS_TASK_PEND_ON_NOTHING:
				 /* �����ź����Ͷ���û�еȴ����У�ֱ���˳� */
                 case OS_TASK_PEND_ON_TASK_Q:               
                 case OS_TASK_PEND_ON_TASK_SEM:
                      break;

                 /* �ӵȴ��б��Ƴ� */
				 case OS_TASK_PEND_ON_FLAG:                 
                 case OS_TASK_PEND_ON_MULTI:
                 case OS_TASK_PEND_ON_MUTEX:
                 case OS_TASK_PEND_ON_Q:
                 case OS_TASK_PEND_ON_SEM:
                      OS_PendListRemove(p_tcb);
                      break;

                 default:
                      break;
             }
             break;
#endif 
        default:
            OS_CRITICAL_EXIT();
            *p_err = OS_ERR_STATE_INVALID;
            return;
    }

    /* ��ʼ��TCBΪĬ��ֵ */
	OS_TaskInitTCB(p_tcb);
	/* �޸������״̬Ϊɾ��̬������������ */
    p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;         

    OS_CRITICAL_EXIT_NO_SCHED();
    /* �����л���Ѱ��������ȼ������� */
	OSSched(); 

    *p_err = OS_ERR_NONE;
}
#endif

