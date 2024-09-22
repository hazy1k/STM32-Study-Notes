/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TIMER MANAGEMENT
*
* File    : OS_TMR.C
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
const  CPU_CHAR  *os_tmr__c = "$Id: $";
#endif


#if OS_CFG_TMR_EN > 0u
/*
************************************************************************************************************************
*                                                     CONSTANTS
************************************************************************************************************************
*/

#define  OS_OPT_LINK_DLY       (OS_OPT)(0u)
#define  OS_OPT_LINK_PERIODIC  (OS_OPT)(1u)

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   CREATE A TIMER
*
* Description: This function is called by your application code to create a timer.
*
* Arguments  : p_tmr           Is a pointer to a timer control block
*
*              p_name          Is a pointer to an ASCII string that is used to name the timer.  Names are useful for
*                              debugging.
*
*              dly             Initial delay.
*                              If the timer is configured for ONE-SHOT mode, this is the timeout used
*                              If the timer is configured for PERIODIC mode, this is the first timeout to wait for
*                              before the timer starts entering periodic mode
*
*              period          The 'period' being repeated for the timer.
*                              If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer expires, it will
*                              automatically restart with the same period.
*
*              opt             Specifies either:
*
*                                  OS_OPT_TMR_ONE_SHOT       The timer counts down only once
*                                  OS_OPT_TMR_PERIODIC       The timer counts down and then reloads itself
*
*              p_callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                              callback function must be declared as follows:
*
*                                  void  MyCallback (OS_TMR *p_tmr, void *p_arg);
*
*              p_callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              p_err           Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the timer after you called
*                                                                  OSSafetyCriticalStart().
*                                 OS_ERR_OBJ_CREATED             if the timer has already been created
*                                 OS_ERR_OBJ_PTR_NULL            is 'p_tmr' is a NULL pointer
*                                 OS_ERR_OBJ_TYPE                if the object type is invalid
*                                 OS_ERR_OPT_INVALID             you specified an invalid option
*                                 OS_ERR_TMR_INVALID_DLY         you specified an invalid delay
*                                 OS_ERR_TMR_INVALID_PERIOD      you specified an invalid period
*                                 OS_ERR_TMR_ISR                 if the call was made from an ISR
*
* Returns    : none
*
* Note(s)    : 1) This function only creates the timer.  In other words, the timer is not started when created.  To
*                 start the timer, call OSTmrStart().
************************************************************************************************************************
*/

void  OSTmrCreate (OS_TMR               *p_tmr,          //��ʱ�����ƿ�ָ��
                   CPU_CHAR             *p_name,         //������ʱ���������ڵ���
                   OS_TICK               dly,            //��ʼ��ʱ������
                   OS_TICK               period,         //���ڶ�ʱ���ؽ�����
                   OS_OPT                opt,            //ѡ��
                   OS_TMR_CALLBACK_PTR   p_callback,     //��ʱ����ʱ�Ļص�����
                   void                 *p_callback_arg, //�����ص������Ĳ���
                   OS_ERR               *p_err)          //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                    //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                    //�����ж�ʱ����ֵ��ԭ�� 

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ���ִ�ж�ʱ����
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ�ؼ�
    if (OSSafetyCriticalStartFlag == DEF_TRUE) { //������ڵ��� OSSafetyCriticalStart() �󴴽��ö�ʱ��
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;  //��������Ϊ���Ƿ������ں˶���
        return;                                  //���أ���ִ�ж�ʱ����
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü��  
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú��������ж��б�����
       *p_err = OS_ERR_TMR_ISR;                 //��������Ϊ�����жϺ����ж�ʱ��
        return;                                 //���أ���ִ�ж�ʱ����
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                          //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_tmr == (OS_TMR *)0) {                     //������� p_tmr Ϊ��
       *p_err = OS_ERR_OBJ_PTR_NULL;                //��������Ϊ����ʱ������Ϊ�ա�
        return;                                     //���أ���ִ�ж�ʱ����
    }

    switch (opt) {                                  //������ʱѡ����� opt �������
        case OS_OPT_TMR_PERIODIC:                   //���ѡ�������Զ�ʱ
             if (period == (OS_TICK)0) {            //�����������ʵ��Ϊ0
                *p_err = OS_ERR_TMR_INVALID_PERIOD; //��������Ϊ����������ʵ����Ч��
                 return;                            //���أ���ִ�ж�ʱ����
             }
             break;

        case OS_OPT_TMR_ONE_SHOT:                   //���ѡ��һ���Զ�ʱ
             if (dly == (OS_TICK)0) {               //�����ʱ��ʼʵ��Ϊ0
                *p_err = OS_ERR_TMR_INVALID_DLY;    //��������Ϊ����ʱ��ʼʵ����Ч��
                 return;                            //���أ���ִ�ж�ʱ����
             }
             break;

        default:                                    //���ѡ���Ԥ��
            *p_err = OS_ERR_OPT_INVALID;            //��������Ϊ��ѡ��Ƿ���
             return;                                //���أ���ִ�ж�ʱ����
    }
