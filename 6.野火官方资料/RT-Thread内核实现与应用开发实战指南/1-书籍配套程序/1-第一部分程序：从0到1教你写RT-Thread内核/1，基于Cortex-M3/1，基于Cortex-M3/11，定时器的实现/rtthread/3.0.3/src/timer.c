#include <rtthread.h>
#include <rthw.h>

/* Ӳ����ʱ���б� */
static rt_list_t rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL];

void rt_system_timer_init(void)
{
    int i;

    for (i = 0; i < sizeof(rt_timer_list) / sizeof(rt_timer_list[0]); i++)
    {
        rt_list_init(rt_timer_list + i);
    }
}

static void _rt_timer_init(rt_timer_t timer,
                           void (*timeout)(void *parameter),
                           void      *parameter,
                           rt_tick_t  time,
                           rt_uint8_t flag)
{
    int i;

    /* ���ñ�־ */
    timer->parent.flag  = flag;

    /* ������Ϊ�Ǽ���̬ */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    timer->timeout_func = timeout;
    timer->parameter    = parameter;

    /* ��ʼ�� ��ʱ��ʵ�ʳ�ʱʱ��ϵͳ������ */
    timer->timeout_tick = 0;
    /* ��ʼ�� ��ʱ����Ҫ��ʱ�Ľ����� */
    timer->init_tick    = time;

    /* ��ʼ����ʱ�������ýڵ� */
    for (i = 0; i < RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        rt_list_init(&(timer->row[i]));
    }
}

/**
 * �ú������ڳ�ʼ��һ����ʱ����ͨ���ú������ڳ�ʼ��һ����̬�Ķ�ʱ��
 *
 * @param timer ��̬��ʱ������
 * @param name ��ʱ��������
 * @param timeout ��ʱ����
 * @param parameter ��ʱ�����β�
 * @param time ��ʱ���ĳ�ʱʱ��
 * @param flag ��ʱ���ı�־
 */
void rt_timer_init(rt_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   rt_tick_t   time,
                   rt_uint8_t  flag)
{
    /* ��ʱ�������ʼ�� */
    rt_object_init((rt_object_t)timer, RT_Object_Class_Timer, name);

    /* ��ʱ����ʼ�� */
    _rt_timer_init(timer, timeout, parameter, time, flag);
}

rt_inline void _rt_timer_remove(rt_timer_t timer)
{
    int i;

    for (i = 0; i < RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        rt_list_remove(&timer->row[i]);
    }
}

/**
 * �ú�����ֹͣһ����ʱ��
 *
 * @param timer ��Ҫ��ֹͣ�Ķ�ʱ��
 *
 * @return ����״̬, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_timer_stop(rt_timer_t timer)
{
    register rt_base_t level;

    /* ֻ��active�Ķ�ʱ�����ܱ�ֹͣ�������˳����ش����� */
    if (!(timer->parent.flag & RT_TIMER_FLAG_ACTIVATED))
        return -RT_ERROR;

    /* ���ж� */
    level = rt_hw_interrupt_disable();

    /* ����ʱ���Ӷ�ʱ���б�ɾ�� */
    _rt_timer_remove(timer);

    /* ���ж� */
    rt_hw_interrupt_enable(level);

    /* �ı䶨ʱ����״̬Ϊ��active */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    return RT_EOK;
}

/**
 * �ú�������ȡ�������ö�ʱ����һЩѡ��
 * 
 * @param timer ��Ҫ�����û��߻�ȡ�Ķ�ʱ��
 * @param cmd ��������
 * @param arg �β�
 *
 * @return RT_EOK
 */
rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg)
{
    switch (cmd)
    {
    case RT_TIMER_CTRL_GET_TIME:
        *(rt_tick_t *)arg = timer->init_tick;
        break;

    case RT_TIMER_CTRL_SET_TIME:
        timer->init_tick = *(rt_tick_t *)arg;
        break;

    case RT_TIMER_CTRL_SET_ONESHOT:
        timer->parent.flag &= ~RT_TIMER_FLAG_PERIODIC;
        break;

    case RT_TIMER_CTRL_SET_PERIODIC:
        timer->parent.flag |= RT_TIMER_FLAG_PERIODIC;
        break;
    }

    return RT_EOK;
}

