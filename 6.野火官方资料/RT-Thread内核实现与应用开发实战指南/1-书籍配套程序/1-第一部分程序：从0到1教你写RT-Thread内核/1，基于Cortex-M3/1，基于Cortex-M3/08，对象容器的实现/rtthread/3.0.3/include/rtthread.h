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
                        rt_uint32_t       stack_size);
												
rt_err_t rt_thread_resume(rt_thread_t thread);
												
rt_err_t rt_thread_startup(rt_thread_t thread);
												
												
												
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
*                             �ں˷�����غ����ӿ�
-------------------------------------------------------------------------
*/                        
                        
char *rt_strncpy(char *dst, const char *src, rt_ubase_t n);                        
                        
#endif /* __RT_THREAD_H__ */

