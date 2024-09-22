/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                EVENT FLAG MANAGEMENT
*
* File    : OS_FLAG.C
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
const  CPU_CHAR  *os_flag__c = "$Id: $";
#endif


#if OS_CFG_FLAG_EN > 0u

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : p_grp          is a pointer to the event flag group to create
*
*              p_name         is the name of the event flag group
*
*              flags          contains the initial value to store in the event flag group (typically 0).
*
*              p_err          is a pointer to an error code which will be returned to your application:
*
*                                 OS_ERR_NONE                    if the call was successful.
*                                 OS_ERR_CREATE_ISR              if you attempted to create an Event Flag from an ISR.
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Event Flag after you
*                                                                   called OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_OBJ_CREATED             if the event flag group has already been created
*                                 OS_ERR_OBJ_PTR_NULL            if 'p_grp' is a NULL pointer
*
* Returns    : none
************************************************************************************************************************
*/

void  OSFlagCreate (OS_FLAG_GRP  *p_grp,  //�¼���־��ָ��
                    CPU_CHAR     *p_name, //�����¼���־��
                    OS_FLAGS      flags,  //��־��ʼֵ
                    OS_ERR       *p_err)  //���ش�������
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

#if OS_CFG_ARG_CHK_EN > 0u           //���ʹ���˲������
    if (p_grp == (OS_FLAG_GRP *)0) { //��� p_grp Ϊ��                                      
       *p_err = OS_ERR_OBJ_PTR_NULL; //��������Ϊ����������Ϊ�ա�
        return;                      //���أ�ִֹͣ��
    }
#endif

    OS_CRITICAL_ENTER();               //�����ٽ��
    p_grp->Type    = OS_OBJ_TYPE_FLAG; //��Ǵ����������ݽṹΪ�¼���־��
    p_grp->NamePtr = p_name;           //����¼���־�������
    p_grp->Flags   = flags;            //���ñ�־��ʼֵ
    p_grp->TS      = (CPU_TS)0;        //�����¼���־���ʱ���
    OS_PendListInit(&p_grp->PendList); //��ʼ�����¼���־��ĵȴ��б�  

#if OS_CFG_DBG_EN > 0u                 //���ʹ���˵��Դ���ͱ���
    OS_FlagDbgListAdd(p_grp);          //���ñ�־����ӵ��¼���־��˫���������
#endif
    OSFlagQty++;                       //�¼���־�������1

    OS_CRITICAL_EXIT_NO_SCHED();       //�˳��ٽ�Σ��޵��ȣ�
   *p_err = OS_ERR_NONE;               //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                             DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag group.
*
* Arguments  : p_grp     is a pointer to the desired event flag group.
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND           Deletes the event flag group ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS            Deletes the event flag group even if tasks are waiting.
*                                                         In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE                  The call was successful and the event flag group was deleted
*                            OS_ERR_DEL_ISR               If you attempted to delete the event flag group from an ISR
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If you didn't pass a pointer to an event flag group
*                            OS_ERR_OPT_INVALID           An invalid option was specified
*                            OS_ERR_TASK_WAITING          One or more tasks were waiting on the event flag group.
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the event flag
*                 group MUST check the return code of OSFlagPost and OSFlagPend().
************************************************************************************************************************
*/

#if OS_CFG_FLAG_DEL_EN > 0u                 //���ʹ���� OSFlagDel() ����
OS_OBJ_QTY  OSFlagDel (OS_FLAG_GRP  *p_grp, //�¼���־��ָ��
                       OS_OPT        opt,   //ѡ��
                       OS_ERR       *p_err) //���ش�������
{
    OS_OBJ_QTY        cnt;
    OS_OBJ_QTY        nbr_tasks;
    OS_PEND_DATA     *p_pend_data;
    OS_PEND_LIST     *p_pend_list;
    OS_TCB           *p_tcb;
    CPU_TS            ts;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0);         //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú������ж��б�����
       *p_err = OS_ERR_DEL_ISR;                //��������Ϊ�����ж���ɾ������
        return ((OS_OBJ_QTY)0);                //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //���ʹ���˲������
    if (p_grp == (OS_FLAG_GRP *)0) {      //��� p_grp Ϊ�� 
       *p_err  = OS_ERR_OBJ_PTR_NULL;     //��������Ϊ������Ϊ�ա�
        return ((OS_OBJ_QTY)0);           //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                        //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:          //���ѡ����Ԥ����
        case OS_OPT_DEL_ALWAYS:
             break;                       //ֱ������

        default:                          //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;  //��������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0);      //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u           //���ʹ���˶������ͼ��
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {//��� p_grp �����¼���־������
       *p_err = OS_ERR_OBJ_TYPE;          //��������Ϊ��������������
        return ((OS_OBJ_QTY)0);           //����0���д��󣩣�ִֹͣ��
    }
#endif
    OS_CRITICAL_ENTER();                         //�����ٽ��
    p_pend_list = &p_grp->PendList;              //��ȡ��Ϣ���еĵȴ��б�
    cnt         = p_pend_list->NbrEntries;       //��ȡ�ȴ��ö��е�������
    nbr_tasks   = cnt;                           //����������Ŀ�������
    switch (opt) {                               //����ѡ����ദ��
        case OS_OPT_DEL_NO_PEND:                 //���ֻ��û����ȴ�ʱ����ɾ��
             if (nbr_tasks == (OS_OBJ_QTY)0) {   //���û�������ڵȴ��ñ�־��
#if OS_CFG_DBG_EN > 0u                           //���ʹ���˵��Դ���ͱ���
                 OS_FlagDbgListRemove(p_grp);    //���ñ�־��ӱ�־������б��Ƴ�
#endif
                 OSFlagQty--;                    //��־����Ŀ��1
                 OS_FlagClr(p_grp);              //����ñ�־�������

                 OS_CRITICAL_EXIT();             //�˳��ٽ��
                *p_err = OS_ERR_NONE;            //��������Ϊ���޴���
             } else {
                 OS_CRITICAL_EXIT();             //�˳��ٽ��
                *p_err = OS_ERR_TASK_WAITING;    //��������Ϊ���������ڵȴ���־�顱
             }
             break;                              //����

        case OS_OPT_DEL_ALWAYS:                  //�������ɾ����־��
             ts = OS_TS_GET();                   //��ȡʱ���
             while (cnt > 0u) {                  //����Ƴ��ñ�־��ȴ��б��е�����
                 p_pend_data = p_pend_list->HeadPtr;
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_grp),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                           //���ʹ���˵��Դ���ͱ��� 
             OS_FlagDbgListRemove(p_grp);        //���ñ�־��ӱ�־������б��Ƴ�
#endif
             OSFlagQty--;                        //��־����Ŀ��1
             OS_FlagClr(p_grp);                  //����ñ�־�������
             OS_CRITICAL_EXIT_NO_SCHED();        //�˳��ٽ�Σ��޵��ȣ�
             OSSched();                          //��������
            *p_err = OS_ERR_NONE;                //��������Ϊ���޴���
             break;                              //����

        default:                                 //���ѡ���Ԥ��
             OS_CRITICAL_EXIT();                 //�˳��ٽ��
            *p_err = OS_ERR_OPT_INVALID;         //��������Ϊ��ѡ��Ƿ���
             break;                              //����
    }
    return (nbr_tasks);                          //����ɾ����־��ǰ�ȴ����������
}
#endif

