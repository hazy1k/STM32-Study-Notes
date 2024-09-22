/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

OS_SEM SemOfKey;          //��־KEY1�Ƿ񱻰��µĶ�ֵ�ź���


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;                                //������ƿ�

static  OS_TCB   AppTaskKey1TCB;
static  OS_TCB   AppTaskKey2TCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskKey1Stk [ APP_TASK_KEY1_STK_SIZE ];
static  CPU_STK  AppTaskKey2Stk [ APP_TASK_KEY2_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);                       //����������

static  void  AppTaskKey1  ( void * p_arg );
static  void  AppTaskKey2  ( void * p_arg );


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                                           //��ʼ�� uC/OS-III

	  /* ������ʼ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",                            //��������
                 (OS_TASK_PTR ) AppTaskStart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

    OSStart(&err);                                                          //�����������������uC/OS-III���ƣ�

}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


    (void)p_arg;

    BSP_Init();                                                 //�弶��ʼ��
    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u                                    //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif                                                          //���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU 
                                                                //ʹ����ʹ�ã���
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��

    
		/* ������ֵ�ź��� SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,    //ָ���ź���������ָ��
               (CPU_CHAR    *)"SemOfKey",    //�ź���������
               (OS_SEM_CTR   )5,             //��ʾ������Դ��Ŀ
               (OS_ERR      *)&err);         //��������
							 

		/* ���� AppTaskKey1 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKey1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key1",                             //��������
                 (OS_TASK_PTR ) AppTaskKey1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskKey1Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

		/* ���� AppTaskKey2 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKey2TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key2",                             //��������
                 (OS_TASK_PTR ) AppTaskKey2,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY2_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskKey2Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY2_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY2_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
    
		OSTaskDel ( & AppTaskStartTCB, & err );                     //ɾ����ʼ������������������
		
		
}


/*
*********************************************************************************************************
*                                          KEY1 TASK
*********************************************************************************************************
*/
static  void  AppTaskKey1 ( void * p_arg )
{
	OS_ERR      err;
	OS_SEM_CTR  ctr;
	CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
									 //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
									//�����ж�ʱ����ֵ��ԭ��	
	uint8_t ucKey1Press = 0;
	
	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                                         //������
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) ) //���KEY1������
		{
			ctr = OSSemPend ((OS_SEM   *)&SemOfKey,               //�ȴ����ź��� SemOfKey
								       (OS_TICK   )0,                       //����ѡ�񲻵ȴ����ò���Ч
								       (OS_OPT    )OS_OPT_PEND_NON_BLOCKING,//���û�ź������ò��ȴ�
								       (CPU_TS   *)0,                       //����ȡʱ���
								       (OS_ERR   *)&err);                   //���ش�������
			
			OS_CRITICAL_ENTER();                                  //�����ٽ��
			
			if ( err == OS_ERR_NONE )                      
				printf ( "\r\nKEY1�����£��ɹ����뵽ͣ��λ��ʣ��%d��ͣ��λ��\r\n", ctr );
			else if ( err == OS_ERR_PEND_WOULD_BLOCK )
				printf ( "\r\nKEY1�����£�������˼������ͣ������������ȴ���\r\n" );
			
			OS_CRITICAL_EXIT(); 

		}
		
		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );  //ÿ20msɨ��һ��
		
	}
	
}


/*
*********************************************************************************************************
*                                          KEY2 TASK
*********************************************************************************************************
*/
static  void  AppTaskKey2 ( void * p_arg )
{
	OS_ERR      err;
	OS_SEM_CTR  ctr;
	CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
									 //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
									 //�����ж�ʱ����ֵ��ԭ��
	uint8_t ucKey2Press = 0;
	
	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                                         //������
		if( Key_Scan ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1, & ucKey2Press ) ) //���KEY2������
		{
		  ctr = OSSemPost((OS_SEM  *)&SemOfKey,                                  //����SemOfKey
							        (OS_OPT   )OS_OPT_POST_ALL,                            //���������еȴ�����
							        (OS_ERR  *)&err);                                      //���ش�������
      
			OS_CRITICAL_ENTER();                                                   //�����ٽ��
			
			printf ( "\r\nKEY2�����£��ͷ�1��ͣ��λ��ʣ��%d��ͣ��λ��\r\n", ctr );
			
			OS_CRITICAL_EXIT();
			
		}
		
		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );                    //ÿ20msɨ��һ��
		
	}
	
}











