/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                             MEMORY PARTITION MANAGEMENT
*
* File    : OS_MEM.C
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
const  CPU_CHAR  *os_mem__c = "$Id: $";
#endif


#if OS_CFG_MEM_EN > 0u
/*
************************************************************************************************************************
*                                               CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-III.
*
* Arguments   : p_mem    is a pointer to a memory partition control block which is allocated in user memory space.
*
*               p_name   is a pointer to an ASCII string to provide a name to the memory partition.
*
*               p_addr   is the starting address of the memory partition
*
*               n_blks   is the number of memory blocks to create from the partition.
*
*               blk_size is the size (in bytes) of each block in the memory partition.
*
*               p_err    is a pointer to a variable containing an error message which will be set by this function to
*                        either:
*
*                            OS_ERR_NONE                    if the memory partition has been created correctly.
*                            OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the memory partition after you
*                                                             called OSSafetyCriticalStart().
*                            OS_ERR_MEM_INVALID_BLKS        user specified an invalid number of blocks (must be >= 2)
*                            OS_ERR_MEM_INVALID_P_ADDR      if you are specifying an invalid address for the memory
*                                                           storage of the partition or, the block does not align on a
*                                                           pointer boundary
*                            OS_ERR_MEM_INVALID_SIZE        user specified an invalid block size
*                                                             - must be greater than the size of a pointer
*                                                             - must be able to hold an integral number of pointers
* Returns    : none
************************************************************************************************************************
*/

