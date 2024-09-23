/**
  ************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��RT-Thread�ں�ʵ����Ӧ�ÿ���ʵսָ�ϡ��鼮����
  *           
  ************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 M4ϵ�п�����
  *
  * ����    :www.embedfire.com  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ************************************************************************
  */


/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/

#include <rtthread.h>
#include <rthw.h>
#include "ARMCM4.h"


/*
*************************************************************************
*                              ȫ�ֱ���
*************************************************************************
*/
rt_uint8_t flag1;
rt_uint8_t flag2;
rt_uint8_t flag3;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/*
*************************************************************************
*                      �߳̿��ƿ� & STACK & �߳�����
*************************************************************************
*/


/* �����߳̿��ƿ� */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;
struct rt_thread rt_flag3_thread;

ALIGN(RT_ALIGN_SIZE)
/* �����߳�ջ */
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];

/* �߳����� */
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);
void flag3_thread_entry(void *p_arg);

/*
*************************************************************************
*                               ��������
*************************************************************************
*/
void delay(uint32_t count);

/************************************************************************
  * @brief  main����
  * @param  ��
  * @retval ��
  *
  * @attention
  *********************************************************************** 
  */
int main(void)
{	
	/* Ӳ����ʼ�� */
	/* ��Ӳ����صĳ�ʼ�����������������������û����س�ʼ������ */
	
    /* ���ж� */
    rt_hw_interrupt_disable();
    
    /* SysTick�ж�Ƶ������ */
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );
    
    /* ϵͳ��ʱ���б��ʼ�� */
	rt_system_timer_init();
	
	/* ��������ʼ�� */
	rt_system_scheduler_init();

    /* ��ʼ�������߳� */    
    rt_thread_idle_init();	
	
	/* ��ʼ���߳� */
	rt_thread_init( &rt_flag1_thread,                 /* �߳̿��ƿ� */
                    "rt_flag1_thread",                /* �߳����֣��ַ�����ʽ */
	                flag1_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag1_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag1_thread_stack),    /* �߳�ջ��С����λΪ�ֽ� */
                    2,                                /* ���ȼ� */
                    4);                               /* ʱ��Ƭ */
	/* ���̲߳��뵽�����б� */
    rt_thread_startup(&rt_flag1_thread);
	
	/* ��ʼ���߳� */
	rt_thread_init( &rt_flag2_thread,                 /* �߳̿��ƿ� */
                    "rt_flag2_thread",                /* �߳����֣��ַ�����ʽ */
	                flag2_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag2_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag2_thread_stack),    /* �߳�ջ��С����λΪ�ֽ� */
                    3,                                /* ���ȼ� */
                    2);                               /* ʱ��Ƭ */
	/* ���̲߳��뵽�����б� */
	rt_thread_startup(&rt_flag2_thread);
                    
                    
	/* ��ʼ���߳� */
	rt_thread_init( &rt_flag3_thread,                 /* �߳̿��ƿ� */
                    "rt_flag3_thread",                /* �߳����֣��ַ�����ʽ */
	                flag3_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag3_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag3_thread_stack),    /* �߳�ջ��С����λΪ�ֽ� */
                    3,                                /* ���ȼ� */
                    3);                               /* ʱ��Ƭ */
	/* ���̲߳��뵽�����б� */
	rt_thread_startup(&rt_flag3_thread);                    
    
	/* ����ϵͳ������ */
	rt_system_scheduler_start(); 
}

/*
*************************************************************************
*                               ����ʵ��
*************************************************************************
*/
/* �����ʱ */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}

/* �߳�1 */
void flag1_thread_entry( void *p_arg )
{
	for( ;; )
	{
        flag1 = 1;
        rt_thread_delay(3); 		
		flag1 = 0;
        rt_thread_delay(3);       
	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg )
{
	for( ;; )
	{
        flag2 = 1;
        //rt_thread_delay(2);
		delay( 100 ); 		
		flag2 = 0;
        //rt_thread_delay(2);
		delay( 100 );        
	}
}

/* �߳�3 */
void flag3_thread_entry( void *p_arg )
{
	for( ;; )
	{
        flag3 = 1;
        //rt_thread_delay(3);
		delay( 100 ); 		
		flag3 = 0;
        //rt_thread_delay(3);
		delay( 100 );        
	}
}


void SysTick_Handler(void)
{
    /* �����ж� */
    rt_interrupt_enter();

    /* ����ʱ�� */
    rt_tick_increase();

    /* �뿪�ж� */
    rt_interrupt_leave();
}








