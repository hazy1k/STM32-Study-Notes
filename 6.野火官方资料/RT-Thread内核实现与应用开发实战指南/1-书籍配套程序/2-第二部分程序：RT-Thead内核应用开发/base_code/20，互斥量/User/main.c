/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ������ͬ��
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
******************************************************************
*                               ����
******************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t receive_thread = RT_NULL;
static rt_thread_t send_thread = RT_NULL;
/* ���廥�������ƿ� */
static rt_mutex_t test_mux = RT_NULL;

/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
uint8_t ucValue [ 2 ] = { 0x00, 0x00 };
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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT������ͬ��ʵ�飡\n");
  rt_kprintf("ͬ���ɹ������Successful,��֮���Fail\n");
   /* ����һ�������� */
  test_mux = rt_mutex_create("test_mux",RT_IPC_FLAG_PRIO);
  
  if (test_mux != RT_NULL)
    rt_kprintf("�����������ɹ���\n\n");
    
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
  /* ������һ������ѭ�������ܷ��� */
  while(1)
	{
		rt_mutex_take(test_mux,	          /* ��ȡ������ */
                RT_WAITING_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			rt_kprintf ( "Successful\n" );			
		}
		else
		{
			rt_kprintf ( "Fail\n" );			
		}
		rt_mutex_release(	test_mux	);    //�ͷŻ�����
		
		rt_thread_delay ( 1000 );  				//ÿ1s��һ��		
  }
  
  
}

static void send_thread_entry(void* parameter)
{	
    /* ������һ������ѭ�������ܷ��� */
    while (1)
    {
			rt_mutex_take(test_mux,           /* ��ȡ������ */
                  RT_WAITING_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */		
		ucValue [ 0 ] ++;		
		rt_thread_delay ( 100 );        /* ��ʱ100ms */		
		ucValue [ 1 ] ++;		
		rt_mutex_release(	test_mux	); //�ͷŻ������		
		rt_thread_yield();  				//����ʣ��ʱ��Ƭ������һ�������л�	
    }
}




/********************************END OF FILE****************************/