/**
 * ������ʱ��
 *
 * @param timer ��Ҫ�����Ķ�ʱ��
 *
 * @return ����״̬, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_timer_start(rt_timer_t timer)
{
    unsigned int row_lvl = 0;
    rt_list_t *timer_list;
    register rt_base_t level;
    rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];
    unsigned int tst_nr;
    static unsigned int random_nr;


    /* ���ж� */
    level = rt_hw_interrupt_disable();
    
    /* ����ʱ����ϵͳ��ʱ���б��Ƴ� */
    _rt_timer_remove(timer);
    
    /* �ı䶨ʱ����״̬Ϊ��active */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
    
    /* ���ж� */
    rt_hw_interrupt_enable(level);

    /* ��ȡ timeout tick,
       ����timeout tick ���ܴ��� RT_TICK_MAX/2 */
    timer->timeout_tick = rt_tick_get() + timer->init_tick;

    /* ���ж� */
    level = rt_hw_interrupt_disable();


    /* ����ʱ�����뵽��ʱ���б� */
    /* ��ȡϵͳ��ʱ���б���ڵ��ַ��rt_timer_list��һ��ȫ�ֱ��� */
    timer_list = rt_timer_list;


    /* ��ȡϵͳ��ʱ���б��һ��������ڵ��ַ */
    row_head[0]  = &timer_list[0];
    
    /* ��ΪRT_TIMER_SKIP_LIST_LEVEL����1�����ѭ��ֻ��ִ��һ�� */
    for (row_lvl = 0; row_lvl < RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        /* �б�Ϊ�գ���û�ж�ʱ�������뵽ϵͳ��ʱ���б�ʱ����ѭ����ִ�� */
        for (; row_head[row_lvl] != timer_list[row_lvl].prev; row_head[row_lvl]  = row_head[row_lvl]->next)
        {
            struct rt_timer *t;
            
            /* ��ȡ��ʱ���б�ڵ��ַ */
            rt_list_t *p = row_head[row_lvl]->next;

            /* ���ݽڵ��ַ��ȡ���ṹ��ָ�� */
            t = rt_list_entry(p,                 /* �ڵ��ַ */ 
                              struct rt_timer,   /* �ڵ����ڸ��ṹ���������� */
                              row[row_lvl]);     /* �ڵ��ڸ��ṹ�н�ʲô�������� */
            
            /* ������ʱ���ĳ�ʱʱ����ͬ��������ڶ�ʱ���б���Ѱ����һ���ڵ� */
            if ((t->timeout_tick - timer->timeout_tick) == 0)
            {
                continue;
            }
            /* ������ʱ���ĳ�ʱʱ����ͬ��������ڶ�ʱ���б���Ѱ����һ���ڵ� */
            else if ((t->timeout_tick - timer->timeout_tick) < RT_TICK_MAX / 2)
            {
                break;
            }
            
        }
        /* ����������棬���ᱻִ�� */
        if (row_lvl != RT_TIMER_SKIP_LIST_LEVEL - 1)
        {
           row_head[row_lvl + 1] = row_head[row_lvl] + 1; 
        }            
    }

    /* random_nr��һ����̬���������ڼ�¼�����˶��ٸ���ʱ�� */
    random_nr++;
    tst_nr = random_nr;

    /* ����ʱ�����뵽ϵͳ��ʱ���б� */
    rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL - 1],       /* ˫���б���ڵ��ַ */
                         &(timer->row[RT_TIMER_SKIP_LIST_LEVEL - 1])); /* Ҫ������Ľڵ�ĵ�ַ */
    
    /* RT_TIMER_SKIP_LIST_LEVEL ����1����forѭ����Զ����ִ�� */
    for (row_lvl = 2; row_lvl <= RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        if (!(tst_nr & RT_TIMER_SKIP_LIST_MASK))
            rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL - row_lvl],
                                 &(timer->row[RT_TIMER_SKIP_LIST_LEVEL - row_lvl]));
        else
            break;

        tst_nr >>= (RT_TIMER_SKIP_LIST_MASK + 1) >> 1;
    }

    /* ���ö�ʱ����־λΪ����̬ */
    timer->parent.flag |= RT_TIMER_FLAG_ACTIVATED;

    /* ���ж� */
    rt_hw_interrupt_enable(level);

    return -RT_EOK;
}


/**
 * �ú�������ɨ��ϵͳ��ʱ���б����г�ʱ�¼�����ʱ
 * �͵��ö�Ӧ�ĳ�ʱ����
 *
 * @note �ú����ڲ���ϵͳ��ʱ���ж��б�����
 */
void rt_timer_check(void)
{
    struct rt_timer *t;
    rt_tick_t current_tick;
    register rt_base_t level;

    /* ��ȡϵͳʱ��������rt_tick��ֵ */
    current_tick = rt_tick_get();

    /* ���ж� */
    level = rt_hw_interrupt_disable();

    /* ϵͳ��ʱ���б�Ϊ�գ���ɨ�趨ʱ���б� */
    while (!rt_list_isempty(&rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL - 1]))
    {
        /* ��ȡ��һ���ڵ㶨ʱ���ĵ�ַ */
        t = rt_list_entry
        (rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL - 1].next,   /* �ڵ��ַ */
         struct rt_timer,                                    /* �ڵ����ڵĸ��ṹ���������� */ 
         row[RT_TIMER_SKIP_LIST_LEVEL - 1]);                 /* �ڵ��ڸ��ṹ�ĳ�Ա�� */
              
        if ((current_tick - t->timeout_tick) < RT_TICK_MAX / 2)
        {
            /* �Ƚ���ʱ���Ӷ�ʱ���б��Ƴ� */
            _rt_timer_remove(t);

            /* ���ó�ʱ���� */
            t->timeout_func(t->parameter);

            /* ���»�ȡ rt_tick */
            current_tick = rt_tick_get();

            /* ���ڶ�ʱ�� */
            if ((t->parent.flag & RT_TIMER_FLAG_PERIODIC) &&
                (t->parent.flag & RT_TIMER_FLAG_ACTIVATED))
            {
                /* ������ʱ�� */
                t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
                rt_timer_start(t);
            }
            /* ���ζ�ʱ�� */
            else
            {
                /* ֹͣ��ʱ�� */
                t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
            }
        }
        else
            break;
    }

    /* ���ж� */
    rt_hw_interrupt_enable(level);
}
























