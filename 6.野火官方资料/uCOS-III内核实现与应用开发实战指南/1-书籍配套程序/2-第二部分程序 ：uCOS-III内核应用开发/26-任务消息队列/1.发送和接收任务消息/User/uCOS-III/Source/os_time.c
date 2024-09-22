/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TIME MANAGEMENT
*
* File    : OS_TIME.C
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
const  CPU_CHAR  *os_time__c = "$Id: $";
#endif

/*
************************************************************************************************************************
*                                                  DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the specified number of
*              system ticks expires.  This, of course, directly equates to delaying the current task for some time to
*              expire.  No delay will result if the specified delay is 0.  If the specified delay is greater than 0
*              then, a context switch will result.
*
* Arguments  : dly       is a value in 'clock ticks' that the task will either delay for or, the target match value
*                        of the tick counter (OSTickCtr).  Note that specifying 0 means the task is not to delay.
*
*                        depending on the option argument, the task will wake up when OSTickCtr reaches:
*
*                            OS_OPT_TIME_DLY      : OSTickCtr + dly
*                            OS_OPT_TIME_TIMEOUT  : OSTickCtr + dly
*                            OS_OPT_TIME_MATCH    : dly
*                            OS_OPT_TIME_PERIODIC : OSTCBCurPtr->TickCtrPrev + dly
*
*              opt       specifies whether 'dly' represents absolute or relative time; default option marked with *** :
*
*                        *** OS_OPT_TIME_DLY        specifies a relative time from the current value of OSTickCtr.
*                            OS_OPT_TIME_TIMEOUT    same as OS_OPT_TIME_DLY.
*                            OS_OPT_TIME_MATCH      indicates that 'dly' specifies the absolute value that OSTickCtr
*                                                   must reach before the task will be resumed.
*                            OS_OPT_TIME_PERIODIC   indicates that 'dly' specifies the periodic value that OSTickCtr
*                                                   must reach before the task will be resumed.
*
*              p_err     is a pointer to a variable that will contain an error code from this call.
*
*                            OS_ERR_NONE            the call was successful and the delay occurred.
*                            OS_ERR_OPT_INVALID     if you specified an invalid option for this function.
*                            OS_ERR_SCHED_LOCKED    can't delay when the scheduler is locked.
*                            OS_ERR_TIME_DLY_ISR    if you called this function from an ISR.
*                            OS_ERR_TIME_ZERO_DLY   if you specified a delay of zero.
*
* Returns    : none
************************************************************************************************************************
*/

void  OSTimeDly (OS_TICK   dly,                        //��ʱ��ʱ�ӽ�����
                 OS_OPT    opt,                        //ѡ��
                 OS_ERR   *p_err)                      //���ش�������
{
    CPU_SR_ALLOC();                                    //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                                                       //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                                                       //�����ж�ʱ����ֵ��ԭ��                

#ifdef OS_SAFETY_CRITICAL                              //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {                        //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();                //ִ�а�ȫ����쳣����
        return;                                        //���أ���ִ����ʱ����
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u                 //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü��   
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {        //�������ʱ���������ж��б�����
       *p_err = OS_ERR_TIME_DLY_ISR;                   //��������Ϊ�����жϺ�������ʱ��
        return;                                        //���أ���ִ����ʱ����
    }
#endif
    /* ������������ʱ��������ʱ */
    if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0u) {  //�������������   
       *p_err = OS_ERR_SCHED_LOCKED;                   //��������Ϊ��������������
        return;                                        //���أ���ִ����ʱ����
    }

    switch (opt) {                                     //������ʱѡ����� opt �������
        case OS_OPT_TIME_DLY:                          //���ѡ�����ʱ�䣨����������ʱ�೤ʱ�䣩
        case OS_OPT_TIME_TIMEOUT:                      //���ѡ��ʱ��ʵ��ͬ�ϣ�
        case OS_OPT_TIME_PERIODIC:                     //���ѡ����������ʱ
             if (dly == (OS_TICK)0u) {                 //������� dly Ϊ0��0��ζ����ʱ��    
                *p_err = OS_ERR_TIME_ZERO_DLY;         //��������Ϊ��0��ʱ��
                 return;                               //���أ���ִ����ʱ����
             }
             break;

        case OS_OPT_TIME_MATCH:                        //���ѡ�����ʱ�䣨ƥ��ϵͳ��ʼ���У�OSStart()�����ʱ�ӽ�������       
             break;

        default:                                       //���ѡ�����Χ
            *p_err = OS_ERR_OPT_INVALID;               //��������Ϊ��ѡ��Ƿ���
             return;                                   //���أ���ִ����ʱ����
    }

    OS_CRITICAL_ENTER();                               //�����ٽ��
    OSTCBCurPtr->TaskState = OS_TASK_STATE_DLY;        //�޸ĵ�ǰ���������״̬Ϊ��ʱ״̬
    OS_TickListInsert(OSTCBCurPtr,                     //����ǰ������뵽�����б�
                      dly,
                      opt,
                      p_err);
    if (*p_err != OS_ERR_NONE) {                       //�����ǰ�����������б�ʱ���ִ���
         OS_CRITICAL_EXIT_NO_SCHED();                  //�˳��ٽ�Σ��޵��ȣ�
         return;                                       //���أ���ִ����ʱ����
    }
    OS_RdyListRemove(OSTCBCurPtr);                     //�Ӿ����б��Ƴ���ǰ����  
    OS_CRITICAL_EXIT_NO_SCHED();                       //�˳��ٽ�Σ��޵��ȣ�
    OSSched();                                         //�����л� 
   *p_err = OS_ERR_NONE;                               //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                             DELAY TASK FOR SPECIFIED TIME
