/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   MUTEX MANAGEMENT
*
* File    : OS_MUTEX.C
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
const  CPU_CHAR  *os_mutex__c = "$Id: $";
#endif


#if OS_CFG_MUTEX_EN > 0u
/*
************************************************************************************************************************
*                                                   CREATE A MUTEX
*
* Description: This function creates a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to initialize.  Your application is responsible for allocating
*                            storage for the mutex.
*
*              p_name        is a pointer to the name you would like to give the mutex.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Mutex after you called
*                                                                 OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name'  is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the mutex has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_mutex' is a NULL pointer
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexCreate (OS_MUTEX  *p_mutex, //�����ź���ָ��
                     CPU_CHAR  *p_name,  //ȡ�ź���������
                     OS_ERR    *p_err)   //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ�� 

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�������ִ��
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ�ؼ�
    if (OSSafetyCriticalStartFlag == DEF_TRUE) { //������ڵ��� OSSafetyCriticalStart() �󴴽�
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;  //��������Ϊ���Ƿ������ں˶���
        return;                                  //���أ�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {   //����ú��������ж��б�����
       *p_err = OS_ERR_CREATE_ISR;               //��������Ϊ�����жϺ����ж�ʱ��
        return;                                  //���أ�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_mutex == (OS_MUTEX *)0) {   //������� p_mutex Ϊ��                       
       *p_err = OS_ERR_OBJ_PTR_NULL;  //��������Ϊ����������Ϊ�ա�
        return;                       //���أ�������ִ��
    }
#endif

    OS_CRITICAL_ENTER();              //�����ٽ�Σ���ʼ�������ź���ָ�� 
    p_mutex->Type              =  OS_OBJ_TYPE_MUTEX;  //��Ǵ����������ݽṹΪ�����ź���  
    p_mutex->NamePtr           =  p_name;
    p_mutex->OwnerTCBPtr       = (OS_TCB       *)0;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)0;   //�����ź���Ŀǰ����
    p_mutex->TS                = (CPU_TS        )0;
    p_mutex->OwnerOriginalPrio =  OS_CFG_PRIO_MAX;
    OS_PendListInit(&p_mutex->PendList);              //��ʼ���û����ź����ĵȴ��б�   

#if OS_CFG_DBG_EN > 0u           //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ��� 
    OS_MutexDbgListAdd(p_mutex); //�����ź�����ӵ������ź���˫���������
#endif
    OSMutexQty++;                //�����ź���������1         

    OS_CRITICAL_EXIT_NO_SCHED(); //�˳��ٽ�Σ��޵��ȣ�
   *p_err = OS_ERR_NONE;          //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   DELETE A MUTEX
*
* Description: This function deletes a mutex and readies all tasks pending on the mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete mutex ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the mutex even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the mutex was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the mutex from an ISR
*                                OS_ERR_OBJ_PTR_NULL         If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_mutex' is not pointing to a mutex
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                                OS_ERR_STATE_INVALID        Task is in an invalid state
*                                OS_ERR_TASK_WAITING         One or more tasks were waiting on the mutex
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the mutex MUST
*                 check the return code of OSMutexPend().
*
*              2) OSMutexAccept() callers will not know that the intended mutex has been deleted.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful in applications where the
*                 mutex is used for mutual exclusion because the resource(s) will no longer be guarded by the mutex.
************************************************************************************************************************
*/

#if OS_CFG_MUTEX_DEL_EN > 0u                //���ʹ���� OSMutexDel()   
OS_OBJ_QTY  OSMutexDel (OS_MUTEX  *p_mutex, //�����ź���ָ��
                        OS_OPT     opt,     //ѡ��
                        OS_ERR    *p_err)   //���ش�������
{
    OS_OBJ_QTY     cnt;
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    OS_TCB        *p_tcb_owner;
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

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u        //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {//����ú������ж��б�����
       *p_err = OS_ERR_DEL_ISR;               //��������Ϊ�����ж�����ֹ�ȴ���
        return ((OS_OBJ_QTY)0);               //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    if (p_mutex == (OS_MUTEX *)0) {       //��� p_mutex Ϊ��             
       *p_err = OS_ERR_OBJ_PTR_NULL;      //��������Ϊ������Ϊ�ա�
        return ((OS_OBJ_QTY)0);           //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                        //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:          //���ѡ����Ԥ����
        case OS_OPT_DEL_ALWAYS:
             break;                       //ֱ������

        default:                          //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID; //��������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0);      //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //���ʹ���˶������ͼ��
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //��� p_mutex �ǻ����ź�������
       *p_err = OS_ERR_OBJ_TYPE;              //��������Ϊ���������ʹ���
        return ((OS_OBJ_QTY)0);               //����0���д��󣩣�ִֹͣ��
    }
