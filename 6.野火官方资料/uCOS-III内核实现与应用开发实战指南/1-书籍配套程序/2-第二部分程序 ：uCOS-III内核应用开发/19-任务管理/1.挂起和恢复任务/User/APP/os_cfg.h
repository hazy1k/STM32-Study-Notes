/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                          All rights reserved.  Protected by international copyright laws.
*
*                                                  CONFIGURATION  FILE
*
* File    : OS_CFG.H
* By      : JJL
* Version : V3.01.2
*
* LICENSING TERMS:
* ---------------
*               uC/OS-III is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
************************************************************************************************************************
*/

#ifndef OS_CFG_H
#define OS_CFG_H


                                             /* ---------------------------- MISCELLANEOUS -------------------------- */
#define OS_CFG_APP_HOOKS_EN             1u   //ʹ��/���ù��Ӻ���
#define OS_CFG_ARG_CHK_EN               1u   //ʹ��/���ò������
#define OS_CFG_CALLED_FROM_ISR_CHK_EN   1u   //ʹ��/���ü���ж��зǷ����ù���
#define OS_CFG_DBG_EN                   1u   //ʹ��/���õ��Դ���ͱ���                                     
#define OS_CFG_ISR_POST_DEFERRED_EN     1u   //ʹ��/�����ж��ӳٷ���
#define OS_CFG_OBJ_TYPE_CHK_EN          1u   //ʹ��/���ö������ͼ��            
#define OS_CFG_TS_EN                    1u   //ʹ��/����ʱ���

#define OS_CFG_PEND_MULTI_EN            1u   //ʹ��/���õȴ�����ں˶���

#define OS_CFG_PRIO_MAX                32u   //�����������ȼ��������Ŀ

#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN  1u   //ʹ��/���ò�����������ʱ�书��
#define OS_CFG_SCHED_ROUND_ROBIN_EN     1u   //ʹ��/����ʱ��Ƭ��ת����
#define OS_CFG_STK_SIZE_MIN            64u   //���������ջ��С�ռ䣨��λ��CPU_STK��


                                             /* ----------------------------- EVENT FLAGS --------------------------- */
#define OS_CFG_FLAG_EN                  1u   //ʹ��/�����¼���־��
#define OS_CFG_FLAG_DEL_EN              1u   //ʹ��/���� OSFlagDel() ����
#define OS_CFG_FLAG_MODE_CLR_EN         1u   //ʹ��/���ñ�־λ��0����ģʽ
#define OS_CFG_FLAG_PEND_ABORT_EN       1u   //ʹ��/���� OSFlagPendAbort() ����


                                             /* -------------------------- MEMORY MANAGEMENT ------------------------ */
#define OS_CFG_MEM_EN                   1u   //ʹ��/�����ڴ����


                                             /* --------------------- MUTUAL EXCLUSION SEMAPHORES ------------------- */
#define OS_CFG_MUTEX_EN                 1u   //ʹ��/���û����ź���
#define OS_CFG_MUTEX_DEL_EN             1u   //ʹ��/���� OSMutexDel() ����    
#define OS_CFG_MUTEX_PEND_ABORT_EN      1u   //ʹ��/���� OSMutexPendAbort() ���� 


                                             /* --------------------------- MESSAGE QUEUES -------------------------- */
#define OS_CFG_Q_EN                     1u   //ʹ��/������Ϣ����
#define OS_CFG_Q_DEL_EN                 1u   //ʹ��/���� OSQDel() ����
#define OS_CFG_Q_FLUSH_EN               1u   //ʹ��/���� OSQFlush() ����
#define OS_CFG_Q_PEND_ABORT_EN          1u   //ʹ��/���� OSQPendAbort() ����


                                             /* ----------------------------- SEMAPHORES ---------------------------- */
#define OS_CFG_SEM_EN                   1u   //ʹ��/���ö�ֵ�ź���
#define OS_CFG_SEM_DEL_EN               1u   //ʹ��/���� OSSemDel() ���� 
#define OS_CFG_SEM_PEND_ABORT_EN        1u   //ʹ��/���� OSSemPendAbort() ����
#define OS_CFG_SEM_SET_EN               1u   //ʹ��/���� OSSemSet() ����


                                             /* -------------------------- TASK MANAGEMENT -------------------------- */
#define OS_CFG_STAT_TASK_EN             1u   //ʹ��/����ͳ������
#define OS_CFG_STAT_TASK_STK_CHK_EN     1u   //ʹ��/���ô�ͳ���������ջ

#define OS_CFG_TASK_CHANGE_PRIO_EN      1u   //ʹ��/���ú��� OSTaskChangePrio()                                   
#define OS_CFG_TASK_DEL_EN              1u   //ʹ��/���ú��� OSTaskDel()                                          
#define OS_CFG_TASK_Q_EN                1u   //ʹ��/���ú��� OSTaskQXXXX()                                        
#define OS_CFG_TASK_Q_PEND_ABORT_EN     1u   //ʹ��/���ú��� OSTaskQPendAbort()                                   
#define OS_CFG_TASK_PROFILE_EN          1u   //ʹ��/����������ƿ�ļ������
#define OS_CFG_TASK_REG_TBL_SIZE        1u   //��������Ĵ�������Ŀ                            
#define OS_CFG_TASK_SEM_PEND_ABORT_EN   1u   //ʹ��/���ú��� OSTaskSemPendAbort()
#define OS_CFG_TASK_SUSPEND_EN          1u   //ʹ��/���ú��� OSTaskSuspend() �� OSTaskResume()                  


                                             /* -------------------------- TIME MANAGEMENT -------------------------- */
#define OS_CFG_TIME_DLY_HMSM_EN         1u   //ʹ��/���� OSTimeDlyHMSM() ����
#define OS_CFG_TIME_DLY_RESUME_EN       1u   //ʹ��/���� OSTimeDlyResume() ����

                                             /* ------------------------- TIMER MANAGEMENT -------------------------- */
#define OS_CFG_TMR_EN                   1u   //ʹ��/���������ʱ��
#define OS_CFG_TMR_DEL_EN               1u   //ʹ��/���� OSTmrDel() ����

#endif