*
* Description: This function is called to delay execution of the currently running task until some time expires.  This
*              call allows you to specify the delay time in HOURS, MINUTES, SECONDS and MILLISECONDS instead of ticks.
*
* Arguments  : hours     specifies the number of hours that the task will be delayed (max. is 999 if the tick rate is
*                        1000 Hz or less otherwise, a higher value would overflow a 32-bit unsigned counter).
*
*              minutes   specifies the number of minutes (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              seconds   specifies the number of seconds (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              milli     specifies the number of milliseconds (max. 999 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              opt       specifies time delay bit-field options logically OR'd; default options marked with *** :
*
*                        *** OS_OPT_TIME_DLY        specifies a relative time from the current value of OSTickCtr.
*                            OS_OPT_TIME_TIMEOUT    same as OS_OPT_TIME_DLY.
*                            OS_OPT_TIME_MATCH      indicates that the delay specifies the absolute value that OSTickCtr
*                                                   must reach before the task will be resumed.
*                            OS_OPT_TIME_PERIODIC   indicates that the delay specifies the periodic value that OSTickCtr
*                                                   must reach before the task will be resumed.
*
*                        *** OS_OPT_TIME_HMSM_STRICT            strictly allow only hours        (0...99)
*                                                                                   minutes      (0...59)
*                                                                                   seconds      (0...59)
*                                                                                   milliseconds (0...999)
*                            OS_OPT_TIME_HMSM_NON_STRICT        allow any value of  hours        (0...999)
*                                                                                   minutes      (0...9999)
*                                                                                   seconds      (0...65535)
*                                                                                   milliseconds (0...4294967295)
*
*              p_err     is a pointer to a variable that will receive an error code from this call.
*
*                            OS_ERR_NONE                        If the function returns from the desired delay
*                            OS_ERR_OPT_INVALID                 If you specified an invalid option for 'opt'
*                            OS_ERR_SCHED_LOCKED                Can't delay when the scheduler is locked
*                            OS_ERR_TIME_DLY_ISR                If called from an ISR
*                            OS_ERR_TIME_INVALID_HOURS          If you didn't specify a valid value for 'hours'
*                            OS_ERR_TIME_INVALID_MINUTES        If you didn't specify a valid value for 'minutes'
*                            OS_ERR_TIME_INVALID_SECONDS        If you didn't specify a valid value for 'seconds'
*                            OS_ERR_TIME_INVALID_MILLISECONDS   If you didn't specify a valid value for 'milli'
*                            OS_ERR_TIME_ZERO_DLY               If hours, minutes, seconds and milli are all 0
*
* Returns    : none
*
* Note(s)    : 1) The resolution on the milliseconds depends on the tick rate.  For example, you can't do a 10 mS delay
*                 if the ticker interrupts every 100 mS.  In this case, the delay would be set to 0.  The actual delay
*                 is rounded to the nearest tick.
*
*              2) Although this function allows you to delay a task for many, many hours, it's not recommended to put
*                 a task to sleep for that long.
************************************************************************************************************************
*/

#if OS_CFG_TIME_DLY_HMSM_EN > 0u              //���ʹ�ܣ�Ĭ��ʹ�ܣ��� OSTimeDlyHMSM() ����
void  OSTimeDlyHMSM (CPU_INT16U   hours,      //��ʱСʱ��
                     CPU_INT16U   minutes,    //������
                     CPU_INT16U   seconds,    //����
                     CPU_INT32U   milli,      //������
                     OS_OPT       opt,        //ѡ��
                     OS_ERR      *p_err)      //���ش�������
{
#if OS_CFG_ARG_CHK_EN > 0u         //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲�����⹦��            
    CPU_BOOLEAN  opt_invalid;      //�����������ڲ������
    CPU_BOOLEAN  opt_non_strict;
#endif
    OS_OPT       opt_time;
    OS_RATE_HZ   tick_rate;
    OS_TICK      ticks;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //���أ���ִ����ʱ����
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u                     //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü�� 
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {            //�������ʱ���������ж��б�����
       *p_err = OS_ERR_TIME_DLY_ISR;                       //��������Ϊ�����жϺ�������ʱ��
        return;                                            //���أ���ִ����ʱ����
    }
#endif
    /* ������������ʱ��������ʱ */
    if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0u) {      //������������� 
       *p_err = OS_ERR_SCHED_LOCKED;                       //��������Ϊ��������������
        return;                                            //���أ���ִ����ʱ����
    }

    opt_time = opt & OS_OPT_TIME_MASK;                     //����ѡ��������ʱʱ�������йص�λ
    switch (opt_time) {                                    //������ʱѡ����� opt �������
        case OS_OPT_TIME_DLY:                              //���ѡ�����ʱ�䣨����������ʱ�೤ʱ�䣩
        case OS_OPT_TIME_TIMEOUT:                          //���ѡ��ʱ��ʵ��ͬ�ϣ�
        case OS_OPT_TIME_PERIODIC:                         //���ѡ����������ʱ
             if (milli == (CPU_INT32U)0u) {                //���������Ϊ0
                 if (seconds == (CPU_INT16U)0u) {          //�������Ϊ0
                     if (minutes == (CPU_INT16U)0u) {      //���������Ϊ0
                         if (hours == (CPU_INT16U)0u) {    //���Сʱ��Ϊ0
                            *p_err = OS_ERR_TIME_ZERO_DLY; //��������Ϊ��0��ʱ��
                             return;                       //���أ���ִ����ʱ����
                         }
                     }
                 }
             }
             break;

        case OS_OPT_TIME_MATCH:                            //���ѡ�����ʱ�䣨��ϵͳ��ʼ���У�OSStart()ʱ��Ϊ��㣩       
             break;

        default:                                           //���ѡ�����Χ
            *p_err = OS_ERR_OPT_INVALID;                   //��������Ϊ��ѡ��Ƿ���
             return;                                       //���أ���ִ����ʱ����
    }

#if OS_CFG_ARG_CHK_EN > 0u                                             //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲�����⹦��   
    opt_invalid = DEF_BIT_IS_SET_ANY(opt, ~OS_OPT_TIME_OPTS_MASK);     //����ѡ��λ�Ժ�����λ�Ƿ���λ
    if (opt_invalid == DEF_YES) {                                      //�����ѡ��λ�Ժ�����λ�б���λ��
       *p_err = OS_ERR_OPT_INVALID;                                    //��������Ϊ��ѡ��Ƿ���
        return;                                                        //���أ���ִ����ʱ����
    }

    opt_non_strict = DEF_BIT_IS_SET(opt, OS_OPT_TIME_HMSM_NON_STRICT); //����й�ʱ�����ȡֵ��Χ��ѡ��λ
    if (opt_non_strict != DEF_YES) {                                   //���ѡ��ѡ���� OS_OPT_TIME_HMSM_STRICT
         if (milli   > (CPU_INT32U)999u) {                             //���������>999
            *p_err = OS_ERR_TIME_INVALID_MILLISECONDS;                 //��������Ϊ�������������á�
             return;                                                   //���أ���ִ����ʱ����
         }
         if (seconds > (CPU_INT16U)59u) {                              //�������>59
            *p_err = OS_ERR_TIME_INVALID_SECONDS;                      //��������Ϊ�����������á�
             return;                                                   //���أ���ִ����ʱ����
         }
         if (minutes > (CPU_INT16U)59u) {                              //���������>59
            *p_err = OS_ERR_TIME_INVALID_MINUTES;                      //��������Ϊ�������������á�
             return;                                                   //���أ���ִ����ʱ����
         }
         if (hours   > (CPU_INT16U)99u) {                              //���Сʱ��>99
            *p_err = OS_ERR_TIME_INVALID_HOURS;                        //��������Ϊ��Сʱ�������á�
             return;                                                   //���أ���ִ����ʱ����
         }
    } else {                                                           //���ѡ��ѡ���� OS_OPT_TIME_HMSM_	NON_STRICT
         if (minutes > (CPU_INT16U)9999u) {                            //���������>9999
            *p_err = OS_ERR_TIME_INVALID_MINUTES;                      //��������Ϊ�������������á�
             return;                                                   //���أ���ִ����ʱ����
         }
         if (hours   > (CPU_INT16U)999u) {                             //���Сʱ��>999
            *p_err = OS_ERR_TIME_INVALID_HOURS;                        //��������Ϊ��Сʱ�������á�
             return;                                                   //���أ���ִ����ʱ����
         }
    }
#endif


    /*����ʱʱ��ת����ʱ�ӽ�����*/             
    tick_rate = OSCfg_TickRate_Hz;                                                                                 //��ȡʱ�ӽ��ĵ�Ƶ��
    ticks     = ((OS_TICK)hours * (OS_TICK)3600u + (OS_TICK)minutes * (OS_TICK)60u + (OS_TICK)seconds) * tick_rate //����ʱʱ��ת����ʱ�ӽ�����
              + (tick_rate * ((OS_TICK)milli + (OS_TICK)500u / tick_rate)) / (OS_TICK)1000u;

    if (ticks > (OS_TICK)0u) {                       //�����ʱ������>0
        OS_CRITICAL_ENTER();                         //�����ٽ��
        OSTCBCurPtr->TaskState = OS_TASK_STATE_DLY;  //�޸ĵ�ǰ���������״̬Ϊ��ʱ״̬
        OS_TickListInsert(OSTCBCurPtr,               //����ǰ������뵽�����б�
                          ticks,
                          opt_time,
                          p_err);
        if (*p_err != OS_ERR_NONE) {                 //�����ǰ�����������б�ʱ���ִ���
             OS_CRITICAL_EXIT_NO_SCHED();            //�˳��ٽ�Σ��޵��ȣ�
             return;                                 //���أ���ִ����ʱ����
        }
        OS_RdyListRemove(OSTCBCurPtr);               //�Ӿ����б��Ƴ���ǰ����  
        OS_CRITICAL_EXIT_NO_SCHED();                 //�˳��ٽ�Σ��޵��ȣ�
        OSSched();                                   //�����л� 
       *p_err = OS_ERR_NONE;                         //��������Ϊ���޴���
    } else {                                         //�����ʱ������=0
       *p_err = OS_ERR_TIME_ZERO_DLY;                //��������Ϊ��0��ʱ��
    }
}
#endif
/*$PAGE*/
/*
************************************************************************************************************************
*                                                RESUME A DELAYED TASK
*
* Description: This function is used resume a task that has been delayed through a call to either OSTimeDly() or
*              OSTimeDlyHMSM().  Note that cannot call this function to resume a task that is waiting for an event
*              with timeout.
*
* Arguments  : p_tcb    is a pointer to the TCB of the task to resume.
*
*              p_err    is a pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE                  Task has been resumed
*                           OS_ERR_STATE_INVALID         Task is in an invalid state
*                           OS_ERR_TIME_DLY_RESUME_ISR   If called from an ISR
*                           OS_ERR_TIME_NOT_DLY          Task is not waiting for time to expire
*                           OS_ERR_TASK_SUSPENDED        Task cannot be resumed, it was suspended by OSTaskSuspend()
*
* Note(s)    : none
************************************************************************************************************************
*/

