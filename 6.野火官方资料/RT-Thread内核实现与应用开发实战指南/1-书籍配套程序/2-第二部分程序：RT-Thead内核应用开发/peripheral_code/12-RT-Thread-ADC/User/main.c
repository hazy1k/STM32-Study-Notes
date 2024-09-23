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
static rt_thread_t test_thread = RT_NULL;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
int ADC_ConvertedValueLocal;    
extern __IO uint16_t ADC_ConvertedValue;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
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
	
	test_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "test",              /* �߳����� */
                      test_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
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
static void test_thread_entry(void* parameter)
{	
	while (1)
	{
		ADC_ConvertedValueLocal =(int) (ADC_ConvertedValue * 825) >> 10; 
	
		rt_kprintf("\r\n The current AD value = 0x%04X \r\n", 
		       ADC_ConvertedValue); 
//		sprintf(adc_value,"\r\n The current AD value = %d mV \r\n",
//		       ADC_ConvertedValueLocal);
		rt_kprintf("\r\n The current AD value = %d mV \r\n",
								ADC_ConvertedValueLocal); 						/*rt_kprintf���ܴ�ӡ������*/
		rt_kprintf("\r\n\r\n");
		
		rt_thread_delay(1000);   /* ��ʱ500��tick */		 		
	}
}

/********************************END OF FILE****************************/
