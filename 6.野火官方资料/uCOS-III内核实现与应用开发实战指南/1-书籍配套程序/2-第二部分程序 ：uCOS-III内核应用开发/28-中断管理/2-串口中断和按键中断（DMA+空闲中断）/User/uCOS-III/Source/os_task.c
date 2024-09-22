/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TASK MANAGEMENT
*
* File    : OS_TASK.C
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

#define  MICRIUM_SOURCE
#include <os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_task__c = "$Id: $";
#endif

/*
************************************************************************************************************************
*                                                CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : p_tcb      is the TCB of the tack to change the priority for
*
*              prio_new   is the new priority
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                 is the call was successful
*                             OS_ERR_PRIO_INVALID         if the priority you specify is higher that the maximum allowed
*                                                         (i.e. >= (OS_CFG_PRIO_MAX-1))
*                             OS_ERR_STATE_INVALID        if the task is in an invalid state
*                             OS_ERR_TASK_CHANGE_PRIO_ISR if you tried to change the task's priority from an ISR
************************************************************************************************************************
*/

#if OS_CFG_TASK_CHANGE_PRIO_EN > 0u           //���ʹ���˺��� OSTaskChangePrio()
void  OSTaskChangePrio (OS_TCB   *p_tcb,      //Ŀ�������������ƿ�ָ��
                        OS_PRIO   prio_new,   //�����ȼ�
                        OS_ERR   *p_err)      //���ش�������
{
    CPU_BOOLEAN   self;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                       //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {                 //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();         //ִ�а�ȫ����쳣����
        return;                                 //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_TASK_CHANGE_PRIO_ISR;    //��������Ϊ�����ж��иı����ȼ���
        return;                                 //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u            //���ʹ�����ж��ӳٷ���
    if (prio_new == 0) {                        //��� prio_new Ϊ0
       *p_err = OS_ERR_PRIO_INVALID;            //��������Ϊ�����ȼ��Ƿ���
        return;                                 //���أ�ִֹͣ��
    }
#endif

    if (prio_new >= (OS_CFG_PRIO_MAX - 1u)) {   //��� prio_new �������Ʒ�Χ
       *p_err = OS_ERR_PRIO_INVALID;            //��������Ϊ�����ȼ��Ƿ���
        return;                                 //���أ�ִֹͣ��
    }

    if (p_tcb == (OS_TCB *)0) {                 //��� p_tcb Ϊ��
        CPU_CRITICAL_ENTER();                   //���ж�
        p_tcb = OSTCBCurPtr;                    //Ŀ������Ϊ��ǰ��������
        CPU_CRITICAL_EXIT();                    //���ж�
        self  = DEF_TRUE;                       //Ŀ������������
    } else {                                    //��� p_tcb �ǿ�
        self  = DEF_FALSE;                      //Ŀ������������
    }

    OS_CRITICAL_ENTER();                        //�����ٽ��
    switch (p_tcb->TaskState) {                 //����Ŀ������״̬���ദ��
        case OS_TASK_STATE_RDY:                 //����Ǿ���״̬
             OS_RdyListRemove(p_tcb);           //������Ӿ����б��Ƴ�
             p_tcb->Prio = prio_new;            //Ϊ�������������ȼ�
             OS_PrioInsert(p_tcb->Prio);        //�����ȼ�����н������ȼ�λ��1
             if (self == DEF_TRUE) {            //���Ŀ������������
                 OS_RdyListInsertHead(p_tcb);   //��Ŀ��������������б�ͷ��
             } else {                           //���Ŀ������������
                 OS_RdyListInsertTail(p_tcb);   //��Ŀ��������������б�β��
             }
             break;                             //����

        case OS_TASK_STATE_DLY:                 //�������ʱ״̬   
        case OS_TASK_STATE_SUSPENDED:           //����ǹ���״̬
        case OS_TASK_STATE_DLY_SUSPENDED:       //�������ʱ�б�����״̬
             p_tcb->Prio = prio_new;            //ֱ���޸���������ȼ�
             break;                             //����

        case OS_TASK_STATE_PEND:                //��������ȴ�״̬
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             switch (p_tcb->PendOn) {          //��������ȴ��Ķ�����ദ��
                 case OS_TASK_PEND_ON_TASK_Q:  //����ȴ�����������Ϣ
                 case OS_TASK_PEND_ON_TASK_SEM://����ȴ����������ź���
                 case OS_TASK_PEND_ON_FLAG:    //����ȴ������¼���־��
                      p_tcb->Prio = prio_new;  //ֱ���޸���������ȼ�
                      break;                   //����

                 case OS_TASK_PEND_ON_MUTEX:   //����ȴ����ǻ����ź���
                 case OS_TASK_PEND_ON_MULTI:   //����ȴ����Ƕ���ں˶���
                 case OS_TASK_PEND_ON_Q:       //����ȴ�������Ϣ����
                 case OS_TASK_PEND_ON_SEM:     //����ȴ����Ƕ�ֵ�ź���
                      OS_PendListChangePrio(p_tcb,    //�ı���������ȼ���
                                            prio_new);//�ڵȴ��б��е�λ�á�
                      break;                   //����
 
                 default:                      //�������ȴ��Ķ��󳬳�Ԥ��
                      break;                   //ֱ������
            }
             break;                            //����
 
        default:                               //���Ŀ������״̬����Ԥ��
             OS_CRITICAL_EXIT();               //�˳��ٽ��
            *p_err = OS_ERR_STATE_INVALID;     //��������Ϊ��״̬�Ƿ���
             return;                           //���أ�ִֹͣ��
    }

    OS_CRITICAL_EXIT_NO_SCHED();               //�˳��ٽ�Σ��޵��ȣ�

    OSSched();                                 //��������

   *p_err = OS_ERR_NONE;                       //��������Ϊ���޴���
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    CREATE A TASK
*
* Description: This function is used to have uC/OS-III manage the execution of a task.  Tasks can either be created
*              prior to the start of multitasking or by a running task.  A task cannot be created by an ISR.
*
* Arguments  : p_tcb          is a pointer to the task's TCB
*
*              p_name         is a pointer to an ASCII string to provide a name to the task.
*
*              p_task         is a pointer to the task's code
*
*              p_arg          is a pointer to an optional data area which can be used to pass parameters to
*                             the task when the task first executes.  Where the task is concerned it thinks
*                             it was invoked and passed the argument 'p_arg' as follows:
*
*                                 void Task (void *p_arg)
*                                 {
*                                     for (;;) {
*                                         Task code;
*                                     }
*                                 }
*
*              prio           is the task's priority.  A unique priority MUST be assigned to each task and the
*                             lower the number, the higher the priority.
*
*              p_stk_base     is a pointer to the base address of the stack (i.e. low address).
*
*              stk_limit      is the number of stack elements to set as 'watermark' limit for the stack.  This value
*                             represents the number of CPU_STK entries left before the stack is full.  For example,
*                             specifying 10% of the 'stk_size' value indicates that the stack limit will be reached
*                             when the stack reaches 90% full.
*
*              stk_size       is the size of the stack in number of elements.  If CPU_STK is set to CPU_INT08U,
*                             'stk_size' corresponds to the number of bytes available.  If CPU_STK is set to
*                             CPU_INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                             CPU_STK is set to CPU_INT32U, 'stk_size' contains the number of 32-bit entries
*                             available on the stack.
*
*              q_size         is the maximum number of messages that can be sent to the task
*
*              time_quanta    amount of time (in ticks) for time slice when round-robin between tasks.  Specify 0 to use
*                             the default.
*
*              p_ext          is a pointer to a user supplied memory location which is used as a TCB extension.
*                             For example, this user memory can hold the contents of floating-point registers
*                             during a context switch, the time each task takes to execute, the number of times
*                             the task has been switched-in, etc.
*
*              opt            contains additional information (or options) about the behavior of the task.
*                             See OS_OPT_TASK_xxx in OS.H.  Current choices are:
*
*                                 OS_OPT_TASK_NONE            No option selected
*                                 OS_OPT_TASK_STK_CHK         Stack checking to be allowed for the task
*                                 OS_OPT_TASK_STK_CLR         Clear the stack when the task is created
*                                 OS_OPT_TASK_SAVE_FP         If the CPU has floating-point registers, save them
*                                                             during a context switch.
*                                 OS_OPT_TASK_NO_TLS          If the caller doesn't want or need TLS (Thread Local 
*                                                             Storage) support for the task.  If you do not include this
*                                                             option, TLS will be supported by default.
*
*              p_err          is a pointer to an error code that will be set during this call.  The value pointer
*                             to by 'p_err' can be:
*
*                                 OS_ERR_NONE                    if the function was successful.
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the task after you called
*                                                                   OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_PRIO_INVALID            if the priority you specify is higher that the maximum
*                                                                   allowed (i.e. >= OS_CFG_PRIO_MAX-1) or,
*                                                                if OS_CFG_ISR_POST_DEFERRED_EN is set to 1 and you tried
*                                                                   to use priority 0 which is reserved.
*                                 OS_ERR_STK_INVALID             if you specified a NULL pointer for 'p_stk_base'
*                                 OS_ERR_STK_SIZE_INVALID        if you specified zero for the 'stk_size'
*                                 OS_ERR_STK_LIMIT_INVALID       if you specified a 'stk_limit' greater than or equal
*                                                                   to 'stk_size'
*                                 OS_ERR_TASK_CREATE_ISR         if you tried to create a task from an ISR.
*                                 OS_ERR_TASK_INVALID            if you specified a NULL pointer for 'p_task'
*                                 OS_ERR_TCB_INVALID             if you specified a NULL pointer for 'p_tcb'
*
* Returns    : A pointer to the TCB of the task created.  This pointer must be used as an ID (i.e handle) to the task.
************************************************************************************************************************
*/
/*$PAGE*/
void  OSTaskCreate (OS_TCB        *p_tcb,        //������ƿ�ָ��
                    CPU_CHAR      *p_name,       //��������
                    OS_TASK_PTR    p_task,       //������
                    void          *p_arg,        //���ݸ��������Ĳ���
                    OS_PRIO        prio,         //�������ȼ�
                    CPU_STK       *p_stk_base,   //�����ջ����ַ
                    CPU_STK_SIZE   stk_limit,    //��ջ��ʣ������
                    CPU_STK_SIZE   stk_size,     //��ջ��С
                    OS_MSG_QTY     q_size,       //������Ϣ����
                    OS_TICK        time_quanta,  //ʱ��Ƭ
                    void          *p_ext,        //������չ
                    OS_OPT         opt,          //ѡ��
                    OS_ERR        *p_err)        //���ش�������
{
    CPU_STK_SIZE   i;
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG_ID      reg_nbr;
#endif
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_ID      id;
#endif

    CPU_STK       *p_sp;
    CPU_STK       *p_stk_limit;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                             //���ʹ���˰�ȫ���         
    if (p_err == (OS_ERR *)0) {                       //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();               //ִ�а�ȫ����쳣����
        return;                                       //���أ�ִֹͣ��
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508                    //���ʹ���˰�ȫ�ؼ�
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {      //����ڵ���OSSafetyCriticalStart()�󴴽�
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;       //��������Ϊ���Ƿ������ں˶���
        return;                                       //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u                //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {        //����ú��������ж��б�����
       *p_err = OS_ERR_TASK_CREATE_ISR;               //��������Ϊ�����ж��д�������
        return;                                       //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                            //���ʹ���˲������      
    if (p_tcb == (OS_TCB *)0) {                       //��� p_tcb Ϊ��     
       *p_err = OS_ERR_TCB_INVALID;                   //��������Ϊ��������ƿ�Ƿ���
        return;                                       //���أ�ִֹͣ��
    }
    if (p_task == (OS_TASK_PTR)0) {                   //��� p_task Ϊ��
       *p_err = OS_ERR_TASK_INVALID;                  //��������Ϊ���������Ƿ���
        return;                                       //���أ�ִֹͣ��
    }
    if (p_stk_base == (CPU_STK *)0) {                 //��� p_stk_base Ϊ��      
       *p_err = OS_ERR_STK_INVALID;                   //��������Ϊ�������ջ�Ƿ���
        return;                                       //���أ�ִֹͣ��
    }
    if (stk_size < OSCfg_StkSizeMin) {                      //������������Ķ�ջ�ռ�С����СҪ��
       *p_err = OS_ERR_STK_SIZE_INVALID;                    //��������Ϊ�������ջ�ռ�Ƿ���
        return;                                             //���أ�ִֹͣ��
    }
    if (stk_limit >= stk_size) {                            //�����ջ���ƿռ�ռ����������ջ
       *p_err = OS_ERR_STK_LIMIT_INVALID;                   //��������Ϊ����ջ���ƷǷ���
        return;                                             //���أ�ִֹͣ��
    }
    if (prio >= OS_CFG_PRIO_MAX) {                          //����������ȼ����������Ʒ�Χ     
       *p_err = OS_ERR_PRIO_INVALID;                        //��������Ϊi�����ȼ��Ƿ���
        return;                                             //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                        //���ʹ�����ж��ӳٷ���
    if (prio == (OS_PRIO)0) {                               //������ȼ���0���ж��ӳ��ύ�������裩
        if (p_tcb != &OSIntQTaskTCB) {                      //��� p_tcb �����ж��ӳ��ύ����
           *p_err = OS_ERR_PRIO_INVALID;                    //��������Ϊ�����ȼ��Ƿ���
            return;                                         //���أ�ִֹͣ��
        }
    }
#endif

    if (prio == (OS_CFG_PRIO_MAX - 1u)) {                   //�����������ȼ�Ϊ��ͣ������������裩
        if (p_tcb != &OSIdleTaskTCB) {                      //��� p_tcb ���ǿ�������
           *p_err = OS_ERR_PRIO_INVALID;                    //��������Ϊ�����ȼ��Ƿ���
            return;                                         //���أ�ִֹͣ��
        }
    }

    OS_TaskInitTCB(p_tcb);                                  //��ʼ��������ƿ� p_tcb

   *p_err = OS_ERR_NONE;                                    //��������Ϊ���޴���
    
    if ((opt & OS_OPT_TASK_STK_CHK) != (OS_OPT)0) {         //���ѡ���˼���ջ
        if ((opt & OS_OPT_TASK_STK_CLR) != (OS_OPT)0) {     //���ѡ���������ջ
            p_sp = p_stk_base;                              //��ȡ��ջ�Ļ���ַ
            for (i = 0u; i < stk_size; i++) {               //����ջ�����飩Ԫ���������
               *p_sp = (CPU_STK)0;                          
                p_sp++;
            }
        }
    }
    /* ��ʼ������Ķ�ջ�ṹ */                                               
#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)         //��� CPU ��ջ��������Ϊ�Ӹߵ���
    p_stk_limit = p_stk_base + stk_limit;                   //��ջ���ƿռ俿��ջ�ĵ͵�ַ��
#else                                                       //��� CPU ��ջ��������Ϊ�ӵ͵���
    p_stk_limit = p_stk_base + (stk_size - 1u) - stk_limit; //��ջ���ƿռ俿��ջ�ĸߵ�ַ��
#endif
    /* ��ʼ�������ջ */
    p_sp = OSTaskStkInit(p_task,                            
                         p_arg,
                         p_stk_base,
                         p_stk_limit,
                         stk_size,
                         opt);

    /* ��ʼ��������ƿ� */                                                        
    p_tcb->TaskEntryAddr = p_task;                          //���������ַ
    p_tcb->TaskEntryArg  = p_arg;                           //���洫�ݸ��������Ĳ���

    p_tcb->NamePtr       = p_name;                          //������������

    p_tcb->Prio          = prio;                            //�����������ռ�

    p_tcb->StkPtr        = p_sp;                            //�����ջ��ջ��ָ��
    p_tcb->StkLimitPtr   = p_stk_limit;                     //�����ջ���Ƶ��ٽ��ַ

    p_tcb->TimeQuanta    = time_quanta;                     //����ʱ��Ƭ
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u                        //���ʹ����ʱ��Ƭ��ת����
    if (time_quanta == (OS_TICK)0) {                        //��� time_quanta Ϊ0
        p_tcb->TimeQuantaCtr = OSSchedRoundRobinDfltTimeQuanta; //ʹ��Ĭ�ϵ�ʱ��ƬСС
    } else {                                                //��� time_quanta ��0
        p_tcb->TimeQuantaCtr = time_quanta;                 //ʱ��ƬΪ time_quanta
    }
#endif
    p_tcb->ExtPtr        = p_ext;                           //����������չ���ֵ�ָ��
    p_tcb->StkBasePtr    = p_stk_base;                      //�����ջ�Ļ���ַ
    p_tcb->StkSize       = stk_size;                        //�����ջ�Ĵ�С
    p_tcb->Opt           = opt;                             //����ѡ��

#if OS_CFG_TASK_REG_TBL_SIZE > 0u                           //�������ʹ������Ĵ���
    for (reg_nbr = 0u; reg_nbr < OS_CFG_TASK_REG_TBL_SIZE; reg_nbr++) { //������Ĵ�����ʼ��Ϊ0
        p_tcb->RegTbl[reg_nbr] = (OS_REG)0;
    }
#endif

#if OS_CFG_TASK_Q_EN > 0u                                   //���ʹ����������Ϣ����
    OS_MsgQInit(&p_tcb->MsgQ,                               //��ʼ��������Ϣ����
                q_size);
#else                                                       //���������������Ϣ����
    (void)&q_size;                                          //���� q_size ��Ч
#endif

    OSTaskCreateHook(p_tcb);                                //�����û�����Ĺ��Ӻ���

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
        p_tcb->TLS_Tbl[id] = (OS_TLS)0;
    }
    OS_TLS_TaskCreate(p_tcb);                               /* Call TLS hook                                          */
#endif
    /* �������������б� */                                  
    OS_CRITICAL_ENTER();                                    //�����ٽ��
    OS_PrioInsert(p_tcb->Prio);                             //�þ������ȼ�λӳ�������Ӧ���ȼ����ھ���״̬
    OS_RdyListInsertTail(p_tcb);                            //���´����������������б�β��

#if OS_CFG_DBG_EN > 0u                                      //���ʹ���˵��Դ���ͱ��� 
    OS_TaskDbgListAdd(p_tcb);                               //��������뵽�������˫���б�
#endif

    OSTaskQty++;                                            //������Ŀ��1

    if (OSRunning != OS_STATE_OS_RUNNING) {                 //���ϵͳ��δ����
        OS_CRITICAL_EXIT();                                 //�˳��ٽ��
        return;                                             //���أ�ִֹͣ��
    }
    /* ���ϵͳ�Ѿ����� */
    OS_CRITICAL_EXIT_NO_SCHED();                            //�˳��ٽ�Σ��޵��ȣ�

    OSSched();                                              //�����������
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                     DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by specifying a NULL
*              pointer for 'p_tcb'.  The deleted task is returned to the dormant state and can be re-activated by
*              creating the deleted task again.
*
* Arguments  : p_tcb      is the TCB of the tack to delete
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                  if the call is successful
*                             OS_ERR_STATE_INVALID         if the state of the task is invalid
*                             OS_ERR_TASK_DEL_IDLE         if you attempted to delete uC/OS-III's idle task
*                             OS_ERR_TASK_DEL_INVALID      if you attempted to delete uC/OS-III's ISR handler task
*                             OS_ERR_TASK_DEL_ISR          if you tried to delete a task from an ISR
*
* Note(s)    : 1) 'p_err' gets set to OS_ERR_NONE before OSSched() to allow the returned error code to be monitored even
*                 for a task that is deleting itself. In this case, 'p_err' MUST point to a global variable that can be
*                 accessed by another task.
************************************************************************************************************************
*/

#if OS_CFG_TASK_DEL_EN > 0u                      //���ʹ���˺��� OSTaskDel()
void  OSTaskDel (OS_TCB  *p_tcb,                 //Ŀ��������ƿ�ָ��
                 OS_ERR  *p_err)                 //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                      //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {                //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();        //ִ�а�ȫ����쳣����
        return;                                //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_TASK_DEL_ISR;           //��������Ϊ�����ж���ɾ������
        return;                                //���أ�ִֹͣ��
    }
#endif

    if (p_tcb == &OSIdleTaskTCB) {             //���Ŀ�������ǿ�������
       *p_err = OS_ERR_TASK_DEL_IDLE;          //��������Ϊ��ɾ����������
        return;                                //���أ�ִֹͣ��
    }

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u           //���ʹ�����ж��ӳٷ���
    if (p_tcb == &OSIntQTaskTCB) {             //���Ŀ���������ж��ӳ��ύ����
       *p_err = OS_ERR_TASK_DEL_INVALID;       //��������Ϊ���Ƿ�ɾ������
        return;                                //���أ�ִֹͣ��
    }
#endif

    if (p_tcb == (OS_TCB *)0) {                //��� p_tcb Ϊ��
        CPU_CRITICAL_ENTER();                  //���ж�
        p_tcb  = OSTCBCurPtr;                  //Ŀ��������Ϊ����
        CPU_CRITICAL_EXIT();                   //���ж�
    }

    OS_CRITICAL_ENTER();                       //�����ٽ��
    switch (p_tcb->TaskState) {                //����Ŀ�����������״̬���ദ��
        case OS_TASK_STATE_RDY:                //����Ǿ���״̬
             OS_RdyListRemove(p_tcb);          //������Ӿ����б��Ƴ�
             break;                            //����

        case OS_TASK_STATE_SUSPENDED:          //����ǹ���״̬
             break;                            //ֱ������

        case OS_TASK_STATE_DLY:                //���������ʱ״̬
        case OS_TASK_STATE_DLY_SUSPENDED:      
             OS_TickListRemove(p_tcb);         //������ӽ����б��Ƴ�
             break;                            //����

        case OS_TASK_STATE_PEND:               //��������ȴ�״̬
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             OS_TickListRemove(p_tcb);         //������ӽ����б��Ƴ�
             switch (p_tcb->PendOn) {          //��������ĵȴ�������ദ��
                 case OS_TASK_PEND_ON_NOTHING: //���û�ڵȴ��ں˶���
                 case OS_TASK_PEND_ON_TASK_Q:  //����ȴ�����������Ϣ����            
                 case OS_TASK_PEND_ON_TASK_SEM://����ȴ����������ź���
                      break;                   //ֱ������

                 case OS_TASK_PEND_ON_FLAG:    //����ȴ������¼���־��  
                 case OS_TASK_PEND_ON_MULTI:   //����ȴ�����ں˶���
                 case OS_TASK_PEND_ON_MUTEX:   //����ȴ����ǻ����ź���
                 case OS_TASK_PEND_ON_Q:       //����ȴ�������Ϣ����
                 case OS_TASK_PEND_ON_SEM:     //����ȴ����Ƕ�ֵ�ź���
                      OS_PendListRemove(p_tcb);//������ӵȴ��б��Ƴ�
                      break;                   //����

                 default:                      //����ȴ����󳬳�Ԥ��
                      break;                   //ֱ������
             }
             break;                            //����

        default:                               //���Ŀ������״̬����Ԥ��
            OS_CRITICAL_EXIT();                //�˳��ٽ��
           *p_err = OS_ERR_STATE_INVALID;      //��������Ϊ��״̬�Ƿ���
            return;                            //���أ�ִֹͣ��
    }

#if OS_CFG_TASK_Q_EN > 0u                      //���ʹ����������Ϣ����
    (void)OS_MsgQFreeAll(&p_tcb->MsgQ);        //�ͷ����������������Ϣ
#endif

    OSTaskDelHook(p_tcb);                      //�����û��Զ���Ĺ��Ӻ���

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_TaskDel(p_tcb);                                  /* Call TLS hook                                          */
#endif

#if OS_CFG_DBG_EN > 0u                         //���ʹ���˵��Դ���ͱ���
    OS_TaskDbgListRemove(p_tcb);               //��������������˫���б��Ƴ�
#endif
    OSTaskQty--;                               //������Ŀ��1

    OS_TaskInitTCB(p_tcb);                     //��ʼ��������ƿ�
    p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;//�궨�����ѱ�ɾ��

    OS_CRITICAL_EXIT_NO_SCHED();               //�˳��ٽ�Σ��޵��ȣ�

   *p_err = OS_ERR_NONE;                       //��������Ϊ���޴���

    OSSched();                                 //��������
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    FLUSH TASK's QUEUE
*
* Description: This function is used to flush the task's internal message queue.
*
* Arguments  : p_tcb       is a pointer to the task's OS_TCB.  Specifying a NULL pointer indicates that you wish to
*                          flush the message queue of the calling task.
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE           upon success
*                              OS_ERR_FLUSH_ISR      if you called this function from an ISR
*
* Returns     : The number of entries freed from the queue
*
* Note(s)     : 1) You should use this function with great care because, when to flush the queue, you LOOSE the
*                  references to what the queue entries are pointing to and thus, you could cause 'memory leaks'.  In
*                  other words, the data you are pointing to that's being referenced by the queue entries should, most
*                  likely, need to be de-allocated (i.e. freed).
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u
OS_MSG_QTY  OSTaskQFlush (OS_TCB  *p_tcb,
                          OS_ERR  *p_err)
{
    OS_MSG_QTY  entries;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_MSG_QTY)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* Can't flush a message queue from an ISR                */
       *p_err = OS_ERR_FLUSH_ISR;
        return ((OS_MSG_QTY)0);
    }