#if OS_CFG_TIME_DLY_RESUME_EN > 0u                       //���ʹ�ܣ�Ĭ��ʹ�ܣ��� OSTimeDlyResume() ����
void  OSTimeDlyResume (OS_TCB  *p_tcb,                   //������ƿ�
                       OS_ERR  *p_err)                   //��������
{
    CPU_SR_ALLOC();                                      //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                                                         //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                                                         //�����ж�ʱ����ֵ��ԭ��   

#ifdef OS_SAFETY_CRITICAL                                //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {                          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();                  //ִ�а�ȫ����쳣����
        return;                                          //���أ�����ִ�в��ɹ�
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u                   //���ʹ�ܣ�Ĭ��ʹ�ܣ����ж��зǷ����ü�� 
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {          //������������ж��б�����
       *p_err = OS_ERR_TIME_DLY_RESUME_ISR;              //��������Ϊ�����ж��н�����ʱ��
        return;                                          //���أ�����ִ�в��ɹ�
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                               //���ʹ�ܣ�Ĭ��ʹ�ܣ��˲�����⹦�� 
    if (p_tcb == (OS_TCB *)0) {                          //�������Ϊ��
       *p_err = OS_ERR_TASK_NOT_DLY;                     //��������Ϊ����������ʱ��
        return;                                          //���أ�����ִ�в��ɹ�
    }
#endif

    CPU_CRITICAL_ENTER();                                //���ж�
    if (p_tcb == OSTCBCurPtr) {                          //�������Ϊ��ǰ����   
       *p_err = OS_ERR_TASK_NOT_DLY;                     //��������Ϊ����������ʱ��
        CPU_CRITICAL_EXIT();                             //���ж�
        return;                                          //���أ�����ִ�в��ɹ�
    }

    switch (p_tcb->TaskState) {                          //�������������״̬���ദ��
        case OS_TASK_STATE_RDY:                          //��������ھ���״̬ 
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_TASK_NOT_DLY;                //��������Ϊ����������ʱ��
             break;

        case OS_TASK_STATE_DLY:                          //�������Ϊ��ʱ״̬
             OS_CRITICAL_ENTER_CPU_EXIT();               //�����ٽ�β����ж�
             p_tcb->TaskState = OS_TASK_STATE_RDY;       //�޸�����Ϊ����״̬
             OS_TickListRemove(p_tcb);                   //��������ӽ����б��Ƴ�
             OS_RdyListInsert(p_tcb);                    //����������뵽�����б�
             OS_CRITICAL_EXIT_NO_SCHED();                //�˳��ٽ�Σ��޵��ȣ�
            *p_err = OS_ERR_NONE;                        //��������Ϊ���޴��� 
             break;

        case OS_TASK_STATE_PEND:                         //�������Ϊ�����޵ȴ�״̬
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_TASK_NOT_DLY;                //��������Ϊ����������ʱ��
             break;

        case OS_TASK_STATE_PEND_TIMEOUT:                 //�������Ϊ�����޵ȴ�״̬
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_TASK_NOT_DLY;                //��������Ϊ����������ʱ��
             break;

        case OS_TASK_STATE_SUSPENDED:                    //�������Ϊ������״̬
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_TASK_NOT_DLY;                //��������Ϊ����������ʱ��
             break;

        case OS_TASK_STATE_DLY_SUSPENDED:                //�������Ϊ��ʱ�б�����״̬
             OS_CRITICAL_ENTER_CPU_EXIT();               //�����ٽ�β����ж�
             p_tcb->TaskState = OS_TASK_STATE_SUSPENDED; //�޸�����Ϊ������״̬
             OS_TickListRemove(p_tcb);                   //��������ӽ����б��Ƴ�
             OS_CRITICAL_EXIT_NO_SCHED();                //�˳��ٽ�Σ��޵��ȣ�
            *p_err            = OS_ERR_TASK_SUSPENDED;   //��������Ϊ�����񱻹���
             break;

        case OS_TASK_STATE_PEND_SUSPENDED:               //�������Ϊ�����޵ȴ��б�����״̬
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_TASK_NOT_DLY;                //��������Ϊ����������ʱ��
             break;

        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:       //�������Ϊ�����޵ȴ��б�����״̬
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_TASK_NOT_DLY;                //��������Ϊ����������ʱ��
             break;

        default:                                         //�������״̬����Χ
             CPU_CRITICAL_EXIT();                        //���ж�
            *p_err = OS_ERR_STATE_INVALID;               //��������Ϊ������״̬�Ƿ���
             break;
    }

    OSSched();                                           //�����л�
}
#endif
/*$PAGE*/
/*
************************************************************************************************************************
*                                               GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the counter which keeps track of
*              the number of clock ticks.
*
* Arguments  : p_err    is a pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE           If the call was successful
*
* Returns    : The current value of OSTickCtr
************************************************************************************************************************
*/

OS_TICK  OSTimeGet (OS_ERR  *p_err) //��ȡ��ǰ��ʱ�ӽ��ļ���ֵ
{
    OS_TICK  ticks;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                    //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                    //�����ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((OS_TICK)0);            //����0������ִ�в��ɹ�
    }
#endif

    CPU_CRITICAL_ENTER();               //���ж�
    ticks = OSTickCtr;                  //��ȡ��ǰ��ʱ�ӽ��ļ���ֵ
    CPU_CRITICAL_EXIT();                //���ж�
   *p_err = OS_ERR_NONE;                //��������Ϊ���޴��� 
    return (ticks);                     //���ص�ǰ��ʱ�ӽ��ļ���ֵ
}