#endif

    OS_CRITICAL_ENTER();         //�����ٽ��
    p_tmr->State          = (OS_STATE           )OS_TMR_STATE_STOPPED;  //��ʼ����ʱ��ָ��
    p_tmr->Type           = (OS_OBJ_TYPE        )OS_OBJ_TYPE_TMR;
    p_tmr->NamePtr        = (CPU_CHAR          *)p_name;
    p_tmr->Dly            = (OS_TICK            )dly;
    p_tmr->Match          = (OS_TICK            )0;
    p_tmr->Remain         = (OS_TICK            )0;
    p_tmr->Period         = (OS_TICK            )period;
    p_tmr->Opt            = (OS_OPT             )opt;
    p_tmr->CallbackPtr    = (OS_TMR_CALLBACK_PTR)p_callback;
    p_tmr->CallbackPtrArg = (void              *)p_callback_arg;
    p_tmr->NextPtr        = (OS_TMR            *)0;
    p_tmr->PrevPtr        = (OS_TMR            *)0;

#if OS_CFG_DBG_EN > 0u           //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ��� 
    OS_TmrDbgListAdd(p_tmr);     //���ö�ʱ��ӵ���ʱ��˫���������
#endif
    OSTmrQty++;                  //��ʱ��������1

    OS_CRITICAL_EXIT_NO_SCHED(); //�˳��ٽ�Σ��޵��ȣ�
   *p_err = OS_ERR_NONE;         //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   DELETE A TIMER
*
* Description: This function is called by your application code to delete a timer.
*
* Arguments  : p_tmr          Is a pointer to the timer to stop and delete.
*
*              p_err          Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE
*                                 OS_ERR_OBJ_TYPE             'p_tmr' is not pointing to a timer
*                                 OS_ERR_TMR_INVALID          'p_tmr' is a NULL pointer
*                                 OS_ERR_TMR_ISR              if the function was called from an ISR
*                                 OS_ERR_TMR_INACTIVE         if the timer was not created
*                                 OS_ERR_TMR_INVALID_STATE    the timer is in an invalid state
*
* Returns    : DEF_TRUE   if the timer was deleted
*              DEF_FALSE  if not or upon an error
************************************************************************************************************************
*/

#if OS_CFG_TMR_DEL_EN > 0u             //���ʹ�ܣ�Ĭ�����ۣ��� OSTmrDel() ����
CPU_BOOLEAN  OSTmrDel (OS_TMR  *p_tmr, //��ʱ�����ƿ�ָ��
                       OS_ERR  *p_err) //���ش�������
{
    OS_ERR       err;
    CPU_BOOLEAN  success;  //�ݴ溯��ִ�н��



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return (DEF_FALSE);             //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü�� 
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //����ú��������ж��б����� 
       *p_err  = OS_ERR_TMR_ISR;                //��������Ϊ�����жϺ����ж�ʱ��
        return (DEF_FALSE);                     //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u          //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_tmr == (OS_TMR *)0) {     //���ʹ�� p_tmr ��ʵ��Ϊ��
       *p_err = OS_ERR_TMR_INVALID; //��������Ϊ����Ч�Ķ�ʱ����
        return (DEF_FALSE);         //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //���ʹ�ܣ�Ĭ��ʹ�ܣ��˶������ͼ��
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {  //����ö�ʱ���Ķ�����������
       *p_err = OS_ERR_OBJ_TYPE;           //��������Ϊ���������ʹ���
        return (DEF_FALSE);                //���� DEF_FALSE��������ִ��
    }
#endif

    OSSchedLock(&err);          //��ס������   
#if OS_CFG_DBG_EN > 0u          //���ʹ�ܣ�Ĭ��ʹ�ܣ��˵��Դ���ͱ��� 
    OS_TmrDbgListRemove(p_tmr); //���ö�ʱ�Ӷ�ʱ��˫����������Ƴ�
#endif
    OSTmrQty--;                 //��ʱ��������1

    switch (p_tmr->State) {                       //���ݶ�ʱ����״̬���ദ��
        case OS_TMR_STATE_RUNNING:                //�����ʱ����������
             OS_TmrUnlink(p_tmr);                 //�ӵ�ǰ��ʱ�����б��Ƴ���ʱ��  
             OS_TmrClr(p_tmr);                    //��λ��ʱ����ָ��
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_NONE;               //��������Ϊ���޴���
             success = DEF_TRUE;                  //ִ�н����Ϊ DEF_TRUE
             break;

        case OS_TMR_STATE_STOPPED:                //�����ʱ���ѱ�ֹͣ  
        case OS_TMR_STATE_COMPLETED:              //�����ʱ������ɵ�һ�ζ�ʱ
             OS_TmrClr(p_tmr);                    //��λ��ʱ����ָ��
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_NONE;               //��������Ϊ���޴���
             success = DEF_TRUE;                  //ִ�н����Ϊ DEF_TRUE
             break;
             
        case OS_TMR_STATE_UNUSED:                 //�����ʱ���ѱ�ɾ��
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_TMR_INACTIVE;       //��������Ϊ����ʱ��δ���
             success = DEF_FALSE;                 //ִ�н����Ϊ DEF_FALSE
             break;

        default:                                  //�����ʱ����״̬����Ԥ��
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_TMR_INVALID_STATE;  //��������Ϊ����ʱ����Ч��
             success = DEF_FALSE;                 //ִ�н����Ϊ DEF_FALSE
             break;
    }
    return (success);                             //����ִ�н��
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                    GET HOW MUCH TIME IS LEFT BEFORE A TIMER EXPIRES
*
* Description: This function is called to get the number of ticks before a timer times out.
*
* Arguments  : p_tmr    Is a pointer to the timer to obtain the remaining time from.
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE
*                           OS_ERR_OBJ_TYPE           'p_tmr' is not pointing to a timer
*                           OS_ERR_TMR_INVALID        'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_ISR            if the call was made from an ISR
*                           OS_ERR_TMR_INACTIVE       'p_tmr' points to a timer that is not active
*                           OS_ERR_TMR_INVALID_STATE  the timer is in an invalid state
*
* Returns    : The time remaining for the timer to expire.  The time represents 'timer' increments.  In other words, if
*              OS_TmrTask() is signaled every 1/10 of a second then the returned value represents the number of 1/10 of
*              a second remaining before the timer expires.
************************************************************************************************************************
*/

OS_TICK  OSTmrRemainGet (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_TICK  remain;
    OS_ERR   err;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_TICK)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if trying to call from an ISR                      */
       *p_err = OS_ERR_TMR_ISR;
        return ((OS_TICK)0);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_tmr == (OS_TMR *)0) {
       *p_err = OS_ERR_TMR_INVALID;
        return ((OS_TICK)0);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                   /* Make sure timer was created                            */
       *p_err = OS_ERR_OBJ_TYPE;
        return ((OS_TICK)0);
    }
