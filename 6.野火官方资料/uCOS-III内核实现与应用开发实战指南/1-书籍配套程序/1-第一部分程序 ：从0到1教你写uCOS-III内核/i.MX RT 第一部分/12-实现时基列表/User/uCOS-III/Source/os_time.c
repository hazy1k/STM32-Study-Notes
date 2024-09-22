#include "os.h"


void  OSTimeTick (void)
{
#if 0
	unsigned int i;
	CPU_SR_ALLOC();
	
	/* �����ٽ��� */
	OS_CRITICAL_ENTER();
	
	for(i=0; i<OS_CFG_PRIO_MAX; i++)
	{
		if(OSRdyList[i].HeadPtr->TaskDelayTicks > 0)
		{
			OSRdyList[i].HeadPtr->TaskDelayTicks --;
			if(OSRdyList[i].HeadPtr->TaskDelayTicks == 0)
			{
				/* Ϊ0���ʾ��ʱʱ�䵽����������� */			
				//OS_RdyListInsert (OSRdyList[i].HeadPtr);
				OS_PrioInsert(i);
			}
		}
	}
	
	/* �˳��ٽ��� */
	OS_CRITICAL_EXIT();

#endif
	
	/* ����ʱ���б� */
	OS_TickListUpdate();
	
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


