#ifndef  CPU_DEF_MODULE_PRESENT
#define  CPU_DEF_MODULE_PRESENT

/*
*********************************************************************************************************
*                                          CPU ������
*********************************************************************************************************
*/                                                        
#define  CPU_WORD_SIZE_08                          1u
#define  CPU_WORD_SIZE_16                          2u
#define  CPU_WORD_SIZE_32                          4u
#define  CPU_WORD_SIZE_64                          8u

/*
*********************************************************************************************************
*                                          �ٽ������
*********************************************************************************************************
*/  
#define  CPU_CRITICAL_METHOD_NONE                  0u  
#define  CPU_CRITICAL_METHOD_INT_DIS_EN            1u   /* DIS/EN       �ж�*/
#define  CPU_CRITICAL_METHOD_STATUS_STK            2u   /* Push/Pop     �ж�״̬�������ջ */
#define  CPU_CRITICAL_METHOD_STATUS_LOCAL          3u   /* Save/Restore �ж�״̬���ֲ�����*/

#endif /* CPU_DEF_MODULE_PRESENT */


