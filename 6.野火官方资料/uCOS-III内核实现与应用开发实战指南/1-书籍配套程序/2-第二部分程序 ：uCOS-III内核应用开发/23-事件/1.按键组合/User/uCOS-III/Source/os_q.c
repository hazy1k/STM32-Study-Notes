/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                               MESSAGE QUEUE MANAGEMENT
*
* File    : OS_Q.C
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
const  CPU_CHAR  *os_q__c = "$Id: $";
#endif


#if OS_CFG_Q_EN > 0u
/*
************************************************************************************************************************
*                                               CREATE A MESSAGE QUEUE
*
* Description: This function is called by your application to create a message queue.  Message queues MUST be created
*              before they can be used.
*
* Arguments  : p_q         is a pointer to the message queue
*
*              p_name      is a pointer to an ASCII string that will be used to name the message queue
*
*              max_qty     indicates the maximum size of the message queue (must be non-zero).  Note that it's also not
*                          possible to have a size higher than the maximum number of OS_MSGs available.
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE                    the call was successful
*                              OS_ERR_CREATE_ISR              can't create from an ISR
*                              OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Queue after you called
*                                                               OSSafetyCriticalStart().
*                              OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                              OS_ERR_OBJ_CREATED             if the message queue has already been created
*                              OS_ERR_OBJ_PTR_NULL            if you passed a NULL pointer for 'p_q'
*                              OS_ERR_Q_SIZE                  if the size you specified is 0
*
* Returns    : none
************************************************************************************************************************
*/

void  OSQCreate (OS_Q        *p_q,     //��Ϣ����ָ��
                 CPU_CHAR    *p_name,  //��Ϣ��������
                 OS_MSG_QTY   max_qty, //��Ϣ���д�С������Ϊ0��
                 OS_ERR      *p_err)   //���ش�������

{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�ִֹͣ��
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508               //���ʹ���˰�ȫ�ؼ�
    if (OSSafetyCriticalStartFlag == DEF_TRUE) { //����ڵ���OSSafetyCriticalStart()�󴴽�
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;  //��������Ϊ���Ƿ������ں˶���
        return;                                  //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú��������ж��б�����
       *p_err = OS_ERR_CREATE_ISR;             //��������Ϊ�����ж��д�������
        return;                                //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //���ʹ���˲������
    if (p_q == (OS_Q *)0) {           //��� p_q Ϊ��                       
       *p_err = OS_ERR_OBJ_PTR_NULL;  //��������Ϊ����������Ϊ�ա�
        return;                       //���أ�ִֹͣ��
    }
    if (max_qty == (OS_MSG_QTY)0) {   //��� max_qty = 0            
       *p_err = OS_ERR_Q_SIZE;        //��������Ϊ�����пռ�Ϊ0��
        return;                       //���أ�ִֹͣ��
    }
#endif

    OS_CRITICAL_ENTER();             //�����ٽ��
    p_q->Type    = OS_OBJ_TYPE_Q;    //��Ǵ����������ݽṹΪ��Ϣ����  
    p_q->NamePtr = p_name;           //�����Ϣ���е�����
    OS_MsgQInit(&p_q->MsgQ,          //��ʼ����Ϣ����
                max_qty);
    OS_PendListInit(&p_q->PendList); //��ʼ������Ϣ���еĵȴ��б�   

#if OS_CFG_DBG_EN > 0u               //���ʹ���˵��Դ���ͱ���
    OS_QDbgListAdd(p_q);             //���ö�����ӵ���Ϣ����˫���������
#endif
    OSQQty++;                        //��Ϣ���и�����1

    OS_CRITICAL_EXIT_NO_SCHED();     //�˳��ٽ�Σ��޵��ȣ�
   *p_err = OS_ERR_NONE;             //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : p_q       is a pointer to the message queue you want to delete
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND          Delete the queue ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS           Deletes the queue even if tasks are waiting.
*                                                        In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                 The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR              If you tried to delete the queue from an ISR
*                            OS_ERR_OBJ_PTR_NULL         if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE             if the message queue was not created
*                            OS_ERR_OPT_INVALID          An invalid option was specified
*                            OS_ERR_TASK_WAITING         One or more tasks were waiting on the queue
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the queue MUST
*                 check the return code of OSQPend().
*
*              2) OSQAccept() callers will not know that the intended queue has been deleted.
*
*              3) Because ALL tasks pending on the queue will be readied, you MUST be careful in applications where the
*                 queue is used for mutual exclusion because the resource(s) will no longer be guarded by the queue.
************************************************************************************************************************
*/

