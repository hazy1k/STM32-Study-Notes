#include "os.h"



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
	
	/* ��ʼ�����ȼ����� */
	OSPrioCur = (OS_PRIO)0;           
    OSPrioHighRdy = (OS_PRIO)0;
	
	/* ��ʼ�����ȼ��� */
	OS_PrioInit();
	
	/* ��ʼ�������б� */
	OS_RdyListInit();
	
	/* �����ʱ��������������ʼ��Ӧ�÷ŵ�ʱ���������� */
	/* ʱ����������ʼ�� */
	OSTickCtr         = (OS_TICK)0u;
	/* ʱ���б��ʼ�� */
	OS_TickListInit();
	
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
#if 0		
		/* �ֶ���������1������ */
		OSTCBHighRdyPtr = OSRdyList[0].HeadPtr;
#endif
		OSPrioHighRdy   = OS_PrioGetHighest();		            /* Ѱ����ߵ����ȼ� */
		OSPrioCur       = OSPrioHighRdy;
		
		OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;		/* �ҵ�������ȼ���TCB */
		OSTCBCurPtr     = OSTCBHighRdyPtr;
		
		OSRunning       = OS_STATE_OS_RUNNING;                  /* ���OS��ʼ���� */
		
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
	CPU_SR_ALLOC();
	
	/* �����ٽ��� */
	OS_CRITICAL_ENTER();
	
	/* ����������ȼ������� */
	OSPrioHighRdy   = OS_PrioGetHighest();
	OSTCBHighRdyPtr = OSRdyList[OSPrioHighRdy].HeadPtr;	
	
	/* ���������ȼ��������ǵ�ǰ������ֱ�ӷ��أ������������л� */
	if (OSTCBHighRdyPtr == OSTCBCurPtr)
	{
		/* �˳��ٽ��� */
		OS_CRITICAL_EXIT();
		
		return;
	}	
		/* �˳��ٽ��� */
	OS_CRITICAL_EXIT();
	
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
				  (OS_PRIO)(OS_CFG_PRIO_MAX - 1u),
			      (CPU_STK    *)OSCfg_IdleTaskStkBasePtr,
			      (CPU_STK_SIZE)OSCfg_IdleTaskStkSize,
				  (OS_TICK       )0,
			      (OS_ERR     *)p_err );
}

/*
*********************************************************************************************************
*                                           �����б���غ���
*********************************************************************************************************
*/
/* �����б��ʼ�� */
/*
*              'OS_RDY_LIST  OSRdyTbl[OS_CFG_PRIO_MAX]'  ��ʼ��֮���������������
*
*                               +---------------+--------------+
*                               |               | TailPtr      |-----> 0
*                          [0]  | NbrEntries=0  +--------------+
*                               |               | HeadPtr      |-----> 0
*                               +---------------+--------------+
*                               |               | TailPtr      |-----> 0
*                          [1]  | NbrEntries=0  +--------------+
*                               |               | HeadPtr      |-----> 0
*                               +---------------+--------------+
*                                       :              :
*                                       :              :
*                                       :              :
*                               +---------------+--------------+
*                               |               | TailPtr      |-----> 0
*          [OS_CFG_PRIO_MAX-1]  | NbrEntries=0  +--------------+
*                               |               | HeadPtr      |-----> 0
*                               +---------------+--------------+
*/
void OS_RdyListInit(void)
{
	OS_PRIO i;
	OS_RDY_LIST *p_rdy_list;
	
	for( i=0u; i<OS_CFG_PRIO_MAX; i++ )
	{
		p_rdy_list = &OSRdyList[i];
		p_rdy_list->NbrEntries = (OS_OBJ_QTY)0;
		p_rdy_list->HeadPtr = (OS_TCB *)0;
		p_rdy_list->TailPtr = (OS_TCB *)0;
	}
}


/* �ھ��������в���һ��TCB */
void  OS_RdyListInsert (OS_TCB  *p_tcb)
{
    /* �����ȼ����뵽���ȼ��� */
	OS_PrioInsert(p_tcb->Prio);
	
    if (p_tcb->Prio == OSPrioCur) 
	{
        /* ����ǵ�ǰ���ȼ�����뵽����β�� */
		OS_RdyListInsertTail(p_tcb);
    } 
	else 
	{
        /* ������뵽����ͷ�� */
		OS_RdyListInsertHead(p_tcb);
    }
}

