/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
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
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t test_thread = RT_NULL;
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void test_thread_entry(void* parameter);

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
	
	led1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "led1",              /* �߳����� */
                      led1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      1024,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;
		
	test_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "test",              /* �߳����� */
                      test_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      1024,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (test_thread != RT_NULL)
        rt_thread_startup(test_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
	/* �ļ�ϵͳ��ʼ�� */
	FileSystem_Init();
	while (1)
	{
		rt_kprintf("*******************led1_thread inter running******************\n");
		/* �ļ�ϵͳ��д���� */
		FileSystem_Test();
		rt_kprintf("*******************led1_thread exti running******************\n");
		LED1_ON;
		//rt_kprintf("led1_thread running,LED1_ON\r\n");
		rt_thread_delay(1000);   /* ��ʱ500��tick */
		
		
//		LED1_OFF;    
//		rt_kprintf("led1_thread running,LED1_OFF\r\n");		
//		rt_thread_delay(500);   /* ��ʱ500��tick */		 		
		
//		rt_kprintf("led1_thread running\r\n");

//		rt_kprintf("led1_thread running over!\r\n");
	}
}

static void test_thread_entry(void* parameter)
{	
//	/* �ļ�ϵͳ��ʼ�� */
//	FileSystem_Init();
	while (1)
	{
		rt_kprintf("*******************test_thread inter running******************\n");
		/* �ļ�ϵͳ��д���� */
		FileSystem_Test();
		rt_kprintf("*******************test_thread exti running******************\n");
		rt_thread_delay(1000);   /* ��ʱ500��tick */		 		

	}
}


/********************************END OF FILE****************************/
