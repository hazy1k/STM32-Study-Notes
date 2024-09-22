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
	OSPrioCur                       = (OS_PRIO)0;           
    OSPrioHighRdy                   = (OS_PRIO)0;
    OSPrioSaved                     = (OS_PRIO)0;
	/* ��ʼ�����ȼ��� */
	OS_PrioInit();
	
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










