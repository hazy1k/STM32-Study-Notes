/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ��Ϣ����
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
static rt_thread_t receive_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
/* ������Ϣ���п��ƿ� */
static rt_mq_t test_mq = RT_NULL;
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void receive_thread_entry(void* parameter);
static void send_thread_entry(void* parameter);

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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT��Ϣ����ʵ�飡\n");
  rt_kprintf("����K1����K2���Ͷ�����Ϣ\n");
  rt_kprintf("receive������յ���Ϣ�ڴ��ڻ���\n");
   /* ����һ����Ϣ���� */
	test_mq = rt_mq_create("test_mq",/* ��Ϣ�������� */
                     4,     /* ��Ϣ����󳤶� */
                     20,    /* ��Ϣ���е�������� */
                     RT_IPC_FLAG_FIFO);/* ����ģʽ FIFO(0x00)*/
  if (test_mq != RT_NULL)
    rt_kprintf("��Ϣ���д����ɹ���\n\n");
    
	receive_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "receive",              /* �߳����� */
                      receive_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (receive_thread != RT_NULL)
        rt_thread_startup(receive_thread);
    else
        return -1;
    
  send_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "send",              /* �߳����� */
                      send_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (send_thread != RT_NULL)
        rt_thread_startup(send_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void receive_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;	
  uint32_t r_queue;
  /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    /* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
		uwRet = rt_mq_recv(test_mq,	/* ��ȡ�����գ����е�ID(���) */
								&r_queue,			/* ��ȡ�����գ������ݱ���λ�� */
								sizeof(r_queue),		/* ��ȡ�����գ������ݵĳ��� */
								RT_WAITING_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */
		if(RT_EOK == uwRet)
		{
			rt_kprintf("���ν��յ��������ǣ�%d\n",r_queue);
		}
		else
		{
			rt_kprintf("���ݽ��ճ���,�������: 0x%lx\n",uwRet);
		}
    rt_thread_delay(200);
  }
}

static void send_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;	
  uint32_t send_data1 = 1;
  uint32_t send_data2 = 2;
    while (1)
    {
      if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )/* K1 ������ */
      {
      	/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
        uwRet = rt_mq_send(	test_mq,	/* д�루���ͣ����е�ID(���) */
                    &send_data1,			/* д�루���ͣ������� */
                    sizeof(send_data1));			/* ���ݵĳ��� */
        if(RT_EOK != uwRet)
        {
          rt_kprintf("���ݲ��ܷ��͵���Ϣ���У��������: %lx\n",uwRet);
        }
      } 
      if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )/* K1 ������ */
      {
      	/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
        uwRet = rt_mq_send(	test_mq,	/* д�루���ͣ����е�ID(���) */
                    &send_data2,			/* д�루���ͣ������� */
                    sizeof(send_data2));			/* ���ݵĳ��� */
        if(RT_EOK != uwRet)
        {
          rt_kprintf("���ݲ��ܷ��͵���Ϣ���У��������: %lx\n",uwRet);
        }
      }
      rt_thread_delay(20);
    }
}




/********************************END OF FILE****************************/