#if OS_CFG_Q_DEL_EN > 0u             //���ʹ���� OSQDel() ����
OS_OBJ_QTY  OSQDel (OS_Q    *p_q,    //��Ϣ����ָ��
                    OS_OPT   opt,    //ѡ��
                    OS_ERR  *p_err)  //���ش�������
{
    OS_OBJ_QTY     cnt;
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0);         //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_DEL_ISR;                 //��������Ϊ�����ж���ɾ������
        return ((OS_OBJ_QTY)0);                 //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    if (p_q == (OS_Q *)0) {               //��� p_q Ϊ��
       *p_err =  OS_ERR_OBJ_PTR_NULL;     //��������Ϊ������Ϊ�ա�
        return ((OS_OBJ_QTY)0u);          //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                        //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:          //���ѡ����Ԥ����
        case OS_OPT_DEL_ALWAYS:  
             break;                       //ֱ������

        default:
            *p_err =  OS_ERR_OPT_INVALID; //���ѡ���Ԥ��
             return ((OS_OBJ_QTY)0u);     //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u       //���ʹ���˶������ͼ��
    if (p_q->Type != OS_OBJ_TYPE_Q) { //��� p_q ������Ϣ��������
       *p_err = OS_ERR_OBJ_TYPE;      //��������Ϊ��������������
        return ((OS_OBJ_QTY)0);       //����0���д��󣩣�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                                  //���ж�
    p_pend_list = &p_q->PendList;                          //��ȡ��Ϣ���еĵȴ��б�
    cnt         = p_pend_list->NbrEntries;                 //��ȡ�ȴ��ö��е�������
    nbr_tasks   = cnt;                                     //����������Ŀ�������
    switch (opt) {                                         //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:                           //���ֻ��û������ȴ��������ɾ������
             if (nbr_tasks == (OS_OBJ_QTY)0) {             //���û�������ڵȴ��ö���
#if OS_CFG_DBG_EN > 0u                                     //���ʹ���˵��Դ���ͱ���  
                 OS_QDbgListRemove(p_q);                   //���ö��д���Ϣ���е����б��Ƴ�
#endif
                 OSQQty--;                                 //��Ϣ������Ŀ��1
                 OS_QClr(p_q);                             //����ö��е�����
                 CPU_CRITICAL_EXIT();                      //���ж�
                *p_err = OS_ERR_NONE;                      //��������Ϊ���޴���
             } else {                                      //����������ڵȴ��ö���
                 CPU_CRITICAL_EXIT();                      //���ж�
                *p_err = OS_ERR_TASK_WAITING;              //��������Ϊ���������ڵȴ��ö��С�
             }
             break;

        case OS_OPT_DEL_ALWAYS:                             //�������ɾ���ź���
             OS_CRITICAL_ENTER_CPU_EXIT();                  //�����ٽ�Σ��ؿ��ж�
             ts = OS_TS_GET();                              //��ȡʱ���
             while (cnt > 0u) {                             //����Ƴ��ö��еȴ��б��е�����
                 p_pend_data = p_pend_list->HeadPtr;
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_q),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                                     //���ʹ���˵��Դ���ͱ��� 
             OS_QDbgListRemove(p_q);                       //���ö��д���Ϣ���е����б��Ƴ�
#endif
             OSQQty--;                                     //��Ϣ������Ŀ��1
             OS_QClr(p_q);                                 //�����Ϣ��������
             OS_CRITICAL_EXIT_NO_SCHED();                  //�˳��ٽ�Σ��޵��ȣ�
             OSSched();                                    //��������
            *p_err = OS_ERR_NONE;                          //��������Ϊ���޴���
             break;                                        //����

        default:                                           //���ѡ���Ԥ��
             CPU_CRITICAL_EXIT();                          //���ж�
            *p_err = OS_ERR_OPT_INVALID;                   //��������Ϊ��ѡ��Ƿ���
             break;                                        //����
    }
    return (nbr_tasks);                                    //����ɾ������ǰ�ȴ����������
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                     FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : p_q        is a pointer to the message queue to flush
*
*               p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE           upon success
*                              OS_ERR_FLUSH_ISR      if you called this function from an ISR
*                              OS_ERR_OBJ_PTR_NULL   If you passed a NULL pointer for 'p_q'
*                              OS_ERR_OBJ_TYPE       If you didn't create the message queue
*
* Returns     : The number of entries freed from the queue
*
* Note(s)     : 1) You should use this function with great care because, when to flush the queue, you LOOSE the
*                  references to what the queue entries are pointing to and thus, you could cause 'memory leaks'.  In
*                  other words, the data you are pointing to that's being referenced by the queue entries should, most
*                  likely, need to be de-allocated (i.e. freed).
************************************************************************************************************************
*/

