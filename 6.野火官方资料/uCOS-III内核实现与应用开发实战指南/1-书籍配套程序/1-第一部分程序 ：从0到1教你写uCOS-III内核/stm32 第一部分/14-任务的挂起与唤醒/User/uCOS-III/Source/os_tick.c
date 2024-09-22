#include "os.h"

/* ��ʼ��ʱ���б�������� */
void  OS_TickListInit (void)
{
    OS_TICK_SPOKE_IX   i;
    OS_TICK_SPOKE     *p_spoke;

    for (i = 0u; i < OSCfg_TickWheelSize; i++) {
        p_spoke                = (OS_TICK_SPOKE *)&OSCfg_TickWheel[i];
        p_spoke->FirstPtr      = (OS_TCB        *)0;
        p_spoke->NbrEntries    = (OS_OBJ_QTY     )0u;
        p_spoke->NbrEntriesMax = (OS_OBJ_QTY     )0u;
    }
}

/* ��һ��������뵽ʱ���б�������ʱʱ��Ĵ�С�������� */
void  OS_TickListInsert (OS_TCB *p_tcb,OS_TICK time)
{
	OS_TICK_SPOKE_IX   spoke;
	OS_TICK_SPOKE     *p_spoke;
	OS_TCB            *p_tcb0;
    OS_TCB            *p_tcb1;
	
	p_tcb->TickCtrMatch = OSTickCtr + time;
	p_tcb->TickRemain   = time;
	
	spoke   = (OS_TICK_SPOKE_IX)(p_tcb->TickCtrMatch % OSCfg_TickWheelSize);
	p_spoke = &OSCfg_TickWheel[spoke];	

	/* ���뵽 OSCfg_TickWheel[spoke] �ĵ�һ���ڵ� */
	if (p_spoke->NbrEntries == (OS_OBJ_QTY)0u) 
	{
        p_tcb->TickNextPtr   = (OS_TCB   *)0;
        p_tcb->TickPrevPtr   = (OS_TCB   *)0;
        p_spoke->FirstPtr    =  p_tcb;
        p_spoke->NbrEntries  = (OS_OBJ_QTY)1u;
    }
	/* �������Ĳ��ǵ�һ���ڵ㣬����TickRemain��С�������� */
	else
	{
		/* ��ȡ��һ���ڵ�ָ�� */
		p_tcb1 = p_spoke->FirstPtr;
		while (p_tcb1 != (OS_TCB *)0)
		{
			/* ����ȽϽڵ��ʣ��ʱ�� */
			p_tcb1->TickRemain = p_tcb1->TickCtrMatch - OSTickCtr;
                               
            /* ���뵽�ȽϽڵ�ĺ��� */
			if (p_tcb->TickRemain > p_tcb1->TickRemain) 
			{        
                if (p_tcb1->TickNextPtr != (OS_TCB *)0) 
				{        
                    /* Ѱ����һ���ȽϽڵ� */
					p_tcb1 =  p_tcb1->TickNextPtr; 
                } 
				else	/* �����һ���ڵ���� */
				{
                    p_tcb->TickNextPtr   = (OS_TCB *)0;
                    p_tcb->TickPrevPtr   =  p_tcb1;
                    p_tcb1->TickNextPtr  =  p_tcb;               
                    p_tcb1               = (OS_TCB *)0;
                }
            }
			/* ���뵽�ȽϽڵ��ǰ�� */ 			
			else
			{                                             
                /* �ڵ�һ���ڵ���� */
				if (p_tcb1->TickPrevPtr == (OS_TCB *)0) 
				{
                    p_tcb->TickPrevPtr   = (OS_TCB *)0;
                    p_tcb->TickNextPtr   =  p_tcb1;
                    p_tcb1->TickPrevPtr  =  p_tcb;
                    p_spoke->FirstPtr    =  p_tcb;
                } 
				else 
				{   /* ���뵽�����ڵ�֮�� */                                      
                    p_tcb0               =  p_tcb1->TickPrevPtr;
                    p_tcb->TickPrevPtr   =  p_tcb0;
                    p_tcb->TickNextPtr   =  p_tcb1;
                    p_tcb0->TickNextPtr  =  p_tcb;
                    p_tcb1->TickPrevPtr  =  p_tcb;
                }
				/* ����whileѭ�� */
                p_tcb1 = (OS_TCB *)0;
            }
		}
		
		/* �ڵ�ɹ����� */
		p_spoke->NbrEntries++; 
	}	
	
	/* ˢ��NbrEntriesMax��ֵ */
	if (p_spoke->NbrEntriesMax < p_spoke->NbrEntries)		
	{          
		p_spoke->NbrEntriesMax = p_spoke->NbrEntries;
    }
	
	/* ����TCB�е�TickSpokePtr��ָ���ڵ� */
	p_tcb->TickSpokePtr = p_spoke;
}