#endif

    if (p_tcb == (OS_TCB *)0) {                             /* Flush message queue of calling task?                   */
        CPU_CRITICAL_ENTER();
        p_tcb = OSTCBCurPtr;
        CPU_CRITICAL_EXIT();
    }

    OS_CRITICAL_ENTER();
    entries = OS_MsgQFreeAll(&p_tcb->MsgQ);                 /* Return all OS_MSGs to the OS_MSG pool                  */
    OS_CRITICAL_EXIT();
   *p_err   = OS_ERR_NONE;
    return (entries);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  WAIT FOR A MESSAGE
*
* Description: This function causes the current task to wait for a message to be posted to it.
*
* Arguments  : timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for a
*                            message to arrive up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever or, until a message arrives.
*
*              opt           determines whether the user wants to block if the task's queue is empty or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message was
*                            received.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you will not get the
*                            timestamp.  In other words, passing a NULL pointer is valid and indicates that you don't
*                            need the timestamp.
*
*              p_err         is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_PEND_ABORT
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                                OS_ERR_Q_EMPTY
*                                OS_ERR_SCHED_LOCKED       If the scheduler is locked
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*
* Returns    : A pointer to the message received or a NULL pointer upon error.
*
* Note(s)    : 1) It is possible to receive NULL pointers when there are no errors.
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u                     //���ʹ����������Ϣ����
void  *OSTaskQPend (OS_TICK       timeout,    //�ȴ����ޣ���λ��ʱ�ӽ��ģ�
                    OS_OPT        opt,        //ѡ��
                    OS_MSG_SIZE  *p_msg_size, //������Ϣ����
                    CPU_TS       *p_ts,       //����ʱ���
                    OS_ERR       *p_err)      //���ش�������
{
    OS_MSG_Q     *p_msg_q;
    void         *p_void;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((void *)0);             //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ISR;                //��������Ϊ�����ж�����ֹ�ȴ���
        return ((void *)0);                     //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                   //���ʹ���˲������
    if (p_msg_size == (OS_MSG_SIZE *)0) {    //��� p_msg_size Ϊ��
       *p_err = OS_ERR_PTR_INVALID;          //��������Ϊ��ָ�벻���á�
        return ((void *)0);                  //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                           //����ѡ����ദ��
        case OS_OPT_PEND_BLOCKING:           //���ѡ����Ԥ����
        case OS_OPT_PEND_NON_BLOCKING:
             break;                          //ֱ������

        default:                             //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;     //��������Ϊ��ѡ��Ƿ���
             return ((void *)0);             //����0���д��󣩣�ִֹͣ��
    }
#endif

    if (p_ts != (CPU_TS *)0) {    //��� p_ts �ǿ�
       *p_ts  = (CPU_TS  )0;      //��ʼ�������㣩p_ts�������ڷ���ʱ���
    }

    CPU_CRITICAL_ENTER();                                  //���ж�
    p_msg_q = &OSTCBCurPtr->MsgQ;                          //��ȡ��ǰ�������Ϣ����
    p_void  = OS_MsgQGet(p_msg_q,                          //�Ӷ������ȡһ����Ϣ
                         p_msg_size,
                         p_ts,
                         p_err);
    if (*p_err == OS_ERR_NONE) {                          //�����ȡ��Ϣ�ɹ�
#if OS_CFG_TASK_PROFILE_EN > 0u                        //���ʹ����������ƿ�ļ������
        if (p_ts != (CPU_TS *)0) {                                         //��� p_ts 
            OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - *p_ts;               //�ǿգ�����
            if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime) {//�ȴ�������
                OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;  //Ϣ���е���
            }                                                              //��ʱ���¼��
        }
#endif
        CPU_CRITICAL_EXIT();                             //���ж� 
        return (p_void);                                 //������Ϣ����
    }
    /* �����ȡ��Ϣ���ɹ���������û����Ϣ�� */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //���ѡ���˲���������
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                 //��������Ϊ��ȱ��������
        CPU_CRITICAL_EXIT();                             //���ж�
        return ((void *)0);                              //����0���д��󣩣�ִֹͣ��
    } else {                                             //���ѡ���˶�������
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //�������������
            CPU_CRITICAL_EXIT();                         //���ж�
           *p_err = OS_ERR_SCHED_LOCKED;                 //��������Ϊ��������������
            return ((void *)0);                          //����0���д��󣩣�ִֹͣ��
        }
    }
    /* ���������δ���� */                                                       
    OS_CRITICAL_ENTER_CPU_EXIT();                        //�����������ؿ��ж�
    OS_Pend((OS_PEND_DATA *)0,                           //������ǰ���񣬵ȴ���Ϣ 
            (OS_PEND_OBJ  *)0,
            (OS_STATE      )OS_TASK_PEND_ON_TASK_Q,
            (OS_TICK       )timeout);
    OS_CRITICAL_EXIT_NO_SCHED();                         //�������������޵��ȣ�

    OSSched();                                           //��������
    /* ��ǰ���񣨻����Ϣ���е���Ϣ�����Լ������� */
    CPU_CRITICAL_ENTER();                                //���ж�
    switch (OSTCBCurPtr->PendStatus) {                   //��������ĵȴ�״̬���ദ��
        case OS_STATUS_PEND_OK:                          //��������ѳɹ������Ϣ
             p_void      = OSTCBCurPtr->MsgPtr;          //��ȡ��Ϣ���ݵ�ַ
            *p_msg_size  = OSTCBCurPtr->MsgSize;         //��ȡ��Ϣ����
             if (p_ts != (CPU_TS *)0) {                  //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS;                //��ȡ����ȵ���Ϣʱ��ʱ���
#if OS_CFG_TASK_PROFILE_EN > 0u                          //���ʹ����������ƿ�ļ������
                OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - OSTCBCurPtr->TS;     //���µȴ�
                if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime) {//������Ϣ
                    OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;  //���е���
                }                                                              //��ʱ���
#endif                                                                         //¼��
             }
            *p_err = OS_ERR_NONE;                        //��������Ϊ���޴���
             break;                                      //����

        case OS_STATUS_PEND_ABORT:                       //����ȴ�����ֹ
             p_void     = (void      *)0;                //������Ϣ����Ϊ��
            *p_msg_size = (OS_MSG_SIZE)0;                //������Ϣ��СΪ0
             if (p_ts  != (CPU_TS *)0) {                 //��� p_ts �ǿ�
                *p_ts   = (CPU_TS  )0;                   //���� p_ts
             }
            *p_err      =  OS_ERR_PEND_ABORT;            //��������Ϊ���ȴ�����ֹ��
             break;                                      //����

        case OS_STATUS_PEND_TIMEOUT:                     //����ȴ���ʱ��
        default:                                         //��������״̬����Ԥ�ڡ�
             p_void     = (void      *)0;                //������Ϣ����Ϊ��
            *p_msg_size = (OS_MSG_SIZE)0;                //������Ϣ��СΪ0
             if (p_ts  != (CPU_TS *)0) {                 //��� p_ts �ǿ�
                *p_ts   =  OSTCBCurPtr->TS;
             }
            *p_err      =  OS_ERR_TIMEOUT;               //������Ϊ���ȴ���ʱ��
             break;                                      //����
    }
    CPU_CRITICAL_EXIT();                                 //���ж�
    return (p_void);                                     //������Ϣ���ݵ�ַ
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              ABORT WAITING FOR A MESSAGE
*
* Description: This function aborts & readies the task specified.  This function should be used to fault-abort the wait
*              for a message, rather than to normally post the message to the task via OSTaskQPost().
*
* Arguments  : p_tcb     is a pointer to the task to pend abort
*
*              opt       provides options for this function:
*
*                            OS_OPT_POST_NONE         No option specified
*                            OS_OPT_POST_NO_SCHED     Indicates that the scheduler will not be called.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE              If the task was readied and informed of the aborted wait
*                            OS_ERR_PEND_ABORT_ISR    If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE   If task was not pending on a message and thus there is nothing to
*                                                     abort.
*                            OS_ERR_PEND_ABORT_SELF   If you passed a NULL pointer for 'p_tcb'
*
* Returns    : == DEF_FALSE   if task was not waiting for a message, or upon error.
*              == DEF_TRUE    if task was waiting for a message and was readied and informed.
************************************************************************************************************************
*/