/*
************************************************************************************************************************
*                                                   SET SYSTEM CLOCK
*
* Description: This function sets the counter which keeps track of the number of clock ticks.
*
* Arguments  : ticks    is the desired tick value
*
*              p_err    is a pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE           If the call was successful
*
* Returns    : none
************************************************************************************************************************
*/

void  OSTimeSet (OS_TICK   ticks,  //���õ�ǰ��ʱ�ӽ��ļ���ֵ
                 OS_ERR   *p_err)  //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
                    //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
                    //�����ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���  
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�����ִ�в��ɹ�
    }
#endif

    CPU_CRITICAL_ENTER();               //���ж�
    OSTickCtr = ticks;                  //���õ�ǰ��ʱ�ӽ��ļ���ֵ
    CPU_CRITICAL_EXIT();                //���ж�
   *p_err     = OS_ERR_NONE;            //��������Ϊ���޴��� 
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 PROCESS SYSTEM TICK
*
* Description: This function is used to signal to uC/OS-III the occurrence of a 'system tick' (also known as a
*              'clock tick').  This function should be called by the tick ISR.
*
* Arguments  : none
*
* Returns    : none
************************************************************************************************************************
*/

void  OSTimeTick (void)
{
    OS_ERR  err;
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
    CPU_TS  ts;
#endif


    OSTimeTickHook();                                //�����û����Զ���Ĺ��Ӻ��������ڴ˺����ж�����ʱ�ӽ��ĵ���ʱ���¼�

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                 //���ʹ�ܣ�Ĭ��ʹ�ܣ����жϷ����ӳ�

    ts = OS_TS_GET();                                //��ȡʱ���     
    OS_IntQPost((OS_OBJ_TYPE) OS_OBJ_TYPE_TICK,      //�����ź�����ʱ���͵��ж϶��У��˳��жϺ������ȼ���ߵ��ӳٷ�������
                (void      *)&OSRdyList[OSPrioCur],  //�������͸�ʱ�ӽ������� OS_TickTask()��OS_TickTask() ���յ����ź���
                (void      *) 0,                     //�ͻ����ִ�С��жϷ����ӳٿ��Լ����ж�ʱ�䣬���жϼ��¼�תΪ����
                (OS_MSG_SIZE) 0u,                    //������˲���ϵͳ��ʵʱ�ԡ�
                (OS_FLAGS   ) 0u,
                (OS_OPT     ) 0u,
                (CPU_TS     ) ts,
                (OS_ERR    *)&err);

#else                                                //������ã�Ĭ��ʹ�ܣ����жϷ����ӳ�

   (void)OSTaskSemPost((OS_TCB *)&OSTickTaskTCB,     //ֱ�ӷ����ź�����ʱ�ӽ������� OS_TickTask()    
                       (OS_OPT  ) OS_OPT_POST_NONE,
                       (OS_ERR *)&err);


#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u                 //���ʹ�ܣ�Ĭ��ʹ�ܣ��ˣ�ͬ���ȼ�����ʱ��Ƭ��ת����
    OS_SchedRoundRobin(&OSRdyList[OSPrioCur]);       //��鵱ǰ�����ʱ��Ƭ�Ƿ�ľ�������ľ��͵���ͬ���ȼ���������������
#endif

#if OS_CFG_TMR_EN > 0u                               //���ʹ�ܣ�Ĭ��ʹ�ܣ��������ʱ��
    OSTmrUpdateCtr--;                                //�����ʱ���������Լ�
    if (OSTmrUpdateCtr == (OS_CTR)0u) {              //��������ʱ������������0
        OSTmrUpdateCtr = OSTmrUpdateCnt;             //���������ʱ��������
        OSTaskSemPost((OS_TCB *)&OSTmrTaskTCB,       //�����ź����������ʱ������ OS_TmrTask()
                      (OS_OPT  ) OS_OPT_POST_NONE,
                      (OS_ERR *)&err);
    }
#endif

#endif
}