#endif

    OS_CRITICAL_ENTER();                        //�����ٽ��
    p_pend_list = &p_mutex->PendList;           //��ȡ�ź����ĵȴ��б�
    cnt         = p_pend_list->NbrEntries;      //��ȡ�ȴ����ź�����������
    nbr_tasks   = cnt;
    switch (opt) {                              //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:                //���ֻ��û����ȴ�ʱɾ���ź���
             if (nbr_tasks == (OS_OBJ_QTY)0) {  //���û�������ڵȴ����ź���
#if OS_CFG_DBG_EN > 0u                          //���ʹ���˵��Դ���ͱ���  
                 OS_MutexDbgListRemove(p_mutex);//�����ź������ź��������б��Ƴ�
#endif
                 OSMutexQty--;                  //�����ź�����Ŀ��1
                 OS_MutexClr(p_mutex);          //����ź�������
                 OS_CRITICAL_EXIT();            //�˳��ٽ��
                *p_err = OS_ERR_NONE;           //��������Ϊ���޴���
             } else {                           //����������ڵȴ����ź���
                 OS_CRITICAL_EXIT();            //�˳��ٽ��
                *p_err = OS_ERR_TASK_WAITING;   //��������Ϊ�����������ڵȴ���
             }
             break;                             //����

        case OS_OPT_DEL_ALWAYS:                                          //�������ɾ���ź���  
             p_tcb_owner = p_mutex->OwnerTCBPtr;                         //��ȡ�ź�����������
             if ((p_tcb_owner       != (OS_TCB *)0) &&                   //�������������ڣ�
                 (p_tcb_owner->Prio !=  p_mutex->OwnerOriginalPrio)) {   //�������ȼ�����������
                 switch (p_tcb_owner->TaskState) {                       //����������״̬����
                     case OS_TASK_STATE_RDY:                             //����Ǿ���״̬
                          OS_RdyListRemove(p_tcb_owner);                 //������Ӿ����б��Ƴ�
                          p_tcb_owner->Prio = p_mutex->OwnerOriginalPrio;//��ԭ��������ȼ�
                          OS_PrioInsert(p_tcb_owner->Prio);              //�������ȼ��������ȼ����
                          OS_RdyListInsertTail(p_tcb_owner);             //�������ز�������б�
                          break;                                         //����

                     case OS_TASK_STATE_DLY:                             //�������ʱ״̬
                     case OS_TASK_STATE_SUSPENDED:                       //����Ǳ�����״̬
                     case OS_TASK_STATE_DLY_SUSPENDED:                   //�������ʱ�б�����״̬
                          p_tcb_owner->Prio = p_mutex->OwnerOriginalPrio;//��ԭ��������ȼ�
                          break;

                     case OS_TASK_STATE_PEND:                            //����������޵ȴ�״̬
                     case OS_TASK_STATE_PEND_TIMEOUT:                    //����������޵ȴ�״̬
                     case OS_TASK_STATE_PEND_SUSPENDED:                  //��������ڵȴ��б���״̬
                     case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:          //��������ڵȴ��б���״̬
                          OS_PendListChangePrio(p_tcb_owner,             //�ı������ڵȴ��б��λ��
                                                p_mutex->OwnerOriginalPrio);
                          break;

                     default:                                            //���״̬����Ԥ��
                          OS_CRITICAL_EXIT();
                         *p_err = OS_ERR_STATE_INVALID;                  //��������Ϊ������״̬�Ƿ���
                          return ((OS_OBJ_QTY)0);                        //����0���д��󣩣�ִֹͣ��
                 }
             }

             ts = OS_TS_GET();                                           //��ȡʱ���
             while (cnt > 0u) {                                          //�Ƴ��û����ź����ȴ��б�
                 p_pend_data = p_pend_list->HeadPtr;                     //�е���������
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_mutex),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                          //���ʹ���˵��Դ���ͱ��� 
             OS_MutexDbgListRemove(p_mutex);    //���ź����ӻ����ź��������б��Ƴ�
#endif
             OSMutexQty--;                      //�����ź�����Ŀ��1
             OS_MutexClr(p_mutex);              //����ź�������
             OS_CRITICAL_EXIT_NO_SCHED();       //�˳��ٽ�Σ���������
             OSSched();                         //����������ȼ���������
            *p_err = OS_ERR_NONE;               //��������Ϊ���޴���
             break;                             //����

        default:                                //���ѡ���Ԥ��
             OS_CRITICAL_EXIT();                //�˳��ٽ��
            *p_err = OS_ERR_OPT_INVALID;        //��������Ϊ��ѡ��Ƿ���
             break;                             //����
    }
    return (nbr_tasks);                         //����ɾ��ǰ�ź����ȴ��б��е�������
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    PEND ON MUTEX
*
* Description: This function waits for a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for the
*                            resource up to the amount of time (in 'ticks') specified by this argument.  If you specify
*                            0, however, your task will wait forever at the specified mutex or, until the resource
*                            becomes available.
*
*              opt           determines whether the user wants to block if the mutex is not available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the mutex was posted or
*                            pend aborted or the mutex deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you
*                            will not get the timestamp.  In other words, passing a NULL pointer is valid and indicates
*                            that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task owns the resource
*                                OS_ERR_MUTEX_OWNER        If calling task already owns the mutex
*                                OS_ERR_OBJ_DEL            If 'p_mutex' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_mutex' is not pointing at a mutex
*                                OS_ERR_OPT_INVALID        If you didn't specify a valid option
*                                OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the mutex was not
*                                                          available.
*                                OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                                OS_ERR_STATE_INVALID      If the task is in an invalid state
*                                OS_ERR_STATUS_INVALID     If the pend status has an invalid value
*                                OS_ERR_TIMEOUT            The mutex was not received within the specified timeout.
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexPend (OS_MUTEX  *p_mutex, //�����ź���ָ��
                   OS_TICK    timeout, //��ʱʱ�䣨���ģ�
                   OS_OPT     opt,     //ѡ��
                   CPU_TS    *p_ts,    //ʱ���
                   OS_ERR    *p_err)   //���ش�������
{
    OS_PEND_DATA  pend_data;
    OS_TCB       *p_tcb;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ISR;               //��������Ϊ�����ж��еȴ���
        return;                                //���أ�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u               //���ʹ���˲������
    if (p_mutex == (OS_MUTEX *)0) {      //��� p_mutex Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;     //���ش�������Ϊ���ں˶���Ϊ�ա�
        return;                          //���أ�������ִ��
    }
    switch (opt) {                       //����ѡ����ദ��
        case OS_OPT_PEND_BLOCKING:       //���ѡ����Ԥ����
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:                         //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID; //��������Ϊ��ѡ��Ƿ���
             return;                     //���أ�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //���ʹ���˶������ͼ��
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //��� p_mutex �ǻ����ź�������
       *p_err = OS_ERR_OBJ_TYPE;              //��������Ϊ���ں˶������ʹ���
        return;                               //���أ�������ִ��
    }
#endif

    if (p_ts != (CPU_TS *)0) {  //��� p_ts �ǿ�
       *p_ts  = (CPU_TS  )0;    //��ʼ�������㣩p_ts�������ڷ���ʱ���    
    }

    CPU_CRITICAL_ENTER();                                //���ж�
    if (p_mutex->OwnerNestingCtr == (OS_NESTING_CTR)0) { //����ź�������
        p_mutex->OwnerTCBPtr       =  OSTCBCurPtr;       //�õ�ǰ��������ź��� 
        p_mutex->OwnerOriginalPrio =  OSTCBCurPtr->Prio; //���������������ȼ�
        p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)1;  //��ʼǶ��
        if (p_ts != (CPU_TS *)0) {                       //��� p_ts �ǿ�    
           *p_ts  = p_mutex->TS;                         //�����ź�����ʱ�����¼
        }
        CPU_CRITICAL_EXIT();                             //���ж�
       *p_err = OS_ERR_NONE;                             //��������Ϊ���޴���
        return;                                          //���أ�������ִ��
    }
    /* ����ź��������� */
    if (OSTCBCurPtr == p_mutex->OwnerTCBPtr) { //�����ǰ�����Ѿ����и��ź���
        p_mutex->OwnerNestingCtr++;            //�ź���ǰ������1
        if (p_ts != (CPU_TS *)0) {             //��� p_ts �ǿ�
           *p_ts  = p_mutex->TS;               //�����ź�����ʱ�����¼
        }
        CPU_CRITICAL_EXIT();                   //���ж�
       *p_err = OS_ERR_MUTEX_OWNER;            //��������Ϊ�������ѳ����ź�����
        return;                                //���أ�������ִ��
    }
    /* �����ǰ����ǳ��и��ź��� */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {//���ѡ���˲���������
        CPU_CRITICAL_EXIT();                            //���ж�
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                //��������Ϊ�����������  
        return;                                         //���أ�������ִ��
    } else {                                            //���ѡ���˶�������
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {//�������������
            CPU_CRITICAL_EXIT();                        //���ж�
           *p_err = OS_ERR_SCHED_LOCKED;                //��������Ϊ��������������
            return;                                     //���أ�������ִ��
        }
    }
    /* ���������δ���� */                                                        
    OS_CRITICAL_ENTER_CPU_EXIT();                          //�������������ؿ��ж�
    p_tcb = p_mutex->OwnerTCBPtr;                          //��ȡ�ź�����������
    if (p_tcb->Prio > OSTCBCurPtr->Prio) {                 //��������������ȼ����ڵ�ǰ����
        switch (p_tcb->TaskState) {                        //���ݳ������������״̬���ദ��
            case OS_TASK_STATE_RDY:                        //����Ǿ���״̬
                 OS_RdyListRemove(p_tcb);                  //�Ӿ����б��Ƴ���������
                 p_tcb->Prio = OSTCBCurPtr->Prio;          //����������������ȼ�����ǰ����
                 OS_PrioInsert(p_tcb->Prio);               //�������ȼ��������ȼ����
                 OS_RdyListInsertHead(p_tcb);              //�����������������б�
                 break;                                    //����

            case OS_TASK_STATE_DLY:                        //�������ʱ״̬
            case OS_TASK_STATE_DLY_SUSPENDED:              //�������ʱ�б�����״̬
            case OS_TASK_STATE_SUSPENDED:                  //����Ǳ�����״̬
                 p_tcb->Prio = OSTCBCurPtr->Prio;          //����������������ȼ�����ǰ����
                 break;                                    //����

            case OS_TASK_STATE_PEND:                       //����������޵ȴ�״̬
            case OS_TASK_STATE_PEND_TIMEOUT:               //����������޵ȴ�״̬
            case OS_TASK_STATE_PEND_SUSPENDED:             //����������޵ȴ��б�����״̬
            case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:     //����������޵ȴ��б�����״̬
                 OS_PendListChangePrio(p_tcb,              //�ı���������ڵȴ��б��λ��
                                       OSTCBCurPtr->Prio);
                 break;                                    //����

            default:                                       //�������״̬����Ԥ��
                 OS_CRITICAL_EXIT();                       //���ж�
                *p_err = OS_ERR_STATE_INVALID;             //��������Ϊ������״̬�Ƿ���
                 return;                                   //���أ�������ִ��
        }
    }
    /* �������񣬽���ǰ������������б������뵽�����б�͵ȴ��б�*/
    OS_Pend(&pend_data,                                     
            (OS_PEND_OBJ *)((void *)p_mutex),
             OS_TASK_PEND_ON_MUTEX,
             timeout);

    OS_CRITICAL_EXIT_NO_SCHED();          //�����������������е���

    OSSched();                            //����������ȼ���������

    CPU_CRITICAL_ENTER();                 //���ж�
    switch (OSTCBCurPtr->PendStatus) {    //���ݵ�ǰ��������ĵȴ�״̬���ദ��
        case OS_STATUS_PEND_OK:           //����ȴ�����������ź�����
             if (p_ts != (CPU_TS *)0) {   //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS; //�����ź������һ�α��ͷŵ�ʱ���
             }
            *p_err = OS_ERR_NONE;         //��������Ϊ���޴���
             break;                       //����

        case OS_STATUS_PEND_ABORT:        //����ȴ�����ֹ
             if (p_ts != (CPU_TS *)0) {   //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS; //���صȴ�����ֹʱ��ʱ���
             }
            *p_err = OS_ERR_PEND_ABORT;   //��������Ϊ���ȴ�����ֹ��
             break;                       //����

        case OS_STATUS_PEND_TIMEOUT:      //�����ʱ��Ϊ����ź���
             if (p_ts != (CPU_TS *)0) {   //��� p_ts �ǿ�
                *p_ts  = (CPU_TS  )0;     //���� p_ts
             }
            *p_err = OS_ERR_TIMEOUT;      //��������Ϊ����ʱ��
             break;                       //����

        case OS_STATUS_PEND_DEL:          //����ź����ѱ�ɾ��                  
             if (p_ts != (CPU_TS *)0) {   //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS; //�����ź�����ɾ��ʱ��ʱ���
             }
            *p_err = OS_ERR_OBJ_DEL;      //��������Ϊ������ɾ����
             break;                       //����

        default:                           //���ݵȴ�״̬����Ԥ��
            *p_err = OS_ERR_STATUS_INVALID;//��������Ϊ��״̬�Ƿ���
             break;                        //����
    }
    CPU_CRITICAL_EXIT();                   //���ж�
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               ABORT WAITING ON A MUTEX
*
* Description: This function aborts & readies any tasks currently waiting on a mutex.  This function should be used
*              to fault-abort the wait on the mutex, rather than to normally signal the mutex via OSMutexPost().
*
* Arguments  : p_mutex   is a pointer to the mutex
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the mutex
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the mutex
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the mutex was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the mutex was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_mutex' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_mutex' is not pointing at a mutex
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you attempted to call this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_MUTEX_PEND_ABORT_EN > 0u               //���ʹ���� OSMutexPendAbort()
OS_OBJ_QTY  OSMutexPendAbort (OS_MUTEX  *p_mutex, //�����ź���ָ��
                              OS_OPT     opt,     //ѡ��
                              OS_ERR    *p_err)   //���ش�������
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0u);        //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) { //����ú������ж��б�����
       *p_err =  OS_ERR_PEND_ABORT_ISR;         //��������Ϊ�����ж�����ֹ�ȴ���
        return ((OS_OBJ_QTY)0u);                //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                   //���ʹ���˲������
    if (p_mutex == (OS_MUTEX *)0) {          //��� p_sem Ϊ��
       *p_err =  OS_ERR_OBJ_PTR_NULL;        //��������Ϊ������Ϊ�ա�
        return ((OS_OBJ_QTY)0u);             //����0���д��󣩣�������ִ��
    }
    switch (opt) {                                         //����ѡ����ദ��
        case OS_OPT_PEND_ABORT_1:                          //���ѡ����Ԥ����
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                                        //ֱ������

        default:                               //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;      //��������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0u);          //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //���ʹ���˶������ͼ��
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //��� p_mutex ���ǻ����ź�������
       *p_err =  OS_ERR_OBJ_TYPE;             //��������Ϊ���������ʹ���
        return ((OS_OBJ_QTY)0u);              //����0���д��󣩣�������ִ��
    }
