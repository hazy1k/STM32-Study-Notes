/**
  ************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��FreeRTOS�ں�ʵ����Ӧ�ÿ���ʵսָ�ϡ��鼮����
  *           �½�FreeRTOS���̡��������
  ************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ϵ�� ������
  * 
  * ����    :www.embedfire.com  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ************************************************************************
  */
  
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
#include "FreeRTOS.h"
#include "task.h"

/*
*************************************************************************
*                              ȫ�ֱ���
*************************************************************************
*/
portCHAR flag1;
portCHAR flag2;


extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

/*
*************************************************************************
*                        ������ƿ� & STACK 
*************************************************************************
*/
TaskHandle_t Task1_Handle;
#define TASK1_STACK_SIZE                    128
StackType_t Task1Stack[TASK1_STACK_SIZE];
TCB_t Task1TCB;

TaskHandle_t Task2_Handle;
#define TASK2_STACK_SIZE                    128
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;

/*
*************************************************************************
*                               �������� 
*************************************************************************
*/
void delay (uint32_t count);
void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );


/*
************************************************************************
*                                main����
************************************************************************
*/
int main(void)
{	
    /* Ӳ����ʼ�� */
	/* ��Ӳ����صĳ�ʼ�����������������������û����س�ʼ������ */
    
    /* �������� */
    Task1_Handle = xTaskCreateStatic( (TaskFunction_t)Task1_Entry,   /* ������� */
					                  (char *)"Task1",               /* �������ƣ��ַ�����ʽ */
					                  (uint32_t)TASK1_STACK_SIZE ,   /* ����ջ��С����λΪ�� */
					                  (void *) NULL,                 /* �����β� */
                                      (UBaseType_t) 2,               /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
					                  (StackType_t *)Task1Stack,     /* ����ջ��ʼ��ַ */
					                  (TCB_t *)&Task1TCB );          /* ������ƿ� */
                                
    Task2_Handle = xTaskCreateStatic( (TaskFunction_t)Task2_Entry,   /* ������� */
					                  (char *)"Task2",               /* �������ƣ��ַ�����ʽ */
					                  (uint32_t)TASK2_STACK_SIZE ,   /* ����ջ��С����λΪ�� */
					                  (void *) NULL,                 /* �����β� */
                                      (UBaseType_t) 2,               /* �������ȼ�����ֵԽ�����ȼ�Խ�� */                                          
					                  (StackType_t *)Task2Stack,     /* ����ջ��ʼ��ַ */
					                  (TCB_t *)&Task2TCB );          /* ������ƿ� */
                                      
    
    /* ������������ǰ���ر��ж� */                                  
    portDISABLE_INTERRUPTS();
                                      
    /* ��������������ʼ��������ȣ������ɹ��򲻷��� */
    vTaskStartScheduler();                                      
    
    for(;;)
	{
		/* ϵͳ�����ɹ����ᵽ������ */
	}
}

/*
*************************************************************************
*                               ����ʵ��
*************************************************************************
*/
/* �����ʱ */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}
/* ����1 */
void Task1_Entry( void *p_arg )
{
	for( ;; )
	{
		flag1 = 1;
        vTaskDelay( 2 );
		
		flag1 = 0;
        vTaskDelay( 2 );      
	}
}

/* ����2 */
void Task2_Entry( void *p_arg )
{
	for( ;; )
	{
		flag2 = 1;
        vTaskDelay( 2 );
		
		flag2 = 0;
        vTaskDelay( 2 );       
	}
}


/* ��ȡ����������ڴ� */
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
TCB_t IdleTaskTCB;
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize )
{
		*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
		*ppxIdleTaskStackBuffer=IdleTaskStack; 
		*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}