#if OS_CFG_Q_FLUSH_EN > 0u             //���ʹ���� OSQFlush() ����
OS_MSG_QTY  OSQFlush (OS_Q    *p_q,    //��Ϣ����ָ��
                      OS_ERR  *p_err)  //���ش�������
{
    OS_MSG_QTY  entries;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_MSG_QTY)0);         //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_FLUSH_ISR;               //��������Ϊ�����ж�����ն��С�
        return ((OS_MSG_QTY)0);                 //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //���ʹ���˲������
    if (p_q == (OS_Q *)0) {           //��� p_q Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;  //��������Ϊ������Ϊ�ա�
        return ((OS_MSG_QTY)0);       //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u       //���ʹ���˶������ͼ��
    if (p_q->Type != OS_OBJ_TYPE_Q) { //��� p_q ������Ϣ��������  
       *p_err = OS_ERR_OBJ_TYPE;      //��������Ϊ��������������
        return ((OS_MSG_QTY)0);       //����0���д��󣩣�ִֹͣ��
    }
#endif

    OS_CRITICAL_ENTER();                  //�����ٽ��
    entries = OS_MsgQFreeAll(&p_q->MsgQ); //�Ѷ��е�������Ϣ���ͷŻ���Ϣ��
    OS_CRITICAL_EXIT();                   //�˳��ٽ��
   *p_err   = OS_ERR_NONE;                //��������Ϊ���޴���
    return ((OS_MSG_QTY)entries);         //������ն���ǰ���е���Ϣ��Ŀ
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                            PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : p_q           is a pointer to the message queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for a
*                            message to arrive at the queue up to the amount of time specified by this argument.  If you
*                            specify 0, however, your task will wait forever at the specified queue or, until a message
*                            arrives.
*
*              opt           determines whether the user wants to block if the queue is empty or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message was
*                            received, pend aborted or the message queue deleted,  If you pass a NULL pointer (i.e.
*                            (CPU_TS *)0) then you will not get the timestamp.  In other words, passing a NULL pointer
*                            is valid and indicates that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_OBJ_PTR_NULL       if you pass a NULL pointer for 'p_q'
*                                OS_ERR_OBJ_TYPE           if the message queue was not created
*                                OS_ERR_PEND_ABORT         the pend was aborted
*                                OS_ERR_PEND_ISR           if you called this function from an ISR
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                                OS_ERR_SCHED_LOCKED       the scheduler is locked
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if you received a NULL pointer message or,
*                            if no message was received or,
*                            if 'p_q' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
************************************************************************************************************************
*/

void  *OSQPend (OS_Q         *p_q,       //��Ϣ����ָ��
                OS_TICK       timeout,   //�ȴ����ޣ���λ��ʱ�ӽ��ģ�
                OS_OPT        opt,       //ѡ��
                OS_MSG_SIZE  *p_msg_size,//������Ϣ��С����λ���ֽڣ�
                CPU_TS       *p_ts,      //��ȡ�ȵ���Ϣʱ��ʱ���
                OS_ERR       *p_err)     //���ش�������
{
    OS_PEND_DATA  pend_data;
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

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ISR;               //��������Ϊ�����ж�����ֹ�ȴ���
        return ((void *)0);                    //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    if (p_q == (OS_Q *)0) {               //��� p_q Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;      //��������Ϊ������Ϊ�ա�
        return ((void *)0);               //����0���д��󣩣�ִֹͣ��
    }
    if (p_msg_size == (OS_MSG_SIZE *)0) { //��� p_msg_size Ϊ��
       *p_err = OS_ERR_PTR_INVALID;       //��������Ϊ��ָ�벻���á�
        return ((void *)0);               //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                        //����ѡ����ദ��
        case OS_OPT_PEND_BLOCKING:        //���ѡ����Ԥ����
        case OS_OPT_PEND_NON_BLOCKING:
             break;                       //ֱ������

        default:                          //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;  //���ش�������Ϊ��ѡ��Ƿ���
             return ((void *)0);          //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u        //���ʹ���˶������ͼ��
    if (p_q->Type != OS_OBJ_TYPE_Q) {  //��� p_q ������Ϣ��������
       *p_err = OS_ERR_OBJ_TYPE;       //��������Ϊ��������������
        return ((void *)0);            //����0���д��󣩣�ִֹͣ��
    }
#endif

    if (p_ts != (CPU_TS *)0) {  //��� p_ts �ǿ�
       *p_ts  = (CPU_TS  )0;    //��ʼ�������㣩p_ts�������ڷ���ʱ���
    }

    CPU_CRITICAL_ENTER();                                 //���ж�
    p_void = OS_MsgQGet(&p_q->MsgQ,                       //����Ϣ���л�ȡһ����Ϣ
                        p_msg_size,
                        p_ts,
                        p_err);
    if (*p_err == OS_ERR_NONE) {                          //�����ȡ��Ϣ�ɹ�
        CPU_CRITICAL_EXIT();                              //���ж�
        return (p_void);                                  //������Ϣ����
    }
    /* �����ȡ��Ϣ���ɹ� */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //���ѡ���˲���������
        CPU_CRITICAL_EXIT();                              //���ж�
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                  //��������Ϊ���ȴ����������
        return ((void *)0);                               //����0���д��󣩣�ִֹͣ��
    } else {                                              //���ѡ���˶�������
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {  //�������������
            CPU_CRITICAL_EXIT();                          //���ж�
           *p_err = OS_ERR_SCHED_LOCKED;                  //��������Ϊ��������������
            return ((void *)0);                           //����0���д��󣩣�ִֹͣ��
        }
    }
    /* ���������δ���� */                                
    OS_CRITICAL_ENTER_CPU_EXIT();                         //�����������ؿ��ж�
    OS_Pend(&pend_data,                                   //������ǰ���񣬵ȴ���Ϣ���У� 
            (OS_PEND_OBJ *)((void *)p_q),                 //����ǰ������������б���
            OS_TASK_PEND_ON_Q,                            //���뵽�����б�͵ȴ��б�
            timeout);
    OS_CRITICAL_EXIT_NO_SCHED();                          //�����������������е���

    OSSched();                                            //�ҵ�������������ȼ���������
    /* ��ǰ���񣨻����Ϣ���е���Ϣ�����Լ������� */
    CPU_CRITICAL_ENTER();                                 //���ж�
    switch (OSTCBCurPtr->PendStatus) {                    //���ݵ�ǰ��������ĵȴ�״̬���ദ��
        case OS_STATUS_PEND_OK:                           //����ȴ�״̬����  
             p_void     = OSTCBCurPtr->MsgPtr;            //�ӣ�����ʱ���ڣ�������ƿ���ȡ��Ϣ
            *p_msg_size = OSTCBCurPtr->MsgSize;           //��ȡ��Ϣ��С
             if (p_ts  != (CPU_TS *)0) {                  //��� p_ts �ǿ�
                *p_ts   =  OSTCBCurPtr->TS;               //��ȡ����ȵ���Ϣʱ��ʱ���
             }
            *p_err      = OS_ERR_NONE;                    //��������Ϊ���޴���
             break;                                       //����

        case OS_STATUS_PEND_ABORT:                        //����ȴ�����ֹ
             p_void     = (void      *)0;                 //������Ϣ����Ϊ��
            *p_msg_size = (OS_MSG_SIZE)0;                 //������Ϣ��СΪ0
             if (p_ts  != (CPU_TS *)0) {                  //��� p_ts �ǿ�
                *p_ts   =  OSTCBCurPtr->TS;               //��ȡ�ȴ�����ֹʱ��ʱ���
             }
            *p_err      = OS_ERR_PEND_ABORT;              //��������Ϊ���ȴ�����ֹ��
             break;                                       //����

        case OS_STATUS_PEND_TIMEOUT:                      //����ȴ���ʱ
             p_void     = (void      *)0;                 //������Ϣ����Ϊ��
            *p_msg_size = (OS_MSG_SIZE)0;                 //������Ϣ��СΪ0
             if (p_ts  != (CPU_TS *)0) {                  //��� p_ts �ǿ�
                *p_ts   = (CPU_TS  )0;                    //���� p_ts
             }
            *p_err      = OS_ERR_TIMEOUT;                 //��������Ϊ���ȴ���ʱ��
             break;                                       //����

        case OS_STATUS_PEND_DEL:                          //����ȴ����ں˶���ɾ��
             p_void     = (void      *)0;                 //������Ϣ����Ϊ��
            *p_msg_size = (OS_MSG_SIZE)0;                 //������Ϣ��СΪ0
             if (p_ts  != (CPU_TS *)0) {                  //��� p_ts �ǿ�
                *p_ts   =  OSTCBCurPtr->TS;               //��ȡ����ɾʱ��ʱ���
             }
            *p_err      = OS_ERR_OBJ_DEL;                 //��������Ϊ���ȴ�����ɾ��
             break;                                       //����

        default:                                          //����ȴ�״̬����Ԥ��
             p_void     = (void      *)0;                 //������Ϣ����Ϊ��
            *p_msg_size = (OS_MSG_SIZE)0;                 //������Ϣ��СΪ0
            *p_err      = OS_ERR_STATUS_INVALID;          //��������Ϊ��״̬�Ƿ���
             break;                                       //����
    }
    CPU_CRITICAL_EXIT();                                  //���ж�
    return (p_void);                                      //������Ϣ����
}