#endif

    CPU_CRITICAL_ENTER();                            //���ж�
    p_pend_list = &p_mutex->PendList;                //��ȡ p_mutex �ĵȴ��б�
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) { //���û�������ڵȴ� p_mutex
        CPU_CRITICAL_EXIT();                         //���ж�
       *p_err =  OS_ERR_PEND_ABORT_NONE;             //��������Ϊ��û�������ڵȴ���
        return ((OS_OBJ_QTY)0u);                     //����0���д��󣩣�������ִ��
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                      //�������������ؿ��ж�
    nbr_tasks = 0u;
    ts        = OS_TS_GET();                           //��ȡʱ���
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) { //����������ڵȴ� p_mutex
        p_tcb = p_pend_list->HeadPtr->TCBPtr;          //��ȡ���ȼ���ߵĵȴ�����
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_mutex), //��ֹ�������p_mutex�ĵȴ�
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {            //�����ѡ����ֹ���еȴ�����
            break;                                     //�������������ټ�����ֹ
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                       //��������������������

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) {  //���ѡ�����������
        OSSched();                                     //�����������
    }

   *p_err = OS_ERR_NONE;                               //����������Ϊ���޴���
    return (nbr_tasks);                                //���ر���ֹ��������Ŀ
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   POST TO A MUTEX
*
* Description: This function signals a mutex
*
* Arguments  : p_mutex  is a pointer to the mutex
*
*              opt      is an option you can specify to alter the behavior of the post.  The choices are:
*
*                           OS_OPT_POST_NONE        No special option selected
*                           OS_OPT_POST_NO_SCHED    If you don't want the scheduler to be called after the post.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE             The call was successful and the mutex was signaled.
*                           OS_ERR_MUTEX_NESTING    Mutex owner nested its use of the mutex
*                           OS_ERR_MUTEX_NOT_OWNER  If the task posting is not the Mutex owner
*                           OS_ERR_OBJ_PTR_NULL     If 'p_mutex' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE         If 'p_mutex' is not pointing at a mutex
*                           OS_ERR_POST_ISR         If you attempted to post from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexPost (OS_MUTEX  *p_mutex, //�����ź���ָ��
                   OS_OPT     opt,     //ѡ��
                   OS_ERR    *p_err)   //���ش�������
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                    //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                    //�����ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_POST_ISR;               //��������Ϊ�����ж��еȴ���
        return;                                //���أ�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                 //���ʹ���˲������
    if (p_mutex == (OS_MUTEX *)0) {        //��� p_mutex Ϊ��            
       *p_err = OS_ERR_OBJ_PTR_NULL;       //��������Ϊ���ں˶���Ϊ�ա�
        return;                            //���أ�������ִ��
    }
    switch (opt) {                         //����ѡ����ദ��  
        case OS_OPT_POST_NONE:             //���ѡ����Ԥ���ڣ�������
        case OS_OPT_POST_NO_SCHED:
             break;

        default:                           //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;  //��������Ϊ��ѡ��Ƿ���
             return;                       //���أ�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //���ʹ���˶������ͼ��   
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //��� p_mutex �����Ͳ��ǻ����ź������� 
       *p_err = OS_ERR_OBJ_TYPE;              //���أ�������ִ��
        return;
    }
