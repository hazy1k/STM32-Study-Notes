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


/*
******************************************************************
*                               ����
******************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t alloc_thread = RT_NULL;
static rt_thread_t free_thread = RT_NULL;
/* �����ڴ�ؿ��ƿ� */
static rt_mp_t test_mp = RT_NULL;
/* ���������ڴ��ָ�� */
static rt_uint32_t *p_test = RT_NULL;


/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 
/* ��غ궨�� */
#define  BLOCK_COUNT   20		//�ڴ������
#define  BLOCK_SIZE   3		  //�ڴ���С


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void alloc_thread_entry(void* parameter);
static void free_thread_entry(void* parameter);

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
	rt_kprintf("����һ��[Ұ��]-STM32F103-ָ����-RTT��̬�ڴ����ʵ�飡\n");
  rt_kprintf("���ڴ���һ���ڴ��...........\n");
   /* ����һ����̬�ڴ�� */
  test_mp = rt_mp_create("test_mp",
                        BLOCK_COUNT,
                        BLOCK_SIZE);
  if(test_mp != RT_NULL)
      rt_kprintf("��̬�ڴ�ش����ɹ���\n\n");
  
  /* ����һ������ */
	alloc_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "alloc",              /* �߳����� */
                      alloc_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      1,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (alloc_thread != RT_NULL)
        rt_thread_startup(alloc_thread);
    else
        return -1;
    
  free_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "free",              /* �߳����� */
                      free_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (free_thread != RT_NULL)
        rt_thread_startup(free_thread);
    else
        return -1;
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void alloc_thread_entry(void* parameter)
{		
  rt_kprintf("�������ڴ�������ڴ�...........\n");
  
  p_test = rt_mp_alloc(test_mp,0);
  if(RT_NULL == p_test)/* û������ɹ� */
     rt_kprintf("��̬�ڴ�����ʧ�ܣ�\n");
  else
    rt_kprintf("��̬�ڴ�����ɹ�����ַΪ%d��\n\n",p_test);
  
  rt_kprintf("������p_testд������...........\n");
  *p_test = 1234;
  rt_kprintf("�Ѿ�д��p_test��ַ������\n");
	rt_kprintf("*p_test = %.4d ,��ַΪ:%d \n\n", *p_test,p_test);

  /* ������һ������ѭ�������ܷ��� */
  while(1)
	{
		LED2_TOGGLE;
    rt_thread_delay(1000);     //ÿ1000msɨ��һ��
  }
}

static void free_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;
  rt_kprintf("�����ͷ��ڴ�...........\n");
  rt_mp_free(p_test);
  rt_kprintf("�ͷ��ڴ�ɹ���\n\n");
  rt_kprintf("����ɾ���ڴ�...........\n");
  uwRet = rt_mp_delete(test_mp);
  if(RT_EOK == uwRet)
    rt_kprintf("ɾ���ڴ�ɹ���\n");
    /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    LED1_TOGGLE;
		rt_thread_delay(500);     //ÿ500msɨ��һ��		
  }
}




/********************************END OF FILE****************************/
