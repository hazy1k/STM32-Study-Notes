/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                              MESSAGE HANDLING SERVICES
*
* File    : OS_MSG.C
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
const  CPU_CHAR  *os_msg__c = "$Id: $";
#endif


#if OS_MSG_EN > 0u
/*$PAGE*/
/*
************************************************************************************************************************
*                                            INITIALIZE THE POOL OF 'OS_MSG'
*
* Description: This function is called by OSInit() to initialize the free list of OS_MSGs.
*
* Argument(s): p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_MSG_POOL_NULL_PTR
*                            OS_ERR_MSG_POOL_EMPTY
*                            OS_ERR_NONE
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_MsgPoolInit (OS_ERR  *p_err)   //���ش�������
{
    OS_MSG      *p_msg1;
    OS_MSG      *p_msg2;
    OS_MSG_QTY   i;
    OS_MSG_QTY   loops;



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return;                         //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                    //���ʹ���˲������
    if (OSCfg_MsgPoolBasePtr == (OS_MSG *)0) {//�����Ϣ�ز�����
       *p_err = OS_ERR_MSG_POOL_NULL_PTR;     //��������Ϊ����Ϣ��ָ��Ϊ�ա�
        return;                               //���أ�ִֹͣ��
    }
    if (OSCfg_MsgPoolSize == (OS_MSG_QTY)0) { //�����Ϣ�ز��ܴ����Ϣ
       *p_err = OS_ERR_MSG_POOL_EMPTY;        //��������Ϊ����Ϣ��Ϊ�ա�
        return;                               //���أ�ִֹͣ��
    }
#endif
    /* ����Ϣ�������Ϣ�������ɵ�������������� */
    p_msg1 = OSCfg_MsgPoolBasePtr;           
    p_msg2 = OSCfg_MsgPoolBasePtr;
    p_msg2++;
    loops  = OSCfg_MsgPoolSize - 1u;
    for (i = 0u; i < loops; i++) {            //��ʼ��ÿһ����Ϣ 
        p_msg1->NextPtr = p_msg2;
        p_msg1->MsgPtr  = (void      *)0;
        p_msg1->MsgSize = (OS_MSG_SIZE)0u;
        p_msg1->MsgTS   = (CPU_TS     )0u;
        p_msg1++;
        p_msg2++;
    }
    p_msg1->NextPtr = (OS_MSG    *)0;         //���һ����Ϣ
    p_msg1->MsgPtr  = (void      *)0;
    p_msg1->MsgSize = (OS_MSG_SIZE)0u;
    p_msg1->MsgTS   = (CPU_TS     )0u;
    /* ��ʼ����Ϣ������ */
    OSMsgPool.NextPtr    =  OSCfg_MsgPoolBasePtr;
    OSMsgPool.NbrFree    =  OSCfg_MsgPoolSize;
    OSMsgPool.NbrUsed    = (OS_MSG_QTY)0;
    OSMsgPool.NbrUsedMax = (OS_MSG_QTY)0;
   *p_err                =  OS_ERR_NONE;      //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        RELEASE ALL MESSAGE IN MESSAGE QUEUE
*
* Description: This function returns all the messages in a message queue to the free list.
*
* Arguments  : p_msg_q       is a pointer to the OS_MSG_Q structure containing messages to free.
*              -------
*
* Returns    : the number of OS_MSGs returned to the free list
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

