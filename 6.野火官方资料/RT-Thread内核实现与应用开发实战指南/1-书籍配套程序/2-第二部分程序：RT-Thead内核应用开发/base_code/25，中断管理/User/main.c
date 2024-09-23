/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ��̬�ڴ����
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
#include <string.h>

/*
******************************************************************
*                               ����
******************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t key_thread = RT_NULL;
static rt_thread_t usart_thread = RT_NULL;
/* ������Ϣ���п��ƿ� */
rt_mq_t test_mq = RT_NULL;
/* �����ź������ƿ� */
rt_sem_t test_sem = RT_NULL;

/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

/* ��غ궨�� */
extern char Usart_Rx_Buf[USART_RBUFF_SIZE];

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void key_thread_entry(void* parameter);
static void usart_thread_entry(void* parameter);

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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT�жϹ���ʵ�飡\n");
  rt_kprintf("����KEY1 | KEY2�����жϣ�\n");
  rt_kprintf("���ڷ������ݴ����ж�,����������!\n");
  /* ����һ����Ϣ���� */
	test_mq = rt_mq_create("test_mq",/* ��Ϣ�������� */
                     4,     /* ��Ϣ����󳤶� */
                     2,    /* ��Ϣ���е�������� */
                     RT_IPC_FLAG_FIFO);/* ����ģʽ FIFO(0x00)*/
  if (test_mq != RT_NULL)
    rt_kprintf("��Ϣ���д����ɹ���\n\n");
  
  /* ����һ���ź��� */
	test_sem = rt_sem_create("test_sem",/* ��Ϣ�������� */
                     0,     /* �ź�����ʼֵ��Ĭ����һ���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/
  if (test_sem != RT_NULL)
    rt_kprintf("�ź��������ɹ���\n\n");
  
  /* ����һ������ */
	key_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "key",              /* �߳����� */
                      key_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      1,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
    
  usart_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "usart",              /* �߳����� */
                      usart_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (usart_thread != RT_NULL)
        rt_thread_startup(usart_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void key_thread_entry(void* parameter)
{		
  rt_err_t uwRet = RT_EOK;	
  uint32_t r_queue;
  /* ������һ������ѭ�������ܷ��� */
  while(1)
	{
    /* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
		uwRet = rt_mq_recv(test_mq,	/* ��ȡ�����գ����е�ID(���) */
								&r_queue,			/* ��ȡ�����գ������ݱ���λ�� */
								sizeof(r_queue),		/* ��ȡ�����գ������ݵĳ��� */
								RT_WAITING_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */
		if(RT_EOK == uwRet)
		{
			rt_kprintf("�����жϵ���KEY%d!\n",r_queue);
		}
		else
		{
			rt_kprintf("���ݽ��ճ���,�������: 0x%lx\n",uwRet);
		}
    LED1_TOGGLE;
  }
}

static void usart_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;	
    /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
		uwRet = rt_sem_take(test_sem,	/* ��ȡ�����жϵ��ź��� */
                        RT_WAITING_FOREVER); 	  /* �ȴ�ʱ�䣺0 */
    if(RT_EOK == uwRet)
    {
      rt_kprintf("�յ�����:%s\n",Usart_Rx_Buf);
      memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* ���� */
    }
  }
}




/********************************END OF FILE****************************/
