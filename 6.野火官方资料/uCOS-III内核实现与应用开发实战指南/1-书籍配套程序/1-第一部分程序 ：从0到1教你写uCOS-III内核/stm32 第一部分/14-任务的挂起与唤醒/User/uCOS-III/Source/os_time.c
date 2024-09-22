#include "os.h"


void  OSTimeTick (void)
{	
	/* ����ʱ���б� */
	OS_TickListUpdate();
	
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	/* ʱ��Ƭ���� */
	OS_SchedRoundRobin(&OSRdyList[OSPrioCur]);
#endif
	
	/* ������� */
	OSSched();
}

/* ������ʱ */
void  OSTimeDly(OS_TICK dly)
{
	CPU_SR_ALLOC();
	
	/* �����ٽ��� */
	OS_CRITICAL_ENTER();
#if 0	
	/* ������ʱʱ�� */
	OSTCBCurPtr->TaskDelayTicks = dly;	
	
	/* �Ӿ����б����Ƴ� */
	//OS_RdyListRemove(OSTCBCurPtr);
	OS_PrioRemove(OSTCBCurPtr->Prio);
#endif
	
	/* ���뵽ʱ���б� */
	OS_TickListInsert(OSTCBCurPtr, dly);
	
	/* �Ӿ����б��Ƴ� */
	OS_RdyListRemove(OSTCBCurPtr);
	
	/* �˳��ٽ��� */
	OS_CRITICAL_EXIT();
	
	/* ������� */
	OSSched();	
}