/* ��ʱ���б����Ƴ�һ������ */
void  OS_TickListRemove (OS_TCB  *p_tcb)
{
	OS_TICK_SPOKE  *p_spoke;
    OS_TCB         *p_tcb1;
    OS_TCB         *p_tcb2;
	
	/* ��ȡ����TCB��������ĸ�ָ�� */
	p_spoke = p_tcb->TickSpokePtr;
	
	/* ȷ�������������� */
	if (p_spoke != (OS_TICK_SPOKE *)0) 
	{                              
        /* ��ʣ��ʱ����0 */
		p_tcb->TickRemain = (OS_TICK)0u;
		
		/* Ҫ�Ƴ��ĸպ��ǵ�һ���ڵ� */
        if (p_spoke->FirstPtr == p_tcb) 
		{                             
            /* ���µ�һ���ڵ㣬ԭ���ĵ�һ���ڵ���Ҫ���Ƴ� */
			p_tcb1            = (OS_TCB *)p_tcb->TickNextPtr;
            p_spoke->FirstPtr = p_tcb1;
            if (p_tcb1 != (OS_TCB *)0) 
			{
                p_tcb1->TickPrevPtr = (OS_TCB *)0;
            }
        }
		/* Ҫ�Ƴ��Ĳ��ǵ�һ���ڵ� */		
		else 
		{
            /* ����Ҫ�Ƴ��Ľڵ��ǰ��ڵ��ָ�� */
			p_tcb1              = p_tcb->TickPrevPtr;
            p_tcb2              = p_tcb->TickNextPtr;
			
			/* �ڵ��Ƴ������ڵ�ǰ��������ڵ�������һ�� */
            p_tcb1->TickNextPtr = p_tcb2;
            if (p_tcb2 != (OS_TCB *)0) 
			{
                p_tcb2->TickPrevPtr = p_tcb1;
            }
        }
		
		/* ��λ����TCB��ʱ���б���ص��ֶγ�Ա */
        p_tcb->TickNextPtr  = (OS_TCB        *)0;
        p_tcb->TickPrevPtr  = (OS_TCB        *)0;
        p_tcb->TickSpokePtr = (OS_TICK_SPOKE *)0;
        p_tcb->TickCtrMatch = (OS_TICK        )0u;
		
		/* �ڵ��1 */
        p_spoke->NbrEntries--;
    }
}

void  OS_TickListUpdate (void)
{
	OS_TICK_SPOKE_IX   spoke;
	OS_TICK_SPOKE     *p_spoke;
	OS_TCB            *p_tcb;
	OS_TCB            *p_tcb_next;
	CPU_BOOLEAN        done;
	
	CPU_SR_ALLOC();
	
	/* �����ٽ�� */
	OS_CRITICAL_ENTER();
	
	/* ʱ��������++ */
	OSTickCtr++;
	
	spoke    = (OS_TICK_SPOKE_IX)(OSTickCtr % OSCfg_TickWheelSize);
	p_spoke  = &OSCfg_TickWheel[spoke];
	
	p_tcb    = p_spoke->FirstPtr;
	done     = DEF_FALSE;
	
	while (done == DEF_FALSE)
	{
		if (p_tcb != (OS_TCB *)0)
		{
			p_tcb_next = p_tcb->TickNextPtr;
			
			p_tcb->TickRemain = p_tcb->TickCtrMatch - OSTickCtr;
			
			/* �ڵ���ʱʱ�䵽 */
			if (OSTickCtr == p_tcb->TickCtrMatch)
			{
				/* ��������� */
				OS_TaskRdy(p_tcb);
			}
			else
			{
				/* �����һ���ڵ���ʱ��δ�������˳�whileѭ��
				   ��Ϊ�����Ǹ����������еģ���һ���ڵ���ʱ��δ�����Ǻ���Ŀ϶�δ�� */
				done = DEF_TRUE;	
			}		

			/* �����һ���ڵ���ʱ���������������������������û����ʱ����������
			   ����У����������� */
			p_tcb = p_tcb_next;
		}
		else
		{
			done  = DEF_TRUE; 
		}
	}
	
	/* �˳��ٽ�� */
	OS_CRITICAL_EXIT();
}
