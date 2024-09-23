#include <rtthread.h>
#include <rthw.h>

/* �жϼ����� */
volatile rt_uint8_t rt_interrupt_nest;

/**
 * ��BSP�ļ����жϷ���������ʱ����øú���
 * 
 * @note �벻Ҫ��Ӧ�ó����е��øú���
 *
 * @see rt_interrupt_leave
 */
void rt_interrupt_enter(void)
{
    rt_base_t level;
    
    
    /* ���ж� */
    level = rt_hw_interrupt_disable();
    
    /* �жϼ�����++ */
    rt_interrupt_nest ++;
    
    /* ���ж� */
    rt_hw_interrupt_enable(level);
}


/**
 * ��BSP�ļ����жϷ������뿪ʱ����øú���
 *
 * @note �벻Ҫ��Ӧ�ó����е��øú���
 *
 * @see rt_interrupt_enter
 */
void rt_interrupt_leave(void)
{
    rt_base_t level;
    
    
    /* ���ж� */
    level = rt_hw_interrupt_disable();
    
    /* �жϼ�����-- */
    rt_interrupt_nest --;

    /* ���ж� */
    rt_hw_interrupt_enable(level);
}


