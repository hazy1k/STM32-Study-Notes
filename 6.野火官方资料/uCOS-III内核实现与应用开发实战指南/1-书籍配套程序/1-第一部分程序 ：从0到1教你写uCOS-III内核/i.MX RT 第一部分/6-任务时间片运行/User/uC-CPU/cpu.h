#ifndef CPU_H
#define CPU_H

typedef  unsigned  short       CPU_INT16U;
typedef  unsigned  int         CPU_INT32U;
typedef  unsigned  char        CPU_INT08U;

typedef  CPU_INT32U  CPU_ADDR;

/* ��ջ���������ض��� */
typedef  CPU_INT32U             CPU_STK;
typedef  CPU_ADDR               CPU_STK_SIZE;

typedef  volatile  CPU_INT32U  CPU_REG32;





/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        CPU_IntDis       (void); 	/* ��cpu_a.asm���� */
void        CPU_IntEn        (void);	/* ��cpu_a.asm���� */





#endif /* CPU_H */