#if (OS_CFG_TASK_Q_EN > 0u) && (OS_CFG_TASK_Q_PEND_ABORT_EN > 0u)//���ʹ���˸ú���
CPU_BOOLEAN  OSTaskQPendAbort (OS_TCB  *p_tcb,                   //Ŀ������
                               OS_OPT   opt,                     //ѡ��
                               OS_ERR  *p_err)                   //���ش�������
{
    CPU_TS         ts;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return (DEF_FALSE);             //���أ��д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ABORT_ISR;         //��������Ϊ�����ж�����ֹ�ȴ���
        return (DEF_FALSE);                    //���أ��д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                   //���ʹ���˲������
    switch (opt) {                           //����ѡ����ദ��
        case OS_OPT_POST_NONE:               //���ѡ����Ԥ����
        case OS_OPT_POST_NO_SCHED:
             break;                          //ֱ������

        default:                             //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;     //��������Ϊ��ѡ��Ƿ���
             return (DEF_FALSE);             //���أ��д��󣩣�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                           //���ж�
#if OS_CFG_ARG_CHK_EN > 0u                          //���ʹ���˲������
    if ((p_tcb == (OS_TCB *)0) ||                   //���Ŀ������Ϊ�գ�
        (p_tcb == OSTCBCurPtr)) {                   //��������
        CPU_CRITICAL_EXIT();                        //���ж�
       *p_err = OS_ERR_PEND_ABORT_SELF;             //��������Ϊ����ֹ����
        return (DEF_FALSE);                         //���أ��д��󣩣�ִֹͣ��
    }
#endif

    if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_Q) {  //�������û�ڵȴ�������Ϣ����
        CPU_CRITICAL_EXIT();                        //���ж�
       *p_err = OS_ERR_PEND_ABORT_NONE;             //��������Ϊ����ֹ��������
        return (DEF_FALSE);                         //���أ��д��󣩣�ִֹͣ��
    }
    /* ����������ڵȴ�������Ϣ���� */
    OS_CRITICAL_ENTER_CPU_EXIT();                   //�����������ؿ��ж�
    ts = OS_TS_GET();                               //��ȡʱ�����Ϊ��ֹʱ��
    OS_PendAbort((OS_PEND_OBJ *)0,                  //��ֹ���������Ϣ���еĵȴ�
                 p_tcb,
                 ts);
    OS_CRITICAL_EXIT_NO_SCHED();                    //�������������޵��ȣ�
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {//���ѡ���˵�������
        OSSched();                                  //��������
    }
   *p_err = OS_ERR_NONE;                            //��������Ϊ���޴���
    return (DEF_TRUE);                              //���أ���ֹ�ɹ���
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A TASK
*
* Description: This function sends a message to a task
*
* Arguments  : p_tcb      is a pointer to the TCB of the task receiving a message.  If you specify a NULL pointer then
*                         the message will be posted to the task's queue of the calling task.  In other words, you'd be
*                         posting a message to yourself.
*
*              p_void     is a pointer to the message to send.
*
*              msg_size   is the size of the message sent (in #bytes)
*
*              opt        specifies whether the post will be FIFO or LIFO:
*
*                             OS_OPT_POST_FIFO       Post at the end   of the queue
*                             OS_OPT_POST_LIFO       Post at the front of the queue
*
*                             OS_OPT_POST_NO_SCHED   Do not run the scheduler after the post
*
*                          Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*
*              p_err      is a pointer to a variable that will hold the error code associated
*                         with the outcome of this call.  Errors can be:
*
*                             OS_ERR_NONE            The call was successful and the message was sent
*                             OS_ERR_Q_MAX           If the queue is full
*                             OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs available from the pool
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u                  //���ʹ����������Ϣ����
void  OSTaskQPost (OS_TCB       *p_tcb,    //Ŀ������
                   void         *p_void,   //��Ϣ���ݵ�ַ
                   OS_MSG_SIZE   msg_size, //��Ϣ����
                   OS_OPT        opt,      //ѡ��
                   OS_ERR       *p_err)    //���ش�������
{
    CPU_TS   ts;



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    switch (opt) {                        //����ѡ����ദ��
        case OS_OPT_POST_FIFO:            //���ѡ����Ԥ����
        case OS_OPT_POST_LIFO:
        case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
             break;                       //ֱ������

        default:                          //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;  //��������Ϊ��ѡ��Ƿ���
             return;                      //���أ�ִֹͣ��
    }
#endif

    ts = OS_TS_GET();                                  //��ȡʱ���

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                   //���ʹ�����ж��ӳٷ���
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {         //����ú������ж��б�����
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_MSG, //����Ϣ�ȷ������ж���Ϣ����  
                    (void      *)p_tcb,
                    (void      *)p_void,
                    (OS_MSG_SIZE)msg_size,
                    (OS_FLAGS   )0,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return;                                         //����
    }
#endif

    OS_TaskQPost(p_tcb,                                 //����Ϣֱ�ӷ���
                 p_void,
                 msg_size,
                 opt,
                 ts,
                 p_err);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                       GET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to obtain the current value of a task register.  Task registers are application
*              specific and can be used to store task specific values such as 'error numbers' (i.e. errno), statistics,
*              etc.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to read the register from.  If 'p_tcb' is a
*                        NULL pointer then you will get the register of the current task.
*
*              id        is the 'id' of the desired task variable.  Note that the 'id' must be less than
*                        OS_CFG_TASK_REG_TBL_SIZE
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_REG_ID_INVALID  if the 'id' is not between 0 and OS_CFG_TASK_REG_TBL_SIZE-1
*
* Returns    : The current value of the task's register or 0 if an error is detected.
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u          //���Ϊ������������Ĵ���
OS_REG  OSTaskRegGet (OS_TCB     *p_tcb,   //Ŀ��������ƿ��ָ��
                      OS_REG_ID   id,      //����Ĵ�����id�������±꣩
                      OS_ERR     *p_err)   //���ش�������
{
    OS_REG     value;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                 //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {           //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();   //ִ�а�ȫ����쳣����
        return ((OS_REG)0);               //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    if (id >= OS_CFG_TASK_REG_TBL_SIZE) { //��� id ������Χ
       *p_err = OS_ERR_REG_ID_INVALID;    //��������Ϊ��id �Ƿ���
        return ((OS_REG)0);               //����0���д��󣩣�ִֹͣ��
    }
#endif
 
    CPU_CRITICAL_ENTER();                 //���ж�
    if (p_tcb == (OS_TCB *)0) {           //��� p_tcb Ϊ��
        p_tcb = OSTCBCurPtr;              //Ŀ������Ϊ��ǰ������������
    }
    value = p_tcb->RegTbl[id];            //��ȡ����Ĵ���������
    CPU_CRITICAL_EXIT();                  //���ж�
   *p_err = OS_ERR_NONE;                  //��������Ϊ���޴���
    return ((OS_REG)value);               //��������Ĵ���������
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                    ALLOCATE THE NEXT AVAILABLE TASK REGISTER ID
*
* Description: This function is called to obtain a task register ID.  This function thus allows task registers IDs to be
*              allocated dynamically instead of statically.
*
* Arguments  : p_err       is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE               if the call was successful
*                            OS_ERR_NO_MORE_ID_AVAIL   if you are attempting to assign more task register IDs than you 
*                                                           have available through OS_CFG_TASK_REG_TBL_SIZE.
*
* Returns    : The next available task register 'id' or OS_CFG_TASK_REG_TBL_SIZE if an error is detected.
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_REG_ID  OSTaskRegGetID (OS_ERR  *p_err)
{
    OS_REG_ID  id;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_REG_ID)OS_CFG_TASK_REG_TBL_SIZE);
    }
#endif

    CPU_CRITICAL_ENTER();
    if (OSTaskRegNextAvailID >= OS_CFG_TASK_REG_TBL_SIZE) {       /* See if we exceeded the number of IDs available   */
       *p_err = OS_ERR_NO_MORE_ID_AVAIL;                          /* Yes, cannot allocate more task register IDs      */
        CPU_CRITICAL_EXIT();
        return ((OS_REG_ID)OS_CFG_TASK_REG_TBL_SIZE);
    }
     
    id    = OSTaskRegNextAvailID;								  /* Assign the next available ID                     */
    OSTaskRegNextAvailID++;										  /* Increment available ID for next request          */
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (id);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                       SET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to change the current value of a task register.  Task registers are application
*              specific and can be used to store task specific values such as 'error numbers' (i.e. errno), statistics,
*              etc.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to set the register for.  If 'p_tcb' is a NULL
*                        pointer then you will change the register of the current task.
*
*              id        is the 'id' of the desired task register.  Note that the 'id' must be less than
*                        OS_CFG_TASK_REG_TBL_SIZE
*
*              value     is the desired value for the task register.
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_REG_ID_INVALID  if the 'id' is not between 0 and OS_CFG_TASK_REG_TBL_SIZE-1
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u        //���Ϊ������������Ĵ���
void  OSTaskRegSet (OS_TCB     *p_tcb,   //Ŀ��������ƿ��ָ��
                    OS_REG_ID   id,      //����Ĵ�����id�������±꣩
                    OS_REG      value,   //��������Ĵ���������
                    OS_ERR     *p_err)   //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    if (id >= OS_CFG_TASK_REG_TBL_SIZE) { //��� id ������Χ
       *p_err = OS_ERR_REG_ID_INVALID;    //��������Ϊ��id �Ƿ���
        return;                           //���أ�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                 //���ж�
    if (p_tcb == (OS_TCB *)0) {           //��� p_tcb Ϊ��
        p_tcb = OSTCBCurPtr;              //Ŀ������Ϊ��ǰ������������
    }
    p_tcb->RegTbl[id] = value;            //��������Ĵ���������
    CPU_CRITICAL_EXIT();                  //���ж�
   *p_err             = OS_ERR_NONE;      //��������Ϊ���޴���
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that will remove an
*              explicit task suspension.
*
* Arguments  : p_tcb      Is a pointer to the task's OS_TCB to resume
*
*              p_err      Is a pointer to a variable that will contain an error code returned by this function
*
*                             OS_ERR_NONE                  if the requested task is resumed
*                             OS_ERR_STATE_INVALID         if the task is in an invalid state
*                             OS_ERR_TASK_RESUME_ISR       if you called this function from an ISR
*                             OS_ERR_TASK_RESUME_SELF      You cannot resume 'self'
*                             OS_ERR_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u            //���ʹ���˺��� OSTaskResume() 
void  OSTaskResume (OS_TCB  *p_tcb,        //������ƿ�ָ��
                    OS_ERR  *p_err)        //���ش�������
{
    CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
									   //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
									   // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                 //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {           //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();   //ִ�а�ȫ����쳣����
        return;                           //���أ�ִֹͣ��
    }
#endif

#if (OS_CFG_ISR_POST_DEFERRED_EN   == 0u) && \
    (OS_CFG_CALLED_FROM_ISR_CHK_EN >  0u)      //����������ж��ӳٷ������ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //������ж��е��øú���
       *p_err = OS_ERR_TASK_RESUME_ISR;        //��������Ϊ�����ж��лָ�����
        return;                                //���أ�ִֹͣ��
    }
