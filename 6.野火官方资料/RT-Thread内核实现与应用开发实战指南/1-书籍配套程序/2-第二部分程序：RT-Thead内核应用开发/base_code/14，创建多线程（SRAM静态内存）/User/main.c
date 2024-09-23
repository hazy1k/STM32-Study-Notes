/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + ָ���� ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */
static struct rt_thread led1_thread;
static struct rt_thread led2_thread;

/* �����߳̿�ջʱҪ��RT_ALIGN_SIZE���ֽڶ��� */
ALIGN(RT_ALIGN_SIZE)
/* �����߳�ջ */
static rt_uint8_t rt_led1_thread_stack[1024];
static rt_uint8_t rt_led2_thread_stack[1024];
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void led2_thread_entry(void* parameter);


/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	
	rt_thread_init(&led1_thread,                 /* �߳̿��ƿ� */
                   "led1",                       /* �߳����� */
                   led1_thread_entry,            /* �߳���ں��� */
                   RT_NULL,                      /* �߳���ں������� */
                   &rt_led1_thread_stack[0],     /* �߳�ջ��ʼ��ַ */
                   sizeof(rt_led1_thread_stack), /* �߳�ջ��С */
                   3,                            /* �̵߳����ȼ� */
                   20);                          /* �߳�ʱ��Ƭ */
    rt_thread_startup(&led1_thread);             /* �����̣߳��������� */
                   
	rt_thread_init(&led2_thread,                 /* �߳̿��ƿ� */
                   "led2",                       /* �߳����� */
                   led2_thread_entry,            /* �߳���ں��� */
                   RT_NULL,                      /* �߳���ں������� */
                   &rt_led2_thread_stack[0],     /* �߳�ջ��ʼ��ַ */
                   sizeof(rt_led2_thread_stack), /* �߳�ջ��С */
                   4,                            /* �̵߳����ȼ� */
                   20);                          /* �߳�ʱ��Ƭ */
    rt_thread_startup(&led2_thread);             /* �����̣߳��������� */                   
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
        
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		

    }
}

static void led2_thread_entry(void* parameter)
{	
    while (1)
    {
        LED2_ON;
        rt_thread_delay(300);   /* ��ʱ300��tick */
        
        LED2_OFF;     
        rt_thread_delay(300);   /* ��ʱ300��tick */		 		

    }
}
/********************************END OF FILE****************************/