#endif

    CPU_CRITICAL_ENTER();                      //���ж�
    if (OSTCBCurPtr != p_mutex->OwnerTCBPtr) { //�����ǰ�������񲻳��и��ź���
        CPU_CRITICAL_EXIT();                   //���ж�
       *p_err = OS_ERR_MUTEX_NOT_OWNER;        //��������Ϊ�����񲻳��и��ź�����
        return;                                //���أ�������ִ��
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                       //�������������ж�
    ts          = OS_TS_GET();                          //��ȡʱ���
    p_mutex->TS = ts;                                   //�洢�ź������һ�α��ͷŵ�ʱ���
    p_mutex->OwnerNestingCtr--;                         //�ź�����Ƕ������1
    if (p_mutex->OwnerNestingCtr > (OS_NESTING_CTR)0) { //����ź����Ա�Ƕ��
        OS_CRITICAL_EXIT();                             //����������
       *p_err = OS_ERR_MUTEX_NESTING;                   //��������Ϊ���ź�����Ƕ�ס�
        return;                                         //���أ�������ִ��
    }
    /* ����ź���δ��Ƕ�ף��ѿ��� */
    p_pend_list = &p_mutex->PendList;                //��ȡ�ź����ĵȴ��б�
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {  //���û�������ڵȴ����ź���
        p_mutex->OwnerTCBPtr     = (OS_TCB       *)0;//����ź�����������Ϣ
        p_mutex->OwnerNestingCtr = (OS_NESTING_CTR)0;
        OS_CRITICAL_EXIT();                          //����������
       *p_err = OS_ERR_NONE;                         //��������Ϊ���޴���
        return;                                      //���أ�������ִ��
    }
    /* ����������ڵȴ����ź��� */
    if (OSTCBCurPtr->Prio != p_mutex->OwnerOriginalPrio) { //�����ǰ��������ȼ����Ĺ�
        OS_RdyListRemove(OSTCBCurPtr);                     //�Ӿ����б��Ƴ���ǰ����
        OSTCBCurPtr->Prio = p_mutex->OwnerOriginalPrio;    //��ԭ��ǰ��������ȼ�
        OS_PrioInsert(OSTCBCurPtr->Prio);                  //�����ȼ�������������ȼ�
        OS_RdyListInsertTail(OSTCBCurPtr);                 //����ǰ�����������б�β��
        OSPrioCur         = OSTCBCurPtr->Prio;             //���ĵ�ǰ�������ȼ�������ֵ
    }

    p_tcb                      = p_pend_list->HeadPtr->TCBPtr; //��ȡ�ȴ��б���׶�����
    p_mutex->OwnerTCBPtr       = p_tcb;                        //���ź�������������
    p_mutex->OwnerOriginalPrio = p_tcb->Prio;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)1;            //��ʼǶ��
    /* �ͷ��ź����������� */
    OS_Post((OS_PEND_OBJ *)((void *)p_mutex), 
            (OS_TCB      *)p_tcb,
            (void        *)0,
            (OS_MSG_SIZE  )0,
            (CPU_TS       )ts);

    OS_CRITICAL_EXIT_NO_SCHED();                     //����������������ִ���������

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) { //��� opt ûѡ�񡰷���ʱ����������
        OSSched();                                   //�������
    }

   *p_err = OS_ERR_NONE;                             //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                            CLEAR THE CONTENTS OF A MUTEX
