#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;




void rt_tick_increase(void)
{
    struct rt_thread *thread;    
    
    
    /* ϵͳʱ����������1����,rt_tick��һ��ȫ�ֱ��� */
    ++ rt_tick;

    /* ��ȡ��ǰ�߳��߳̿��ƿ� */
    thread = rt_thread_self();

    /* ʱ��Ƭ�ݼ� */
    -- thread->remaining_tick;
    
    /* ���ʱ��Ƭ���꣬������ʱ��Ƭ��Ȼ���ó������� */
    if (thread->remaining_tick == 0)
    {
        /* ����ʱ��Ƭ */
        thread->remaining_tick = thread->init_tick;

        /* �ó������� */
        rt_thread_yield();
    }
    
    /* ɨ��ϵͳ��ʱ���б� */
    rt_timer_check();
}


/**
 * �ú������ڷ��ز���ϵͳ���������ڵĵ�ǰ tick��tick��һ��ȫ�ֱ���
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* ����ȫ�ֱ��� rt_tick */
    return rt_tick;
}