/* �ھ�������ͷ������һ��TCB */

	/*
	*  ������ͷ������һ��TCB�������������
	*              CASE 0: ���뵽һ���յ�����
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-> 0
	*                     +--------------+
	*                     | HeadPtr      |-> 0
	*                     +--------------+
	*                     | NbrEntries=0 |
	*                     +--------------+
	*
	*
	*
	*              CASE 1: �����Ѿ��нڵ�
	*
	*                     OS_RDY_LIST
	*                     +--------------+          OS_TCB
	*                     | TailPtr      |--+---> +------------+
	*                     +--------------+  |     | NextPtr    |->0
	*                     | HeadPtr      |--/     +------------+
	*                     +--------------+     0<-| PrevPtr    |
	*                     | NbrEntries=1 |        +------------+
	*                     +--------------+        :            :
	*                                             :            :
	*                                             +------------+
	*
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/
	
void  OS_RdyListInsertHead (OS_TCB  *p_tcb)
{
    OS_RDY_LIST  *p_rdy_list;
    OS_TCB       *p_tcb2;



    /* ��ȡ������� */
	p_rdy_list = &OSRdyList[p_tcb->Prio];
	
	/* CASE 0: �����ǿ����� */
    if (p_rdy_list->NbrEntries == (OS_OBJ_QTY)0) 
	{
		p_rdy_list->NbrEntries =  (OS_OBJ_QTY)1;            
        p_tcb->NextPtr         =  (OS_TCB   *)0;            
        p_tcb->PrevPtr         =  (OS_TCB   *)0;
        p_rdy_list->HeadPtr    =  p_tcb;                    
        p_rdy_list->TailPtr    =  p_tcb;
    }
	/* CASE 1: �������нڵ� */	
	else 
	{                                               
        p_rdy_list->NbrEntries++;                           
        p_tcb->NextPtr         = p_rdy_list->HeadPtr;       
        p_tcb->PrevPtr         = (OS_TCB    *)0;
        p_tcb2                 = p_rdy_list->HeadPtr;
        p_tcb2->PrevPtr        = p_tcb;
        p_rdy_list->HeadPtr    = p_tcb;
    }
}


/* �ھ�������β������һ��TCB */

	/*
	*  �ھ�������β������һ��TCB ������ �������
	*              CASE 0: �����ǿյ�
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-> 0
	*                     +--------------+
	*                     | HeadPtr      |-> 0
	*                     +--------------+
	*                     | NbrEntries=0 |
	*                     +--------------+
	*
	*
	*
	*              CASE 1: �������нڵ�
	*
	*                     OS_RDY_LIST
	*                     +--------------+          OS_TCB
	*                     | TailPtr      |--+---> +------------+
	*                     +--------------+  |     | NextPtr    |->0
	*                     | HeadPtr      |--/     +------------+
	*                     +--------------+     0<-| PrevPtr    |
	*                     | NbrEntries=1 |        +------------+
	*                     +--------------+        :            :
	*                                             :            :
	*                                             +------------+
	*
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/
	
void  OS_RdyListInsertTail (OS_TCB  *p_tcb)
{
    OS_RDY_LIST  *p_rdy_list;
    OS_TCB       *p_tcb2;


    /* ��ȡ������� */
    p_rdy_list = &OSRdyList[p_tcb->Prio];
	
	/* CASE 0: �����ǿ����� */
    if (p_rdy_list->NbrEntries == (OS_OBJ_QTY)0) 
	{          
        p_rdy_list->NbrEntries  = (OS_OBJ_QTY)1;            
        p_tcb->NextPtr          = (OS_TCB   *)0;           
        p_tcb->PrevPtr          = (OS_TCB   *)0;
        p_rdy_list->HeadPtr     = p_tcb;                   
        p_rdy_list->TailPtr     = p_tcb;
    } 
	/* CASE 1: �������нڵ� */
	else 
	{                                               
        p_rdy_list->NbrEntries++;                           
        p_tcb->NextPtr          = (OS_TCB   *)0;            
        p_tcb2                  = p_rdy_list->TailPtr;
        p_tcb->PrevPtr          = p_tcb2;
        p_tcb2->NextPtr         = p_tcb;                   
        p_rdy_list->TailPtr     = p_tcb;
    }
}


