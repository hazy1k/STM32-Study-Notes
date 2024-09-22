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










