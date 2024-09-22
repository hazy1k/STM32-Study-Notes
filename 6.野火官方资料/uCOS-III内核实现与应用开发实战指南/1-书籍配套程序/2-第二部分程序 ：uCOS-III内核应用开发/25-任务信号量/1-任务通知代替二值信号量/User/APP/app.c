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


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;      //������ƿ�

static  OS_TCB   AppTaskPostTCB;
static  OS_TCB   AppTaskPendTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskPostStk [ APP_TASK_POST_STK_SIZE ];
static  CPU_STK  AppTaskPendStk [ APP_TASK_PEND_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //����������

static  void  AppTaskPost   ( void * p_arg );
static  void  AppTaskPend   ( void * p_arg );


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


		/* ���� AppTaskPost ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Post",                             //��������
                 (OS_TASK_PTR ) AppTaskPost,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_POST_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskPostStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

		/* ���� AppTaskPend ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Pend",                             //��������
                 (OS_TASK_PTR ) AppTaskPend,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_PEND_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskPendStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
    
		OSTaskDel ( & AppTaskStartTCB, & err );                     //ɾ����ʼ������������������
		
		
}


/*
*********************************************************************************************************
*                                          POST TASK
*********************************************************************************************************
*/
static  void  AppTaskPost ( void * p_arg )
{
	OS_ERR      err;

	uint8_t ucKey1Press = 0;     //���䰴��KEY1״̬                
	
	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                                         //������
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) ) //���KEY1������
		{
      printf("���������ź���\n");
			/* ���������ź��� */
			OSTaskSemPost((OS_TCB  *)&AppTaskPendTCB,                              //Ŀ������
										(OS_OPT   )OS_OPT_POST_NONE,                             //ûѡ��Ҫ��
										(OS_ERR  *)&err);                                        //���ش�������
      
      
	 }
		
		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );                   //ÿ20msɨ��һ��
		
	}
	
}


/*
*********************************************************************************************************
*                                          PEND TASK
*********************************************************************************************************
*/
static  void  AppTaskPend ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS         ts;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();

	
	(void)p_arg;

					 
  cpu_clk_freq = BSP_CPU_ClkFreq();                   //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���

	
	while (DEF_TRUE) {                                  //������
		/* ��������ֱ��KEY1������ */
		OSTaskSemPend ((OS_TICK   )0,                     //�����޵ȴ�
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									 (CPU_TS   *)&ts,                   //��ȡ�ź�����������ʱ���
									 (OS_ERR   *)&err);                 //���ش�������
		
		ts = OS_TS_GET() - ts;                            //�����ź����ӷ��������յ�ʱ���
		
		macLED1_TOGGLE ();                                //�л�LED1������״̬
		
		OS_CRITICAL_ENTER();                              //�����ٽ�Σ����⴮�ڴ�ӡ�����

		printf ( "�����ź����ӱ����͵������յ�ʱ�����%dus\n\n", 
						ts / ( cpu_clk_freq / 1000000 ) );        
		
		OS_CRITICAL_EXIT();                               //�˳��ٽ��
		
	}
	
}