/* �������еĽڵ��ͷ���ƶ���β�� */

	/*
	*  �������еĽڵ��ͷ���ƶ���β���������������������
	*              CASE 0: ����Ϊ�գ����¿�����
	*
	*              CASE 1: ����ֻ��һ���ڵ㣬���¿�����
	*
	*              CASE 2: ����ֻ�������ڵ㡣
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |--------------------------+
	*                     +--------------+          OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------> | NextPtr    |->0
	*                     | NbrEntries=2 |        +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    | <------| PrevPtr    |
	*                                             +------------+        +------------+
	*                                             :            :        :            :
	*                                             :            :        :            :
	*                                             +------------+        +------------+
	*
	*
	*              CASE N: �����������ڵ����ϡ�
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/
	
void  OS_RdyListMoveHeadToTail (OS_RDY_LIST  *p_rdy_list)
{
    OS_TCB  *p_tcb1;
    OS_TCB  *p_tcb2;
    OS_TCB  *p_tcb3;



    switch (p_rdy_list->NbrEntries) 
	{
        case 0:
        case 1:
             break;

        case 2:                                             
             p_tcb1              = p_rdy_list->HeadPtr;    
             p_tcb2              = p_rdy_list->TailPtr;    
             p_tcb1->PrevPtr     = p_tcb2;
             p_tcb1->NextPtr     = (OS_TCB *)0;
             p_tcb2->PrevPtr     = (OS_TCB *)0;
             p_tcb2->NextPtr     = p_tcb1;
             p_rdy_list->HeadPtr = p_tcb2;
             p_rdy_list->TailPtr = p_tcb1;
             break;

        default:                                            
             p_tcb1              = p_rdy_list->HeadPtr;     
             p_tcb2              = p_rdy_list->TailPtr;     
             p_tcb3              = p_tcb1->NextPtr;         
             p_tcb3->PrevPtr     = (OS_TCB *)0;            
             p_tcb1->NextPtr     = (OS_TCB *)0;           
             p_tcb1->PrevPtr     = p_tcb2;                  
             p_tcb2->NextPtr     = p_tcb1;                 
             p_rdy_list->HeadPtr = p_tcb3;                  
             p_rdy_list->TailPtr = p_tcb1;
             break;
    }
}

/* ���������Ƴ�һ��TCB */

	/*
	*  ���������Ƴ�һ��TCB�����¼��������
	*              CASE 0: ����Ϊ�գ����¿�����
	*
	*              CASE 1: ����ֻ��һ���ڵ㡣
	*
	*                     OS_RDY_LIST
	*                     +--------------+          OS_TCB
	*                     | TailPtr      |--+---> +------------+
	*                     +--------------+  |     | NextPtr    |->0
	*                     | HeadPtr      |--/     +------------+
	*                     +--------------+     0<-| PrevPtr    |
	*                     | NbrEntries=1 |        +------------+
	*                     +--------------+        :            :
	*                                             :            :
	*                                             +------------+
	*
	*              CASE N: ���������������ϵĽڵ㡣
	*
	*                     OS_RDY_LIST
	*                     +--------------+
	*                     | TailPtr      |-----------------------------------------------+
	*                     +--------------+          OS_TCB               OS_TCB          |     OS_TCB
	*                     | HeadPtr      |------> +------------+       +------------+    +-> +------------+
	*                     +--------------+        | NextPtr    |------>| NextPtr    | ...... | NextPtr    |->0
	*                     | NbrEntries=N |        +------------+       +------------+        +------------+
	*                     +--------------+     0<-| PrevPtr    |<------| PrevPtr    | ...... | PrevPtr    |
	*                                             +------------+       +------------+        +------------+
	*                                             :            :       :            :        :            :
	*                                             :            :       :            :        :            :
	*                                             +------------+       +------------+        +------------+
	*/

