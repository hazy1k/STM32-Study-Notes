#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
#include <rtdef.h>
#include <rtconfig.h>
#include <rtservice.h>

/*
*************************************************************************
*                               ��������
*************************************************************************
*/

/*
-------------------------------------------------------------------------
*                               �ں˶���ӿ�
-------------------------------------------------------------------------
*/
void rt_object_init(struct rt_object         *object,
                    enum rt_object_class_type type,
                    const char               *name);

/*
-------------------------------------------------------------------------
*                               �߳̽ӿ�
-------------------------------------------------------------------------
*/
                    
rt_err_t rt_thread_init(struct rt_thread *thread,
                        const char       *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
                        rt_uint8_t        priority);
                        
void rt_thread_delay_old(rt_tick_t tick);

void rt_schedule_remove_thread(struct rt_thread *thread);                        
void rt_schedule_insert_thread(struct rt_thread *thread);
rt_err_t rt_thread_resume(rt_thread_t thread);
rt_thread_t rt_thread_self(void);                        
rt_err_t rt_thread_startup(rt_thread_t thread);
                        
rt_err_t rt_thread_suspend(rt_thread_t thread);                        
rt_err_t rt_thread_sleep(rt_tick_t tick);												
rt_err_t rt_thread_delay(rt_tick_t tick);												

void rt_thread_timeout(void *parameter);                        
/*
-------------------------------------------------------------------------
*                             �������ӿ�
-------------------------------------------------------------------------
*/
void rt_system_scheduler_init(void);
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_system_scheduler_start(void);

void rt_schedule(void);

/*
-------------------------------------------------------------------------
*                             �����߳̽ӿ�
-------------------------------------------------------------------------
*/                        
void rt_thread_idle_init(void);


/*
-------------------------------------------------------------------------
*                             �жϽӿ�
-------------------------------------------------------------------------
*/                        
void rt_interrupt_enter(void);
void rt_interrupt_leave(void);

/*
-------------------------------------------------------------------------
*                             ʱ����ؽӿ�
-------------------------------------------------------------------------
*/ 
void rt_tick_increase(void);
rt_tick_t rt_tick_get(void);

/*
-------------------------------------------------------------------------
*                             ��ʱ���ӿ�
-------------------------------------------------------------------------
*/
void rt_system_timer_init(void);

void rt_timer_init(rt_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   rt_tick_t   time,
                   rt_uint8_t  flag);

rt_err_t rt_timer_stop(rt_timer_t timer);

rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg);
rt_err_t rt_timer_start(rt_timer_t timer);
void rt_timer_check(void);

/*
-------------------------------------------------------------------------
*                             �ں˷�����غ����ӿ�
-------------------------------------------------------------------------
*/                        
                        
char *rt_strncpy(char *dst, const char *src, rt_ubase_t n);                        
int __rt_ffs(int value);


#endif /* __RT_THREAD_H__ */