#endif

    OSSchedLock(&err);
    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
             remain        = p_tmr->Match                   /* Determine how much time is left to timeout             */
                           - OSTmrTickCtr;
             p_tmr->Remain = remain;
             OSSchedUnlock(&err);
            *p_err         = OS_ERR_NONE;
             break;

        case OS_TMR_STATE_STOPPED:                          /* It's assumed that the timer has not started yet        */
             if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
                 if (p_tmr->Dly == 0u) {
                     remain = p_tmr->Period;
                 } else {
                     remain = p_tmr->Dly;
                 }
             } else {
                 remain = p_tmr->Dly;
             }
             p_tmr->Remain = remain;
             OSSchedUnlock(&err);
            *p_err         = OS_ERR_NONE;
             break;

        case OS_TMR_STATE_COMPLETED:                        /* Only ONE-SHOT that timed out can be in this state      */
             OSSchedUnlock(&err);
            *p_err  = OS_ERR_NONE;
             remain = (OS_TICK)0;
             break;

        case OS_TMR_STATE_UNUSED:
             OSSchedUnlock(&err);
            *p_err  = OS_ERR_TMR_INACTIVE;
             remain = (OS_TICK)0;
             break;

        default:
             OSSchedUnlock(&err);
            *p_err = OS_ERR_TMR_INVALID_STATE;
             remain = (OS_TICK)0;
             break;
    }
    return (remain);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   START A TIMER
*
* Description: This function is called by your application code to start a timer.
*
* Arguments  : p_tmr    Is a pointer to an OS_TMR
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE
*                           OS_ERR_OBJ_TYPE            if 'p_tmr' is not pointing to a timer
*                           OS_ERR_TMR_INVALID
*                           OS_ERR_TMR_INACTIVE        if the timer was not created
*                           OS_ERR_TMR_INVALID_STATE   the timer is in an invalid state
*                           OS_ERR_TMR_ISR             if the call was made from an ISR
*
* Returns    : DEF_TRUE      is the timer was started
*              DEF_FALSE     if not or upon an error
*
* Note(s)    : 1) When starting/restarting a timer, regardless if it is in PERIODIC or ONE-SHOT mode, the timer is 
*                 linked to the timer wheel with the OS_OPT_LINK_DLY option. This option sets the initial expiration 
*                 time for the timer. For timers in PERIODIC mode, subsequent expiration times are handled by 
*                 the OS_TmrTask().
************************************************************************************************************************
*/

CPU_BOOLEAN  OSTmrStart (OS_TMR  *p_tmr,  //��ʱ�����ƿ�ָ��
                         OS_ERR  *p_err)  //���ش�������
{
    OS_ERR       err;
    CPU_BOOLEAN  success; //�ݴ溯��ִ�н��



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return (DEF_FALSE);             //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü�� 
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú��������ж��б�����               
       *p_err = OS_ERR_TMR_ISR;                //��������Ϊ�����жϺ����ж�ʱ��
        return (DEF_FALSE);                    //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u           //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_tmr == (OS_TMR *)0) {      //���ʹ�� p_tmr ��ʵ��Ϊ��
       *p_err = OS_ERR_TMR_INVALID;  //��������Ϊ����Ч�Ķ�ʱ����
        return (DEF_FALSE);          //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u           //���ʹ�ܣ�Ĭ��ʹ�ܣ��˶������ͼ��
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) { //����ö�ʱ���Ķ�����������
       *p_err = OS_ERR_OBJ_TYPE;          //��������Ϊ���������ʹ���
        return (DEF_FALSE);               //���� DEF_FALSE��������ִ��
    }