#endif


    CPU_CRITICAL_ENTER();                     //���ж�
#if OS_CFG_ARG_CHK_EN > 0u                    //���ʹ���˲������
    if ((p_tcb == (OS_TCB *)0) ||             //������ָ�����Ϊ�ջ�������
        (p_tcb == OSTCBCurPtr)) {
        CPU_CRITICAL_EXIT();                  //���ж�
       *p_err  = OS_ERR_TASK_RESUME_SELF;     //��������Ϊ���ָ�����
        return;                               //���أ�ִֹͣ��
    }
#endif
    CPU_CRITICAL_EXIT();                      //���ж�

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                      //���ʹ�����ж��ӳٷ���
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {            //����ú������ж��б�����
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_RESUME, //�ѻָ�������������ж���Ϣ����
                    (void      *)p_tcb,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )0,
                    (CPU_TS     )0,
                    (OS_ERR    *)p_err);
        return;                                          //���أ�ִֹͣ��
    }
#endif
    /* ����������ж��ӳٷ����������ж��е��øú��� */
    OS_TaskResume(p_tcb, p_err);                        //ֱ�ӽ����� p_tcb ���� 
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              WAIT FOR A TASK SEMAPHORE
*
* Description: This function is called to block the current task until a signal is sent by another task or ISR.
*
* Arguments  : timeout       is the amount of time you are will to wait for the signal
*
*              opt           determines whether the user wants to block if a semaphore post was not received:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the semaphore was posted
*                            or pend aborted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you will not get the
*                            timestamp.  In other words, passing a NULL pointer is valid and indicates that you don't
*                            need the timestamp.
*
*              p_err         is a pointer to an error code that will be set by this function
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_PEND_ABORT
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but no signal was received
*                                OS_ERR_SCHED_LOCKED       If the scheduler is locked
*                                OS_ERR_STATUS_INVALID     If the pend status is invalid
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*
* Returns    : The current count of signals the task received, 0 if none.
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemPend (OS_TICK   timeout,  //�ȴ���ʱʱ��
                           OS_OPT    opt,      //ѡ��
                           CPU_TS   *p_ts,     //����ʱ���
                           OS_ERR   *p_err)    //���ش�������
{
    OS_SEM_CTR    ctr;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return ((OS_SEM_CTR)0);          //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ISR;                //���ش�������Ϊ�����ж��еȴ���
        return ((OS_SEM_CTR)0);                 //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //���ʹ���˲������
    switch (opt) {                          //����ѡ����ദ��
        case OS_OPT_PEND_BLOCKING:          //���ѡ����Ԥ����
        case OS_OPT_PEND_NON_BLOCKING:
             break;                         //ֱ������

        default:                            //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;    //��������Ϊ��ѡ��Ƿ���
             return ((OS_SEM_CTR)0);        //����0���д��󣩣�ִֹͣ��
    }
#endif

    if (p_ts != (CPU_TS *)0) {      //��� p_ts �ǿ�
       *p_ts  = (CPU_TS  )0;        //���㣨��ʼ����p_ts
    }

    CPU_CRITICAL_ENTER();                        //���ж�  
    if (OSTCBCurPtr->SemCtr > (OS_SEM_CTR)0) {   //��������ź�����ǰ����
        OSTCBCurPtr->SemCtr--;                   //�ź�����������1
        ctr    = OSTCBCurPtr->SemCtr;            //��ȡ�ź����ĵ�ǰ����ֵ
        if (p_ts != (CPU_TS *)0) {               //��� p_ts �ǿ�
           *p_ts  = OSTCBCurPtr->TS;             //�����ź�����������ʱ���
        }
#if OS_CFG_TASK_PROFILE_EN > 0u                  //���ʹ����������ƿ�ļ������
        OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;     //��������ȴ�
        if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) { //�����ź�����
            OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;   //�ʱ���¼��
        }
#endif
        CPU_CRITICAL_EXIT();                     //���ж�            
       *p_err = OS_ERR_NONE;                     //��������Ϊ���޴���
        return (ctr);                            //�����ź����ĵ�ǰ����ֵ
    }
    /* ��������ź�����ǰ������ */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //���ѡ���˲���������
        CPU_CRITICAL_EXIT();                              //���ж�
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                  //��������Ϊ��ȱ��������
        return ((OS_SEM_CTR)0);                           //����0���д��󣩣�ִֹͣ��
    } else {                                              //���ѡ������������
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {  //�������������
            CPU_CRITICAL_EXIT();                          //���ж�
           *p_err = OS_ERR_SCHED_LOCKED;                  //��������Ϊ��������������
            return ((OS_SEM_CTR)0);                       //����0���д��󣩣�ִֹͣ��
        }
    }
    /* ���������δ���� */
    OS_CRITICAL_ENTER_CPU_EXIT();                         //�����������ؿ��ж�                      
    OS_Pend((OS_PEND_DATA *)0,                            //�������񣬵ȴ��ź�����
            (OS_PEND_OBJ  *)0,                            //�������ȴ��б�
            (OS_STATE      )OS_TASK_PEND_ON_TASK_SEM,
            (OS_TICK       )timeout);
    OS_CRITICAL_EXIT_NO_SCHED();                          //�����������޵��ȣ�

    OSSched();                                            //��������
    /* �������ź�������Լ������� */
    CPU_CRITICAL_ENTER();                                 //���ж�
    switch (OSTCBCurPtr->PendStatus) {                    //��������ĵȴ�״̬���ദ��
        case OS_STATUS_PEND_OK:                           //�������ɹ�����ź���
             if (p_ts != (CPU_TS *)0) {                   //�����ź�����������ʱ���
                *p_ts                    =  OSTCBCurPtr->TS;
#if OS_CFG_TASK_PROFILE_EN > 0u                           //������ȴ�ʱ���¼
                OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
                if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) {
                    OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;
                }
#endif
             }
            *p_err = OS_ERR_NONE;                         //��������Ϊ���޴���
             break;                                       //����

        case OS_STATUS_PEND_ABORT:                        //����ȴ�����ֹ
             if (p_ts != (CPU_TS *)0) {                   //���ر���ֹʱ��ʱ���
                *p_ts  =  OSTCBCurPtr->TS;
             }
            *p_err = OS_ERR_PEND_ABORT;                   //��������Ϊ���ȴ�����ֹ��
             break;                                       //����

        case OS_STATUS_PEND_TIMEOUT:                      //����ȴ���ʱ
             if (p_ts != (CPU_TS *)0) {                   //����ʱ���Ϊ0
                *p_ts  = (CPU_TS  )0;
             }
            *p_err = OS_ERR_TIMEOUT;                      //��������Ϊ���ȴ���ʱ��
             break;                                       //����

        default:                                          //����ȴ�״̬����Ԥ��
            *p_err = OS_ERR_STATUS_INVALID;               //��������Ϊ��״̬�Ƿ���
             break;                                       //����
    }                                                     
    ctr = OSTCBCurPtr->SemCtr;                            //��ȡ�ź����ĵ�ǰ����ֵ
    CPU_CRITICAL_EXIT();                                  //���ж�
    return (ctr);                                         //�����ź����ĵ�ǰ����ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               ABORT WAITING FOR A SIGNAL
