/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                 SEMAPHORE MANAGEMENT
*
* File    : OS_SEM.C
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
const  CPU_CHAR  *os_sem__c = "$Id: $";
#endif


#if OS_CFG_SEM_EN > 0u
/*
************************************************************************************************************************
*                                                  CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to initialize.  Your application is responsible for
*                            allocating storage for the semaphore.
*
*              p_name        is a pointer to the name you would like to give the semaphore.
*
*              cnt           is the initial value for the semaphore.
*                            If used to share resources, you should initialize to the number of resources available.
*                            If used to signal the occurrence of event(s) then you should initialize to 0.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the semaphore after you
*                                                                 called OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the semaphore has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_sem'  is a NULL pointer
*                                OS_ERR_OBJ_TYPE                if 'p_sem' has already been initialized to a different
*                                                               object type
*
* Returns    : none
************************************************************************************************************************
*/

void  OSSemCreate (OS_SEM      *p_sem,  //��ֵ�ź������ƿ�ָ��
                   CPU_CHAR    *p_name, //��ֵ�ź�������
                   OS_SEM_CTR   cnt,    //��Դ��Ŀ���¼��Ƿ�����־
                   OS_ERR      *p_err)  //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                    //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                    //�����ж�ʱ����ֵ��ԭ�� 
	
#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�������ִ��
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ�ؼ�
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {  //������ڵ��� OSSafetyCriticalStart() �󴴽��ö�ֵ�ź���
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;   //��������Ϊ���Ƿ������ں˶���
        return;                                   //���أ�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u            //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {    //����ú��������ж��б�����
       *p_err = OS_ERR_CREATE_ISR;                //��������Ϊ�����жϺ����ж�ʱ��
        return;                                   //���أ�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_sem == (OS_SEM *)0) {       //������� p_sem Ϊ��  
       *p_err = OS_ERR_OBJ_PTR_NULL;  //��������Ϊ����ֵ�ź�������Ϊ�ա�
        return;                       //���أ�������ִ��
    }
#endif

    OS_CRITICAL_ENTER();               //�����ٽ��
    p_sem->Type    = OS_OBJ_TYPE_SEM;  //��ʼ����ֵ�ź���ָ��  
    p_sem->Ctr     = cnt;                                 
    p_sem->TS      = (CPU_TS)0;
    p_sem->NamePtr = p_name;                               
    OS_PendListInit(&p_sem->PendList); //��ʼ���ö�ֵ�ź����ĵȴ��б�     

#if OS_CFG_DBG_EN > 0u       //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ��� 
    OS_SemDbgListAdd(p_sem); //�����ź�����ӵ���ֵ�ź���˫���������
#endif
    OSSemQty++;              //��ֵ�ź���������1

    OS_CRITICAL_EXIT_NO_SCHED();     //�˳��ٽ�Σ��޵��ȣ�
   *p_err = OS_ERR_NONE;             //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete semaphore ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the semaphore even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the semaphore was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the semaphore from an ISR
*                                OS_ERR_OBJ_PTR_NULL         If 'p_sem' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_sem' is not pointing at a semaphore
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                                OS_ERR_TASK_WAITING         One or more tasks were waiting on the semaphore
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the semaphore
*                 MUST check the return code of OSSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted.
*              3) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in applications where
*                 the semaphore is used for mutual exclusion because the resource(s) will no longer be guarded by the
*                 semaphore.
************************************************************************************************************************
*/

