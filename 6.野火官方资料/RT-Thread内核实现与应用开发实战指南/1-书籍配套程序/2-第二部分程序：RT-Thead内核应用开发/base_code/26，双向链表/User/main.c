/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 ˫������ʵ��
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
static rt_thread_t test1_thread = RT_NULL;
static rt_thread_t test2_thread = RT_NULL;

/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void test1_thread_entry(void* parameter);
static void test2_thread_entry(void* parameter);

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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT˫���������ʵ�飡\n");
  
	test1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "test1",              /* �߳����� */
                      test1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (test1_thread != RT_NULL)
        rt_thread_startup(test1_thread);
    else
        return -1;
    
  test2_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "test2",              /* �߳����� */
                      test2_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (test2_thread != RT_NULL)
        rt_thread_startup(test2_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void test1_thread_entry(void* parameter)
{		
  rt_list_t *head;  /* ����һ��˫�������ͷ�ڵ� */
  rt_list_t *node1;  /* ����һ��˫�������ͷ�ڵ� */
  rt_list_t *node2;  /* ����һ��˫�������ͷ�ڵ� */
  
  head = rt_malloc(sizeof(rt_list_t));/* ���붯̬�ڴ� */
  if(RT_NULL == head)/* û������ɹ� */
     rt_kprintf("��̬�ڴ�����ʧ�ܣ�\n");
  else
    rt_kprintf("��̬�ڴ�����ɹ���ͷ����ַΪ%d��\n",head);
  
  rt_kprintf("\n˫�������ʼ����......\n");
  rt_list_init(head);
  if(rt_list_isempty(head))
    rt_kprintf("˫�������ʼ���ɹ�!\n\n");
  
  /* ����ڵ㣺˳��������ĩβ���� */
  rt_kprintf("��ӽڵ��β�ڵ����......\n");			
  /* ��̬�����һ�������ڴ� */
  node1 = rt_malloc(sizeof(rt_list_t));
  /* ��̬����ڶ��������ڴ� */
  node2 = rt_malloc(sizeof(rt_list_t));
  
  rt_kprintf("��ӵ�һ���ڵ���ڶ����ڵ�.....\n");
  /* ��Ϊ������ĳ���ڵ�������һ���ڵ㺯��
     Ϊ�����rt_list_insert_before��ĳ���ڵ�֮ǰ��
     ��ӽڵ����̵�,�������������֮���˳����
     head -> node1 -> node2 */
  rt_list_insert_after(head,node2);
  rt_list_insert_before(node2,node1);
  if ((node1->prev == head) && (node2->prev == node1))
    rt_kprintf("��ӽڵ�ɹ�!\n\n");
  else
    rt_kprintf("��ӽڵ�ʧ��!\n\n");
  
  rt_kprintf("ɾ���ڵ�......\n");	/* ɾ�����нڵ� */
  rt_list_remove(node1);
  rt_free(node1);/* �ͷŵ�һ���ڵ���ڴ� */
  if (node2->prev == head)
    rt_kprintf("ɾ���ڵ�ɹ�\n\n");

  /* ������һ������ѭ�������ܷ��� */
  while(1)
	{
    LED1_TOGGLE;
		rt_thread_delay(500);     //ÿ500msɨ��һ��
  }
}

static void test2_thread_entry(void* parameter)
{	

    /* ������һ������ѭ�������ܷ��� */
  while (1)
  {     
    rt_kprintf("����������!\n");
    LED2_TOGGLE;
		rt_thread_delay(1000);     //ÿ1000msɨ��һ��		
  }
}




/********************************END OF FILE****************************/