void  OS_RdyListRemove (OS_TCB  *p_tcb)
{
    OS_RDY_LIST  *p_rdy_list;
    OS_TCB       *p_tcb1;
    OS_TCB       *p_tcb2;



    p_rdy_list = &OSRdyList[p_tcb->Prio];
	
	/* ����Ҫɾ����TCB�ڵ��ǰһ���ͺ�һ���ڵ� */
    p_tcb1     = p_tcb->PrevPtr;
    p_tcb2     = p_tcb->NextPtr;
	
	/* Ҫ�Ƴ���TCB�ڵ��������еĵ�һ���ڵ� */
    if (p_tcb1 == (OS_TCB *)0) 
	{                            
        /* �Ҹ�������ֻ��һ���ڵ� */
		if (p_tcb2 == (OS_TCB *)0) 
		{                       
            /* ���ڵ�ȫ����ʼ��Ϊ0 */
			p_rdy_list->NbrEntries = (OS_OBJ_QTY)0;
            p_rdy_list->HeadPtr    = (OS_TCB   *)0;
            p_rdy_list->TailPtr    = (OS_TCB   *)0;
			
            /* ��������ȼ�������Ӧ��λ */
			OS_PrioRemove(p_tcb->Prio);
        }
		/* �������в�ֹһ���ڵ� */		
		else 
		{
            /* �ڵ��1 */
			p_rdy_list->NbrEntries--;
            p_tcb2->PrevPtr        = (OS_TCB   *)0;        
            p_rdy_list->HeadPtr    = p_tcb2; 
        }
    }
	/* Ҫ�Ƴ���TCB�ڵ㲻�������еĵ�һ���ڵ� */	
	else 
	{
        p_rdy_list->NbrEntries--;                          
        p_tcb1->NextPtr = p_tcb2;
		
        /* ���Ҫɾ���Ľڵ����һ���ڵ���0����Ҫɾ���Ľڵ������һ���ڵ� */
		if (p_tcb2 == (OS_TCB *)0) 
		{
            p_rdy_list->TailPtr = p_tcb1;
        } 
		else 
		{
            p_tcb2->PrevPtr     = p_tcb1;
        }
    }
	
	/* ��λ�Ӿ����б���ɾ����TCB��PrevPtr��NextPtr������ָ�� */
    p_tcb->PrevPtr = (OS_TCB *)0;
    p_tcb->NextPtr = (OS_TCB *)0;
}


/*
*********************************************************************************************************
*                                             �����������
*********************************************************************************************************
*/
void  OS_TaskRdy (OS_TCB  *p_tcb)
{
    /* ��ʱ���б�ɾ�� */
	OS_TickListRemove(p_tcb);

	/* ��������б� */
	OS_RdyListInsert(p_tcb);
}


/*
*********************************************************************************************************
*                                             ʱ��Ƭ����
*********************************************************************************************************
*/
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void OS_SchedRoundRobin(OS_RDY_LIST  *p_rdy_list)
{
	OS_TCB   *p_tcb;
    CPU_SR_ALLOC();
	
	/*  �����ٽ�� */
	CPU_CRITICAL_ENTER();
	
	p_tcb = p_rdy_list->HeadPtr;
	
	/* ���TCB�ڵ�Ϊ�գ����˳� */
    if (p_tcb == (OS_TCB *)0) 
	{
        CPU_CRITICAL_EXIT();
        return;
    }

    /* ����ǿ�������Ҳ�˳� */
	if (p_tcb == &OSIdleTaskTCB) 
	{
        CPU_CRITICAL_EXIT();
        return;
    }

    /* ʱ��Ƭ�Լ� */
	if (p_tcb->TimeQuantaCtr > (OS_TICK)0) 
	{
        p_tcb->TimeQuantaCtr--;
    }

    /* ʱ��Ƭû�����꣬���˳� */
	if (p_tcb->TimeQuantaCtr > (OS_TICK)0) 
	{               
        CPU_CRITICAL_EXIT();
        return;
    }

    /* �����ǰ���ȼ�ֻ��һ���������˳� */
	if (p_rdy_list->NbrEntries < (OS_OBJ_QTY)2) 
	{           
        CPU_CRITICAL_EXIT();                        
        return;
    }	
	
	/* ʱ��Ƭ���꣬������ŵ���������һ���ڵ� */
	OS_RdyListMoveHeadToTail(p_rdy_list);
	
	/* ���»�ȡ����ڵ� */
	p_tcb = p_rdy_list->HeadPtr;
	/* ����Ĭ�ϵ�ʱ��Ƭ����ֵ */
	p_tcb->TimeQuantaCtr = p_tcb->TimeQuanta;
	
	/* �˳��ٽ�� */
	CPU_CRITICAL_EXIT();
}
#endif 	/* OS_CFG_SCHED_ROUND_ROBIN_EN > 0u */