/*
************************************************************************************************************************
*                                             WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag group.  Your
*              application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
*                            The bits you want are specified by setting the corresponding bits in 'flags'.
*                            e.g. if your application wants to wait for bits 0 and 1 then 'flags' would contain 0x03.
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              opt           specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the 'ONE' of the following arguments:
*
*                                OS_OPT_PEND_FLAG_CLR_ALL   You will wait for ALL bits in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_CLR_ANY   You will wait for ANY bit  in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_SET_ALL   You will wait for ALL bits in 'flags' to be set   (1)
*                                OS_OPT_PEND_FLAG_SET_ANY   You will wait for ANY bit  in 'flags' to be set   (1)
*
*                            You can 'ADD' OS_OPT_PEND_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                      the call.  Example, to wait for any flag in a group AND then clear
*                                      the flags that are present, set 'wait_opt' to:
*
*                                      OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
*
*                            You can also 'ADD' the type of pend with 'ONE' of the two option:
*
*                                OS_OPT_PEND_NON_BLOCKING   Task will NOT block if flags are not available
*                                OS_OPT_PEND_BLOCKING       Task will     block if flags are not available
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the event flag group was
*                            posted, aborted or the event flag group deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid and
*                            indicates that you don't need the timestamp.
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The desired bits have been set within the specified 'timeout'
*                                OS_ERR_OBJ_PTR_NULL        If 'p_grp' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You didn't specify a proper 'opt' argument.
*                                OS_ERR_PEND_ABORT          The wait on the flag was aborted.
*                                OS_ERR_PEND_ISR            If you tried to PEND from an ISR
*                                OS_ERR_PEND_WOULD_BLOCK    If you specified non-blocking but the flags were not
*                                                           available.
*                                OS_ERR_SCHED_LOCKED        If you called this function when the scheduler is locked
*                                OS_ERR_TIMEOUT             The bit(s) have not been set in the specified 'timeout'.
*
* Returns    : The flags in the event flag group that made the task ready or, 0 if a timeout or an error
*              occurred.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP  *p_grp,   //�¼���־��ָ��
                      OS_FLAGS      flags,   //ѡ��Ҫ�����ı�־λ
                      OS_TICK       timeout, //�ȴ����ޣ���λ��ʱ�ӽ��ģ�
                      OS_OPT        opt,     //ѡ��
                      CPU_TS       *p_ts,    //���صȵ��¼���־ʱ��ʱ���
                      OS_ERR       *p_err)   //���ش�������
{
    CPU_BOOLEAN   consume;
    OS_FLAGS      flags_rdy;
    OS_OPT        mode;
    OS_PEND_DATA  pend_data;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_FLAGS)0);           //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú������ж��б�����
       *p_err = OS_ERR_PEND_ISR;                //��������Ϊ�����ж�����ֹ�ȴ���
        return ((OS_FLAGS)0);                   //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u               //���ʹ���˲������
    if (p_grp == (OS_FLAG_GRP *)0) {     //��� p_grp Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;     //��������Ϊ������Ϊ�ա�
        return ((OS_FLAGS)0);            //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                       //����ѡ����ദ��
        case OS_OPT_PEND_FLAG_CLR_ALL:   //���ѡ����Ԥ����
        case OS_OPT_PEND_FLAG_CLR_ANY:
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
             break;                     //ֱ������

        default:                        //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;//��������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0);    //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //���ʹ���˶������ͼ��
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) { //��� p_grp �����¼���־������ 
       *p_err = OS_ERR_OBJ_TYPE;           //��������Ϊ��������������
        return ((OS_FLAGS)0);              //����0���д��󣩣�ִֹͣ��
    }
#endif

    if ((opt & OS_OPT_PEND_FLAG_CONSUME) != (OS_OPT)0) { //ѡ���˱�־λƥ����Զ�ȡ��
        consume = DEF_TRUE;
    } else {                                             //δѡ���־λƥ����Զ�ȡ��
        consume = DEF_FALSE;
    }

    if (p_ts != (CPU_TS *)0) {      //��� p_ts �ǿ�
       *p_ts = (CPU_TS)0;           //��ʼ�������㣩p_ts�������ڷ���ʱ���
    }

    mode = opt & OS_OPT_PEND_FLAG_MASK;                    //��ѡ������ȡ�Ա�־λ��Ҫ��
    CPU_CRITICAL_ENTER();                                  //���ж�
    switch (mode) {                                        //�����¼�����ģʽ���ദ��
        case OS_OPT_PEND_FLAG_SET_ALL:                     //���Ҫ�����б�־λ��Ҫ��1
             flags_rdy = (OS_FLAGS)(p_grp->Flags & flags); //��ȡ��Ҫ�ı�־λ��ֵ
             if (flags_rdy == flags) {                     //�����ֵ������ֵƥ��
                 if (consume == DEF_TRUE) {                //���Ҫ�󽫱�־λƥ���ȡ��
                     p_grp->Flags &= ~flags_rdy;           //��0��־�����ر�־λ
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;        //��������������ȴ��ı�־ֵ
                 if (p_ts != (CPU_TS *)0) {                //��� p_ts �ǿ�
                    *p_ts  = p_grp->TS;                    //��ȡ����ȵ���־��ʱ��ʱ���
                 }
                 CPU_CRITICAL_EXIT();                      //���ж�        
                *p_err = OS_ERR_NONE;                      //��������Ϊ���޴���
                 return (flags_rdy);                       //��������������ȴ��ı�־ֵ
             } else {                                      //�����Ҫ��־λ��ֵ������ֵ��ƥ��                  
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //���ѡ���˲���������
                     CPU_CRITICAL_EXIT();                  //���ж�
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;      //��������Ϊ�����������  
                     return ((OS_FLAGS)0);                 //����0���д��󣩣�ִֹͣ��
                 } else {                                  //���ѡ���˶�������
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //�������������
                         CPU_CRITICAL_EXIT();              //���ж�
                        *p_err = OS_ERR_SCHED_LOCKED;      //��������Ϊ��������������
                         return ((OS_FLAGS)0);             //����0���д��󣩣�ִֹͣ��
                     }
                 }
                 /* ���������δ���� */
                 OS_CRITICAL_ENTER_CPU_EXIT();             //�����ٽ�Σ��ؿ��ж�           
                 OS_FlagBlock(&pend_data,                  //������ǰ�������񣬵ȴ��¼���־��
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();              //�˳��ٽ�Σ��޵��ȣ�
             }
             break;                                        //����

        case OS_OPT_PEND_FLAG_SET_ANY:                     //���Ҫ���б�־λ����1����
             flags_rdy = (OS_FLAGS)(p_grp->Flags & flags); //��ȡ��Ҫ�ı�־λ��ֵ
             if (flags_rdy != (OS_FLAGS)0) {               //�����λ����1         
                 if (consume == DEF_TRUE) {                //���Ҫ�󽫱�־λƥ���ȡ��             
                     p_grp->Flags &= ~flags_rdy;           //��0ʪ���־�����ر�־λ          
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;        //��������������ȴ��ı�־ֵ
                 if (p_ts != (CPU_TS *)0) {                //��� p_ts �ǿ�
                    *p_ts  = p_grp->TS;                    //��ȡ����ȵ���־��ʱ��ʱ���
                 }
                 CPU_CRITICAL_EXIT();                      //���ж�                
                *p_err = OS_ERR_NONE;                      //��������Ϊ���޴���
                 return (flags_rdy);                       //��������������ȴ��ı�־ֵ
             } else {                                      //���û��λ����1                          
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //���û���ö�������
                     CPU_CRITICAL_EXIT();                  //���ж�
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;      //��������Ϊ�����������     
                     return ((OS_FLAGS)0);                 //����0���д��󣩣�ִֹͣ��
                 } else {                                  //��������˶�������          
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //�������������
                         CPU_CRITICAL_EXIT();              //���ж�
                        *p_err = OS_ERR_SCHED_LOCKED;      //��������Ϊ��������������
                         return ((OS_FLAGS)0);             //����0���д��󣩣�ִֹͣ��
                     }
                 }
                 /* ���������û���� */                                    
                 OS_CRITICAL_ENTER_CPU_EXIT();             //�����ٽ�Σ��ؿ��ж�             
                 OS_FlagBlock(&pend_data,                  //������ǰ�������񣬵ȴ��¼���־��
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();              //�˳��жϣ��޵��ȣ�
             }
             break;                                        //����

#if OS_CFG_FLAG_MODE_CLR_EN > 0u                           //���ʹ���˱�־λ��0����ģʽ
        case OS_OPT_PEND_FLAG_CLR_ALL:                     //���Ҫ�����б�־λ��Ҫ��0
             flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);//��ȡ��Ҫ�ı�־λ��ֵ
             if (flags_rdy == flags) {                     //�����ֵ������ֵƥ��
                 if (consume == DEF_TRUE) {                //���Ҫ�󽫱�־λƥ�����0
                     p_grp->Flags |= flags_rdy;            //��1��־�����ر�־λ
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;        //��������������ȴ��ı�־ֵ
                 if (p_ts != (CPU_TS *)0) {                //��� p_ts �ǿ�
                    *p_ts  = p_grp->TS;                    //��ȡ����ȵ���־��ʱ��ʱ���
                 }
                 CPU_CRITICAL_EXIT();                      //���ж�
                *p_err = OS_ERR_NONE;                      //��������Ϊ���޴���
                 return (flags_rdy);                       //����0���д��󣩣�ִֹͣ��
             } else {                                      //�����Ҫ��־λ��ֵ������ֵ��ƥ��
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //���ѡ���˲���������
                     CPU_CRITICAL_EXIT();                  //���ж�
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;      //��������Ϊ�����������
                     return ((OS_FLAGS)0);                 //����0���д��󣩣�ִֹͣ��
                 } else {                                  //���ѡ���˶�������
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //�������������
                         CPU_CRITICAL_EXIT();              //���ж�           
                        *p_err = OS_ERR_SCHED_LOCKED;      //��������Ϊ��������������
                         return ((OS_FLAGS)0);             //����0���д��󣩣�ִֹͣ��
                     }
                 }
                 /* ���������δ���� */                                          
                 OS_CRITICAL_ENTER_CPU_EXIT();             //�����ٽ�Σ��ؿ��ж�      
                 OS_FlagBlock(&pend_data,                  //������ǰ�������񣬵ȴ��¼���־��
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();             //�˳��ٽ�Σ��޵��ȣ�
             }
             break;                                       //����

        case OS_OPT_PEND_FLAG_CLR_ANY:                    //���Ҫ���б�־λ����0����
             flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);//��ȡ��Ҫ�ı�־λ��ֵ
             if (flags_rdy != (OS_FLAGS)0) {              //�����λ����0
                 if (consume == DEF_TRUE) {               //���Ҫ�󽫱�־λƥ���ȡ�� 
                     p_grp->Flags |= flags_rdy;           //��1ʪ���־�����ر�־λ  
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;       //��������������ȴ��ı�־ֵ 
                 if (p_ts != (CPU_TS *)0) {               //��� p_ts �ǿ�
                    *p_ts  = p_grp->TS;                   //��ȡ����ȵ���־��ʱ��ʱ���
                 }
                 CPU_CRITICAL_EXIT();                     //���ж� 
                *p_err = OS_ERR_NONE;                     //��������Ϊ���޴���
                 return (flags_rdy);                      //����0���д��󣩣�ִֹͣ��
             } else {                                     //���û��λ����0
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //���û���ö�������
                     CPU_CRITICAL_EXIT();                 //���ж�
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;     //��������Ϊ�����������
                     return ((OS_FLAGS)0);                //����0���д��󣩣�ִֹͣ��
                 } else {                                 //��������˶�������  
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //�������������
                         CPU_CRITICAL_EXIT();             //���ж�
                        *p_err = OS_ERR_SCHED_LOCKED;     //��������Ϊ��������������
                         return ((OS_FLAGS)0);            //����0���д��󣩣�ִֹͣ��
                     }
                 }
                 /* ���������û���� */                                          
                 OS_CRITICAL_ENTER_CPU_EXIT();            //�����ٽ�Σ��ؿ��ж�
                 OS_FlagBlock(&pend_data,                 //������ǰ�������񣬵ȴ��¼���־��
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();             //�˳��жϣ��޵��ȣ�
             }
             break;                                       //����
#endif

        default:                                          //���Ҫ�󳬳�Ԥ��
             CPU_CRITICAL_EXIT();
            *p_err = OS_ERR_OPT_INVALID;                  //��������Ϊ��ѡ��Ƿ���
             return ((OS_FLAGS)0);                        //����0���д��󣩣�ִֹͣ��
    }

    OSSched();                                            //�������
    /* ����ȵ����¼���־�����Լ������� */
    CPU_CRITICAL_ENTER();                                 //���ж�
    switch (OSTCBCurPtr->PendStatus) {                    //������������ĵȴ�״̬���ദ��
        case OS_STATUS_PEND_OK:                           //����ȵ����¼���־��
             if (p_ts != (CPU_TS *)0) {                   //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS;                 //���صȵ���־��ʱ��ʱ���
             }
            *p_err = OS_ERR_NONE;                         //��������Ϊ���޴���
             break;                                       //����

        case OS_STATUS_PEND_ABORT:                        //����ȴ�����ֹ
             if (p_ts != (CPU_TS *)0) {                   //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS;                 //���صȴ�����ֹʱ��ʱ���
             }
             CPU_CRITICAL_EXIT();                         //���ж�
            *p_err = OS_ERR_PEND_ABORT;                   //��������Ϊ���ȴ�����ֹ��
             break;                                       //����

        case OS_STATUS_PEND_TIMEOUT:                      //����ȴ���ʱ
             if (p_ts != (CPU_TS *)0) {                   //��� p_ts �ǿ�
                *p_ts  = (CPU_TS  )0;                     //���� p_ts
             }
             CPU_CRITICAL_EXIT();                         //���ж�
            *p_err = OS_ERR_TIMEOUT;                      //��������Ϊ����ʱ��
             break;                                       //����

        case OS_STATUS_PEND_DEL:                          //����ȴ�����ɾ��
             if (p_ts != (CPU_TS *)0) {                   //��� p_ts �ǿ�
                *p_ts  = OSTCBCurPtr->TS;                 //���ض���ɾʱ��ʱ���
             }
             CPU_CRITICAL_EXIT();                         //���ж�
            *p_err = OS_ERR_OBJ_DEL;                      //��������Ϊ������ɾ��
             break;                                       //����

        default:                                          //����ȴ�״̬����Ԥ��
             CPU_CRITICAL_EXIT();                         //���ж�
            *p_err = OS_ERR_STATUS_INVALID;               //��������Ϊ��״̬�Ƿ���
             break;                                       //����
    }
    if (*p_err != OS_ERR_NONE) {                          //����д������
        return ((OS_FLAGS)0);                             //����0���д��󣩣�ִֹͣ��
    }
    /* ���û�д������ */
    flags_rdy = OSTCBCurPtr->FlagsRdy;                    //��ȡ����������ȴ��ı�־ֵ
    if (consume == DEF_TRUE) {                            //�����Ҫȡ�������¼��ı�־λ
        switch (mode) {                                   //�����¼�����ģʽ���ദ��
            case OS_OPT_PEND_FLAG_SET_ALL:                //�����ͨ����1����־�¼��ķ���
            case OS_OPT_PEND_FLAG_SET_ANY:                                           
                 p_grp->Flags &= ~flags_rdy;              //��0��־���ﴥ���¼��ı�־λ
                 break;                                   //����

#if OS_CFG_FLAG_MODE_CLR_EN > 0u                          //���ʹ���˱�־λ��0����ģʽ
            case OS_OPT_PEND_FLAG_CLR_ALL:                //�����ͨ����0����־�¼��ķ���
            case OS_OPT_PEND_FLAG_CLR_ANY:                 
                 p_grp->Flags |=  flags_rdy;              //��1��־���ﴥ���¼��ı�־λ
                 break;                                   //����
#endif
            default:                                      //�������ģʽ����Ԥ��
                 CPU_CRITICAL_EXIT();                     //���ж�
                *p_err = OS_ERR_OPT_INVALID;              //��������Ϊ��ѡ��Ƿ���
                 return ((OS_FLAGS)0);                    //����0���д��󣩣�ִֹͣ��
        }
    }
    CPU_CRITICAL_EXIT();                                  //���ж�
   *p_err = OS_ERR_NONE;                                  //��������Ϊ���޴���
    return (flags_rdy);                                   //��������������ȴ��ı�־ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                          ABORT WAITING ON AN EVENT FLAG GROUP
