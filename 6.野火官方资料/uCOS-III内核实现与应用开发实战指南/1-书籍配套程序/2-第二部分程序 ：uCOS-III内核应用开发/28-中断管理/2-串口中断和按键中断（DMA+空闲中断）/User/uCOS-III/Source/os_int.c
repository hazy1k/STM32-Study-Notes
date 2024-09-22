/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                 ISR QUEUE MANAGEMENT
*
* File    : OS_INT.C
* By      : JJL
* Version : V3.03.01
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#define   MICRIUM_SOURCE
#include  <os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_int__c = "$Id: $";
#endif


#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
/*$PAGE*/
/*
************************************************************************************************************************
*                                                   POST TO ISR QUEUE
*
* Description: This function places contents of posts into an intermediate queue to help defer processing of interrupts
*              at the task level.
*
* Arguments  : type       is the type of kernel object the post is destined to:
*
*                             OS_OBJ_TYPE_SEM
*                             OS_OBJ_TYPE_Q
*                             OS_OBJ_TYPE_FLAG
*                             OS_OBJ_TYPE_TASK_MSG
*                             OS_OBJ_TYPE_TASK_SIGNAL
*
*              p_obj      is a pointer to the kernel object to post to.  This can be a pointer to a semaphore,
*              -----      a message queue or a task control clock.
*
*              p_void     is a pointer to a message that is being posted.  This is used when posting to a message
*                         queue or directly to a task.
*
*              msg_size   is the size of the message being posted
*
*              flags      if the post is done to an event flag group then this corresponds to the flags being
*                         posted
*
*              ts         is a timestamp as to when the post was done
*
*              opt        this corresponds to post options and applies to:
*
*                             OSFlagPost()
*                             OSSemPost()
*                             OSQPost()
*                             OSTaskQPost()
*
*              p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE         if the post to the ISR queue was successful
*                             OS_ERR_INT_Q_FULL   if the ISR queue is full and cannot accepts any further posts.  This
*                                                 generally indicates that you are receiving interrupts faster than you
*                                                 can process them or, that you didn't make the ISR queue large enough.
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OS_IntQPost (OS_OBJ_TYPE   type,        //�ں˶�������
                   void         *p_obj,       //���������ں˶���
                   void         *p_void,      //��Ϣ���л�������Ϣ
                   OS_MSG_SIZE   msg_size,    //��Ϣ����Ŀ
                   OS_FLAGS      flags,       //�¼���־��
                   OS_OPT        opt,         //�����ں˶���ʱ��ѡ��
                   CPU_TS        ts,          //�����ں˶���ʱ��ʱ���
                   OS_ERR       *p_err)       //���ش�������
{
    CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                     //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                     //�����ж�ʱ����ֵ��ԭ�� 

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�������ִ��
    }
#endif

    CPU_CRITICAL_ENTER();                                   //���ж�
    if (OSIntQNbrEntries < OSCfg_IntQSize) {                //����ж϶���δռ��   
        OSIntQNbrEntries++;

        if (OSIntQNbrEntriesMax < OSIntQNbrEntries) {       //�����ж϶��е����ʹ����Ŀ����ʷ��¼
            OSIntQNbrEntriesMax = OSIntQNbrEntries;
        }
        /* ��Ҫ�����ύ���ں˶������Ϣ���뵽�ж϶�����ڵ���Ϣ��¼�� */
        OSIntQInPtr->Type       = type;                     //�����ں˶�������
        OSIntQInPtr->ObjPtr     = p_obj;                    //���汻�������ں˶���
        OSIntQInPtr->MsgPtr     = p_void;                   //������Ϣ����ָ��
        OSIntQInPtr->MsgSize    = msg_size;                 //������Ϣ��С
        OSIntQInPtr->Flags      = flags;                    //�����¼���־��
        OSIntQInPtr->Opt        = opt;                      //����ѡ��
        OSIntQInPtr->TS         = ts;                       //������󱻷�����ʱ���

        OSIntQInPtr             =  OSIntQInPtr->NextPtr;    //ָ����һ���������Ա
        /* ���ж϶��й������� OSIntQTask ���� */
        OSRdyList[0].NbrEntries = (OS_OBJ_QTY)1;            //���¾����б��ϵ����ȼ�0��������Ϊ1�� 
        OSRdyList[0].HeadPtr    = &OSIntQTaskTCB;           //�þ����б��ͷָ��ָ�� OSIntQTask ����
        OSRdyList[0].TailPtr    = &OSIntQTaskTCB;           //�þ����б��βָ��ָ�� OSIntQTask ����
        OS_PrioInsert(0u);                                  //�����ȼ��б����������ȼ�0
        if (OSPrioCur != 0) {                               //�����ǰ���еĲ��� OSIntQTask ����
            OSPrioSaved         = OSPrioCur;                //���浱ǰ��������ȼ�
        }

       *p_err                   = OS_ERR_NONE;              //���ش�������Ϊ���޴���
    } else {                                                //����ж϶�����ռ��
        OSIntQOvfCtr++;                                     //�ж϶��������Ŀ��1
       *p_err                   = OS_ERR_INT_Q_FULL;        //���ش�������Ϊ���޴���
    }
    CPU_CRITICAL_EXIT();                                    //���ж�
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INTERRUPT QUEUE MANAGEMENT TASK
*
* Description: This task is created by OS_IntQTaskInit().
*
* Arguments  : p_arg     is a pointer to an optional argument that is passed during task creation.  For this function
*                        the argument is not used and will be a NULL pointer.
*
* Returns    : none
************************************************************************************************************************
*/

