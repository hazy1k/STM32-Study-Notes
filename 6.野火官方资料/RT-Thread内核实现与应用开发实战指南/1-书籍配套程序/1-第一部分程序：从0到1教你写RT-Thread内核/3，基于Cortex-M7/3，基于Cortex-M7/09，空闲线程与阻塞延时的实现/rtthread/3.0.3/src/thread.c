#include <rtthread.h>
#include <rthw.h>

extern struct rt_thread *rt_current_thread;

rt_err_t rt_thread_init(struct rt_thread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size)
{
	/* �̶߳����ʼ�� */
	/* �߳̽ṹ�忪ͷ���ֵĳ�Ա����rt_object_t���� */
	rt_object_init((rt_object_t)thread, RT_Object_Class_Thread, name);
    rt_list_init(&(thread->tlist));
	
	thread->entry = (void *)entry;
	thread->parameter = parameter;

	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/* ��ʼ���߳�ջ���������߳�ջָ�� */
	thread->sp = (void *)rt_hw_stack_init( thread->entry, 
		                                   thread->parameter,
							               (void *)((char *)thread->stack_addr + thread->stack_size - 4) );
	
	return RT_EOK;
}


void rt_thread_delay(rt_tick_t tick)
{
    struct rt_thread *thread;    
    
    /* ��ȡ��ǰ�̵߳��߳̿��ƿ� */    
    thread = rt_current_thread;
    
    /* ������ʱʱ�� */
    thread->remaining_tick = tick;
	
	/* ����ϵͳ���� */
	rt_schedule();
}