*
* Description: This function aborts & readies any tasks currently waiting on an event flag group.  This function should
*              be used to fault-abort the wait on the event flag group, rather than to normally post to the event flag
*              group OSFlagPost().
*
* Arguments  : p_grp     is a pointer to the event flag group
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the event flag
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the event flag
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the event flag group and was
*                                                         readied and informed of the aborted wait; check return value
*                                                         for the number of tasks whose wait on the event flag group
*                                                         was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_grp' is not pointing at an event flag group
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_FLAG_PEND_ABORT_EN > 0u                //���ʹ���� OSFlagPendAbort() ����
OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP  *p_grp, //�¼���־��ָ��
                             OS_OPT        opt,   //ѡ��
                             OS_ERR       *p_err) //���ش�������
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return ((OS_OBJ_QTY)0u);         //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //���ʹ�����ж��зǷ����ü��
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {  //����ú��������ж��б�����
       *p_err = OS_ERR_PEND_ABORT_ISR;           //��������Ϊ�����ж��д�������
        return ((OS_OBJ_QTY)0u);                 //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                 //���ʹ���˲������
    if (p_grp == (OS_FLAG_GRP *)0) {       //��� p_grp Ϊ��
       *p_err  =  OS_ERR_OBJ_PTR_NULL;     //��������Ϊ����������Ϊ�ա�
        return ((OS_OBJ_QTY)0u);           //����0���д��󣩣�ִֹͣ��
    }
    switch (opt) {                         //����ѡ����ദ��  
        case OS_OPT_PEND_ABORT_1:          //���ѡ����Ԥ����
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                        //ֱ������

        default:                           //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;   //��������Ϊ��ѡ��Ƿ���
             return ((OS_OBJ_QTY)0u);      //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u              //���ʹ���˶������ͼ��
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {   //��� p_grp �����¼���־������  
       *p_err = OS_ERR_OBJ_TYPE;             //��������Ϊ�������������� 
        return ((OS_OBJ_QTY)0u);             //����0���д��󣩣�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                            //���ж�
    p_pend_list = &p_grp->PendList;                  //��ȡ��Ϣ���еĵȴ��б�
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) { //���û�������ڵȴ�
        CPU_CRITICAL_EXIT();                         //���ж�
       *p_err = OS_ERR_PEND_ABORT_NONE;              //��������Ϊ��û�����ڵȴ���
        return ((OS_OBJ_QTY)0u);                     //����0���д��󣩣�ִֹͣ��
    }
    /* ����������ڵȴ� */
    OS_CRITICAL_ENTER_CPU_EXIT();                     //�����ٽ�Σ��ؿ��ж� 
    nbr_tasks = 0u;                                   //׼����������ֹ�ĵȴ�����
    ts        = OS_TS_GET();                          //��ȡʱ���
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) {//������������ڵȴ�
        p_tcb = p_pend_list->HeadPtr->TCBPtr;         //��ȡͷ�ˣ�������ȼ�������
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_grp),  //��ֹ������� p_grp �ĵȴ�
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {           //�������ѡ������ֹ���еȴ�����
            break;                                    //�������������ټ�����ֹ
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                      //�˳��ٽ�Σ��޵��ȣ�

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
*                                       GET FLAGS WHO CAUSED TASK TO BECOME READY
*
* Description: This function is called to obtain the flags that caused the task to become ready to run.
*              In other words, this function allows you to tell "Who done it!".
*
* Arguments  : p_err     is a pointer to an error code
*
*                            OS_ERR_NONE       if the call was successful
*                            OS_ERR_PEND_ISR   if called from an ISR
*
* Returns    : The flags that caused the task to be ready.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPendGetFlagsRdy (OS_ERR  *p_err)
{
    OS_FLAGS   flags;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_FLAGS)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if called from ISR ...                             */
       *p_err = OS_ERR_PEND_ISR;                            /* ... can't get from an ISR                              */
        return ((OS_FLAGS)0);
    }
#endif

    CPU_CRITICAL_ENTER();
    flags = OSTCBCurPtr->FlagsRdy;
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (flags);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                                OS_OPT_POST_FLAG_SET       set
*                                OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The call was successful
*                                OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Note(s)    : 1) The execution time of this function depends on the number of tasks waiting on the event flag group.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPost (OS_FLAG_GRP  *p_grp, //�¼���־��ָ��
                      OS_FLAGS      flags, //ѡ��Ҫ�����ı�־λ
                      OS_OPT        opt,   //ѡ��
                      OS_ERR       *p_err) //���ش�������
{
    OS_FLAGS  flags_cur;
    CPU_TS    ts;



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_FLAGS)0);           //����0��ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u               //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_grp == (OS_FLAG_GRP *)0) {     //������� p_grp Ϊ�� 
       *p_err  = OS_ERR_OBJ_PTR_NULL;    //��������Ϊ���¼���־�����Ϊ�ա�
        return ((OS_FLAGS)0);            //����0��ִֹͣ��
    }
    switch (opt) {                       //����ѡ����ദ��
        case OS_OPT_POST_FLAG_SET:       //���ѡ����Ԥ��֮��
        case OS_OPT_POST_FLAG_CLR:
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             break;                      //ֱ������

        default:                         //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID; //��������Ϊ��ѡ��Ƿ���
             return ((OS_FLAGS)0);       //����0��ִֹͣ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //���ʹ���˶������ͼ��
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) { //��� p_grp �����¼���־������
       *p_err = OS_ERR_OBJ_TYPE;           //�������͡�������������
        return ((OS_FLAGS)0);              //����0��ִֹͣ��
    }