*
* Description: This function aborts & readies the task specified.  This function should be used to fault-abort the wait
*              for a signal, rather than to normally post the signal to the task via OSTaskSemPost().
*
* Arguments  : p_tcb     is a pointer to the task to pend abort
*
*              opt       provides options for this function:
*
*                            OS_OPT_POST_NONE         No option selected
*                            OS_OPT_POST_NO_SCHED     Indicates that the scheduler will not be called.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE              If the task was readied and informed of the aborted wait
*                            OS_ERR_PEND_ABORT_ISR    If you tried calling this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE   If the task was not waiting for a signal
*                            OS_ERR_PEND_ABORT_SELF   If you attempted to pend abort the calling task.  This is not
*                                                     possible since the calling task cannot be pending because it's
*                                                     running.
*
* Returns    : == DEF_FALSE   if task was not waiting for a message, or upon error.
*              == DEF_TRUE    if task was waiting for a message and was readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SEM_PEND_ABORT_EN > 0u  //���ʹ���� OSTaskSemPendAbort()
CPU_BOOLEAN  OSTaskSemPendAbort (OS_TCB  *p_tcb, //Ŀ������
                                 OS_OPT   opt,   //ѡ��
                                 OS_ERR  *p_err) //���ش�������
{
    CPU_TS         ts;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return (DEF_FALSE);             //���أ�ʧ�ܣ���ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {   //����ú��������ж��б�����
       *p_err = OS_ERR_PEND_ABORT_ISR;           //��������Ϊ�����ж��д�������
        return (DEF_FALSE);                      //���أ�ʧ�ܣ���ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //���ʹ���˲������
    switch (opt) {                          //����ѡ����ദ��
        case OS_OPT_POST_NONE:              //���ѡ����Ԥ����
        case OS_OPT_POST_NO_SCHED:
             break;                         //ֱ������

        default:                            //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;   //��������Ϊ��ѡ��Ƿ���
             return (DEF_FALSE);            //���أ�ʧ�ܣ���ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                 //���ж�
    if ((p_tcb == (OS_TCB *)0) ||         //��� p_tcb Ϊ�գ�����
        (p_tcb == OSTCBCurPtr)) {         //p_tcb ָ��ǰ��������
        CPU_CRITICAL_EXIT();              //���ж�
       *p_err = OS_ERR_PEND_ABORT_SELF;   //��������Ϊ����ֹ����
        return (DEF_FALSE);               //���أ�ʧ�ܣ���ִֹͣ��
    }
    /* ��� p_tcb ��Ŀ������ ���ǵ�ǰ������������ */
    if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_SEM) { //���Ŀ������û�ڵȴ������ź���
        CPU_CRITICAL_EXIT();                         //���ж�
       *p_err = OS_ERR_PEND_ABORT_NONE;              //��������Ϊ��û�ڵȴ������ź�����
        return (DEF_FALSE);                          //���أ�ʧ�ܣ���ִֹͣ��
    }
    CPU_CRITICAL_EXIT();                             //���ж�

    OS_CRITICAL_ENTER();                             //�����ٽ��
    ts = OS_TS_GET();                                //��ȡʱ���
    OS_PendAbort((OS_PEND_OBJ *)0,                   //��ֹĿ��������ź����ĵȴ�
                 p_tcb, 
                 ts);
    OS_CRITICAL_EXIT_NO_SCHED();                     //�˳��ٽ�Σ��޵��ȣ�
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) { //���ѡ�����������
        OSSched();                                   //��������
    }
   *p_err = OS_ERR_NONE;                             //��������Ϊ���޴���
    return (DEF_TRUE);                               //���أ���ֹ�ɹ���
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    SIGNAL A TASK
*
* Description: This function is called to signal a task waiting for a signal.
*
* Arguments  : p_tcb     is the pointer to the TCB of the task to signal.  A NULL pointer indicates that you are sending
*                        a signal to yourself.
*
*              opt       determines the type of POST performed:
*
*                             OS_OPT_POST_NONE         No option
*                             OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              p_err     is a pointer to an error code returned by this function:
*
*                            OS_ERR_NONE              If the requested task is signaled
*                            OS_ERR_SEM_OVF           If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the task's signal counter or 0 if called from an ISR
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemPost (OS_TCB  *p_tcb,   //Ŀ������
                           OS_OPT   opt,     //ѡ��
                           OS_ERR  *p_err)   //���ش�������
{
    OS_SEM_CTR  ctr;
    CPU_TS      ts;



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_SEM_CTR)0);         //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲�����⹦��
    switch (opt) {                          //����ѡ����ദ��
        case OS_OPT_POST_NONE:              //���ѡ����Ԥ��֮��
        case OS_OPT_POST_NO_SCHED:
             break;                         //����

        default:                            //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;   //��������Ϊ��ѡ��Ƿ���
             return ((OS_SEM_CTR)0u);       //����0���д��󣩣�ִֹͣ��
    }
#endif

    ts = OS_TS_GET();                                      //��ȡʱ���

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                       //���ʹ�����ж��ӳٷ���
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {             //����ú��������ж��б�����
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_SIGNAL,  //�����ź����������ж���Ϣ����
                    (void      *)p_tcb,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )0,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return ((OS_SEM_CTR)0);                           //����0����δ������   
    }
#endif

    ctr = OS_TaskSemPost(p_tcb,                          //���ź���������ͨ��ʽ����
                         opt,
                         ts,
                         p_err);

    return (ctr);                                       //�����źŵĵ�ǰ����ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                            SET THE SIGNAL COUNTER OF A TASK
