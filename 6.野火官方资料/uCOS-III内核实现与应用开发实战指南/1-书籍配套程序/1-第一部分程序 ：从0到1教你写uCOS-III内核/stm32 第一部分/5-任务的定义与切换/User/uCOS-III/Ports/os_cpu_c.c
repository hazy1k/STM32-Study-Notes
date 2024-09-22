#include "os.h"

/* �����ջ��ʼ�� */
CPU_STK *OSTaskStkInit (OS_TASK_PTR  p_task,
                        void         *p_arg,
                        CPU_STK      *p_stk_base,
                        CPU_STK_SIZE stk_size)
{
	CPU_STK  *p_stk;

	p_stk = &p_stk_base[stk_size];
															/* �쳣����ʱ�Զ�����ļĴ���                              */
	*--p_stk = (CPU_STK)0x01000000u;                        /* xPSR��bit24������1                                     */
	*--p_stk = (CPU_STK)p_task;                             /* �������ڵ�ַ                                         */
	*--p_stk = (CPU_STK)0x14141414u;                        /* R14 (LR)                                               */
	*--p_stk = (CPU_STK)0x12121212u;                        /* R12                                                    */
	*--p_stk = (CPU_STK)0x03030303u;                        /* R3                                                     */
	*--p_stk = (CPU_STK)0x02020202u;                        /* R2                                                     */
	*--p_stk = (CPU_STK)0x01010101u;                        /* R1                                                     */
	*--p_stk = (CPU_STK)p_arg;                              /* R0 : �����β�                                          */
															/* �쳣����ʱ���ֶ�����ļĴ���                            */
	*--p_stk = (CPU_STK)0x11111111u;                        /* R11                                                    */
	*--p_stk = (CPU_STK)0x10101010u;                        /* R10                                                    */
	*--p_stk = (CPU_STK)0x09090909u;                        /* R9                                                     */
	*--p_stk = (CPU_STK)0x08080808u;                        /* R8                                                     */
	*--p_stk = (CPU_STK)0x07070707u;                        /* R7                                                     */
	*--p_stk = (CPU_STK)0x06060606u;                        /* R6                                                     */
	*--p_stk = (CPU_STK)0x05050505u;                        /* R5                                                     */
	*--p_stk = (CPU_STK)0x04040404u;                        /* R4                                                     */
	return (p_stk);
}