*
* Description: This function is called by OSMutexDel() to clear the contents of a mutex
*

* Argument(s): p_mutex      is a pointer to the mutex to clear
*              -------
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_MutexClr (OS_MUTEX  *p_mutex)
{
    p_mutex->Type              =  OS_OBJ_TYPE_NONE;         /* Mark the data structure as a NONE                      */
    p_mutex->NamePtr           = (CPU_CHAR     *)((void *)"?MUTEX");
    p_mutex->OwnerTCBPtr       = (OS_TCB       *)0;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)0;
    p_mutex->TS                = (CPU_TS        )0;
    p_mutex->OwnerOriginalPrio =  OS_CFG_PRIO_MAX;
    OS_PendListInit(&p_mutex->PendList);                    /* Initialize the waiting list                            */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                          ADD/REMOVE MUTEX TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a mutex to/from the debug list.
*
* Arguments  : p_mutex     is a pointer to the mutex to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u                        //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ���  
void  OS_MutexDbgListAdd (OS_MUTEX  *p_mutex) //���û����ź������뵽�����ź����б����ǰ��
{
    p_mutex->DbgNamePtr               = (CPU_CHAR *)((void *)" "); //�Ȳ�ָ���κ����������
    p_mutex->DbgPrevPtr               = (OS_MUTEX *)0;             //�����ź�����Ϊ�б����ǰ��
    if (OSMutexDbgListPtr == (OS_MUTEX *)0) {                      //����б�Ϊ��
        p_mutex->DbgNextPtr           = (OS_MUTEX *)0;             //���ź�������һ��Ԫ��ҲΪ��
    } else {                                                       //����б�ǿ�
        p_mutex->DbgNextPtr           =  OSMutexDbgListPtr;        //�б�ԭ������Ԫ����Ϊ���ź�������һ��Ԫ��
        OSMutexDbgListPtr->DbgPrevPtr =  p_mutex;                  //ԭ������Ԫ�ص�ǰ���Ϊ�˸��ź���
    }
    OSMutexDbgListPtr                 =  p_mutex;                  //���ź�����Ϊ�б������Ԫ��
}