void  OS_IntQRePost (void)
{
    CPU_TS  ts;
    OS_ERR  err;


    switch (OSIntQOutPtr->Type) {   //�����ں˶������ͷ��ദ��
        case OS_OBJ_TYPE_FLAG:      //��������������¼���־
#if OS_CFG_FLAG_EN > 0u             //���ʹ�����¼���־���򷢲��¼���־
             (void)OS_FlagPost((OS_FLAG_GRP *) OSIntQOutPtr->ObjPtr,
                               (OS_FLAGS     ) OSIntQOutPtr->Flags,
                               (OS_OPT       ) OSIntQOutPtr->Opt,
                               (CPU_TS       ) OSIntQOutPtr->TS,
                               (OS_ERR      *)&err);
#endif
             break;                 //����

        case OS_OBJ_TYPE_Q:         //���������������Ϣ����
#if OS_CFG_Q_EN > 0u                //���ʹ������Ϣ���У��򷢲���Ϣ����
             OS_QPost((OS_Q      *) OSIntQOutPtr->ObjPtr,
                      (void      *) OSIntQOutPtr->MsgPtr,
                      (OS_MSG_SIZE) OSIntQOutPtr->MsgSize,
                      (OS_OPT     ) OSIntQOutPtr->Opt,
                      (CPU_TS     ) OSIntQOutPtr->TS,
                      (OS_ERR    *)&err);
#endif
             break;                 //����

        case OS_OBJ_TYPE_SEM:       //������������Ƕ�ֵ�ź���
#if OS_CFG_SEM_EN > 0u              //���ʹ���˶�ֵ�ź������򷢲���ֵ�ź���
             (void)OS_SemPost((OS_SEM *) OSIntQOutPtr->ObjPtr,
                              (OS_OPT  ) OSIntQOutPtr->Opt,
                              (CPU_TS  ) OSIntQOutPtr->TS,
                              (OS_ERR *)&err);
#endif
             break;                 //����

        case OS_OBJ_TYPE_TASK_MSG:  //�������������������Ϣ
#if OS_CFG_TASK_Q_EN > 0u           //���ʹ����������Ϣ���򷢲�������Ϣ
             OS_TaskQPost((OS_TCB    *) OSIntQOutPtr->ObjPtr,
                          (void      *) OSIntQOutPtr->MsgPtr,
                          (OS_MSG_SIZE) OSIntQOutPtr->MsgSize,
                          (OS_OPT     ) OSIntQOutPtr->Opt,
                          (CPU_TS     ) OSIntQOutPtr->TS,
                          (OS_ERR    *)&err);
#endif
             break;                 //����

        case OS_OBJ_TYPE_TASK_RESUME://������������ǻָ�����
#if OS_CFG_TASK_SUSPEND_EN > 0u      //���ʹ���˺���OSTaskResume()���ָ�������
             (void)OS_TaskResume((OS_TCB *) OSIntQOutPtr->ObjPtr,
                                 (OS_ERR *)&err);
#endif
             break;                  //����

        case OS_OBJ_TYPE_TASK_SIGNAL://������������������ź���
             (void)OS_TaskSemPost((OS_TCB *) OSIntQOutPtr->ObjPtr,//���������ź���
                                  (OS_OPT  ) OSIntQOutPtr->Opt,
                                  (CPU_TS  ) OSIntQOutPtr->TS,
                                  (OS_ERR *)&err);
             break;                  //����

        case OS_OBJ_TYPE_TASK_SUSPEND://������������ǹ�������
#if OS_CFG_TASK_SUSPEND_EN > 0u       //���ʹ���˺��� OSTaskSuspend()�����������
             (void)OS_TaskSuspend((OS_TCB *) OSIntQOutPtr->ObjPtr,
                                  (OS_ERR *)&err);
#endif
             break;                   //����

        case OS_OBJ_TYPE_TICK:       //�������������ʱ�ӽ���
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u //���ʹ����ʱ��Ƭ��ת���ȣ�
             OS_SchedRoundRobin(&OSRdyList[OSPrioSaved]); //��ת���Ƚ��ж�ǰ���ȼ�����
#endif

             (void)OS_TaskSemPost((OS_TCB *)&OSTickTaskTCB, //�����ź�����ʱ�ӽ�������
                                  (OS_OPT  ) OS_OPT_POST_NONE,
                                  (CPU_TS  ) OSIntQOutPtr->TS,
                                  (OS_ERR *)&err);
#if OS_CFG_TMR_EN > 0u               //���ʹ���������ʱ���������ź�������ʱ������
             OSTmrUpdateCtr--;
             if (OSTmrUpdateCtr == (OS_CTR)0u) {
                 OSTmrUpdateCtr = OSTmrUpdateCnt;
                 ts             = OS_TS_GET();                             
                 (void)OS_TaskSemPost((OS_TCB *)&OSTmrTaskTCB,             
                                      (OS_OPT  ) OS_OPT_POST_NONE,
                                      (CPU_TS  ) ts,
                                      (OS_ERR *)&err);
             }
#endif
             break;                  //����

        default:                     //����ں˶������ͳ���Ԥ��
             break;                  //ֱ������
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INTERRUPT QUEUE MANAGEMENT TASK
*
* Description: This task is created by OS_IntQTaskInit().
*
* Arguments  : p_arg     is a pointer to an optional argument that is passed during task creation.  For this function
*                        the argument is not used and will be a NULL pointer.
*
* Returns    : none
************************************************************************************************************************
*/

void  OS_IntQTask (void  *p_arg)
{
    CPU_BOOLEAN  done;
    CPU_TS       ts_start;
    CPU_TS       ts_end;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

    p_arg = p_arg;                                          
    while (DEF_ON) {                                        //������ѭ��
        done = DEF_FALSE;
        while (done == DEF_FALSE) {
            CPU_CRITICAL_ENTER();                           //���ж�
            if (OSIntQNbrEntries == (OS_OBJ_QTY)0u) {       //����ж϶�������ں˶��󷢲����
                OSRdyList[0].NbrEntries = (OS_OBJ_QTY)0u;   //�Ӿ����б��Ƴ��ж϶��й������� OS_IntQTask
                OSRdyList[0].HeadPtr    = (OS_TCB   *)0;
                OSRdyList[0].TailPtr    = (OS_TCB   *)0;
                OS_PrioRemove(0u);                          //�����ȼ�����Ƴ����ȼ�0
                CPU_CRITICAL_EXIT();                        //���ж�
                OSSched();                                  //�������
                done = DEF_TRUE;                            //�˳�ѭ��
            } else {                                        //����ж϶����ﻹ���ں˶���
                CPU_CRITICAL_EXIT();                        //���ж�
                ts_start = OS_TS_GET();                     //��ȡʱ���
                OS_IntQRePost();                            //�����ж϶�������ں˶���
                ts_end   = OS_TS_GET() - ts_start;          //����ôη���ʱ��
                if (OSIntQTaskTimeMax < ts_end) {           //�����ж϶��з����ں˶�������ʱ�����ʷ��¼
                    OSIntQTaskTimeMax = ts_end;
                }
                CPU_CRITICAL_ENTER();                       //���ж�
                OSIntQOutPtr = OSIntQOutPtr->NextPtr;       //�ж϶��г���ת����һ��
                OSIntQNbrEntries--;                         //�ж϶��еĳ�Ա��Ŀ��1
                CPU_CRITICAL_EXIT();                        //���ж�
            }
        }
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 INITIALIZE THE ISR QUEUE
*
* Description: This function is called by OSInit() to initialize the ISR queue.
*
* Arguments  : p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_INT_Q             If you didn't provide an ISR queue in OS_CFG.C
*                           OS_ERR_INT_Q_SIZE        If you didn't specify a large enough ISR queue.
*                           OS_ERR_STK_INVALID       If you specified a NULL pointer for the task of the ISR task
*                                                    handler
*                           OS_ERR_STK_SIZE_INVALID  If you didn't specify a stack size greater than the minimum
*                                                    specified by OS_CFG_STK_SIZE_MIN
*                           OS_ERR_???               An error code returned by OSTaskCreate().
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OS_IntQTaskInit (OS_ERR  *p_err)
{
    OS_INT_Q      *p_int_q;
    OS_INT_Q      *p_int_q_next;
    OS_OBJ_QTY     i;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSIntQOvfCtr = (OS_QTY)0u;                              /* Clear the ISR queue overflow counter                   */

    if (OSCfg_IntQBasePtr == (OS_INT_Q *)0) {
       *p_err = OS_ERR_INT_Q;
        return;
    }

    if (OSCfg_IntQSize < (OS_OBJ_QTY)2u) {
       *p_err = OS_ERR_INT_Q_SIZE;
        return;
    }

    OSIntQTaskTimeMax = (CPU_TS)0;

    p_int_q           = OSCfg_IntQBasePtr;                  /* Initialize the circular ISR queue                      */
    p_int_q_next      = p_int_q;
    p_int_q_next++;
    for (i = 0u; i < OSCfg_IntQSize; i++) {
        p_int_q->Type    =  OS_OBJ_TYPE_NONE;
        p_int_q->ObjPtr  = (void      *)0;
        p_int_q->MsgPtr  = (void      *)0;
        p_int_q->MsgSize = (OS_MSG_SIZE)0u;
        p_int_q->Flags   = (OS_FLAGS   )0u;
        p_int_q->Opt     = (OS_OPT     )0u;
        p_int_q->NextPtr = p_int_q_next;
        p_int_q++;
        p_int_q_next++;
    }
    p_int_q--;
    p_int_q_next        = OSCfg_IntQBasePtr;
    p_int_q->NextPtr    = p_int_q_next;
    OSIntQInPtr         = p_int_q_next;
    OSIntQOutPtr        = p_int_q_next;
    OSIntQNbrEntries    = (OS_OBJ_QTY)0u;
    OSIntQNbrEntriesMax = (OS_OBJ_QTY)0u;

                                                            /* -------------- CREATE THE ISR QUEUE TASK ------------- */
    if (OSCfg_IntQTaskStkBasePtr == (CPU_STK *)0) {
       *p_err = OS_ERR_INT_Q_STK_INVALID;
        return;
    }

    if (OSCfg_IntQTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_INT_Q_STK_SIZE_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSIntQTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III ISR Queue Task"),
                 (OS_TASK_PTR )OS_IntQTask,
                 (void       *)0,
                 (OS_PRIO     )0u,                          /* This task is ALWAYS at priority '0' (i.e. highest)     */
                 (CPU_STK    *)OSCfg_IntQTaskStkBasePtr,
                 (CPU_STK_SIZE)OSCfg_IntQTaskStkLimit,
                 (CPU_STK_SIZE)OSCfg_IntQTaskStkSize,
                 (OS_MSG_QTY  )0u,
                 (OS_TICK     )0u,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)p_err);
}

#endif