#if OS_CFG_SEM_DEL_EN > 0u             //���ʹ���� OSSemDel() ���� 
OS_OBJ_QTY  OSSemDel (OS_SEM  *p_sem,  //��ֵ�ź���ָ��
                      OS_OPT   opt,    //ѡ��
                      OS_ERR  *p_err)  //���ش�������
{
    OS_OBJ_QTY     cnt;
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                      //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {                //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();        //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0);                //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_DEL_ISR;                //���ش�������Ϊ�����ж���ɾ����
        return ((OS_OBJ_QTY)0);                //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                    //���ʹ���˲������
    if (p_sem == (OS_SEM *)0) {               //��� p_sem Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;          //���ش�������Ϊ���ں˶���Ϊ�ա�
        return ((OS_OBJ_QTY)0);               //����0���д��󣩣�������ִ��
    }
    switch (opt) {                            //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:              //���ѡ����Ԥ��֮��
        case OS_OPT_DEL_ALWAYS:
             break;                           //ֱ������

        default:                              //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;      //���ش�������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0);          //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u              //���ʹ���˶������ͼ��
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {    //��� p_sem ���Ƕ�ֵ�ź�������
       *p_err = OS_ERR_OBJ_TYPE;             //���ش�������Ϊ���ں˶������ʹ���
        return ((OS_OBJ_QTY)0);              //����0���д��󣩣�������ִ��
    }
#endif

    CPU_CRITICAL_ENTER();                      //���ж�
    p_pend_list = &p_sem->PendList;            //��ȡ�ź����ĵȴ��б� p_pend_list
    cnt         = p_pend_list->NbrEntries;     //��ȡ�ȴ����ź�����������
    nbr_tasks   = cnt;
    switch (opt) {                             //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:               //���ֻ��û������ȴ��������ɾ���ź���
             if (nbr_tasks == (OS_OBJ_QTY)0) { //���û�������ڵȴ����ź���
#if OS_CFG_DBG_EN > 0u                         //���ʹ���˵��Դ���ͱ���   
                 OS_SemDbgListRemove(p_sem);   //�����ź������ź��������б��Ƴ�
#endif
                 OSSemQty--;                   //�ź�����Ŀ��1
                 OS_SemClr(p_sem);             //����ź�������
                 CPU_CRITICAL_EXIT();          //���ж�
                *p_err = OS_ERR_NONE;          //���ش�������Ϊ���޴���
             } else {                          //����������ڵȴ����ź���
                 CPU_CRITICAL_EXIT();          //���ж�
                *p_err = OS_ERR_TASK_WAITING;  //���ش�������Ϊ���������ڵȴ����ź�����
             }
             break;

        case OS_OPT_DEL_ALWAYS:                             //�������ɾ���ź���
             OS_CRITICAL_ENTER_CPU_EXIT();                  //���������������ж�
             ts = OS_TS_GET();                              //��ȡʱ���
             while (cnt > 0u) {                             //����Ƴ����ź����ȴ��б��е�����
                 p_pend_data = p_pend_list->HeadPtr;
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_sem),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                                      //���ʹ���˵��Դ���ͱ��� 
             OS_SemDbgListRemove(p_sem);                    //�����ź������ź��������б��Ƴ�
#endif
             OSSemQty--;                                    //�ź�����Ŀ��1
             OS_SemClr(p_sem);                              //����ź�������
             OS_CRITICAL_EXIT_NO_SCHED();                   //�������������������е���
             OSSched();                                     //������ȣ�ִ��������ȼ��ľ�������
            *p_err = OS_ERR_NONE;                           //���ش�������Ϊ���޴���
             break;

        default:                                            //���ѡ���Ԥ��
             CPU_CRITICAL_EXIT();                           //���ж�
            *p_err = OS_ERR_OPT_INVALID;                    //���ش�������Ϊ��ѡ��Ƿ���
             break;
    }
    return ((OS_OBJ_QTY)nbr_tasks);                         //����ɾ���ź���ǰ�ȴ����������
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for the
*                            resource up to the amount of time (in 'ticks') specified by this argument.  If you specify
*                            0, however, your task will wait forever at the specified semaphore or, until the resource
*                            becomes available (or the event occurs).
*
*              opt           determines whether the user wants to block if the semaphore is not available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the semaphore was posted
*                            or pend aborted or the semaphore deleted.  If you pass a NULL pointer (i.e. (CPU_TS*)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid
*                            and indicates that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task owns the resource
*                                                          or, the event you are waiting for occurred.
*                                OS_ERR_OBJ_DEL            If 'p_sem' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_sem' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_sem' is not pointing at a semaphore
*                                OS_ERR_OPT_INVALID        If you specified an invalid value for 'opt'
*                                OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the semaphore was not
*                                                          available.
*                                OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                                OS_ERR_STATUS_INVALID     Pend status is invalid
*                                OS_ERR_TIMEOUT            The semaphore was not received within the specified
*                                                          timeout.
*
*
* Returns    : The current value of the semaphore counter or 0 if not available.
************************************************************************************************************************
*/

OS_SEM_CTR  OSSemPend (OS_SEM   *p_sem,   //��ֵ�ź���ָ��
                       OS_TICK   timeout, //�ȴ���ʱʱ��
                       OS_OPT    opt,     //ѡ��
                       CPU_TS   *p_ts,    //�ȵ��ź���ʱ��ʱ���
                       OS_ERR   *p_err)   //���ش�������
{
    OS_SEM_CTR    ctr;
    OS_PEND_DATA  pend_data;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                       //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {                 //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();         //ִ�а�ȫ����쳣����
        return ((OS_SEM_CTR)0);                 //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ISR;                //���ش�������Ϊ�����ж��еȴ���
        return ((OS_SEM_CTR)0);                 //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                     //���ʹ���˲������
    if (p_sem == (OS_SEM *)0) {                //��� p_sem Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;           //���ش�������Ϊ���ں˶���Ϊ�ա�
        return ((OS_SEM_CTR)0);                //����0���д��󣩣�������ִ��
    }
    switch (opt) {                             //����ѡ����ദ��
        case OS_OPT_PEND_BLOCKING:             //���ѡ�񡰵ȴ�����������ж�����
        case OS_OPT_PEND_NON_BLOCKING:         //���ѡ�񡰵ȴ��������󲻽��ж�����
             break;                            //ֱ��������������

        default:                               //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;       //���ش�������Ϊ��ѡ��Ƿ���
             return ((OS_SEM_CTR)0);           //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //���ʹ���˶������ͼ��
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {     //��� p_sem ���Ƕ�ֵ�ź�������
       *p_err = OS_ERR_OBJ_TYPE;              //���ش�������Ϊ���ں˶������ʹ���
        return ((OS_SEM_CTR)0);               //����0���д��󣩣�������ִ��
    }
#endif

    if (p_ts != (CPU_TS *)0) {                //��� p_ts �ǿ�
       *p_ts  = (CPU_TS)0;                    //��ʼ�������㣩p_ts�������ڷ���ʱ���
    }
    CPU_CRITICAL_ENTER();                     //���ж�
    if (p_sem->Ctr > (OS_SEM_CTR)0) {         //�����Դ����
        p_sem->Ctr--;                         //��Դ��Ŀ��1
        if (p_ts != (CPU_TS *)0) {            //��� p_ts �ǿ�
           *p_ts  = p_sem->TS;                //��ȡ���ź������һ�η�����ʱ���
        }
        ctr   = p_sem->Ctr;                   //��ȡ�ź����ĵ�ǰ��Դ��Ŀ
        CPU_CRITICAL_EXIT();                  //���ж�
       *p_err = OS_ERR_NONE;                  //���ش�������Ϊ���޴���
        return (ctr);                         //�����ź����ĵ�ǰ��Դ��Ŀ��������ִ��
    }

    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {    //���û����Դ���ã�����ѡ���˲���������
        ctr   = p_sem->Ctr;                                 //��ȡ�ź�������Դ��Ŀ�� ctr
        CPU_CRITICAL_EXIT();                                //���ж�
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                    //���ش�������Ϊ���ȴ����������  
        return (ctr);                                       //�����ź����ĵ�ǰ��Դ��Ŀ��������ִ��
    } else {                                                //���û����Դ���ã���ѡ���˶�������
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {    //�������������
            CPU_CRITICAL_EXIT();                            //���ж�
           *p_err = OS_ERR_SCHED_LOCKED;                    //���ش�������Ϊ��������������
            return ((OS_SEM_CTR)0);                         //����0���д��󣩣�������ִ��
        }
    }
                                                            
    OS_CRITICAL_ENTER_CPU_EXIT();                           //�������������ؿ��ж�
    OS_Pend(&pend_data,                                     //�����ȴ����񣬽���ǰ������������б�
            (OS_PEND_OBJ *)((void *)p_sem),                 //�����뵽�����б�͵ȴ��б�
            OS_TASK_PEND_ON_SEM,
            timeout);

    OS_CRITICAL_EXIT_NO_SCHED();                            //�����������������е���

    OSSched();                                              //�ҵ�������������ȼ���������
    /* ��ǰ���񣨻���ź��������Լ������� */
    CPU_CRITICAL_ENTER();                                   //���ж�
    switch (OSTCBCurPtr->PendStatus) {                      //���ݵ�ǰ��������ĵȴ�״̬���ദ��
        case OS_STATUS_PEND_OK:                             //����ȴ�״̬����
             if (p_ts != (CPU_TS *)0) {                     //��� p_ts �ǿ�
                *p_ts  =  OSTCBCurPtr->TS;                  //��ȡ�źű�������ʱ���
             }
            *p_err = OS_ERR_NONE;                           //���ش�������Ϊ���޴���
             break;

        case OS_STATUS_PEND_ABORT:                          //����ȴ�����ֹ��ֹ
             if (p_ts != (CPU_TS *)0) {                     //��� p_ts �ǿ�
                *p_ts  =  OSTCBCurPtr->TS;                  //��ȡ�ȴ�����ֹ��ʱ���
             }
            *p_err = OS_ERR_PEND_ABORT;                     //���ش�������Ϊ���ȴ�����ֹ��
             break;

        case OS_STATUS_PEND_TIMEOUT:                        //����ȴ���ʱ
             if (p_ts != (CPU_TS *)0) {                     //��� p_ts �ǿ�
                *p_ts  = (CPU_TS  )0;                       //���� p_ts
             }
            *p_err = OS_ERR_TIMEOUT;                        //���ش�������Ϊ���ȴ���ʱ��
             break;

        case OS_STATUS_PEND_DEL:                            //����ȴ����ں˶���ɾ��
             if (p_ts != (CPU_TS *)0) {                     //��� p_ts �ǿ�
                *p_ts  =  OSTCBCurPtr->TS;                  //��ȡ�ں˶���ɾ����ʱ���
             }
            *p_err = OS_ERR_OBJ_DEL;                        //���ش�������Ϊ���ȴ�����ɾ����
             break;

        default:                                            //����ȴ�״̬����Ԥ��
            *p_err = OS_ERR_STATUS_INVALID;                 //���ش�������Ϊ���ȴ�״̬�Ƿ���
             CPU_CRITICAL_EXIT();                           //���ж�
             return ((OS_SEM_CTR)0);                        //����0���д��󣩣�������ִ��
    }
    ctr = p_sem->Ctr;                                       //��ȡ�ź����ĵ�ǰ��Դ��Ŀ
    CPU_CRITICAL_EXIT();                                    //���ж�
    return (ctr);                                           //�����ź����ĵ�ǰ��Դ��Ŀ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                             ABORT WAITING ON A SEMAPHORE
*
* Description: This function aborts & readies any tasks currently waiting on a semaphore.  This function should be used
*              to fault-abort the wait on the semaphore, rather than to normally signal the semaphore via OSSemPost().
*
* Arguments  : p_sem     is a pointer to the semaphore
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the semaphore
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the semaphore
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the semaphore was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the semaphore was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_sem' is not pointing at a semaphore
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_SEM_PEND_ABORT_EN > 0u           //���ʹ���� OSSemPendAbort() ����
OS_OBJ_QTY  OSSemPendAbort (OS_SEM  *p_sem, //��ֵ�ź���ָ��
                            OS_OPT   opt,   //ѡ��
                            OS_ERR  *p_err) //���ش�������
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0u);        //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) { //����ú������ж��б�����
       *p_err =  OS_ERR_PEND_ABORT_ISR;         //���ش�������Ϊ�����ж�����ֹ�ȴ���
        return ((OS_OBJ_QTY)0u);                //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                      //���ʹ���˲������
    if (p_sem == (OS_SEM *)0) {                 //��� p_sem Ϊ��
       *p_err =  OS_ERR_OBJ_PTR_NULL;           //���ش�������Ϊ���ں˶���Ϊ�ա�
        return ((OS_OBJ_QTY)0u);                //����0���д��󣩣�������ִ��
    }
    switch (opt) {                                          //����ѡ����ദ��
        case OS_OPT_PEND_ABORT_1:                           //���ѡ����Ԥ����
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                                         //������ֱ������

        default:                                            //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;                   //���ش�������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0u);                       //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u             //���ʹ���˶������ͼ��
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {   //��� p_sem ���Ƕ�ֵ�ź�������
       *p_err =  OS_ERR_OBJ_TYPE;           //���ش�������Ϊ���ں˶������ʹ���
        return ((OS_OBJ_QTY)0u);            //����0���д��󣩣�������ִ��
    }
#endif

    CPU_CRITICAL_ENTER();                             //���ж�
    p_pend_list = &p_sem->PendList;                   //��ȡ p_sem �ĵȴ��б� p_pend_list
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) {  //���û�������ڵȴ� p_sem
        CPU_CRITICAL_EXIT();                          //���ж�
       *p_err =  OS_ERR_PEND_ABORT_NONE;              //���ش�������Ϊ��û�������ڵȴ���
        return ((OS_OBJ_QTY)0u);                      //����0���д��󣩣�������ִ��
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                      //�����������������ж�
    nbr_tasks = 0u;
    ts        = OS_TS_GET();                           //��ȡʱ���
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) { //����������ڵȴ� p_sem
        p_tcb = p_pend_list->HeadPtr->TCBPtr;          //��ȡ���ȼ���ߵĵȴ�����
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_sem),   //��ֹ������� p_sem �ĵȴ�
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {            //�������ѡ������ֹ���еȴ�����
            break;                                     //�������������ټ�����ֹ
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                       //��������������������

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) {  //���ѡ�����������
        OSSched();                                     //�����������
    }

   *p_err = OS_ERR_NONE;   //���ش�������Ϊ���޴���
    return (nbr_tasks);    //���ر���ֹ��������Ŀ
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : p_sem    is a pointer to the semaphore
*
*              opt      determines the type of POST performed:
*
*                           OS_OPT_POST_1            POST and ready only the highest priority task waiting on semaphore
*                                                    (if tasks are waiting).
*                           OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the semaphore
*
*                           OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                           Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE          The call was successful and the semaphore was signaled.
*                           OS_ERR_OBJ_PTR_NULL  If 'p_sem' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE      If 'p_sem' is not pointing at a semaphore
*                           OS_ERR_SEM_OVF       If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the semaphore counter or 0 upon error.
************************************************************************************************************************
*/

OS_SEM_CTR  OSSemPost (OS_SEM  *p_sem,    //��ֵ�ź������ƿ�ָ��
                       OS_OPT   opt,      //ѡ��
                       OS_ERR  *p_err)    //���ش�������
{
    OS_SEM_CTR  ctr;
    CPU_TS      ts;



#ifdef OS_SAFETY_CRITICAL                 //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {           //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();   //ִ�а�ȫ����쳣����
        return ((OS_SEM_CTR)0);           //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲�����⹦��   
    if (p_sem == (OS_SEM *)0) {           //��� p_sem Ϊ��
       *p_err  = OS_ERR_OBJ_PTR_NULL;     //���ش�������Ϊ���ں˶���ָ��Ϊ�ա�
        return ((OS_SEM_CTR)0);           //����0���д��󣩣�������ִ��
    }
    switch (opt) {                                   //����ѡ��������ദ��
        case OS_OPT_POST_1:                          //���ѡ����Ԥ���ڣ�������
        case OS_OPT_POST_ALL:
        case OS_OPT_POST_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:                                     //���ѡ���Ԥ��
            *p_err =  OS_ERR_OPT_INVALID;            //���ش�������Ϊ��ѡ��Ƿ���
             return ((OS_SEM_CTR)0u);                //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //���ʹ���˶������ͼ��           
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {  //��� p_sem �����Ͳ��Ƕ�ֵ�ź�������
       *p_err = OS_ERR_OBJ_TYPE;           //���ش�������Ϊ���������ʹ���
        return ((OS_SEM_CTR)0);            //����0���д��󣩣�������ִ��
    }
#endif

    ts = OS_TS_GET();                             //��ȡʱ���

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u              //���ʹ�����ж��ӳٷ���
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {    //����ú��������ж��б�����
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_SEM, //�����ź����������ж���Ϣ����
                    (void      *)p_sem,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return ((OS_SEM_CTR)0);                   //����0����δ��������������ִ��        
    }
#endif

    ctr = OS_SemPost(p_sem,                       //���ź���������ͨ��ʽ����
                     opt,
                     ts,
                     p_err);

    return (ctr);                                 //�����źŵĵ�ǰ����ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    SET SEMAPHORE
*
* Description: This function sets the semaphore count to the value specified as an argument.  Typically, this value
*              would be 0 but of course, we can set the semaphore to any value.
*
*              You would typically use this function when a semaphore is used as a signaling mechanism
*              and, you want to reset the count value.
*
* Arguments  : p_sem     is a pointer to the semaphore
*
*              cnt       is the new value for the semaphore count.  You would pass 0 to reset the semaphore count.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE           The call was successful and the semaphore value was set.
*                            OS_ERR_OBJ_PTR_NULL   If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE       If 'p_sem' is not pointing to a semaphore.
*                            OS_ERR_TASK_WAITING   If tasks are waiting on the semaphore.
*
* Returns    : None
************************************************************************************************************************
*/

#if OS_CFG_SEM_SET_EN > 0u           //���ʹ�� OSSemSet() ����
void  OSSemSet (OS_SEM      *p_sem,  //��ֵ�ź���ָ��
                OS_SEM_CTR   cnt,    //�ź�������ֵ
                OS_ERR      *p_err)  //���ش�������
{
    OS_PEND_LIST  *p_pend_list;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_SET_ISR;                 //���ش�������Ϊ�����ж������á�
        return;                                 //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //���ʹ���˲������
    if (p_sem == (OS_SEM *)0) {       //��� p_sem Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;  //���ش�������Ϊ���ں˶���Ϊ�ա�
        return;                       //����0���д��󣩣�������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //���ʹ���˶������ͼ��
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {  //��� p_sem ���Ƕ�ֵ�ź������� 
       *p_err = OS_ERR_OBJ_TYPE;           //���ش�������Ϊ���ں˶������ʹ���
        return;                            //����0���д��󣩣�������ִ��
    }
#endif

   *p_err = OS_ERR_NONE;                               //���ش�������Ϊ���޴���
    CPU_CRITICAL_ENTER();                              //���ж�
    if (p_sem->Ctr > (OS_SEM_CTR)0) {                  //����ź�������ֵ>0��˵��û
        p_sem->Ctr = cnt;                              //���ȴ�����ֱ�����ü���ֵ��
    } else {                                           //����ź�������ֵ=0
        p_pend_list = &p_sem->PendList;                //��ȡ�ź����ĵȴ��б�
        if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {//���û�����ڵȴ��ź���
            p_sem->Ctr = cnt;                          //����ֱ�������źż���ֵ
        } else {                                       //����������ڵȴ��ź���
           *p_err      = OS_ERR_TASK_WAITING;          //���ش�������Ϊ��������ȴ���
        }
    }
    CPU_CRITICAL_EXIT();
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                           CLEAR THE CONTENTS OF A SEMAPHORE
*
* Description: This function is called by OSSemDel() to clear the contents of a semaphore
*

* Argument(s): p_sem      is a pointer to the semaphore to clear
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_SemClr (OS_SEM  *p_sem)
{
    p_sem->Type    = OS_OBJ_TYPE_NONE;                      /* Mark the data structure as a NONE                      */
    p_sem->Ctr     = (OS_SEM_CTR)0;                         /* Set semaphore value                                    */
    p_sem->TS      = (CPU_TS    )0;                         /* Clear the time stamp                                   */
    p_sem->NamePtr = (CPU_CHAR *)((void *)"?SEM");
    OS_PendListInit(&p_sem->PendList);                      /* Initialize the waiting list                            */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        ADD/REMOVE SEMAPHORE TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a semaphore to/from the debug list.
*
* Arguments  : p_sem     is a pointer to the semaphore to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u                   //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ���      
void  OS_SemDbgListAdd (OS_SEM  *p_sem)  //���ö�ֵ�ź������뵽��ֵ�ź����б����ǰ��
{
    p_sem->DbgNamePtr               = (CPU_CHAR *)((void *)" "); //�Ȳ�ָ���κ����������
    p_sem->DbgPrevPtr               = (OS_SEM   *)0;             //�����ź�����Ϊ�б����ǰ��
    if (OSSemDbgListPtr == (OS_SEM *)0) {                        //����б�Ϊ��
        p_sem->DbgNextPtr           = (OS_SEM   *)0;             //���ź�������һ��Ԫ��ҲΪ��
    } else {                                                     //����б�ǿ�
        p_sem->DbgNextPtr           =  OSSemDbgListPtr;          //�б�ԭ������Ԫ����Ϊ���ź�������һ��Ԫ��
        OSSemDbgListPtr->DbgPrevPtr =  p_sem;                    //ԭ������Ԫ�ص�ǰ���Ϊ�˸��ź���
    }
    OSSemDbgListPtr                 =  p_sem;                    //���ź�����Ϊ�б������Ԫ��
}



void  OS_SemDbgListRemove (OS_SEM  *p_sem)
{
    OS_SEM  *p_sem_next;
    OS_SEM  *p_sem_prev;


    p_sem_prev = p_sem->DbgPrevPtr;
    p_sem_next = p_sem->DbgNextPtr;

    if (p_sem_prev == (OS_SEM *)0) {
        OSSemDbgListPtr = p_sem_next;
        if (p_sem_next != (OS_SEM *)0) {
            p_sem_next->DbgPrevPtr = (OS_SEM *)0;
        }
        p_sem->DbgNextPtr = (OS_SEM *)0;

    } else if (p_sem_next == (OS_SEM *)0) {
        p_sem_prev->DbgNextPtr = (OS_SEM *)0;
        p_sem->DbgPrevPtr      = (OS_SEM *)0;

    } else {
        p_sem_prev->DbgNextPtr =  p_sem_next;
        p_sem_next->DbgPrevPtr =  p_sem_prev;
        p_sem->DbgNextPtr      = (OS_SEM *)0;
        p_sem->DbgPrevPtr      = (OS_SEM *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                SEMAPHORE INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the semaphore management.
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

void  OS_SemInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSSemDbgListPtr = (OS_SEM *)0;
#endif

    OSSemQty        = (OS_OBJ_QTY)0;
   *p_err           = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : p_sem    is a pointer to the semaphore
*
*              opt      determines the type of POST performed:
*
*                           OS_OPT_POST_1            POST to a single waiting task
*                           OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the semaphore
*
*                           OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                           Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*              ts       is a timestamp indicating when the post occurred.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE          The call was successful and the semaphore was signaled.
*                           OS_ERR_OBJ_PTR_NULL  If 'p_sem' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE      If 'p_sem' is not pointing at a semaphore
*                           OS_ERR_SEM_OVF       If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the semaphore counter or 0 upon error.
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

OS_SEM_CTR  OS_SemPost (OS_SEM  *p_sem, //��ֵ�ź���ָ��
                        OS_OPT   opt,   //ѡ��
                        CPU_TS   ts,    //ʱ���
                        OS_ERR  *p_err) //���ش�������
{
    OS_OBJ_QTY     cnt;
    OS_SEM_CTR     ctr;
    OS_PEND_LIST  *p_pend_list;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_DATA  *p_pend_data_next;
    OS_TCB        *p_tcb;
    CPU_SR_ALLOC();



    CPU_CRITICAL_ENTER();                                   //���ж�
    p_pend_list = &p_sem->PendList;                         //ȡ�����ź����ĵȴ��б�
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {         //���û�������ڵȴ����ź���
        switch (sizeof(OS_SEM_CTR)) {                       //�ж��Ƿ񽫵��¸��ź�������ֵ�����
            case 1u:                                        //�����������жϣ����ش�������Ϊ
                 if (p_sem->Ctr == DEF_INT_08U_MAX_VAL) {   //������ֵ�����������0���д��󣩣�
                     CPU_CRITICAL_EXIT();                   //������ִ�С�
                    *p_err = OS_ERR_SEM_OVF;
                     return ((OS_SEM_CTR)0);
                 }
                 break;

            case 2u:
                 if (p_sem->Ctr == DEF_INT_16U_MAX_VAL) {
                     CPU_CRITICAL_EXIT();
                    *p_err = OS_ERR_SEM_OVF;
                     return ((OS_SEM_CTR)0);
                 }
                 break;

            case 4u:
                 if (p_sem->Ctr == DEF_INT_32U_MAX_VAL) {
                     CPU_CRITICAL_EXIT();
                    *p_err = OS_ERR_SEM_OVF;
                     return ((OS_SEM_CTR)0);
                 }
                 break;

            default:
                 break;
        }
        p_sem->Ctr++;                                       //�ź�������ֵ��������1
        ctr       = p_sem->Ctr;                             //��ȡ�ź�������ֵ�� ctr
        p_sem->TS = ts;                                     //����ʱ���
        CPU_CRITICAL_EXIT();                                //���ж�
       *p_err     = OS_ERR_NONE;                            //���ش�������Ϊ���޴���
        return (ctr);                                       //�����ź����ļ���ֵ��������ִ��
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                           //�����������������ж�
    if ((opt & OS_OPT_POST_ALL) != (OS_OPT)0) {             //���Ҫ���ź������������еȴ�����
        cnt = p_pend_list->NbrEntries;                      //��ȡ�ȴ�������Ŀ�� cnt
    } else {                                                //���Ҫ���ź������������ȼ���ߵĵȴ�����
        cnt = (OS_OBJ_QTY)1;                                //��Ҫ������������Ϊ1��cnt ��1
    }
    p_pend_data = p_pend_list->HeadPtr;                     //��ȡ�ȴ��б���׸����� p_pend_data
    while (cnt > 0u) {                                      //�������Ҫ����������
        p_tcb            = p_pend_data->TCBPtr;             //ȡ����ǰ����
        p_pend_data_next = p_pend_data->NextPtr;            //ȡ����һ������
        OS_Post((OS_PEND_OBJ *)((void *)p_sem),             //�����ź�������ǰ����
                p_tcb,
                (void      *)0,
                (OS_MSG_SIZE)0,
                ts);
        p_pend_data = p_pend_data_next;                     //������һ������          
        cnt--;
    }
    ctr = p_sem->Ctr;                                       //��ȡ�ź�������ֵ�� ctr
    OS_CRITICAL_EXIT_NO_SCHED();                            //����������������ִ���������
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {        //��� opt ûѡ�񡰷���ʱ����������
        OSSched();                                          //�������
    }
   *p_err = OS_ERR_NONE;                                    //���ش�������Ϊ���޴���
    return (ctr);                                           //�����ź����ĵ�ǰ����ֵ
}

#endif