#endif

    OSSchedLock(&err);                            //��ס������     
    switch (p_tmr->State) {                       //���ݶ�ʱ����״̬���ദ��
        case OS_TMR_STATE_RUNNING:                //�����ʱ���������У�������
             OS_TmrUnlink(p_tmr);                 //�Ӷ�ʱ�������Ƴ��ö�ʱ��
             OS_TmrLink(p_tmr, OS_OPT_LINK_DLY);  //���ö�ʱ�����²��뵽��ʱ����
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_NONE;               //��������Ϊ���޴���
             success = DEF_TRUE;                  //ִ�н����Ϊ DEF_TRUE
             break;

        case OS_TMR_STATE_STOPPED:                //�����ʱ���ѱ�ֹͣ������  
        case OS_TMR_STATE_COMPLETED:              //�����ʱ��������ˣ�����  
             OS_TmrLink(p_tmr, OS_OPT_LINK_DLY);  //���ö�ʱ�����²��뵽��ʱ����
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_NONE;               //��������Ϊ���޴���
             success = DEF_TRUE;                  //ִ�н����Ϊ DEF_TRUE
             break;

        case OS_TMR_STATE_UNUSED:                 //�����ʱ��δ������
             OSSchedUnlock(&err);                 //����������
            *p_err   = OS_ERR_TMR_INACTIVE;       //��������Ϊ����ʱ��δ���
             success = DEF_FALSE;                 //ִ�н����Ϊ DEF_FALSE
             break;

        default:                                  //�����ʱ����״̬����Ԥ��
             OSSchedUnlock(&err);                 //����������
            *p_err = OS_ERR_TMR_INVALID_STATE;    //��������Ϊ����ʱ����Ч��
             success = DEF_FALSE;                 //ִ�н����Ϊ DEF_FALSE
             break;
    }
    return (success);                             //����ִ�н��
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                           FIND OUT WHAT STATE A TIMER IS IN
*
* Description: This function is called to determine what state the timer is in:
*
*                  OS_TMR_STATE_UNUSED     the timer has not been created
*                  OS_TMR_STATE_STOPPED    the timer has been created but has not been started or has been stopped
*                  OS_TMR_COMPLETED        the timer is in ONE-SHOT mode and has completed it's timeout
*                  OS_TMR_RUNNING          the timer is currently running
*
* Arguments  : p_tmr    Is a pointer to the desired timer
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE
*                           OS_ERR_OBJ_TYPE            if 'p_tmr' is not pointing to a timer
*                           OS_ERR_TMR_INVALID        'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_INVALID_STATE  if the timer is not in a valid state
*                           OS_ERR_TMR_ISR            if the call was made from an ISR
*
* Returns    : The current state of the timer (see description).
************************************************************************************************************************
*/

