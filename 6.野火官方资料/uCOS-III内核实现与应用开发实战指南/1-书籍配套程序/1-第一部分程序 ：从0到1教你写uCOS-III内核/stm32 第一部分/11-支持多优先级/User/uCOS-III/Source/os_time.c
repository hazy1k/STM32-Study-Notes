#include "os.h"


void  OSTimeTick (void)
{
	unsigned int i;
	CPU_SR_ALLOC();
	
	/* �����ٽ��� */
	OS_CRITICAL_ENTER();	

	/* ɨ������б������������TaskDelayTicks�������Ϊ0�����1 */	
#if 0
	for(i=0; i<OS_CFG_PRIO_MAX; i++)
	{
		if(OSRdyList[i].HeadPtr->TaskDelayTicks > 0)
		{
			OSRdyList[i].HeadPtr->TaskDelayTicks --;
		}
	}
#endif
	
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
	
	/* ������� */
	OSSched();
}

/* ������ʱ */
void  OSTimeDly(OS_TICK dly)
{
#if 0
	/* ������ʱʱ�� */
	OSTCBCurPtr->TaskDelayTicks = dly;
	
	/* ����������� */
	OSSched();
#endif

	CPU_SR_ALLOC();
	
	/* �����ٽ��� */
	OS_CRITICAL_ENTER();
	
	/* ������ʱʱ�� */
	OSTCBCurPtr->TaskDelayTicks = dly;
	
	/* �Ӿ����б����Ƴ� */
	//OS_RdyListRemove(OSTCBCurPtr);
	OS_PrioRemove(OSTCBCurPtr->Prio);	
	
	/* �˳��ٽ��� */
	OS_CRITICAL_EXIT();
	
	/* ������� */
	OSSched();	
}


