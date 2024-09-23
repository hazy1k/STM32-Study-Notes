/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �̹߳���
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
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
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void test_thread_entry(void* parameter);
static void iic_test(void);
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
  rt_kprintf("����һ�� RTT-EEPROM ��дС���ͳ�����ʵ�飡\n\n");
	test_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "test",              /* �߳����� */
                      test_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      2048,                 /* �߳�ջ��С */
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
			iic_test();
			rt_thread_delay(1000);	
    }
}


static void iic_test(void)
{
uint8_t cal_flag = 0;
uint8_t k;

/*�洢С�������������飬��7��*/
long double double_buffer[7] = {0};
int int_bufffer[7] = {0};

#define DOUBLE_ADDR       10
#define LONGINT_ADDR      70
	
		/*��ȡ���ݱ�־λ*/
I2C_EE_BufferRead(&cal_flag, 0, 1);

if( cal_flag != 0xCD )	/*����־����0xcd����ʾ֮ǰ����д������*/
{      
		printf("\r\nû�м�⵽���ݱ�־��FLASHû�д洢���ݣ���������С��д��ʵ��\r\n");
		cal_flag =0xCD;
		
		/*д���־��0��ַ*/
		I2C_EE_BufferWrite(&cal_flag, 0, 1); 
		
		/*����Ҫд�������*/
		for( k=0; k<7; k++ )
		{
			 double_buffer[k] = k +0.1;
			 int_bufffer[k]=k*500+1 ;
		}

		/*д��С�����ݵ���ַ10*/
		I2C_EE_BufferWrite((void*)double_buffer,DOUBLE_ADDR, sizeof(double_buffer));
		/*д���������ݵ���ַ60*/
		I2C_EE_BufferWrite((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));
					
		printf("��оƬд�����ݣ�");
		/*��ӡ������*/
		for( k=0; k<7; k++ )
		{
			printf("С��tx = %LF\r\n",double_buffer[k]);
			printf("����tx = %d\r\n",int_bufffer[k]);
		}
		
		printf("\r\n�븴λ�����壬�Զ�ȡ���ݽ��м���\r\n");      

}    
else
{      
		 printf("\r\n��⵽���ݱ�־\r\n");

		/*��ȡС������*/
		I2C_EE_BufferRead((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
		
		/*��ȡ��������*/
		I2C_EE_BufferRead((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));

	
		printf("\r\n��оƬ�������ݣ�\r\n");			
		for( k=0; k<7; k++ )
		{
			printf("С�� rx = %LF \r\n",double_buffer[k]);
			printf("���� rx = %d \r\n",int_bufffer[k]);				
		}
	
} 

}



/********************************END OF FILE****************************/
