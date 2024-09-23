/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �����ź���
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
/* ������Ϣ���п��ƿ� */
static rt_sem_t test_sem = RT_NULL;

/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT�����ź���ʵ�飡\n");
  rt_kprintf("��λĬ��ֵΪ5��������K1���복λ������K2�ͷų�λ��\n\n");
   /* ����һ���ź��� */
	test_sem = rt_sem_create("test_sem",/* �����ź������� */
                     5,     /* �ź�����ʼֵ��Ĭ����5���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/
  if (test_sem != RT_NULL)
    rt_kprintf("�����ź��������ɹ���\n\n");
    
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
  /* ������һ������ѭ�������ܷ��� */
  while(1)
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       //���KEY2������
		{
			/* ��ȡһ�������ź��� */
      uwRet = rt_sem_take(test_sem,	
                          0); 	/* �ȴ�ʱ�䣺0 */
			if ( RT_EOK == uwRet ) 
				rt_kprintf( "KEY1���������ɹ����뵽ͣ��λ��\r\n" );
			else
				rt_kprintf( "KEY1��������������˼������ͣ����������\r\n" );							
		}
		rt_thread_delay(20);     //ÿ20msɨ��һ��		
  }
}

static void send_thread_entry(void* parameter)
{	
	rt_err_t uwRet = RT_EOK;
    /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       //���KEY2������
		{
			/* �ͷ�һ�������ź��� */
			uwRet = rt_sem_release(test_sem);    
			if ( RT_EOK == uwRet ) 
				rt_kprintf ( "KEY2���������ͷ�1��ͣ��λ��\r\n" );	
			else
				rt_kprintf ( "KEY2�������������޳�λ�����ͷţ�\r\n" );					
		}
		rt_thread_delay(20);     //ÿ20msɨ��һ��		
  }
}




/********************************END OF FILE****************************/
