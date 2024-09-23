#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;

#if 0
void rt_tick_increase(void)
{
    rt_ubase_t i;
    struct rt_thread *thread;
    rt_tick ++;

    /* ɨ������б��������̵߳�remaining_tick�������Ϊ0�����1 */
	for(i=0; i<RT_THREAD_PRIORITY_MAX; i++)
	{
        thread = rt_list_entry( rt_thread_priority_table[i].next,
								 struct rt_thread,
							     tlist);
		if(thread->remaining_tick > 0)
		{
			thread->remaining_tick --;
            if(thread->remaining_tick == 0)
            {
                //rt_schedule_insert_thread(thread);
                rt_thread_ready_priority_group |= thread->number_mask;
            }
		}
	}
    
    /* ������� */
	rt_schedule();
}

#else
void rt_tick_increase(void)
{
    /* ϵͳʱ����������1����,rt_tick��һ��ȫ�ֱ��� */
    ++ rt_tick;
    
    /* ɨ��ϵͳ��ʱ���б� */
    rt_timer_check();
}
#endif

/**
 * �ú������ڷ��ز���ϵͳ���������ڵĵ�ǰ tick��tick��һ��ȫ�ֱ���
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* return the global tick */
    return rt_tick;
}


