#include "os.h"


/*
*********************************************************************************************************
*                                           ȫ�ֱ���
*********************************************************************************************************
*/

/* �������ȼ�����os.h����extern���� */
CPU_DATA   OSPrioTbl[OS_PRIO_TBL_SIZE];

/*
*********************************************************************************************************
*                                           ����ʵ��
*********************************************************************************************************
*/


/* ��ʼ�����ȼ��� */
void OS_PrioInit( void )
{
	CPU_DATA i;
	
	/* Ĭ��ȫ����ʼ��Ϊ0 */
	for( i=0u; i<OS_PRIO_TBL_SIZE; i++ )
	{
		OSPrioTbl[i] = (CPU_DATA)0;
	}
}

/* ��λ���ȼ�������Ӧ��λ */
void  OS_PrioInsert (OS_PRIO  prio)
{
    CPU_DATA  bit;
    CPU_DATA  bit_nbr;
    OS_PRIO   ix;


    /* ��ģ��������ȡ���ȼ���������±����� */
	ix             = prio / DEF_INT_CPU_NBR_BITS;
	
    /* ��������������ȼ�������DEF_INT_CPU_NBR_BITS֮�� */
	bit_nbr        = (CPU_DATA)prio & (DEF_INT_CPU_NBR_BITS - 1u);
	
	/* ��ȡ���ȼ������ȼ����ж�Ӧ��λ��λ�� */
    bit            = 1u;
    bit          <<= (DEF_INT_CPU_NBR_BITS - 1u) - bit_nbr;
	
	/* �����ȼ������ȼ����ж�Ӧ��λ��1 */
    OSPrioTbl[ix] |= bit;
}

/* ������ȼ�������Ӧ��λ */
void  OS_PrioRemove (OS_PRIO  prio)
{
    CPU_DATA  bit;
    CPU_DATA  bit_nbr;
    OS_PRIO   ix;


	/* ��ģ��������ȡ���ȼ���������±����� */
    ix             = prio / DEF_INT_CPU_NBR_BITS;
	
    /* ��������������ȼ�������DEF_INT_CPU_NBR_BITS֮�� */
	bit_nbr        = (CPU_DATA)prio & (DEF_INT_CPU_NBR_BITS - 1u);
	
	/* ��ȡ���ȼ������ȼ����ж�Ӧ��λ��λ�� */	
    bit            = 1u;
    bit          <<= (DEF_INT_CPU_NBR_BITS - 1u) - bit_nbr;
	
	/* �����ȼ������ȼ����ж�Ӧ��λ��0 */
    OSPrioTbl[ix] &= ~bit;
}


/* ��ȡ��ߵ����ȼ� */
OS_PRIO  OS_PrioGetHighest (void)
{
    CPU_DATA  *p_tbl;
    OS_PRIO    prio;


    prio  = (OS_PRIO)0;
    /* ��ȡ���ȼ����׵�ַ */
	p_tbl = &OSPrioTbl[0];
	
	/* �ҵ���ֵ��Ϊ0�������Ա */
    while (*p_tbl == (CPU_DATA)0) {
        prio += DEF_INT_CPU_NBR_BITS;
        p_tbl++;
    }
	
	/* �ҵ����ȼ�������λ����ߵ����ȼ� */
    prio += (OS_PRIO)CPU_CntLeadZeros(*p_tbl);
    return (prio);
}