/*$PAGE*/
/*
************************************************************************************************************************
*                                             ABORT WAITING ON A MESSAGE QUEUE
*
* Description: This function aborts & readies any tasks currently waiting on a queue.  This function should be used to
*              fault-abort the wait on the queue, rather than to normally signal the queue via OSQPost().
*
* Arguments  : p_q       is a pointer to the message queue
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the queue
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the queue
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the queue was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the queue was aborted.
*                            OS_ERR_OPT_INVALID           if you specified an invalid option
*                            OS_ERR_OBJ_PTR_NULL          if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE              if the message queue was not created
*                            OS_ERR_PEND_ABORT_ISR        If this function was called from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_Q_PEND_ABORT_EN > 0u            //���ʹ���� OSQPendAbort() ����
OS_OBJ_QTY  OSQPendAbort (OS_Q    *p_q,    //��Ϣ����
                          OS_OPT   opt,    //ѡ��
                          OS_ERR  *p_err)  //���ش�������
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0u);         //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {  //����ú������ж��б�����
       *p_err =  OS_ERR_PEND_ABORT_ISR;          //��������Ϊ�����ж�����ֹ�ȴ���
        return ((OS_OBJ_QTY)0u);                 //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                                  //����0���д��󣩣�ִֹͣ��
    if (p_q == (OS_Q *)0) {                                 //��� p_q Ϊ��
       *p_err =  OS_ERR_OBJ_PTR_NULL;                       //��������Ϊ������Ϊ�ա�
        return ((OS_OBJ_QTY)0u);                            //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                                          //����ѡ����ദ��
        case OS_OPT_PEND_ABORT_1:                           //���ѡ����Ԥ��֮��
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                                         //ֱ������

        default:                                            //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;                   //��������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0u);                       //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u        //���ʹ���˶������ͼ��
    if (p_q->Type != OS_OBJ_TYPE_Q) {  //��� p_q ������Ϣ��������
       *p_err =  OS_ERR_OBJ_TYPE;      //��������Ϊ��������������
        return ((OS_OBJ_QTY)0u);       //����0���д��󣩣�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                            //���ж�
    p_pend_list = &p_q->PendList;                    //��ȡ��Ϣ���еĵȴ��б�
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) { //���û�������ڵȴ�
        CPU_CRITICAL_EXIT();                         //���ж�
       *p_err =  OS_ERR_PEND_ABORT_NONE;             //��������Ϊ��û�����ڵȴ���
        return ((OS_OBJ_QTY)0u);
    }
    /* ����������ڵȴ� */
    OS_CRITICAL_ENTER_CPU_EXIT();                     //�����������ؿ��ж�   
    nbr_tasks = 0u;                                   //׼����������ֹ�ĵȴ�����
    ts        = OS_TS_GET();                          //��ȡʱ���
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) {//������������ڵȴ�
        p_tcb = p_pend_list->HeadPtr->TCBPtr;         //��ȡͷ�ˣ�������ȼ�������
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_q),    //��ֹ������� p_q �ĵȴ�
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {           //�������ѡ������ֹ���еȴ�����
            break;                                    //�������������ټ�����ֹ
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                      //��������������������

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) { //���ѡ�����������
        OSSched();                                    //�����������
    }

   *p_err = OS_ERR_NONE;                              //��������Ϊ���޴���
    return (nbr_tasks);                               //���ر���ֹ��������Ŀ
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  With the 'opt' argument, you can specify whether the message
*              is broadcast to all waiting tasks and/or whether you post the message to the front of the queue (LIFO)
*              or normally (FIFO) at the end of the queue.
*
* Arguments  : p_q           is a pointer to a message queue that must have been created by OSQCreate().
*
*              p_void        is a pointer to the message to send.
*
*              msg_size      specifies the size of the message (in bytes)
*
*              opt           determines the type of POST performed:
*
*                                OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the queue.  This option
*                                                         can be added to either OS_OPT_POST_FIFO or OS_OPT_POST_LIFO
*                                OS_OPT_POST_FIFO         POST message to end of queue (FIFO) and wake up a single
*                                                         waiting task.
*                                OS_OPT_POST_LIFO         POST message to the front of the queue (LIFO) and wake up
*                                                         a single waiting task.
*                                OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                            Note(s): 1) OS_OPT_POST_NO_SCHED can be added (or OR'd) with one of the other options.
*                                     2) OS_OPT_POST_ALL      can be added (or OR'd) with one of the other options.
*                                     3) Possible combination of options are:
*
*                                        OS_OPT_POST_FIFO
*                                        OS_OPT_POST_LIFO
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_ALL
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_ALL
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                                OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs to use to place the message into
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_Q_MAX           If the queue is full
*
* Returns    : None
************************************************************************************************************************
*/

