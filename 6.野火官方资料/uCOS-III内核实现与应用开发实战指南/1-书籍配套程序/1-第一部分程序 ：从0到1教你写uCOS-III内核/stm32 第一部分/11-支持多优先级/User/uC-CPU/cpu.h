#ifndef CPU_H
#define CPU_H

/*
*********************************************************************************************************
*                                          ������ͷ�ļ�
*********************************************************************************************************
*/
#include  <cpu_def.h>
#include  <cpu_cfg.h>

/*
*********************************************************************************************************
*                                        ��׼������������
*********************************************************************************************************
*/
typedef  unsigned  short       CPU_INT16U;
typedef  unsigned  int         CPU_INT32U;
typedef  unsigned  char        CPU_INT08U;
typedef  volatile  CPU_INT32U  CPU_REG32;

/*
*********************************************************************************************************
*                                        CPU �ֳ�����
*********************************************************************************************************
*/
#define  CPU_CFG_ADDR_SIZE              CPU_WORD_SIZE_32
#define  CPU_CFG_DATA_SIZE              CPU_WORD_SIZE_32
#define  CPU_CFG_DATA_SIZE_MAX          CPU_WORD_SIZE_64
typedef  CPU_INT32U                     CPU_DATA;

/*
*********************************************************************************************************
*                                 ���� CPU ��ַ & ���� ������
*********************************************************************************************************
*/
typedef  CPU_INT32U  CPU_ADDR;

/*
*********************************************************************************************************
*                                          CPU ��ջ����
*********************************************************************************************************
*/
typedef  CPU_INT32U             CPU_STK;
typedef  CPU_ADDR               CPU_STK_SIZE;

/*
*********************************************************************************************************
*                                          �ٽ������
*********************************************************************************************************
*/

                                                               
#define  CPU_CFG_CRITICAL_METHOD    CPU_CRITICAL_METHOD_STATUS_LOCAL

typedef  CPU_INT32U                 CPU_SR;                    

                                                                
#if     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
/* ����һ���ֲ����������ڱ���CPU�ж�״̬ */
#define  CPU_SR_ALLOC()             CPU_SR  cpu_sr = (CPU_SR)0
#else
#define  CPU_SR_ALLOC()
#endif


/* Save    CPU ״̬ & ʧ���ж� */
#define  CPU_INT_DIS()         do { cpu_sr = CPU_SR_Save(); } while (0)
/* Restore CPU ״̬  */
#define  CPU_INT_EN()          do { CPU_SR_Restore(cpu_sr); } while (0)

/* ʧ���ж� */
#define  CPU_CRITICAL_ENTER()  do { CPU_INT_DIS(); } while (0)
/* ����ʹ���ж� */
#define  CPU_CRITICAL_EXIT()   do { CPU_INT_EN();  } while (0) 

/*
*********************************************************************************************************
*                                             CPU ǰ/�� ��0����
*********************************************************************************************************
*/

#define  CPU_CFG_LEAD_ZEROS_ASM_PRESENT                        

                                                                
#define  CPU_CFG_TRAIL_ZEROS_ASM_PRESENT



/*
*********************************************************************************************************
*                                             ��������
*********************************************************************************************************
*/

void        CPU_IntDis       (void); 	           /* ��cpu_a.asm���� */
void        CPU_IntEn        (void);	           /* ��cpu_a.asm���� */



CPU_DATA    CPU_CntLeadZeros (CPU_DATA    val);    
CPU_DATA    CPU_CntTrailZeros(CPU_DATA  val);

/* ��cpu_a.asm���� */
CPU_SR      CPU_SR_Save      (void);
void        CPU_SR_Restore   (CPU_SR      cpu_sr);

#endif /* CPU_H */