#endif

    ts = OS_TS_GET();                             //��ȡʱ���              
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u              //���ʹ�����ж��ӳٷ���
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {    //����ú��������ж��б�����
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_FLAG,//���ñ�־�鷢�����ж���Ϣ����
                    (void      *)p_grp,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )flags,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return ((OS_FLAGS)0);                     //����0��ִֹͣ��
    }
#endif
    /* ���û��ʹ���ж��ӳٷ��� */
    flags_cur = OS_FlagPost(p_grp,               //����־��ֱ�ӷ���
                            flags,
                            opt,
                            ts,
                            p_err);

    return (flags_cur);                         //���ص�ǰ��־λ��ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                         SUSPEND TASK UNTIL EVENT FLAG(s) RECEIVED OR TIMEOUT OCCURS
*
* Description: This function is internal to uC/OS-III and is used to put a task to sleep until the desired
*              event flag bit(s) are set.
*
* Arguments  : p_pend_data    is a pointer to an object used to link the task being blocked to the list of task(s)
*              -----------    pending on the desired event flag group.
*
*              p_grp         is a pointer to the desired event flag group.
*              -----
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to check.
*                            The bits you want are specified by setting the corresponding bits in
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then
*                            'flags' would contain 0x03.
*
*              opt           specifies whether you want ALL bits to be set/cleared or ANY of the bits
*                            to be set/cleared.
*                            You can specify the following argument:
*
*                                OS_OPT_PEND_FLAG_CLR_ALL   You will check ALL bits in 'mask' to be clear (0)
*                                OS_OPT_PEND_FLAG_CLR_ANY   You will check ANY bit  in 'mask' to be clear (0)
*                                OS_OPT_PEND_FLAG_SET_ALL   You will check ALL bits in 'mask' to be set   (1)
*                                OS_OPT_PEND_FLAG_SET_ANY   You will check ANY bit  in 'mask' to be set   (1)
*
*              timeout       is the desired amount of time that the task will wait for the event flag
*                            bit(s) to be set.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagBlock (OS_PEND_DATA  *p_pend_data,  //�ȴ��б�Ԫ��
                    OS_FLAG_GRP   *p_grp,        //�¼���־��
                    OS_FLAGS       flags,        //Ҫ�����ı�־λ
                    OS_OPT         opt,          //ѡ��
                    OS_TICK        timeout)      //�ȴ�����
{
    OSTCBCurPtr->FlagsPend = flags;          //������Ҫ�ȴ��ı�־λ              
    OSTCBCurPtr->FlagsOpt  = opt;            //����Ա�־λ��Ҫ��
    OSTCBCurPtr->FlagsRdy  = (OS_FLAGS)0;

    OS_Pend(p_pend_data,                     //�������񣬵ȴ��¼���־��
            (OS_PEND_OBJ *)((void *)p_grp),
             OS_TASK_PEND_ON_FLAG,
             timeout);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                      CLEAR THE CONTENTS OF AN EVENT FLAG GROUP
*
* Description: This function is called by OSFlagDel() to clear the contents of an event flag group
*

* Argument(s): p_grp     is a pointer to the event flag group to clear
*              -----
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagClr (OS_FLAG_GRP  *p_grp)
{
    OS_PEND_LIST  *p_pend_list;



    p_grp->Type             = OS_OBJ_TYPE_NONE;
    p_grp->NamePtr          = (CPU_CHAR *)((void *)"?FLAG");    /* Unknown name                                       */
    p_grp->Flags            = (OS_FLAGS )0;
    p_pend_list             = &p_grp->PendList;
    OS_PendListInit(p_pend_list);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                          INITIALIZE THE EVENT FLAG MODULE
*
* Description: This function is called by uC/OS-III to initialize the event flag module.  Your application MUST NOT call
*              this function.  In other words, this function is internal to uC/OS-III.
*
* Arguments  : p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE   The call was successful.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSFlagDbgListPtr = (OS_FLAG_GRP *)0;
#endif

    OSFlagQty        = (OS_OBJ_QTY   )0;
   *p_err            = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                    ADD/REMOVE EVENT FLAG GROUP TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove an event flag group from the event flag debug
*              list.
*
* Arguments  : p_grp     is a pointer to the event flag group to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u                        //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ���
void  OS_FlagDbgListAdd (OS_FLAG_GRP  *p_grp) //���¼���־����뵽�¼���־������б����ǰ��
{
    p_grp->DbgNamePtr                = (CPU_CHAR    *)((void *)" "); //�Ȳ�ָ���κ����������
    p_grp->DbgPrevPtr                = (OS_FLAG_GRP *)0;             //���ñ�־����Ϊ�б����ǰ��
    if (OSFlagDbgListPtr == (OS_FLAG_GRP *)0) {                      //����б�Ϊ��
        p_grp->DbgNextPtr            = (OS_FLAG_GRP *)0;             //�ö��е���һ��Ԫ��ҲΪ��
    } else {                                                         //����б�ǿ�
        p_grp->DbgNextPtr            =  OSFlagDbgListPtr;            //�б�ԭ������Ԫ����Ϊ�ö��е���һ��Ԫ��
        OSFlagDbgListPtr->DbgPrevPtr =  p_grp;                       //ԭ������Ԫ�ص�ǰ���Ϊ�˸ö���
    }
    OSFlagDbgListPtr                 =  p_grp;                       //�ñ�־���Ϊ�б������Ԫ��
}



void  OS_FlagDbgListRemove (OS_FLAG_GRP  *p_grp)
{
    OS_FLAG_GRP  *p_grp_next;
    OS_FLAG_GRP  *p_grp_prev;


    p_grp_prev = p_grp->DbgPrevPtr;
    p_grp_next = p_grp->DbgNextPtr;

    if (p_grp_prev == (OS_FLAG_GRP *)0) {
        OSFlagDbgListPtr = p_grp_next;
        if (p_grp_next != (OS_FLAG_GRP *)0) {
            p_grp_next->DbgPrevPtr = (OS_FLAG_GRP *)0;
        }
        p_grp->DbgNextPtr = (OS_FLAG_GRP *)0;

    } else if (p_grp_next == (OS_FLAG_GRP *)0) {
        p_grp_prev->DbgNextPtr = (OS_FLAG_GRP *)0;
        p_grp->DbgPrevPtr      = (OS_FLAG_GRP *)0;

    } else {
        p_grp_prev->DbgNextPtr =  p_grp_next;
        p_grp_next->DbgPrevPtr =  p_grp_prev;
        p_grp->DbgNextPtr      = (OS_FLAG_GRP *)0;
        p_grp->DbgPrevPtr      = (OS_FLAG_GRP *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                                OS_OPT_POST_FLAG_SET       set
*                                OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*
*              ts            is the timestamp of the post
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The call was successful
*                                OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Note(s)    : 1) The execution time of this function depends on the number of tasks waiting on the event flag group.
************************************************************************************************************************
*/

OS_FLAGS  OS_FlagPost (OS_FLAG_GRP  *p_grp, //�¼���־��ָ��
                       OS_FLAGS      flags, //ѡ��Ҫ�����ı�־λ
                       OS_OPT        opt,   //ѡ��
                       CPU_TS        ts,    //ʱ���
                       OS_ERR       *p_err) //���ش�������
{
    OS_FLAGS        flags_cur;
    OS_FLAGS        flags_rdy;
    OS_OPT          mode;
    OS_PEND_DATA   *p_pend_data;
    OS_PEND_DATA   *p_pend_data_next;
    OS_PEND_LIST   *p_pend_list;
    OS_TCB         *p_tcb;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ�� 
	
    CPU_CRITICAL_ENTER();                                //���ж�
    switch (opt) {                                       //����ѡ����ദ��
        case OS_OPT_POST_FLAG_SET:                       //���Ҫ��ѡ��λ��1
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
             p_grp->Flags |=  flags;                     //��ѡ��λ��1
             break;                                      //����

        case OS_OPT_POST_FLAG_CLR:                       //���Ҫ��ѡ��λ��0
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             p_grp->Flags &= ~flags;                     //��ѡ��λ��0
             break;                                      //����

        default:                                         //���ѡ���Ԥ��
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_OPT_INVALID;                 //��������Ϊ��ѡ��Ƿ���
             return ((OS_FLAGS)0);                       //����0��ִֹͣ��
    }
    p_grp->TS   = ts;                                    //��ʱ��������¼���־��
    p_pend_list = &p_grp->PendList;                      //��ȡ�¼���־��ĵȴ��б�
    if (p_pend_list->NbrEntries == 0u) {                 //���û�������ڵȴ���־��
        CPU_CRITICAL_EXIT();                             //���ж�
       *p_err = OS_ERR_NONE;                             //��������Ϊ���޴���
        return (p_grp->Flags);                           //�����¼���־��ı�־ֵ
    }
    /* ����������ڵȴ���־�� */
    OS_CRITICAL_ENTER_CPU_EXIT();                     //�����ٽ�Σ��ؿ��ж�
    p_pend_data = p_pend_list->HeadPtr;               //��ȡ�ȴ��б�ͷ���ȴ�����
    p_tcb       = p_pend_data->TCBPtr;
    while (p_tcb != (OS_TCB *)0) {                    //��ͷ��β�����ȴ��б����������
        p_pend_data_next = p_pend_data->NextPtr;
        mode             = p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_MASK; //��ȡ����ı�־ѡ��
        switch (mode) {                               //��������ı�־ѡ����ദ��
            case OS_OPT_PEND_FLAG_SET_ALL:            //���Ҫ������ȴ��ı�־λ������1
                 flags_rdy = (OS_FLAGS)(p_grp->Flags & p_tcb->FlagsPend); 
                 if (flags_rdy == p_tcb->FlagsPend) { //�������ȴ��ı�־λ����1��
                     OS_FlagTaskRdy(p_tcb,            //�ø�����׼������
                                    flags_rdy,
                                    ts);
                 }
                 break;                               //����

            case OS_OPT_PEND_FLAG_SET_ANY:            //���Ҫ������ȴ��ı�־λ��1λ��1����
                 flags_rdy = (OS_FLAGS)(p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy != (OS_FLAGS)0) {      //�������ȴ��ı�־λ����1��
                     OS_FlagTaskRdy(p_tcb,            //�ø�����׼������
                                    flags_rdy,
                                    ts);
                 }
                 break;                              //����

#if OS_CFG_FLAG_MODE_CLR_EN > 0u                     //���ʹ���˱�־λ��0����ģʽ
            case OS_OPT_PEND_FLAG_CLR_ALL:           //���Ҫ������ȴ��ı�־λ������0
                 flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy == p_tcb->FlagsPend) {//�������ȴ��ı�־λ����0��
                     OS_FlagTaskRdy(p_tcb,           //�ø�����׼������
                                    flags_rdy,
                                    ts);
                 }
                 break;            //����

            case OS_OPT_PEND_FLAG_CLR_ANY:          //���Ҫ������ȴ��ı�־λ��1λ��0����
                 flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy != (OS_FLAGS)0) {    //�������ȴ��ı�־λ����0��
                     OS_FlagTaskRdy(p_tcb,          //�ø�����׼������
                                    flags_rdy,
                                    ts);
                 }
                 break;                            //����
#endif
            default:                               //�����־ѡ���Ԥ��
                 OS_CRITICAL_EXIT();               //�˳��ٽ��
                *p_err = OS_ERR_FLAG_PEND_OPT;     //��������Ϊ����־ѡ��Ƿ���
                 return ((OS_FLAGS)0);             //����0��ֹͣ����
        }
        p_pend_data = p_pend_data_next;            //׼��������һ���ȴ�����
        if (p_pend_data != (OS_PEND_DATA *)0) {    //������������
            p_tcb = p_pend_data->TCBPtr;           //��ȡ�������������ƿ�
        } else {                                   //��������񲻴���
            p_tcb = (OS_TCB *)0;                   //��� p_tcb���˳� while ѭ��
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                  //�˳��ٽ�Σ��޵��ȣ�

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {  //��� opt ûѡ�񡰷���ʱ����������
        OSSched();                                    //�������
    }

    CPU_CRITICAL_ENTER();        //���ж�
    flags_cur = p_grp->Flags;    //��ȡ�¼���־��ı�־ֵ
    CPU_CRITICAL_EXIT();         //���ж�
   *p_err     = OS_ERR_NONE;     //��������Ϊ���޴���
    return (flags_cur);          //�����¼���־��ĵ�ǰ��־ֵ
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        MAKE TASK READY-TO-RUN, EVENT(s) OCCURRED
*
* Description: This function is internal to uC/OS-III and is used to make a task ready-to-run because the desired event
*              flag bits have been set.
*
* Arguments  : p_tcb         is a pointer to the OS_TCB of the task to remove
*              -----
*
*              flags_rdy     contains the bit pattern of the event flags that cause the task to become ready-to-run.
*
*              ts            is a timestamp associated with the post
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void   OS_FlagTaskRdy (OS_TCB    *p_tcb,            //������ƿ�ָ��
                       OS_FLAGS   flags_rdy,        //����������ı�־λ    
                       CPU_TS     ts)               //�¼���־�鱻����ʱ��ʱ���
{
    p_tcb->FlagsRdy   = flags_rdy;                  //���������������¼���־λ
    p_tcb->PendStatus = OS_STATUS_PEND_OK;          //�������ĵȴ�״̬
    p_tcb->PendOn     = OS_TASK_PEND_ON_NOTHING;    //�������û�еȴ��κζ���
    p_tcb->TS         = ts;                         //��¼��������ȴ�ʱ��ʱ���
    switch (p_tcb->TaskState) {                     //�������������״̬���ദ��
        case OS_TASK_STATE_RDY:                     //��������Ǿ���״̬
        case OS_TASK_STATE_DLY:                     //�����������ʱ״̬
        case OS_TASK_STATE_DLY_SUSPENDED:           //�����������ʱ�б�����״̬
        case OS_TASK_STATE_SUSPENDED:               //��������Ǳ�����״̬
             break;                                 //ֱ�����������账��

        case OS_TASK_STATE_PEND:                    //��������������޵ȴ�״̬
        case OS_TASK_STATE_PEND_TIMEOUT:            //��������������޵ȴ�״̬
             OS_TaskRdy(p_tcb);                     //������������״̬
             p_tcb->TaskState = OS_TASK_STATE_RDY;  //�޸������״̬Ϊ����״̬
             break;                                 //����

        case OS_TASK_STATE_PEND_SUSPENDED:              //��������������޵ȴ��б�����״̬
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:      //��������������޵ȴ��б�����״̬
             p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;//�޸������״̬Ϊ������״̬
             break;                                     //����

        default:                                        //�������״̬����Ԥ��
             break;                                     //ֱ������
    }
    OS_PendListRemove(p_tcb);                           //������ӵȴ��б��Ƴ�       
}
#endif