OS_MSG_QTY  OS_MsgQFreeAll (OS_MSG_Q  *p_msg_q)
{
    OS_MSG      *p_msg;
    OS_MSG_QTY   qty;



    qty = p_msg_q->NbrEntries;                              /* Get the number of OS_MSGs being freed                  */
    if (p_msg_q->NbrEntries > (OS_MSG_QTY)0) {
        p_msg                   = p_msg_q->InPtr;           /* Point to end of message chain                          */
        p_msg->NextPtr          = OSMsgPool.NextPtr;
        OSMsgPool.NextPtr       = p_msg_q->OutPtr;          /* Point to beginning of message chain                    */
        OSMsgPool.NbrUsed      -= p_msg_q->NbrEntries;      /* Update statistics for free list of messages            */
        OSMsgPool.NbrFree      += p_msg_q->NbrEntries;
        p_msg_q->NbrEntries     = (OS_MSG_QTY)0;            /* Flush the message queue                                */
        p_msg_q->NbrEntriesMax  = (OS_MSG_QTY)0;
        p_msg_q->InPtr          = (OS_MSG   *)0;
        p_msg_q->OutPtr         = (OS_MSG   *)0;
    }
    return (qty);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INITIALIZE A MESSAGE QUEUE
*
* Description: This function is called to initialize a message queue
*
* Arguments  : p_msg_q      is a pointer to the message queue to initialize
*              -------
*
*              max          is the maximum number of entries that a message queue can have.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_MsgQInit (OS_MSG_Q    *p_msg_q, //��Ϣ����ָ��
                   OS_MSG_QTY   size)    //��Ϣ���пռ�
{
    p_msg_q->NbrEntriesSize = (OS_MSG_QTY)size; //��Ϣ���пɴ����Ϣ��Ŀ
    p_msg_q->NbrEntries     = (OS_MSG_QTY)0;    //��Ϣ����Ŀǰ������Ϣ��
    p_msg_q->NbrEntriesMax  = (OS_MSG_QTY)0;    //������Ϣ���������ʷ��¼
    p_msg_q->InPtr          = (OS_MSG   *)0;    //���е����ָ��
    p_msg_q->OutPtr         = (OS_MSG   *)0;    //���еĳ���ָ��
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                           RETRIEVE MESSAGE FROM MESSAGE QUEUE
*
* Description: This function retrieves a message from a message queue
*
* Arguments  : p_msg_q     is a pointer to the message queue where we want to extract the message from
*              -------
*
*              p_msg_size  is a pointer to where the size (in bytes) of the message will be placed
*
*              p_ts        is a pointer to where the time stamp will be placed
*
*              p_err       is a pointer to an error code that will be returned from this call.
*
*                              OS_ERR_Q_EMPTY
*                              OS_ERR_NONE
*
* Returns    : The message (a pointer)
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  *OS_MsgQGet (OS_MSG_Q     *p_msg_q,     //��Ϣ����
                   OS_MSG_SIZE  *p_msg_size,  //������Ϣ��С
                   CPU_TS       *p_ts,        //����ĳЩ������ʱ���
                   OS_ERR       *p_err)       //���ش�������
{
    OS_MSG  *p_msg;
    void    *p_void;



#ifdef OS_SAFETY_CRITICAL               //���ʹ�ܣ�Ĭ�Ͻ��ã��˰�ȫ���
    if (p_err == (OS_ERR *)0) {         //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION(); //ִ�а�ȫ����쳣����
        return ((void *)0);             //���ؿ���Ϣ��ִֹͣ��
    }
#endif

    if (p_msg_q->NbrEntries == (OS_MSG_QTY)0) {  //�����Ϣ����û����Ϣ
       *p_msg_size = (OS_MSG_SIZE)0;             //������Ϣ����Ϊ0
        if (p_ts != (CPU_TS *)0) {               //��� p_ts �ǿ�
           *p_ts  = (CPU_TS  )0;                 //���� p_ts
        }
       *p_err = OS_ERR_Q_EMPTY;                  //��������Ϊ������û��Ϣ��
        return ((void *)0);                      //���ؿ���Ϣ��ִֹͣ��
    }
    /* �����Ϣ��������Ϣ */
    p_msg           = p_msg_q->OutPtr;          //�Ӷ��еĳ��ڶ���ȡ��Ϣ           
    p_void          = p_msg->MsgPtr;            //��ȡ��Ϣ����
   *p_msg_size      = p_msg->MsgSize;           //��ȡ��Ϣ����
    if (p_ts != (CPU_TS *)0) {                  //��� p_ts �ǿ�
       *p_ts  = p_msg->MsgTS;                   //��ȡ��Ϣ������ʱ��ʱ���
    }

    p_msg_q->OutPtr = p_msg->NextPtr;           //�޸Ķ��еĳ���ָ��

    if (p_msg_q->OutPtr == (OS_MSG *)0) {       //�������û����Ϣ��
        p_msg_q->InPtr      = (OS_MSG   *)0;    //�������ָ��
        p_msg_q->NbrEntries = (OS_MSG_QTY)0;    //������Ϣ��
    } else {                                    //������л�����Ϣ
        p_msg_q->NbrEntries--;                  //���е���Ϣ����1
    }
    /* ����Ϣ������ȡ����Ϣ��Ϣ�󣬽���Ϣ�ͷŻ���Ϣ�ع�����ʹ�� */
    p_msg->NextPtr    = OSMsgPool.NextPtr;      //��Ϣ�����Ϣ��
    OSMsgPool.NextPtr = p_msg;
    OSMsgPool.NbrFree++;                        //��Ϣ�صĿ�����Ϣ����1
    OSMsgPool.NbrUsed--;                        //��Ϣ�ص�������Ϣ����1

   *p_err             = OS_ERR_NONE;            //��������Ϊ���޴���
    return (p_void);                            //������϶����
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                           DEPOSIT MESSAGE IN MESSAGE QUEUE
*
* Description: This function places a message in a message queue
*
* Arguments  : p_msg_q     is a pointer to the OS_TCB of the task to post the message to
*              -------
*
*              p_void      is a pointer to the message to send.
*
*              msg_size    is the size of the message (in bytes)
*
*              opt         specifies whether the message will be posted in FIFO or LIFO order
*
*                              OS_OPT_POST_FIFO
*                              OS_OPT_POST_LIFO
*
*              ts          is a timestamp as to when the message was posted
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_Q_MAX           if the queue is full
*                              OS_ERR_MSG_POOL_EMPTY  if we no longer have any OS_MSG to use
*                              OS_ERR_NONE            the message was deposited in the queue
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_MsgQPut (OS_MSG_Q     *p_msg_q,   //��Ϣ����ָ��
                  void         *p_void,    //��Ϣָ��
                  OS_MSG_SIZE   msg_size,  //��Ϣ��С����λ���ֽڣ�
                  OS_OPT        opt,       //ѡ��
                  CPU_TS        ts,        //��Ϣ������ʱ��ʱ���
                  OS_ERR       *p_err)     //���ش�������
{
    OS_MSG  *p_msg;
    OS_MSG  *p_msg_in;



#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //���أ�ִֹͣ��
    }
#endif

    if (p_msg_q->NbrEntries >= p_msg_q->NbrEntriesSize) { //�����Ϣ������û�п��ÿռ�
       *p_err = OS_ERR_Q_MAX;                             //��������Ϊ������������
        return;                                           //���أ�ִֹͣ��
    }

    if (OSMsgPool.NbrFree == (OS_MSG_QTY)0) {  //�����Ϣ��û�п�����Ϣ
       *p_err = OS_ERR_MSG_POOL_EMPTY;         //��������Ϊ����Ϣ��û����Ϣ��  
        return;                                //���أ�ִֹͣ��
    }
    /* ����Ϣ�ػ�ȡһ����Ϣ���ݴ��� p_msg ��*/
    p_msg             = OSMsgPool.NextPtr;          //����Ϣ���ƿ����Ϣ���Ƴ�               
    OSMsgPool.NextPtr = p_msg->NextPtr;             //ָ����һ����Ϣ��ȡ���׸���Ϣ��
    OSMsgPool.NbrFree--;                            //��Ϣ�ؿ�����Ϣ����1
    OSMsgPool.NbrUsed++;                            //��Ϣ�ر�����Ϣ����1
    if (OSMsgPool.NbrUsedMax < OSMsgPool.NbrUsed) { //������Ϣ���������Ŀ����ʷ��¼
        OSMsgPool.NbrUsedMax = OSMsgPool.NbrUsed;
    }
    /* ����ȡ����Ϣ���뵽��Ϣ���� */
    if (p_msg_q->NbrEntries == (OS_MSG_QTY)0) {             //�����Ϣ����Ŀǰû����Ϣ
        p_msg_q->InPtr         = p_msg;                     //�������ָ��ָ�����Ϣ
        p_msg_q->OutPtr        = p_msg;                     //����ָ��Ҳָ�����Ϣ
        p_msg_q->NbrEntries    = (OS_MSG_QTY)1;             //���е���Ϣ��Ϊ1
        p_msg->NextPtr         = (OS_MSG *)0;               //����Ϣ����һ����ϢΪ��
    } else {                                                //�����Ϣ����Ŀǰ������Ϣ
        if ((opt & OS_OPT_POST_LIFO) == OS_OPT_POST_FIFO) { //�����FIFO��ʽ������У�
            p_msg_in           = p_msg_q->InPtr;            //����Ϣ���뵽��Ӷˣ����
            p_msg_in->NextPtr  = p_msg;                     //ָ��ָ�����Ϣ��
            p_msg_q->InPtr     = p_msg;
            p_msg->NextPtr     = (OS_MSG *)0;
        } else {                                            //�����LIFO��ʽ������У�
            p_msg->NextPtr     = p_msg_q->OutPtr;           //����Ϣ���뵽���Ӷˣ�����
            p_msg_q->OutPtr    = p_msg;                     //ָ��ָ�����Ϣ��
        }
        p_msg_q->NbrEntries++;                              //��Ϣ���е���Ϣ��Ŀ��1
    }
    if (p_msg_q->NbrEntriesMax < p_msg_q->NbrEntries) {     //���¸���Ϣ���е������Ϣ
        p_msg_q->NbrEntriesMax = p_msg_q->NbrEntries;       //��Ŀ����ʷ��¼��
    }
    p_msg->MsgPtr  = p_void;                                //������Ϣ��д��Ϣ����
    p_msg->MsgSize = msg_size;                              //������Ϣ��д��Ϣ��С
    p_msg->MsgTS   = ts;                                    //��д��������Ϣʱ��ʱ���
   *p_err          = OS_ERR_NONE;                           //��������Ϊ���޴���
}
#endif