*
* Description: This function is called to clear the signal counter
*
* Arguments  : p_tcb      is the pointer to the TCB of the task to clear the counter.  If you specify a NULL pointer
*                         then the signal counter of the current task will be cleared.
*
*              cnt        is the desired value of the semaphore counter
*
*              p_err      is a pointer to an error code returned by this function
*
*                             OS_ERR_NONE        if the signal counter of the requested task is cleared
*                             OS_ERR_SET_ISR     if the function was called from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemSet (OS_TCB      *p_tcb,
                          OS_SEM_CTR   cnt,
                          OS_ERR      *p_err)
{
    OS_SEM_CTR  ctr;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* Not allowed to call from an ISR                        */
       *p_err = OS_ERR_SET_ISR;
        return ((OS_SEM_CTR)0);
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {
        p_tcb = OSTCBCurPtr;
    }
    ctr           = p_tcb->SemCtr;
    p_tcb->SemCtr = (OS_SEM_CTR)cnt;
    CPU_CRITICAL_EXIT();
   *p_err         =  OS_ERR_NONE;
    return (ctr);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    STACK CHECKING
*
* Description: This function is called to calculate the amount of free memory left on the specified task's stack.
*
* Arguments  : p_tcb       is a pointer to the TCB of the task to check.  If you specify a NULL pointer then
*                          you are specifying that you want to check the stack of the current task.
*
*              p_free      is a pointer to a variable that will receive the number of free 'entries' on the task's stack.
*
*              p_used      is a pointer to a variable that will receive the number of used 'entries' on the task's stack.
*
*              p_err       is a pointer to a variable that will contain an error code.
*
*                              OS_ERR_NONE               upon success
*                              OS_ERR_PTR_INVALID        if either 'p_free' or 'p_used' are NULL pointers
*                              OS_ERR_TASK_NOT_EXIST     if the stack pointer of the task is a NULL pointer
*                              OS_ERR_TASK_OPT           if you did NOT specified OS_OPT_TASK_STK_CHK when the task
*                                                        was created
*                              OS_ERR_TASK_STK_CHK_ISR   you called this function from an ISR
************************************************************************************************************************
*/

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
void  OSTaskStkChk (OS_TCB        *p_tcb,
                    CPU_STK_SIZE  *p_free,
                    CPU_STK_SIZE  *p_used,
                    OS_ERR        *p_err)
{
    CPU_STK_SIZE  free_stk;
    CPU_STK      *p_stk;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if trying to check stack from ISR                  */
       *p_err = OS_ERR_TASK_STK_CHK_ISR;
        return;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_free == (CPU_STK_SIZE*)0) {                       /* User must specify valid destinations for the sizes     */
       *p_err  = OS_ERR_PTR_INVALID;
        return;
    }

    if (p_used == (CPU_STK_SIZE*)0) {
       *p_err  = OS_ERR_PTR_INVALID;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {                             /* Check the stack of the current task?                   */
        p_tcb = OSTCBCurPtr;                                /* Yes                                                    */
    }

    if (p_tcb->StkPtr == (CPU_STK*)0) {                     /* Make sure task exist                                   */
        CPU_CRITICAL_EXIT();
       *p_free = (CPU_STK_SIZE)0;
       *p_used = (CPU_STK_SIZE)0;
       *p_err  =  OS_ERR_TASK_NOT_EXIST;
        return;
    }

    if ((p_tcb->Opt & OS_OPT_TASK_STK_CHK) == (OS_OPT)0) {  /* Make sure stack checking option is set                 */
        CPU_CRITICAL_EXIT();
       *p_free = (CPU_STK_SIZE)0;
       *p_used = (CPU_STK_SIZE)0;
       *p_err  =  OS_ERR_TASK_OPT;
        return;
    }
    CPU_CRITICAL_EXIT();

    free_stk  = 0u;
#if CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO
    p_stk = p_tcb->StkBasePtr;                              /* Start at the lowest memory and go up                   */
    while (*p_stk == (CPU_STK)0) {                          /* Compute the number of zero entries on the stk          */
        p_stk++;
        free_stk++;
    }
#else
    p_stk = p_tcb->StkBasePtr + p_tcb->StkSize - 1u;        /* Start at the highest memory and go down                */
    while (*p_stk == (CPU_STK)0) {
        free_stk++;
        p_stk--;
    }
#endif
   *p_free = free_stk;
   *p_used = (p_tcb->StkSize - free_stk);                   /* Compute number of entries used on the stack            */
   *p_err  = OS_ERR_NONE;
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if 'p_tcb' is a NULL pointer
*              or the pointer to the TCB of the calling task.
*
* Arguments  : p_tcb    is a pointer to the TCB to suspend.
*                       If p_tcb is a NULL pointer then, suspend the current task.
*
*              p_err    is a pointer to a variable that will receive an error code from this function.
*
*                           OS_ERR_NONE                      if the requested task is suspended
*                           OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                            locked
*                           OS_ERR_TASK_SUSPEND_ISR          if you called this function from an ISR
*                           OS_ERR_TASK_SUSPEND_IDLE         if you attempted to suspend the idle task which is not
*                                                            allowed.
*                           OS_ERR_TASK_SUSPEND_INT_HANDLER  if you attempted to suspend the idle task which is not
*                                                            allowed.
*
* Note(s)    : 1) You should use this function with great care.  If you suspend a task that is waiting for an event
*                 (i.e. a message, a semaphore, a queue ...) you will prevent this task from running when the event
*                 arrives.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u                         //���ʹ���˺��� OSTaskSuspend()     
void   OSTaskSuspend (OS_TCB  *p_tcb,                   //������ƿ�ָ��
                      OS_ERR  *p_err)                   //���ش�������
{
#ifdef OS_SAFETY_CRITICAL                               //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {                         //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();                 //ִ�а�ȫ����쳣����
        return;                                         //���أ�ִֹͣ��
    }
#endif

#if (OS_CFG_ISR_POST_DEFERRED_EN   == 0u) && \
    (OS_CFG_CALLED_FROM_ISR_CHK_EN >  0u)               //����������ж��ӳٷ������ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {          //������ж��е��øú���
       *p_err = OS_ERR_TASK_SUSPEND_ISR;                //��������Ϊ�����ж��й�������
        return;                                         //���أ�ִֹͣ��
    }
#endif

    if (p_tcb == &OSIdleTaskTCB) {                      //��� p_tcb �ǿ�������   
       *p_err = OS_ERR_TASK_SUSPEND_IDLE;               //��������Ϊ�������������
        return;                                         //���أ�ִֹͣ��
    }
 
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                    //���ʹ�����ж��ӳٷ���
    if (p_tcb == &OSIntQTaskTCB) {                      //��� p_tcb Ϊ�ж��ӳ��ύ����
       *p_err = OS_ERR_TASK_SUSPEND_INT_HANDLER;        //��������Ϊ�������ж��ӳ��ύ����
        return;                                         //���أ�ִֹͣ��
    }

    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              //������ж��е��øú���
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_SUSPEND,  //�ѹ���������������ж���Ϣ����
                    (void      *)p_tcb,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )0,
                    (CPU_TS     )0,
                    (OS_ERR    *)p_err);
        return;                                            //���أ�ִֹͣ��
    }
#endif
    /* ����������ж��ӳٷ����������ж��е��øú��� */
    OS_TaskSuspend(p_tcb, p_err);                          //ֱ�ӽ����� p_tcb ����
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                CHANGE A TASK'S TIME SLICE
*
* Description: This function is called to change the value of the task's specific time slice.
*
* Arguments  : p_tcb        is the pointer to the TCB of the task to change. If you specify an NULL pointer, the current
*                           task is assumed.
*
*              time_quanta  is the number of ticks before the CPU is taken away when round-robin scheduling is enabled.
*
*              p_err        is a pointer to an error code returned by this function:
*
*                               OS_ERR_NONE       upon success
*                               OS_ERR_SET_ISR    if you called this function from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u                //���ʹ����ʱ��Ƭ��ת����
void  OSTaskTimeQuantaSet (OS_TCB   *p_tcb,         //������ƿ�ָ��
                           OS_TICK   time_quanta,   //����ʱ��Ƭ
                           OS_ERR   *p_err)         //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //��� p_err Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_SET_ISR;                //��������Ϊ�����ж������á�
        return;                                //���أ�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                      //���ж�
    if (p_tcb == (OS_TCB *)0) {                //��� p_tcb Ϊ��
        p_tcb = OSTCBCurPtr;                   //Ŀ������Ϊ��ǰ��������
    }

    if (time_quanta == 0u) {                   //��� time_quanta Ϊ0
        p_tcb->TimeQuanta    = OSSchedRoundRobinDfltTimeQuanta; //����ΪĬ��ʱ��Ƭ
    } else {                                   //��� time_quanta ��0
        p_tcb->TimeQuanta    = time_quanta;    //�� time_quanta ��Ϊ�����ʱ��Ƭ
    }
    if (p_tcb->TimeQuanta > p_tcb->TimeQuantaCtr) { //��������ʣ��ʱ��ƬС��������ֵ
        p_tcb->TimeQuantaCtr = p_tcb->TimeQuanta;   //��ʣ��ʱ��Ƭ����Ϊ������ֵ
    }
    CPU_CRITICAL_EXIT();                       //���ж�
   *p_err = OS_ERR_NONE;                       //��������Ϊ���޴���
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                            ADD/REMOVE TASK TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove an OS_TCB from the debug list.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u                                    //���ʹ���˵��Դ���ͱ���
void  OS_TaskDbgListAdd (OS_TCB  *p_tcb)                  //��������뵽��������б����ǰ��
{
    p_tcb->DbgPrevPtr                = (OS_TCB *)0;       //����������Ϊ�б����ǰ��
    if (OSTaskDbgListPtr == (OS_TCB *)0) {                //����б�Ϊ��
        p_tcb->DbgNextPtr            = (OS_TCB *)0;       //�ö��е���һ��Ԫ��ҲΪ��
    } else {                                              //����б�ǿ�
        p_tcb->DbgNextPtr            =  OSTaskDbgListPtr; //�б�ԭ������Ԫ����Ϊ���������һ��Ԫ��
        OSTaskDbgListPtr->DbgPrevPtr =  p_tcb;            //ԭ������Ԫ�ص�ǰ���Ϊ�˸�����
    }
    OSTaskDbgListPtr                 =  p_tcb;            //�������Ϊ�б������Ԫ��
}



