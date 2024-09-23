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
  * ʵ��ƽ̨:Ұ�� STM32 M7ϵ�п�����
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
#include "ARMCM7.h"


/*
*************************************************************************
*                              ȫ�ֱ���
*************************************************************************
*/
rt_uint8_t flag1;
rt_uint8_t flag2;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/*
*************************************************************************
*                      �߳̿��ƿ� & STACK & �߳�����
*************************************************************************
*/


/* �����߳̿��ƿ� */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;

ALIGN(RT_ALIGN_SIZE)
/* �����߳�ջ */
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];

/* �߳����� */
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);

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
	
	/* ��������ʼ�� */
	rt_system_scheduler_init();
	
	
	/* ��ʼ���߳� */
	rt_thread_init( &rt_flag1_thread,                 /* �߳̿��ƿ� */
	                flag1_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag1_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag1_thread_stack) );  /* �߳�ջ��С����λΪ�ֽ� */
	/* ���̲߳��뵽�����б� */
	rt_list_insert_before( &(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist) );
	
	/* ��ʼ���߳� */
	rt_thread_init( &rt_flag2_thread,                 /* �߳̿��ƿ� */
	                flag2_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag2_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag2_thread_stack) );  /* �߳�ջ��С����λΪ�ֽ� */
	/* ���̲߳��뵽�����б� */
	rt_list_insert_before( &(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist) );
	
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
		delay( 100 );		
		flag1 = 0;
		delay( 100 );
		
		/* �߳��л����������ֶ��л� */		
		rt_schedule();
	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg )
{
	for( ;; )
	{
		flag2 = 1;
		delay( 100 );		
		flag2 = 0;
		delay( 100 );
		
		/* �߳��л����������ֶ��л� */
		rt_schedule();
	}
}