void  OSQPost (OS_Q         *p_q,      //��Ϣ����ָ��
               void         *p_void,   //��Ϣָ��
               OS_MSG_SIZE   msg_size, //��Ϣ��С����λ���ֽڣ�
               OS_OPT        opt,      //ѡ�� 
               OS_ERR       *p_err)    //���ش�������
{
    CPU_TS  ts;



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u             //���ʹ���˲������
    if (p_q == (OS_Q *)0) {            //��� p_q Ϊ��  
       *p_err = OS_ERR_OBJ_PTR_NULL;   //��������Ϊ���ں˶���Ϊ�ա�
        return;                        //���أ�ִֹͣ��
    }
    switch (opt) {                         //����ѡ����ദ��
        case OS_OPT_POST_FIFO:             //���ѡ����Ԥ����
        case OS_OPT_POST_LIFO:
        case OS_OPT_POST_FIFO | OS_OPT_POST_ALL:
        case OS_OPT_POST_LIFO | OS_OPT_POST_ALL:
        case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;                       //ֱ������

        default:                          //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID; //��������Ϊ��ѡ��Ƿ���
             return;                      //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u       //���ʹ���˶������ͼ��  
    if (p_q->Type != OS_OBJ_TYPE_Q) { //��� p_q ������Ϣ�������� 
       *p_err = OS_ERR_OBJ_TYPE;      //��������Ϊ���������ʹ���
        return;                       //���أ�ִֹͣ��
    }
#endif

    ts = OS_TS_GET();                 //��ȡʱ���

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u            //���ʹ�����ж��ӳٷ���
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_Q, //������Ϣ�������ж���Ϣ����
                    (void      *)p_q,
                    (void      *)p_void,
                    (OS_MSG_SIZE)msg_size,
                    (OS_FLAGS   )0,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return;                                //���أ���δ��������ִֹͣ��  
    }
#endif

    OS_QPost(p_q,                              //����Ϣ������ͨ��ʽ
             p_void,
             msg_size,
             opt,
             ts,
             p_err);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        CLEAR THE CONTENTS OF A MESSAGE QUEUE
*
* Description: This function is called by OSQDel() to clear the contents of a message queue
*

* Argument(s): p_q      is a pointer to the queue to clear
*              ---
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_QClr (OS_Q  *p_q)
{
    (void)OS_MsgQFreeAll(&p_q->MsgQ);                       /* Return all OS_MSGs to the free list                    */
    p_q->Type    =  OS_OBJ_TYPE_NONE;                       /* Mark the data structure as a NONE                      */
    p_q->NamePtr = (CPU_CHAR *)((void *)"?Q");
    OS_MsgQInit(&p_q->MsgQ,                                 /* Initialize the list of OS_MSGs                         */
                0u);
    OS_PendListInit(&p_q->PendList);                        /* Initialize the waiting list                            */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                      ADD/REMOVE MESSAGE QUEUE TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a message queue to/from a message queue debug
*              list.
*
* Arguments  : p_q     is a pointer to the message queue to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u             //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ���
void  OS_QDbgListAdd (OS_Q  *p_q)  //����Ϣ���в��뵽��Ϣ�����б����ǰ��
{
    p_q->DbgNamePtr               = (CPU_CHAR *)((void *)" "); //�Ȳ�ָ���κ����������
    p_q->DbgPrevPtr               = (OS_Q     *)0;             //���ö�����Ϊ�б����ǰ��
    if (OSQDbgListPtr == (OS_Q *)0) {                          //����б�Ϊ��
        p_q->DbgNextPtr           = (OS_Q     *)0;             //�ö��е���һ��Ԫ��ҲΪ��
    } else {                                                   //����б�ǿ�
        p_q->DbgNextPtr           =  OSQDbgListPtr;            //�б�ԭ������Ԫ����Ϊ�ö��е���һ��Ԫ��
        OSQDbgListPtr->DbgPrevPtr =  p_q;                      //ԭ������Ԫ�ص�ǰ���Ϊ�˸ö���
    }
    OSQDbgListPtr                 =  p_q;                      //���ź�����Ϊ�б������Ԫ��
}



void  OS_QDbgListRemove (OS_Q  *p_q)
{
    OS_Q  *p_q_next;
    OS_Q  *p_q_prev;


    p_q_prev = p_q->DbgPrevPtr;
    p_q_next = p_q->DbgNextPtr;

    if (p_q_prev == (OS_Q *)0) {
        OSQDbgListPtr = p_q_next;
        if (p_q_next != (OS_Q *)0) {
            p_q_next->DbgPrevPtr = (OS_Q *)0;
        }
        p_q->DbgNextPtr = (OS_Q *)0;

    } else if (p_q_next == (OS_Q *)0) {
        p_q_prev->DbgNextPtr = (OS_Q *)0;
        p_q->DbgPrevPtr      = (OS_Q *)0;

    } else {
        p_q_prev->DbgNextPtr =  p_q_next;
        p_q_next->DbgPrevPtr =  p_q_prev;
        p_q->DbgNextPtr      = (OS_Q *)0;
        p_q->DbgPrevPtr      = (OS_Q *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              MESSAGE QUEUE INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the message queue management.
*

* Arguments  : p_err         is a pointer to a variable that will receive an error code.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_QInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSQDbgListPtr = (OS_Q *)0;
#endif

    OSQQty        = (OS_OBJ_QTY)0;
   *p_err         = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  With the 'opt' argument, you can specify whether the message
*              is broadcast to all waiting tasks and/or whether you post the message to the front of the queue (LIFO)
*              or normally (FIFO) at the end of the queue.
*
* Arguments  : p_q           is a pointer to a message queue that must have been created by OSQCreate().
*
*              p_void        is a pointer to the message to send.
*
*              msg_size      specifies the size of the message (in bytes)
*
*              opt           determines the type of POST performed:
*
*                                OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the queue
*
*                                OS_OPT_POST_FIFO         POST as FIFO and wake up single waiting task
*                                OS_OPT_POST_LIFO         POST as LIFO and wake up single waiting task
*
*                                OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              ts            is the timestamp of the post
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                                OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs to use to place the message into
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_Q_MAX           If the queue is full
*
* Returns    : None
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_QPost (OS_Q         *p_q,      //��Ϣ����ָ��
                void         *p_void,   //��Ϣָ��
                OS_MSG_SIZE   msg_size, //��Ϣ��С����λ���ֽڣ�
                OS_OPT        opt,      //ѡ��
                CPU_TS        ts,       //��Ϣ������ʱ��ʱ���
                OS_ERR       *p_err)    //���ش�������
{
    OS_OBJ_QTY     cnt;
    OS_OPT         post_type;
    OS_PEND_LIST  *p_pend_list;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_DATA  *p_pend_data_next;
    OS_TCB        *p_tcb;
    CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
									   //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
									   // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

    OS_CRITICAL_ENTER();                              //�����ٽ��
    p_pend_list = &p_q->PendList;                     //ȡ���ö��еĵȴ��б�
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {   //���û�������ڵȴ��ö���
        if ((opt & OS_OPT_POST_LIFO) == (OS_OPT)0) {  //����Ϣ���������е�ĩ��
            post_type = OS_OPT_POST_FIFO;
        } else {                                      //����Ϣ���������е�ǰ��
            post_type = OS_OPT_POST_LIFO;
        }
        OS_MsgQPut(&p_q->MsgQ,                        //����Ϣ������Ϣ����          /* Place message in the message queue                     */
                   p_void,
                   msg_size,
                   post_type,
                   ts,
                   p_err);
        OS_CRITICAL_EXIT();                          //�˳��ٽ��
        return;                                      //���أ�ִ�����
    }
    /* ����������ڵȴ��ö��� */
    if ((opt & OS_OPT_POST_ALL) != (OS_OPT)0) {     //���Ҫ����Ϣ���������еȴ�����
        cnt = p_pend_list->NbrEntries;              //��ȡ�ȴ�������Ŀ
    } else {                                        //���Ҫ����Ϣ������һ���ȴ�����
        cnt = (OS_OBJ_QTY)1;                        //Ҫ�����������ĿΪ1
    }
    p_pend_data = p_pend_list->HeadPtr;             //��ȡ�ȴ��б��ͷ��������
    while (cnt > 0u) {                              //����Ҫ������������Ŀ�������
        p_tcb            = p_pend_data->TCBPtr;
        p_pend_data_next = p_pend_data->NextPtr;
        OS_Post((OS_PEND_OBJ *)((void *)p_q),       //����Ϣ����������
                p_tcb,
                p_void,
                msg_size,
                ts);
        p_pend_data = p_pend_data_next;
        cnt--;
    }
    OS_CRITICAL_EXIT_NO_SCHED();                    //�˳��ٽ�Σ��޵��ȣ�
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {//���ûѡ�񡰷����겻��������
        OSSched();                                  //�������
    }
   *p_err = OS_ERR_NONE;                            //��������Ϊ���޴���
}

#endif