void  OS_MutexDbgListRemove (OS_MUTEX  *p_mutex)
{
    OS_MUTEX  *p_mutex_next;
    OS_MUTEX  *p_mutex_prev;


    p_mutex_prev = p_mutex->DbgPrevPtr;
    p_mutex_next = p_mutex->DbgNextPtr;

    if (p_mutex_prev == (OS_MUTEX *)0) {
        OSMutexDbgListPtr = p_mutex_next;
        if (p_mutex_next != (OS_MUTEX *)0) {
            p_mutex_next->DbgPrevPtr = (OS_MUTEX *)0;
        }
        p_mutex->DbgNextPtr = (OS_MUTEX *)0;

    } else if (p_mutex_next == (OS_MUTEX *)0) {
        p_mutex_prev->DbgNextPtr = (OS_MUTEX *)0;
        p_mutex->DbgPrevPtr      = (OS_MUTEX *)0;

    } else {
        p_mutex_prev->DbgNextPtr =  p_mutex_next;
        p_mutex_next->DbgPrevPtr =  p_mutex_prev;
        p_mutex->DbgNextPtr      = (OS_MUTEX *)0;
        p_mutex->DbgPrevPtr      = (OS_MUTEX *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                MUTEX INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the mutex management.
*

* Argument(s): p_err        is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_MutexInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSMutexDbgListPtr = (OS_MUTEX *)0;
#endif

    OSMutexQty        = (OS_OBJ_QTY)0;
   *p_err             =  OS_ERR_NONE;
}

#endif                                                      /* OS_CFG_MUTEX_EN                                        */
