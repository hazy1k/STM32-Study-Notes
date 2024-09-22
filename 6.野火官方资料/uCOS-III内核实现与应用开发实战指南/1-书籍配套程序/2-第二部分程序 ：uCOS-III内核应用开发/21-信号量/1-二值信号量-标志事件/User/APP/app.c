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
***************************************************************************
*                             LOCAL DEFINES
***************************************************************************
*/

OS_SEM SemOfKey;          //��־KEY1�Ƿ񱻵����Ķ�ֵ�ź���


/*
**************************************************************************
*                                TCB
**************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;                                //������ƿ�

static  OS_TCB   AppTaskKeyTCB;
static  OS_TCB   AppTaskLed1TCB;


/*
***************************************************************************
*                                                STACKS
*************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskKeyStk [ APP_TASK_KEY_STK_SIZE ];
static  CPU_STK  AppTaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];


/*
*************************************************************************
*                                         FUNCTION PROTOTYPES
*************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);                       //����������

static  void  AppTaskKey  ( void * p_arg );
static  void  AppTaskLed1 ( void * p_arg );


/*
******************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  
*               It is assumed that your code will callmain() 
*               once you have performed all necessary initialization.
* Arguments   : none
*
* Returns     : none
*****************************************************************************
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
               (OS_SEM_CTR   )0,             //�ź���������ָʾ�¼����������Ը�ֵΪ0����ʾ�¼���û�з���
               (OS_ERR      *)&err);         //��������
							 

		/* ���� AppTaskKey ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskKeyTCB,                              //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Key",                              //��������
                 (OS_TASK_PTR ) AppTaskKey,                                 //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,                          //��������ȼ�
                 (CPU_STK    *)&AppTaskKeyStk[0],                           //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,                 //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,                      //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
    
		/* ���� LED1 ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskLed1TCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Led1",                             //��������
                 (OS_TASK_PTR ) AppTaskLed1,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LED1_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskLed1Stk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LED1_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
		
		OSTaskDel ( & AppTaskStartTCB, & err );                     //ɾ����ʼ������������������
		
		
}


/*
****************************************************************************
*                                          KEY TASK
****************************************************************************
*/
static  void  AppTaskKey ( void * p_arg )
{
	OS_ERR      err;

	uint8_t ucKey1Press = 0;
	
	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                                         //������
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) ) //���KEY1������
		  OSSemPost((OS_SEM  *)&SemOfKey,                                        //����SemOfKey
							 (OS_OPT   )OS_OPT_POST_ALL,                                   //���������еȴ�����
							 (OS_ERR  *)&err);                                             //���ش�������

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );                   //ÿ20msɨ��һ��
		
	}
	
}


/*
*********************************************************************************
*                                          LED1 TASK
*********************************************************************************
*/

static  void  AppTaskLed1 ( void * p_arg )
{
    OS_ERR         err;
	  CPU_INT32U     cpu_clk_freq;
	  CPU_TS         ts_sem_post, ts_sem_get;
		CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
										 //�������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR���ٽ�ι��ж�ֻ�豣��SR��
										 //�����ж�ʱ����ֵ��ԭ��
    (void)p_arg;


	  cpu_clk_freq = BSP_CPU_ClkFreq();               //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���
	
	
    while (DEF_TRUE) {                              //������
					
			OSSemPend ((OS_SEM   *)&SemOfKey,             //�ȴ����ź���������
								 (OS_TICK   )0,                     //�����޵ȴ�
								 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //���û���ź������þ͵ȴ�
								 (CPU_TS   *)&ts_sem_post,          //��ȡ�ź������һ�α�������ʱ���
								 (OS_ERR   *)&err);                 //���ش�������
				
			ts_sem_get = OS_TS_GET();                     //��ȡ����ȴ�ʱ��ʱ���
				
			macLED1_TOGGLE ();                            //�л�LED1������״̬
				
			OS_CRITICAL_ENTER();                          //�����ٽ�Σ���ϣ�����洮�ڴ�ӡ�⵽�ж�
			
			printf ( "\r\n�����ź�����ʱ�����%d", ts_sem_post );
			printf ( "\r\n����ȴ�״̬��ʱ�����%d", ts_sem_get );
			printf ( "\r\n���յ��ź����뷢���ź�����ʱ�����%dus\r\n", 
			        ( ts_sem_get - ts_sem_post ) / ( cpu_clk_freq / 1000000 ) );
			
			OS_CRITICAL_EXIT(); 

    }
		
		
}