void  OS_TaskDbgListRemove (OS_TCB  *p_tcb)
{
    OS_TCB  *p_tcb_next;
    OS_TCB  *p_tcb_prev;


    p_tcb_prev = p_tcb->DbgPrevPtr;
    p_tcb_next = p_tcb->DbgNextPtr;

    if (p_tcb_prev == (OS_TCB *)0) {
        OSTaskDbgListPtr = p_tcb_next;
        if (p_tcb_next != (OS_TCB *)0) {
            p_tcb_next->DbgPrevPtr = (OS_TCB *)0;
        }
        p_tcb->DbgNextPtr = (OS_TCB *)0;

    } else if (p_tcb_next == (OS_TCB *)0) {
        p_tcb_prev->DbgNextPtr = (OS_TCB *)0;
        p_tcb->DbgPrevPtr      = (OS_TCB *)0;

    } else {
        p_tcb_prev->DbgNextPtr =  p_tcb_next;
        p_tcb_next->DbgPrevPtr =  p_tcb_prev;
        p_tcb->DbgNextPtr      = (OS_TCB *)0;
        p_tcb->DbgPrevPtr      = (OS_TCB *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                             TASK MANAGER INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the task management.
*

* Argument(s): p_err        is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSTaskDbgListPtr = (OS_TCB      *)0;
#endif

    OSTaskQty        = (OS_OBJ_QTY   )0;                    /* Clear the number of tasks                              */
    OSTaskCtxSwCtr   = (OS_CTX_SW_CTR)0;                    /* Clear the context switch counter                       */

   *p_err            = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INITIALIZE TCB FIELDS
*
* Description: This function is called to initialize a TCB to default values
*
* Arguments  : p_tcb    is a pointer to the TCB to initialize
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskInitTCB (OS_TCB  *p_tcb)
{
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG_ID   reg_id;
#endif
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_ID   id;
#endif
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS      ts;
#endif


    p_tcb->StkPtr             = (CPU_STK       *)0;
    p_tcb->StkLimitPtr        = (CPU_STK       *)0;

    p_tcb->ExtPtr             = (void          *)0;

    p_tcb->NextPtr            = (OS_TCB        *)0;
    p_tcb->PrevPtr            = (OS_TCB        *)0;

    p_tcb->TickNextPtr        = (OS_TCB        *)0;
    p_tcb->TickPrevPtr        = (OS_TCB        *)0;
    p_tcb->TickSpokePtr       = (OS_TICK_SPOKE *)0;

    p_tcb->NamePtr            = (CPU_CHAR      *)((void *)"?Task");

    p_tcb->StkBasePtr         = (CPU_STK       *)0;

    p_tcb->TaskEntryAddr      = (OS_TASK_PTR    )0;
    p_tcb->TaskEntryArg       = (void          *)0;

#if (OS_CFG_PEND_MULTI_EN > 0u)
    p_tcb->PendDataTblPtr     = (OS_PEND_DATA  *)0;
    p_tcb->PendDataTblEntries = (OS_OBJ_QTY     )0u;
#endif

    p_tcb->TS                 = (CPU_TS         )0u;

#if (OS_MSG_EN > 0u)
    p_tcb->MsgPtr             = (void          *)0;
    p_tcb->MsgSize            = (OS_MSG_SIZE    )0u;
#endif

#if OS_CFG_TASK_Q_EN > 0u
    OS_MsgQInit(&p_tcb->MsgQ,
                (OS_MSG_QTY)0u);
#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->MsgQPendTime       = (CPU_TS         )0u;
    p_tcb->MsgQPendTimeMax    = (CPU_TS         )0u;
#endif
#endif

#if OS_CFG_FLAG_EN > 0u
    p_tcb->FlagsPend          = (OS_FLAGS       )0u;
    p_tcb->FlagsOpt           = (OS_OPT         )0u;
    p_tcb->FlagsRdy           = (OS_FLAGS       )0u;
#endif

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    for (reg_id = 0u; reg_id < OS_CFG_TASK_REG_TBL_SIZE; reg_id++) {
        p_tcb->RegTbl[reg_id] = (OS_REG)0u;
    }
#endif

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
        p_tcb->TLS_Tbl[id]    = (OS_TLS)0;
    }
#endif

    p_tcb->SemCtr             = (OS_SEM_CTR     )0u;
#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->SemPendTime        = (CPU_TS         )0u;
    p_tcb->SemPendTimeMax     = (CPU_TS         )0u;
#endif

    p_tcb->StkSize            = (CPU_STK_SIZE   )0u;


#if OS_CFG_TASK_SUSPEND_EN > 0u
    p_tcb->SuspendCtr         = (OS_NESTING_CTR )0u;
#endif

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
    p_tcb->StkFree            = (CPU_STK_SIZE   )0u;
    p_tcb->StkUsed            = (CPU_STK_SIZE   )0u;
#endif

    p_tcb->Opt                = (OS_OPT         )0u;

    p_tcb->TickCtrPrev        = (OS_TICK        )OS_TICK_TH_INIT;
    p_tcb->TickCtrMatch       = (OS_TICK        )0u;
    p_tcb->TickRemain         = (OS_TICK        )0u;

    p_tcb->TimeQuanta         = (OS_TICK        )0u;
    p_tcb->TimeQuantaCtr      = (OS_TICK        )0u;

#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->CPUUsage           = (OS_CPU_USAGE   )0u;
    p_tcb->CPUUsageMax        = (OS_CPU_USAGE   )0u;
    p_tcb->CtxSwCtr           = (OS_CTX_SW_CTR  )0u;
    p_tcb->CyclesDelta        = (CPU_TS         )0u;
    ts                        = OS_TS_GET();                /* Read the current timestamp and save                    */
    p_tcb->CyclesStart        = ts;
    p_tcb->CyclesTotal        = (OS_CYCLES      )0u;
#endif
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    p_tcb->IntDisTimeMax      = (CPU_TS         )0u;
#endif
#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
    p_tcb->SchedLockTimeMax   = (CPU_TS         )0u;
#endif

    p_tcb->PendOn             = (OS_STATE       )OS_TASK_PEND_ON_NOTHING;
    p_tcb->PendStatus         = (OS_STATUS      )OS_STATUS_PEND_OK;
    p_tcb->TaskState          = (OS_STATE       )OS_TASK_STATE_RDY;

    p_tcb->Prio               = (OS_PRIO        )OS_PRIO_INIT;

#if OS_CFG_DBG_EN > 0u
    p_tcb->DbgPrevPtr         = (OS_TCB        *)0;
    p_tcb->DbgNextPtr         = (OS_TCB        *)0;
    p_tcb->DbgNamePtr         = (CPU_CHAR      *)((void *)" ");
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A TASK
*
* Description: This function sends a message to a task
*
* Arguments  : p_tcb      is a pointer to the TCB of the task receiving a message.  If you specify a NULL pointer then
*                         the message will be posted to the task's queue of the calling task.  In other words, you'd be
*                         posting a message to yourself.
*
*              p_void     is a pointer to the message to send.
*
*              msg_size   is the size of the message sent (in #bytes)
*
*              opt        specifies whether the post will be FIFO or LIFO:
*
*                             OS_OPT_POST_FIFO       Post at the end   of the queue
*                             OS_OPT_POST_LIFO       Post at the front of the queue
*
*                             OS_OPT_POST_NO_SCHED   Do not run the scheduler after the post
*
*                          Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*
*              ts         is a timestamp indicating when the post occurred.
*
*              p_err      is a pointer to a variable that will hold the error code associated
*                         with the outcome of this call.  Errors can be:
*
*                             OS_ERR_NONE            The call was successful and the message was sent
*                             OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs available from the pool
*                             OS_ERR_Q_MAX           If the queue is full
*                             OS_ERR_STATE_INVALID   If the task is in an invalid state.  This should never happen
*                                                    and if it does, would be considered a system failure.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u                   //���ʹ����������Ϣ����
void  OS_TaskQPost (OS_TCB       *p_tcb,    //Ŀ������
                    void         *p_void,   //��Ϣ���ݵ�ַ
                    OS_MSG_SIZE   msg_size, //��Ϣ����
                    OS_OPT        opt,      //ѡ��
                    CPU_TS        ts,       //ʱ���
                    OS_ERR       *p_err)    //���ش�������
{
    CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
									   //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
									   // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

    OS_CRITICAL_ENTER();                                   //�����ٽ��
    if (p_tcb == (OS_TCB *)0) {                            //��� p_tcb Ϊ��
        p_tcb = OSTCBCurPtr;                               //Ŀ������Ϊ����
    }
   *p_err  = OS_ERR_NONE;                                  //��������Ϊ���޴���
    switch (p_tcb->TaskState) {                            //��������״̬���ദ��
        case OS_TASK_STATE_RDY:                            //���Ŀ������û�ȴ�״̬
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_SUSPENDED:
        case OS_TASK_STATE_DLY_SUSPENDED:
             OS_MsgQPut(&p_tcb->MsgQ,                      //����Ϣ����������Ϣ����
                        p_void,
                        msg_size,
                        opt,
                        ts,
                        p_err);
             OS_CRITICAL_EXIT();                           //�˳��ٽ��
             break;                                        //����

        case OS_TASK_STATE_PEND:                           //���Ŀ�������еȴ�״̬ 
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             if (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_Q) {//����ȵ���������Ϣ����
                 OS_Post((OS_PEND_OBJ *)0,                 //����Ϣ������Ŀ������
                         p_tcb,
                         p_void,
                         msg_size,
                         ts);
                 OS_CRITICAL_EXIT_NO_SCHED();              //�˳��ٽ�Σ��޵��ȣ�
                 if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) { //���Ҫ��������
                     OSSched();                                    //��������
                 }
             } else {                                      //���û�ڵȴ�������Ϣ����
                 OS_MsgQPut(&p_tcb->MsgQ,                  //����Ϣ����������Ϣ����
                            p_void,                        
                            msg_size,
                            opt,
                            ts,
                            p_err);
                 OS_CRITICAL_EXIT();                      //�˳��ٽ��
             }
             break;                                       //����

        default:                                          //���״̬����Ԥ��
             OS_CRITICAL_EXIT();                          //�˳��ٽ��
            *p_err = OS_ERR_STATE_INVALID;                //��������Ϊ��״̬�Ƿ���
             break;                                       //����
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that will remove an
*              explicit task suspension.
*
* Arguments  : p_tcb      Is a pointer to the task's OS_TCB to resume
*
*              p_err      Is a pointer to a variable that will contain an error code returned by this function
*
*                             OS_ERR_NONE                  if the requested task is resumed
*                             OS_ERR_STATE_INVALID         if the task is in an invalid state
*                             OS_ERR_TASK_RESUME_ISR       if you called this function from an ISR
*                             OS_ERR_TASK_RESUME_SELF      You cannot resume 'self'
*                             OS_ERR_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u           //���ʹ���˺��� OSTaskResume()
void  OS_TaskResume (OS_TCB  *p_tcb,      //������ƿ�ָ��
                     OS_ERR  *p_err)      //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��
    CPU_CRITICAL_ENTER();                                      //���ж�
   *p_err  = OS_ERR_NONE;                                      //��������Ϊ���޴���
    switch (p_tcb->TaskState) {                                //���� p_tcb ������״̬���ദ��
        case OS_TASK_STATE_RDY:                                //���״̬��û�й���״̬
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_PEND:
        case OS_TASK_STATE_PEND_TIMEOUT:
             CPU_CRITICAL_EXIT();                              //���ж�
            *p_err = OS_ERR_TASK_NOT_SUSPENDED;                //��������Ϊ������δ������
             break;                                            //����

        case OS_TASK_STATE_SUSPENDED:                          //����ǡ�����״̬��
             OS_CRITICAL_ENTER_CPU_EXIT();                     //�����������ؿ��ж�
             p_tcb->SuspendCtr--;                              //����Ĺ���Ƕ������1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //�������ǰ����Ϊ0
                 p_tcb->TaskState = OS_TASK_STATE_RDY;         //�޸�״̬Ϊ������״̬��
                 OS_TaskRdy(p_tcb);                            //�� p_tcb ��������б�
             }
             OS_CRITICAL_EXIT_NO_SCHED();                      //��������������������
             break;                                            //����

        case OS_TASK_STATE_DLY_SUSPENDED:                      //����ǡ���ʱ�б�����
             p_tcb->SuspendCtr--;                              //����Ĺ���Ƕ������1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //�������ǰ����Ϊ0
                 p_tcb->TaskState = OS_TASK_STATE_DLY;         //�޸�״̬Ϊ����ʱ״̬��
             }
             CPU_CRITICAL_EXIT();                              //���ж�
             break;                                            //����

        case OS_TASK_STATE_PEND_SUSPENDED:                     //����ǡ������޵ȴ��б�����
             p_tcb->SuspendCtr--;                              //����Ĺ���Ƕ������1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //�������ǰ����Ϊ0
                 p_tcb->TaskState = OS_TASK_STATE_PEND;        //�޸�״̬Ϊ�������޵ȴ�״̬��
             }
             CPU_CRITICAL_EXIT();                              //���ж�
             break;                                            //����

        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:             //����ǡ������޵ȴ��б�����
             p_tcb->SuspendCtr--;                              //����Ĺ���Ƕ������1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //�������ǰ����Ϊ0
                 p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;//�޸�״̬Ϊ�������޵ȴ�״̬��
             }
             CPU_CRITICAL_EXIT();                              //���ж�
             break;                                            //����

        default:                                               //��� p_tcb ����״̬����Ԥ��
             CPU_CRITICAL_EXIT();                              //���ж�
            *p_err = OS_ERR_STATE_INVALID;                     //��������Ϊ��״̬�Ƿ���
             return;                                           //����
    }

    OSSched();                                                 //��������
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              CATCH ACCIDENTAL TASK RETURN
*
* Description: This function is called if a task accidentally returns without deleting itself.  In other words, a task
*              should either be an infinite loop or delete itself if it's done.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskReturn (void)
{
    OS_ERR  err;



    OSTaskReturnHook(OSTCBCurPtr);                          /* Call hook to let user decide on what to do             */
#if OS_CFG_TASK_DEL_EN > 0u
    OSTaskDel((OS_TCB *)0,                                  /* Delete task if it accidentally returns!                */
              (OS_ERR *)&err);
#else
    for (;;) {
        OSTimeDly((OS_TICK )OSCfg_TickRate_Hz,
                  (OS_OPT  )OS_OPT_TIME_DLY,
                  (OS_ERR *)&err);
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    SIGNAL A TASK
*
* Description: This function is called to signal a task waiting for a signal.
*
* Arguments  : p_tcb     is the pointer to the TCB of the task to signal.  A NULL pointer indicates that you are sending
*                        a signal to yourself.
*
*              opt       determines the type of POST performed:
*
*                             OS_OPT_POST_NONE         No option
*
*                             OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              ts        is a timestamp indicating when the post occurred.
*
*              p_err     is a pointer to an error code returned by this function:
*
*                            OS_ERR_NONE           If the requested task is signaled
*                            OS_ERR_SEM_OVF        If the post would cause the semaphore count to overflow.
*                            OS_ERR_STATE_INVALID  If the task is in an invalid state.  This should never happen
*                                                  and if it does, would be considered a system failure.
*
* Returns    : The current value of the task's signal counter or 0 if called from an ISR
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

OS_SEM_CTR  OS_TaskSemPost (OS_TCB  *p_tcb,   //Ŀ������
                            OS_OPT   opt,     //ѡ��
                            CPU_TS   ts,      //ʱ���
                            OS_ERR  *p_err)   //���ش�������
{
    OS_SEM_CTR  ctr;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

    OS_CRITICAL_ENTER();                               //�����ٽ��
    if (p_tcb == (OS_TCB *)0) {                        //��� p_tcb Ϊ��
        p_tcb = OSTCBCurPtr;                           //�������ź��������Լ�������
    }
    p_tcb->TS = ts;                                    //��¼�ź�����������ʱ���
   *p_err     = OS_ERR_NONE;                           //��������Ϊ���޴���
    switch (p_tcb->TaskState) {                        //����Ŀ�����������״̬���ദ��
        case OS_TASK_STATE_RDY:                        //���Ŀ������û�еȴ�״̬
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_SUSPENDED:
        case OS_TASK_STATE_DLY_SUSPENDED:
             switch (sizeof(OS_SEM_CTR)) {                        //�ж��Ƿ񽫵��¸���
                 case 1u:                                         //��������ֵ�������
                      if (p_tcb->SemCtr == DEF_INT_08U_MAX_VAL) { //����������жϣ�
                          OS_CRITICAL_EXIT();                     //���ش�������Ϊ����
                         *p_err = OS_ERR_SEM_OVF;                 //��ֵ�����������0
                          return ((OS_SEM_CTR)0);                 //���д��󣩣�������
                      }                                           //ִ�С�
                      break;                                      

                 case 2u:
                      if (p_tcb->SemCtr == DEF_INT_16U_MAX_VAL) {
                          OS_CRITICAL_EXIT();
                         *p_err = OS_ERR_SEM_OVF;
                          return ((OS_SEM_CTR)0);
                      }
                      break;

                 case 4u:
                      if (p_tcb->SemCtr == DEF_INT_32U_MAX_VAL) {
                          OS_CRITICAL_EXIT();
                         *p_err = OS_ERR_SEM_OVF;
                          return ((OS_SEM_CTR)0);
                      }
                      break;

                 default:
                      break;
             }
             p_tcb->SemCtr++;                              //�ź�������ֵ��������1
             ctr = p_tcb->SemCtr;                          //��ȡ�ź����ĵ�ǰ����ֵ
             OS_CRITICAL_EXIT();                           //�˳��ٽ��
             break;                                        //����

        case OS_TASK_STATE_PEND:                           //��������еȴ�״̬
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             if (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_SEM) { //������ȴ������ź���
                 OS_Post((OS_PEND_OBJ *)0,                    //�����ź�����Ŀ������
                         (OS_TCB      *)p_tcb,
                         (void        *)0,
                         (OS_MSG_SIZE  )0u,
                         (CPU_TS       )ts);
                 ctr = p_tcb->SemCtr;                         //��ȡ�ź����ĵ�ǰ����ֵ
                 OS_CRITICAL_EXIT_NO_SCHED();                 //�˳��ٽ�Σ��޵��ȣ�
                 if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) { //���ѡ���˵�������
                     OSSched();                               //��������
                 }
             } else {                                         //���û�ȴ������ź���
                 switch (sizeof(OS_SEM_CTR)) {                         //�ж��Ƿ񽫵���
                     case 1u:                                          //���ź�������ֵ
                          if (p_tcb->SemCtr == DEF_INT_08U_MAX_VAL) {  //�������������
                              OS_CRITICAL_EXIT();                      //���жϣ�����
                             *p_err = OS_ERR_SEM_OVF;                  //��������Ϊ����
                              return ((OS_SEM_CTR)0);                  //��ֵ���������
                          }                                            //��0���д��󣩣�
                          break;                                       //������ִ�С�

                     case 2u:
                          if (p_tcb->SemCtr == DEF_INT_16U_MAX_VAL) {
                              OS_CRITICAL_EXIT();
                             *p_err = OS_ERR_SEM_OVF;
                              return ((OS_SEM_CTR)0);
                          }
                          break;

                     case 4u:
                          if (p_tcb->SemCtr == DEF_INT_32U_MAX_VAL) {
                              OS_CRITICAL_EXIT();
                             *p_err = OS_ERR_SEM_OVF;
                              return ((OS_SEM_CTR)0);
                          }
                          break;

                     default:
                          break;
                 }
                 p_tcb->SemCtr++;                            //�ź�������ֵ��������1
                 ctr = p_tcb->SemCtr;                        //��ȡ�ź����ĵ�ǰ����ֵ
                 OS_CRITICAL_EXIT();                         //�˳��ٽ��
             }
             break;                                          //����

        default:                                             //�������״̬����Ԥ��
             OS_CRITICAL_EXIT();                             //�˳��ٽ��
            *p_err = OS_ERR_STATE_INVALID;                   //��������Ϊ��״̬�Ƿ���
             ctr   = (OS_SEM_CTR)0;                          //���� ctr
             break;                                          //����
    }
    return (ctr);                                            //�����ź����ĵ�ǰ����ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if 'p_tcb' is a NULL pointer
*              or the pointer to the TCB of the calling task.
*
* Arguments  : p_tcb    is a pointer to the TCB to suspend.
*                       If p_tcb is a NULL pointer then, suspend the current task.
*
*              p_err    is a pointer to a variable that will receive an error code from this function.
*
*                           OS_ERR_NONE                      if the requested task is suspended
*                           OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                            locked
*                           OS_ERR_TASK_SUSPEND_ISR          if you called this function from an ISR
*                           OS_ERR_TASK_SUSPEND_IDLE         if you attempted to suspend the idle task which is not
*                                                            allowed.
*                           OS_ERR_TASK_SUSPEND_INT_HANDLER  if you attempted to suspend the idle task which is not
*                                                            allowed.
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
*
*              2) You should use this function with great care.  If you suspend a task that is waiting for an event
*                 (i.e. a message, a semaphore, a queue ...) you will prevent this task from running when the event
*                 arrives.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u          //���ʹ���˺��� OSTaskSuspend()
void   OS_TaskSuspend (OS_TCB  *p_tcb,   //������ƿ�ָ��
                       OS_ERR  *p_err)   //���ش�������
{
    CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
									   //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
									   // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

    CPU_CRITICAL_ENTER();                                   //���ж�
    if (p_tcb == (OS_TCB *)0) {                             //��� p_tcb Ϊ��
        p_tcb = OSTCBCurPtr;                                //��������
    }

    if (p_tcb == OSTCBCurPtr) {                             //����ǹ�������
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {    //�������������
            CPU_CRITICAL_EXIT();                            //���ж�
           *p_err = OS_ERR_SCHED_LOCKED;                    //��������Ϊ��������������
            return;                                         //���أ�ִֹͣ��
        }
    }

   *p_err = OS_ERR_NONE;                                      //��������Ϊ���޴���
    switch (p_tcb->TaskState) {                               //���� p_tcb ������״̬���ദ��
        case OS_TASK_STATE_RDY:                               //����Ǿ���״̬
             OS_CRITICAL_ENTER_CPU_EXIT();                    //�����������ؿ��ж�
             p_tcb->TaskState  =  OS_TASK_STATE_SUSPENDED;    //����״̬��Ϊ������״̬��
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //����ǰ����Ϊ1
             OS_RdyListRemove(p_tcb);                         //������Ӿ����б��Ƴ�
             OS_CRITICAL_EXIT_NO_SCHED();                     //���������������е���
             break;                                           //����

        case OS_TASK_STATE_DLY:                               //�������ʱ״̬
             p_tcb->TaskState  = OS_TASK_STATE_DLY_SUSPENDED; //����״̬��Ϊ����ʱ�б�����
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //����ǰ����Ϊ1
             CPU_CRITICAL_EXIT();                             //���ж�
             break;                                           //����

        case OS_TASK_STATE_PEND:                              //����������޵ȴ�״̬
             p_tcb->TaskState  = OS_TASK_STATE_PEND_SUSPENDED;//����״̬��Ϊ�������޵ȴ��б�����
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //����ǰ����Ϊ1
             CPU_CRITICAL_EXIT();                             //���ж�
             break;                                           //����

        case OS_TASK_STATE_PEND_TIMEOUT:                      //����������޵ȴ�״̬
             p_tcb->TaskState  = OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED; //��Ϊ�������޵ȴ��б�����
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //����ǰ����Ϊ1
             CPU_CRITICAL_EXIT();                             //���ж�
             break;                                           //����

        case OS_TASK_STATE_SUSPENDED:                         //���״̬���й���״̬
        case OS_TASK_STATE_DLY_SUSPENDED:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             p_tcb->SuspendCtr++;                             //����Ƕ������1
             CPU_CRITICAL_EXIT();                             //���ж�
             break;                                           //����

        default:                                              //�������״̬����Ԥ��
             CPU_CRITICAL_EXIT();                             //���ж�
            *p_err = OS_ERR_STATE_INVALID;                    //��������Ϊ��״̬�Ƿ���
             return;                                          //���أ�ִֹͣ��
    } 

    OSSched();                                                //��������
}
#endif