void  OSMemCreate (OS_MEM       *p_mem,    //�ڴ�������ƿ�
                   CPU_CHAR     *p_name,   //�����ڴ����
                   void         *p_addr,   //�ڴ�����׵�ַ
                   OS_MEM_QTY    n_blks,   //�ڴ����Ŀ
                   OS_MEM_SIZE   blk_size, //�ڴ���С����λ���ֽڣ�
                   OS_ERR       *p_err)    //���ش�������
{
#if OS_CFG_ARG_CHK_EN > 0u      
    CPU_DATA       align_msk;
#endif
    OS_MEM_QTY     i;
    OS_MEM_QTY     loops;
    CPU_INT08U    *p_blk;
    void         **p_link;               //����ָ�룬���ָ���ָ��
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //���أ�ִֹͣ��
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
       *p_err = OS_ERR_MEM_CREATE_ISR;         //��������Ϊ�����ж��д�������
        return;                                //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                             //���ʹ���˲������
    if (p_addr == (void *)0) {                         //��� p_addr Ϊ��      
       *p_err   = OS_ERR_MEM_INVALID_P_ADDR;           //��������Ϊ��������ַ�Ƿ���
        return;                                        //���أ�ִֹͣ��
    }
    if (n_blks < (OS_MEM_QTY)2) {                      //����������ڴ����Ŀ����2
       *p_err = OS_ERR_MEM_INVALID_BLKS;               //��������Ϊ���ڴ����Ŀ�Ƿ���
        return;                                        //���أ�ִֹͣ��
    }
    if (blk_size < sizeof(void *)) {                   //����ڴ��ռ�С��ָ���
       *p_err = OS_ERR_MEM_INVALID_SIZE;               //��������Ϊ���ڴ�ռ�Ƿ���
        return;                                        //���أ�ִֹͣ��
    }
    align_msk = sizeof(void *) - 1u;                   //��ʼ����ڴ��ַ�Ƿ����
    if (align_msk > 0u) {
        if (((CPU_ADDR)p_addr & align_msk) != 0u){     //��������׵�ַû����
           *p_err = OS_ERR_MEM_INVALID_P_ADDR;         //��������Ϊ��������ַ�Ƿ���
            return;                                    //���أ�ִֹͣ��
        }
        if ((blk_size & align_msk) != 0u) {            //����ڴ���ַû����     
           *p_err = OS_ERR_MEM_INVALID_SIZE;           //��������Ϊ���ڴ���С�Ƿ���
            return;                                    //���أ�ִֹͣ��
        }
    }
#endif
    /* �������ڴ�鴮����һ���������� */
    p_link = (void **)p_addr;                          //�ڴ�����׵�ַתΪ����ָ��
    p_blk  = (CPU_INT08U *)p_addr;                     //�׸��ڴ���ַ
    loops  = n_blks - 1u;
    for (i = 0u; i < loops; i++) {                     //���ڴ��������ɵ�������
        p_blk +=  blk_size;                            //��һ�ڴ���ַ
       *p_link = (void  *)p_blk;                       //�ڵ�ǰ�ڴ�鱣����һ���ڴ���ַ
        p_link = (void **)(void *)p_blk;               //��һ���ڴ��ĵ�ַתΪ����ָ�� 
    }
   *p_link             = (void *)0;                    //���һ���ڴ��ָ���

    OS_CRITICAL_ENTER();                               //�����ٽ��
    p_mem->Type        = OS_OBJ_TYPE_MEM;              //���ö��������   
    p_mem->NamePtr     = p_name;                       //�����ڴ����������     
    p_mem->AddrPtr     = p_addr;                       //�洢�ڴ�������׵�ַ     
    p_mem->FreeListPtr = p_addr;                       //��ʼ�������ڴ��ص��׵�ַ 
    p_mem->NbrFree     = n_blks;                       //�洢�����ڴ�����Ŀ   
    p_mem->NbrMax      = n_blks;                       //�洢�ڴ�������Ŀ
    p_mem->BlkSize     = blk_size;                     //�洢�ڴ��Ŀռ��С  

#if OS_CFG_DBG_EN > 0u            //���ʹ���˵��Դ���ͱ��� 
    OS_MemDbgListAdd(p_mem);      //���ڴ�����������ڴ����˫������б�
#endif

    OSMemQty++;                   //�ڴ���������Ŀ��1

    OS_CRITICAL_EXIT_NO_SCHED();  //�˳��ٽ�Σ��޵��ȣ�
   *p_err = OS_ERR_NONE;          //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : p_mem   is a pointer to the memory partition control block
*
*               p_err   is a pointer to a variable containing an error message which will be set by this function to
*                       either:
*
*                       OS_ERR_NONE               if the memory partition has been created correctly.
*                       OS_ERR_MEM_INVALID_P_MEM  if you passed a NULL pointer for 'p_mem'
*                       OS_ERR_MEM_NO_FREE_BLKS   if there are no more free memory blocks to allocate to the caller
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
************************************************************************************************************************
*/

void  *OSMemGet (OS_MEM  *p_mem, //�ڴ�������
                 OS_ERR  *p_err) //���ش�������
{
    void    *p_blk;
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return ((void *)0);              //����0���д��󣩣�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                 //���ʹ���˲������
    if (p_mem == (OS_MEM *)0) {            //��� p_mem Ϊ��            
       *p_err  = OS_ERR_MEM_INVALID_P_MEM; //��������Ϊ���ڴ�����Ƿ���
        return ((void *)0);                //����0���д��󣩣�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                    //���ж�
    if (p_mem->NbrFree == (OS_MEM_QTY)0) {   //���û�п��е��ڴ��
        CPU_CRITICAL_EXIT();                 //���ж�
       *p_err = OS_ERR_MEM_NO_FREE_BLKS;     //��������Ϊ��û�п����ڴ�顱  
        return ((void *)0);                  //����0���д��󣩣�ִֹͣ��
    }
    p_blk              = p_mem->FreeListPtr; //������п����ڴ�飬�ͻ�ȡ��
    p_mem->FreeListPtr = *(void **)p_blk;    //���������ڴ��ָ��
    p_mem->NbrFree--;                        //�����ڴ����Ŀ��1
    CPU_CRITICAL_EXIT();                     //���ж�
   *p_err = OS_ERR_NONE;                     //��������Ϊ���޴���
    return (p_blk);                          //���ػ�ȡ�����ڴ��
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : p_mem    is a pointer to the memory partition control block
*
*               p_blk    is a pointer to the memory block being released.
*
*               p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE               if the memory block was inserted into the partition
*                            OS_ERR_MEM_FULL           if you are returning a memory block to an already FULL memory
*                                                      partition (You freed more blocks than you allocated!)
*                            OS_ERR_MEM_INVALID_P_BLK  if you passed a NULL pointer for the block to release.
*                            OS_ERR_MEM_INVALID_P_MEM  if you passed a NULL pointer for 'p_mem'
************************************************************************************************************************
*/

void  OSMemPut (OS_MEM  *p_mem,   //�ڴ�������
                void    *p_blk,   //Ҫ�˻ص��ڴ��
                OS_ERR  *p_err)   //���ش�������
{
    CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
                    //����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
                    // SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��

#ifdef OS_SAFETY_CRITICAL                //���ʹ���˰�ȫ���
    if (p_err == (OS_ERR *)0) {          //�����������ʵ��Ϊ��
        OS_SAFETY_CRITICAL_EXCEPTION();  //ִ�а�ȫ����쳣����
        return;                          //���أ�ִֹͣ��
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //���ʹ���˲������
    if (p_mem == (OS_MEM *)0) {             //��� p_mem Ϊ��                
       *p_err  = OS_ERR_MEM_INVALID_P_MEM;  //��������Ϊ���ڴ�����Ƿ���
        return;                             //���أ�ִֹͣ��
    }
    if (p_blk == (void *)0) {               //����ڴ��Ϊ��
       *p_err  = OS_ERR_MEM_INVALID_P_BLK;  //��������Ϊ"�ڴ��Ƿ�"
        return;                             //���أ�ִֹͣ��
    }
#endif

    CPU_CRITICAL_ENTER();                    //���ж�
    if (p_mem->NbrFree >= p_mem->NbrMax) {   //������е��ڴ���Ѿ��˳�����                
        CPU_CRITICAL_EXIT();                 //���ж�
       *p_err = OS_ERR_MEM_FULL;             //��������Ϊ���ڴ����������
        return;                              //���أ�ִֹͣ��
    }
    *(void **)p_blk    = p_mem->FreeListPtr; //���ڴ���������ڴ������
    p_mem->FreeListPtr = p_blk;              //�ڴ���˻ص��������ǰ��
    p_mem->NbrFree++;                        //�����ڴ����Ŀ��1
    CPU_CRITICAL_EXIT();                     //���ж�
   *p_err              = OS_ERR_NONE;        //��������Ϊ���޴���
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                           ADD MEMORY PARTITION TO DEBUG LIST
*
* Description : This function is called by OSMemCreate() to add the memory partition to the debug table.
*
* Arguments   : p_mem    Is a pointer to the memory partition
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u                                  //���ʹ���˵��Դ���ͱ��� 
void  OS_MemDbgListAdd (OS_MEM  *p_mem)                 //���ڴ���������뵽�ڴ��������б����ǰ��
{
    p_mem->DbgPrevPtr               = (OS_MEM *)0;      //���ö�����Ϊ�б����ǰ��
    if (OSMemDbgListPtr == (OS_MEM *)0) {               //����б�Ϊ��
        p_mem->DbgNextPtr           = (OS_MEM *)0;      //�ö��е���һ��Ԫ��ҲΪ��
    } else {                                            //����б�ǿ�
        p_mem->DbgNextPtr           =  OSMemDbgListPtr; //�б�ԭ������Ԫ����Ϊ�ö��е���һ��Ԫ��
        OSMemDbgListPtr->DbgPrevPtr =  p_mem;           //ԭ������Ԫ�ص�ǰ���Ϊ�˸ö���
    }
    OSMemDbgListPtr                 =  p_mem;           //�ö����Ϊ�б������Ԫ��
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                           INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-III to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_MemInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSMemDbgListPtr = (OS_MEM   *)0;
#endif

    OSMemQty        = (OS_OBJ_QTY)0;
   *p_err           = OS_ERR_NONE;
}
#endif
