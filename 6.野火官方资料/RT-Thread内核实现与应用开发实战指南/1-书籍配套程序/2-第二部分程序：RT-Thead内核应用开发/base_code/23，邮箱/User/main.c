/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ����
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
/* ����������ƿ� */
static rt_mailbox_t test_mail = RT_NULL;

/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
char test_str1[] = "this is a mail test 1";/* ������Ϣtest1 */
char test_str2[] = "this is a mail test 2";/* ������Ϣtest2 */
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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT������Ϣʵ�飡\n");
	rt_kprintf("����K1 | K2��������ʵ�����!\n");
  /* ����һ������ */
	 test_mail = rt_mb_create("test_mail", /* �������� */
                            10, 
                            RT_IPC_FLAG_FIFO);/* �ź���ģʽ FIFO(0x00)*/
  if (test_mail != RT_NULL)
    rt_kprintf("���䴴���ɹ���\n\n");
    
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
  char *r_str;
  /* ������һ������ѭ�������ܷ��� */
  while(1)
	{
    /* �ȴ���������Ϣ */
    uwRet = rt_mb_recv(test_mail, /* ��������� */
                      (rt_uint32_t*)&r_str, /* ����������Ϣ */
                      RT_WAITING_FOREVER);/* ָ����ʱ�¼�,һֱ�� */
    
      if(RT_EOK == uwRet) /* ���������ɲ�����ȷ */
      {
        rt_kprintf ( "�����������:%s\n\n",r_str);		
        LED1_TOGGLE;       //LED1	��ת
      }
      else
        rt_kprintf ( "������մ��󣡴�������0x%x\n",uwRet);	
  }
}

static void send_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;
    /* ������һ������ѭ�������ܷ��� */
  while (1)
  {     //���KEY1������
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )      
		{
      rt_kprintf ( "KEY1������\n" );
			/* ����һ��������Ϣ1 */
			uwRet = rt_mb_send(test_mail,(rt_uint32_t)&test_str1); 
      if(RT_EOK == uwRet)
        rt_kprintf ( "������Ϣ���ͳɹ�\n" );
      else
        rt_kprintf ( "������Ϣ����ʧ��\n" );
		}
      //���KEY2������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON ) 
		{
      rt_kprintf ( "KEY2������\n" );	
			/* ����һ������2 */
			uwRet = rt_mb_send(test_mail,(rt_uint32_t)&test_str2); 
      if(RT_EOK == uwRet)
        rt_kprintf ( "������Ϣ���ͳɹ�\n" );
      else
        rt_kprintf ( "������Ϣ����ʧ��\n" );
		}
		rt_thread_delay(20);     //ÿ20msɨ��һ��		
  }
}




/********************************END OF FILE****************************/