OS_STATE  OSTmrStateGet (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_STATE  state;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if trying to call from an ISR                      */
       *p_err = OS_ERR_TMR_ISR;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_tmr == (OS_TMR *)0) {
       *p_err = OS_ERR_TMR_INVALID;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                   /* Make sure timer was created                            */
       *p_err = OS_ERR_OBJ_TYPE;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

    CPU_CRITICAL_ENTER();
    state = p_tmr->State;
    switch (state) {
        case OS_TMR_STATE_UNUSED:
        case OS_TMR_STATE_STOPPED:
        case OS_TMR_STATE_COMPLETED:
        case OS_TMR_STATE_RUNNING:
            *p_err = OS_ERR_NONE;
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
             break;
    }
    CPU_CRITICAL_EXIT();
    return (state);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    STOP A TIMER
*
* Description: This function is called by your application code to stop a timer.
*
* Arguments  : p_tmr          Is a pointer to the timer to stop.
*
*              opt           Allows you to specify an option to this functions which can be:
*
*                               OS_OPT_TMR_NONE            Do nothing special but stop the timer
*                               OS_OPT_TMR_CALLBACK        Execute the callback function, pass it the callback argument
*                                                          specified when the timer was created.
*                               OS_OPT_TMR_CALLBACK_ARG    Execute the callback function, pass it the callback argument
*                                                          specified in THIS function call
*
*              callback_arg  Is a pointer to a 'new' callback argument that can be passed to the callback function
*                               instead of the timer's callback argument.  In other words, use 'callback_arg' passed in
*                               THIS function INSTEAD of p_tmr->OSTmrCallbackArg
*
*              p_err          Is a pointer to an error code.  '*p_err' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_OBJ_TYPE            if 'p_tmr' is not pointing to a timer
*                               OS_ERR_OPT_INVALID         if you specified an invalid option for 'opt'
*                               OS_ERR_TMR_INACTIVE        if the timer was not created
*                               OS_ERR_TMR_INVALID         'p_tmr' is a NULL pointer
*                               OS_ERR_TMR_INVALID_STATE   the timer is in an invalid state
*                               OS_ERR_TMR_ISR             if the function was called from an ISR
*                               OS_ERR_TMR_NO_CALLBACK     if the timer does not have a callback function defined
*                               OS_ERR_TMR_STOPPED         if the timer was already stopped
*
* Returns    : DEF_TRUE       If we stopped the timer (if the timer is already stopped, we also return DEF_TRUE)
*              DEF_FALSE      If not
************************************************************************************************************************
*/

CPU_BOOLEAN  OSTmrStop (OS_TMR  *p_tmr,          //��ʱ�����ƿ�ָ��
                        OS_OPT   opt,            //ѡ��
                        void    *p_callback_arg, //�����ص��������²���
                        OS_ERR  *p_err)          //���ش�������
{
    OS_TMR_CALLBACK_PTR  p_fnct;
    OS_ERR               err;
    CPU_BOOLEAN          success;  //�ݴ溯��ִ�н��



#ifdef OS_SAFETY_CRITICAL                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return (DEF_FALSE);              //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü�� 
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //����ú��������ж��б����� 
       *p_err = OS_ERR_TMR_ISR;                //��������Ϊ�����жϺ����ж�ʱ��
        return (DEF_FALSE);                    //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u          //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲������
    if (p_tmr == (OS_TMR *)0) {     //���ʹ�� p_tmr ��ʵ��Ϊ��
       *p_err = OS_ERR_TMR_INVALID; //��������Ϊ����Ч�Ķ�ʱ����
        return (DEF_FALSE);         //���� DEF_FALSE��������ִ��
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //���ʹ�ܣ�Ĭ��ʹ�ܣ��˶������ͼ��
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {  //����ö�ʱ���Ķ�����������
       *p_err = OS_ERR_OBJ_TYPE;           //��������Ϊ���������ʹ���
        return (DEF_FALSE);                //���� DEF_FALSE��������ִ��
    }
#endif

    OSSchedLock(&err);                                                   //��ס������  
    switch (p_tmr->State) {                                              //���ݶ�ʱ����״̬���ദ��
        case OS_TMR_STATE_RUNNING:                                       //�����ʱ����������
             OS_TmrUnlink(p_tmr);                                        //�Ӷ�ʱ�����б����Ƴ��ö�ʱ��
            *p_err = OS_ERR_NONE;                                        //��������Ϊ���޴���
             switch (opt) {                                              //����ѡ����ദ��
                 case OS_OPT_TMR_CALLBACK:                               //ִ�лص�������ʹ�ô�����ʱ��ʱ��ʵ��
                      p_fnct = p_tmr->CallbackPtr;                       //ȡ��ʱ���Ļص����� 
                      if (p_fnct != (OS_TMR_CALLBACK_PTR)0) {            //����ص��������� 
                        (*p_fnct)((void *)p_tmr, p_tmr->CallbackPtrArg); //ʹ�ô�����ʱ��ʱ��ʵ��ִ�лص�����
                      } else {                                           //����ص����������� 
                         *p_err = OS_ERR_TMR_NO_CALLBACK;                //��������Ϊ����ʱ��û�лص�������
                      }
                      break;

                 case OS_OPT_TMR_CALLBACK_ARG:                    //ִ�лص�������ʹ�� p_callback_arg ��Ϊʵ��
                      p_fnct = p_tmr->CallbackPtr;                //ȡ��ʱ���Ļص����� 
                      if (p_fnct != (OS_TMR_CALLBACK_PTR)0) {     //����ص��������� 
                        (*p_fnct)((void *)p_tmr, p_callback_arg); //ʹ�� p_callback_arg ��Ϊʵ��ִ�лص�����
                      } else {                                    //����ص����������� 
                         *p_err = OS_ERR_TMR_NO_CALLBACK;         //��������Ϊ����ʱ��û�лص�������
                      }
                      break;

                 case OS_OPT_TMR_NONE:           //ֻ��ͣ����ʱ��
                      break;

                 default:                        //�������Ԥ��
                     OSSchedUnlock(&err);        //����������
                    *p_err = OS_ERR_OPT_INVALID; //��������Ϊ��ѡ����Ч��
                     return (DEF_FALSE);         //���� DEF_FALSE��������ִ��
             }
             OSSchedUnlock(&err);
             success = DEF_TRUE;
             break;

        case OS_TMR_STATE_COMPLETED:            //�����ʱ������ɵ�һ�ζ�ʱ                     
        case OS_TMR_STATE_STOPPED:              //�����ʱ���ѱ�ֹͣ                 
             OSSchedUnlock(&err);               //����������
            *p_err   = OS_ERR_TMR_STOPPED;      //��������Ϊ����ʱ���ѱ�ֹͣ��
             success = DEF_TRUE;                //ִ�н����Ϊ DEF_TRUE
             break;

        case OS_TMR_STATE_UNUSED:               //����ö�ʱ��δ��������                    
             OSSchedUnlock(&err);               //����������
            *p_err   = OS_ERR_TMR_INACTIVE;     //��������Ϊ����ʱ��δ���
             success = DEF_FALSE;               //ִ�н����Ϊ DEF_FALSE
             break;

        default:                                //�����ʱ��״̬����Ԥ��
             OSSchedUnlock(&err);               //����������
            *p_err   = OS_ERR_TMR_INVALID_STATE;//��������Ϊ����ʱ��״̬�Ƿ���
             success = DEF_FALSE;               //ִ�н����Ϊ DEF_FALSE
             break;
    }
    return (success);                           //����ִ�н��
} 

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 CLEAR TIMER FIELDS
*
* Description: This function is called to clear all timer fields.
*
* Argument(s): p_tmr    is a pointer to the timer to clear
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrClr (OS_TMR  *p_tmr)
{
    p_tmr->State          = OS_TMR_STATE_UNUSED;            /* Clear timer fields                                     */
    p_tmr->Type           = OS_OBJ_TYPE_NONE;
    p_tmr->NamePtr        = (CPU_CHAR          *)((void *)"?TMR");
    p_tmr->Dly            = (OS_TICK            )0;
    p_tmr->Match          = (OS_TICK            )0;
    p_tmr->Remain         = (OS_TICK            )0;
    p_tmr->Period         = (OS_TICK            )0;
    p_tmr->Opt            = (OS_OPT             )0;
    p_tmr->CallbackPtr    = (OS_TMR_CALLBACK_PTR)0;
    p_tmr->CallbackPtrArg = (void              *)0;
    p_tmr->NextPtr        = (OS_TMR            *)0;
    p_tmr->PrevPtr        = (OS_TMR            *)0;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                         ADD/REMOVE TIMER TO/FROM DEBUG TABLE
*
* Description: These functions are called by uC/OS-III to add or remove a timer to/from a timer debug table.
*
* Arguments  : p_tmr     is a pointer to the timer to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u
void  OS_TmrDbgListAdd (OS_TMR  *p_tmr)
{
    p_tmr->DbgPrevPtr               = (OS_TMR *)0;
    if (OSTmrDbgListPtr == (OS_TMR *)0) {
        p_tmr->DbgNextPtr           = (OS_TMR *)0;
    } else {
        p_tmr->DbgNextPtr           =  OSTmrDbgListPtr;
        OSTmrDbgListPtr->DbgPrevPtr =  p_tmr;
    }
    OSTmrDbgListPtr                 =  p_tmr;
}



void  OS_TmrDbgListRemove (OS_TMR  *p_tmr)
{
    OS_TMR  *p_tmr_next;
    OS_TMR  *p_tmr_prev;


    p_tmr_prev = p_tmr->DbgPrevPtr;
    p_tmr_next = p_tmr->DbgNextPtr;

    if (p_tmr_prev == (OS_TMR *)0) {
        OSTmrDbgListPtr = p_tmr_next;
        if (p_tmr_next != (OS_TMR *)0) {
            p_tmr_next->DbgPrevPtr = (OS_TMR *)0;
        }
        p_tmr->DbgNextPtr = (OS_TMR *)0;

    } else if (p_tmr_next == (OS_TMR *)0) {
        p_tmr_prev->DbgNextPtr = (OS_TMR *)0;
        p_tmr->DbgPrevPtr      = (OS_TMR *)0;

    } else {
        p_tmr_prev->DbgNextPtr =  p_tmr_next;
        p_tmr_next->DbgPrevPtr =  p_tmr_prev;
        p_tmr->DbgNextPtr      = (OS_TMR *)0;
        p_tmr->DbgPrevPtr      = (OS_TMR *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                             INITIALIZE THE TIMER MANAGER
*
* Description: This function is called by OSInit() to initialize the timer manager module.
*
* Argument(s): p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE
*                           OS_ERR_TMR_STK_INVALID       if you didn't specify a stack for the timer task
*                           OS_ERR_TMR_STK_SIZE_INVALID  if you didn't allocate enough space for the timer stack
*                           OS_ERR_PRIO_INVALID          if you specified the same priority as the idle task
*                           OS_ERR_xxx                   any error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrInit (OS_ERR  *p_err)
{
    OS_TMR_SPOKE_IX   i;
    OS_TMR_SPOKE     *p_spoke;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSTmrDbgListPtr = (OS_TMR *)0;
#endif

    if (OSCfg_TmrTaskRate_Hz > (OS_RATE_HZ)0) {
        OSTmrUpdateCnt = OSCfg_TickRate_Hz / OSCfg_TmrTaskRate_Hz;
    } else {
        OSTmrUpdateCnt = OSCfg_TickRate_Hz / (OS_RATE_HZ)10;
    }
    OSTmrUpdateCtr   = OSTmrUpdateCnt;

    OSTmrTickCtr     = (OS_TICK)0;

    OSTmrTaskTimeMax = (CPU_TS)0;

    for (i = 0u; i < OSCfg_TmrWheelSize; i++) {
        p_spoke                = &OSCfg_TmrWheel[i];
        p_spoke->NbrEntries    = (OS_OBJ_QTY)0;
        p_spoke->NbrEntriesMax = (OS_OBJ_QTY)0;
        p_spoke->FirstPtr      = (OS_TMR   *)0;
    }

                                                            /* ---------------- CREATE THE TIMER TASK --------------- */
    if (OSCfg_TmrTaskStkBasePtr == (CPU_STK*)0) {
       *p_err = OS_ERR_TMR_STK_INVALID;
        return;
    }

    if (OSCfg_TmrTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_TMR_STK_SIZE_INVALID;
        return;
    }

    if (OSCfg_TmrTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_TMR_PRIO_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSTmrTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III Timer Task"),
                 (OS_TASK_PTR )OS_TmrTask,
                 (void       *)0,
                 (OS_PRIO     )OSCfg_TmrTaskPrio,
                 (CPU_STK    *)OSCfg_TmrTaskStkBasePtr,
                 (CPU_STK_SIZE)OSCfg_TmrTaskStkLimit,
                 (CPU_STK_SIZE)OSCfg_TmrTaskStkSize,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_NO_TLS),
                 (OS_ERR     *)p_err);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                         INSERT A TIMER INTO THE TIMER WHEEL
*
* Description: This function is called to insert the timer into the timer wheel.  The timer is always inserted at the
*              beginning of the list.
*
* Arguments  : p_tmr          Is a pointer to the timer to insert.
*              -----
*
*              opt            Is either:
*
*                               OS_OPT_LINK_PERIODIC    Means to re-insert the timer after a period expired
*                               OS_OPT_LINK_DLY         Means to insert    the timer the first time
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrLink (OS_TMR  *p_tmr,  //��ʱ�����ƿ�ָ��
                  OS_OPT   opt)    //ѡ��
{
    OS_TMR_SPOKE     *p_spoke;
    OS_TMR           *p_tmr0;
    OS_TMR           *p_tmr1;
    OS_TMR_SPOKE_IX   spoke;



    p_tmr->State = OS_TMR_STATE_RUNNING;                           //���ö�ʱ��Ϊ����״̬
    if (opt == OS_OPT_LINK_PERIODIC) {                             //�����ʱ�����ٴβ���
        p_tmr->Match = p_tmr->Period + OSTmrTickCtr;               //ƥ��ʱ��Ӹ���������ֵ
    } else {                                                       //�����ʱ�����״β���
        if (p_tmr->Dly == (OS_TICK)0) {                            //�����ʱ���� Dly = 0
            p_tmr->Match = p_tmr->Period + OSTmrTickCtr;           //ƥ��ʱ��Ӹ���������ֵ
        } else {                                                   //�����ʱ���� Dly != 0
            p_tmr->Match = p_tmr->Dly    + OSTmrTickCtr;           //ƥ��ʱ��Ӹ� Dly
        }
    }
    spoke  = (OS_TMR_SPOKE_IX)(p_tmr->Match % OSCfg_TmrWheelSize); //ͨ����ϣ�㷨���ý��ö�ʱ��
    p_spoke = &OSCfg_TmrWheel[spoke];                              //���뵽��ʱ���ֵ��ĸ��б�

    if (p_spoke->FirstPtr ==  (OS_TMR *)0) {                //����б�Ϊ�գ�
        p_tmr->NextPtr      = (OS_TMR *)0;                  //ֱ�ӽ��ö�ʱ����Ϊ�б�ĵ�һ��Ԫ�ء�
        p_tmr->PrevPtr      = (OS_TMR *)0;
        p_spoke->FirstPtr   = p_tmr;
        p_spoke->NbrEntries = 1u;
    } else {                                                //����б�ǿ�
        p_tmr->Remain  = p_tmr->Match                       //�����ʱ�� p_tmr ��ʣ��ʱ��
                       - OSTmrTickCtr;
        p_tmr1         = p_spoke->FirstPtr;                 //ȡ�б���׸�Ԫ�ص� p_tmr1
        while (p_tmr1 != (OS_TMR *)0) {                     //��� p_tmr1 �ǿ�
            p_tmr1->Remain = p_tmr1->Match                  //��� p_tmr1 ��ʣ��ʱ��
                           - OSTmrTickCtr;
            if (p_tmr->Remain > p_tmr1->Remain) {           //��� p_tmr ��ʣ��ʱ����� p_tmr1 ��
                if (p_tmr1->NextPtr  != (OS_TMR *)0) {      //��� p_tmr1 ����ǿ�
                    p_tmr1            = p_tmr1->NextPtr;    //ȡp_tmr1��һ����ʱ��Ϊ�µ�p_tmr1������һ��ѭ��
                } else {                                    //��� p_tmr1 ����Ϊ��
                    p_tmr->NextPtr    = (OS_TMR *)0;        //�� p_tmr �嵽 p_tmr1 �ĺ��棬����ѭ��
                    p_tmr->PrevPtr    =  p_tmr1;
                    p_tmr1->NextPtr   =  p_tmr;             
                    p_tmr1            = (OS_TMR *)0;        
                }
            } else {                                        //��� p_tmr ��ʣ��ʱ�䲻���� p_tmr1 �ģ�
                if (p_tmr1->PrevPtr == (OS_TMR *)0) {       //�� p_tmr ���뵽 p_tmr1 ��ǰһ��������ѭ����
                    p_tmr->PrevPtr    = (OS_TMR *)0;
                    p_tmr->NextPtr    = p_tmr1;
                    p_tmr1->PrevPtr   = p_tmr;
                    p_spoke->FirstPtr = p_tmr;
                } else {                                   
                    p_tmr0            = p_tmr1->PrevPtr;
                    p_tmr->PrevPtr    = p_tmr0;
                    p_tmr->NextPtr    = p_tmr1;
                    p_tmr0->NextPtr   = p_tmr;
                    p_tmr1->PrevPtr   = p_tmr;
                }
                p_tmr1 = (OS_TMR *)0;                      
            }
        }
        p_spoke->NbrEntries++;                              //�б�Ԫ�س�Ա����1
    }
    if (p_spoke->NbrEntriesMax < p_spoke->NbrEntries) {     //�����б��Ա�����ֵ��ʷ��¼
        p_spoke->NbrEntriesMax = p_spoke->NbrEntries;
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                              RESET TIMER LIST PEAK DETECTOR
*
* Description: This function is used to reset the peak detector for the number of entries in each spoke.
*
* Arguments  : void
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TmrResetPeak (void)
{
    OS_TMR_SPOKE     *p_spoke;
    OS_TMR_SPOKE_IX   i;



    for (i = 0u; i < OSCfg_TmrWheelSize; i++) {
        p_spoke                = (OS_TMR_SPOKE *)&OSCfg_TmrWheel[i];
        p_spoke->NbrEntriesMax = (OS_OBJ_QTY    )0u;
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                         REMOVE A TIMER FROM THE TIMER WHEEL
*
* Description: This function is called to remove the timer from the timer wheel.
*
* Arguments  : p_tmr          Is a pointer to the timer to remove.
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrUnlink (OS_TMR  *p_tmr)   //��ʱ�����ƿ�ָ��
{
    OS_TMR_SPOKE    *p_spoke;
    OS_TMR          *p_tmr1;
    OS_TMR          *p_tmr2;
    OS_TMR_SPOKE_IX  spoke;



    spoke   = (OS_TMR_SPOKE_IX)(p_tmr->Match % OSCfg_TmrWheelSize); //�����ʱһ����ͨ����ϣ�㷨�ҳ�
    p_spoke = &OSCfg_TmrWheel[spoke];                               //�ö�ʱ���ڶ�ʱ���ֵ��ĸ��б�

    if (p_spoke->FirstPtr == p_tmr) {                       //��� p_tmr ���б���׸�Ԫ��
        p_tmr1            = (OS_TMR *)p_tmr->NextPtr;       //ȡ p_tmr ��һ��Ԫ��Ϊ p_tmr1(����Ϊ��)
        p_spoke->FirstPtr = (OS_TMR *)p_tmr1;               //���׸�Ϊ p_tmr1 
        if (p_tmr1 != (OS_TMR *)0) {                        //��� p_tmr1 ȷ���ǿ�
            p_tmr1->PrevPtr = (OS_TMR *)0;                  //p_tmr1 ��ǰ�����
        }
    } else {                                                //��� p_tmr �����б���׸�Ԫ��
        p_tmr1          = (OS_TMR *)p_tmr->PrevPtr;         //�� p_tmr ���б��Ƴ������� p_tmr 
        p_tmr2          = (OS_TMR *)p_tmr->NextPtr;         //ǰ�������Ԫ��������һ��.
        p_tmr1->NextPtr = p_tmr2;
        if (p_tmr2 != (OS_TMR *)0) {                        
            p_tmr2->PrevPtr = (OS_TMR *)p_tmr1;
        }
    }
    p_tmr->State   = OS_TMR_STATE_STOPPED;   //��λ p_tmr ��ָ��             
    p_tmr->NextPtr = (OS_TMR *)0;
    p_tmr->PrevPtr = (OS_TMR *)0;
    p_spoke->NbrEntries--;                   //�б�Ԫ�س�Ա��1
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 TIMER MANAGEMENT TASK
*
* Description: This task is created by OS_TmrInit().
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrTask (void  *p_arg)
{
    CPU_BOOLEAN          done;
    OS_ERR               err;
    OS_TMR_CALLBACK_PTR  p_fnct;
    OS_TMR_SPOKE        *p_spoke;
    OS_TMR              *p_tmr;
    OS_TMR              *p_tmr_next;
    OS_TMR_SPOKE_IX      spoke;
    CPU_TS               ts;
    CPU_TS               ts_start;
    CPU_TS               ts_end;



    p_arg = p_arg;                                               /* Not using 'p_arg', prevent compiler warning       */
    while (DEF_ON) {
        (void)OSTaskSemPend((OS_TICK )0,                         /* Wait for signal indicating time to update tmrs    */
                            (OS_OPT  )OS_OPT_PEND_BLOCKING,
                            (CPU_TS *)&ts,
                            (OS_ERR *)&err);

        OSSchedLock(&err);
        ts_start = OS_TS_GET();
        OSTmrTickCtr++;                                          /* Increment the current time                        */
        spoke    = (OS_TMR_SPOKE_IX)(OSTmrTickCtr % OSCfg_TmrWheelSize);
        p_spoke  = &OSCfg_TmrWheel[spoke];
        p_tmr    = p_spoke->FirstPtr;
        done     = DEF_FALSE;
        while (done == DEF_FALSE) {
            if (p_tmr != (OS_TMR *)0) {
                p_tmr_next = (OS_TMR *)p_tmr->NextPtr;           /* Point to next tmr to update because current ...   */
                                                                 /* ... timer could get unlinked from the wheel.      */
                if (OSTmrTickCtr == p_tmr->Match) {              /* Process each timer that expires                   */
                    OS_TmrUnlink(p_tmr);                         /* Remove from current wheel spoke                   */
                    if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
                        OS_TmrLink(p_tmr,
                                   OS_OPT_LINK_PERIODIC);        /* Recalculate new position of timer in wheel        */
                    } else {
                        p_tmr->State = OS_TMR_STATE_COMPLETED;   /* Indicate that the timer has completed             */
                    }
                    p_fnct = p_tmr->CallbackPtr;                 /* Execute callback function if available            */
                    if (p_fnct != (OS_TMR_CALLBACK_PTR)0) {
                        (*p_fnct)((void *)p_tmr,
                                  p_tmr->CallbackPtrArg);
                    }
                    p_tmr = p_tmr_next;                          /* See if next timer matches                         */
                } else {
                    done  = DEF_TRUE;
                }
            } else {
                done = DEF_TRUE;
            }
        }
        ts_end = OS_TS_GET() - ts_start;                         /* Measure execution time of timer task              */
        OSSchedUnlock(&err);
        if (OSTmrTaskTimeMax < ts_end) {
            OSTmrTaskTimeMax = ts_end;
        }
    }
}

#endif
