/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "includes.h"

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/
static  OS_TCB	 AppTaskStartTCB;		     //����������ƿ�
static  OS_TCB   AppTaskUpdateTCB;
static  OS_TCB   AppTaskShotTCB;
static  OS_TCB   AppTaskUserappTCB;
/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/
static	CPU_STK	 AppTaskStartStk[APP_TASK_START_STK_SIZE];	   //���������ջ
static  CPU_STK  AppTaskUpdateStk[APP_TASK_UPDATE_STK_SIZE];
static  CPU_STK  AppTaskCOMStk[APP_TASK_SHOT_STK_SIZE];
__align(8) static  CPU_STK  AppTaskUserappStk[APP_TASK_USERAPP_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void   AppTaskStart(void *p_arg);
static  void   AppTaskCreate(void);
static  void   AppObjCreate (void);
static  void   AppTaskUpdate(void *p_arg);
static  void   AppTaskShot(void *p_arg);
static  void   AppTaskUserapp(void *p_arg);

/*********************************************************************
*
*       data
*
**********************************************************************
*/
/*---------- kernel objects ----------*/
static  OS_SEM   SEM_SHOT;

/*---------- FatFS data ----------*/
FIL 		file;				/* file objects */
FRESULT result; 
UINT 		bw;  			/* File R/W count */

static uint8_t Pic_Name=0;
static KEY Key1,Key2;
uint8_t key_flag=1;//1����������ͼ��0�����ð�����ͼ
uint8_t IsCal =0;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	OS_ERR err;	
	
	/* ��ʼ��"uC/OS-III"�ں� */  
	OSInit(&err);	
	
	/*��������*/
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,              // ������ƿ�ָ��          
               (CPU_CHAR   *)"App Task Start",		          // ��������
               (OS_TASK_PTR )AppTaskStart, 	                  // �������ָ��
               (void       *)0,			                      // ���ݸ�����Ĳ���parg
               (OS_PRIO     )APP_TASK_START_PRIO,			  // �������ȼ�
               (CPU_STK    *)&AppTaskStartStk[0],	          // �����ջ����ַ
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE/10,	  // ��ջʣ�ྯ����
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,		  // ��ջ��С
               (OS_MSG_QTY  )1u,			                      // �ɽ��յ������Ϣ������
               (OS_TICK     )0u,			                      // ʱ��Ƭ��תʱ��
               (void       *)0,			                      // ������ƿ���չ��Ϣ
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	      // ����ѡ��
               (OS_ERR     *)&err);		                    // ����ֵ
	  
  /* ����������ϵͳ������Ȩ����uC/OS-III */
  OSStart(&err);  
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

static void  AppTaskStart(void *p_arg)
{
    OS_ERR      err;

   (void)p_arg;
	
	/* �弶��ʼ�� */	
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();
		BSP_Tick_Init();

//    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif
    
    OS_INFO("Creating Application Tasks...\n\r");
    AppTaskCreate();                                            /* Create Application Tasks                             */
    OS_INFO("Creating Application Events...\n\r");
    AppObjCreate();                                             /* Create Application Objects  */
	
	/*Delete task*/
//	OSTaskDel(&AppTaskStartTCB,&err);	
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        GUI_TOUCH_Exec();
				Key_RefreshState(&Key2);//ˢ�°���״̬
				if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0 && key_flag)//���������¹�
				{
					OSSemPost ((OS_SEM  *)&SEM_SHOT,
							 (OS_OPT   )OS_OPT_POST_ALL,
							 (OS_ERR  *)&err);
					Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
				}
        OSTimeDlyHMSM(0, 0, 0, 15,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskGUIUpdate
*	����˵��: 
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����3
*********************************************************************************************************
*/
static void AppTaskUpdate(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
	
	while(TPAD_Init())
	{
		bsp_DelayUS(5*1000);
	}	
	
	while(1)
	{	
		if(TPAD_Scan(0))
		{
			//LED1_TOGGLE;
			macBEEP_ON();
		}
		OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
		macBEEP_OFF();		
		//printf("cpuusage:%d\n",OSStatTaskCPUUsage);
	}   
}

/*
*********************************************************************************************************
*	�� �� ��: _WriteByte2File()
*	����˵��: 
*	��    �Σ�  	
*	�� �� ֵ: 
*********************************************************************************************************
*/
static void _WriteByte2File(uint8_t Data, void * p) 
{
	result = f_write(p, &Data, 1, &bw);
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCom
*	����˵��: 
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����3
*********************************************************************************************************
*/
static void AppTaskShot(void *p_arg)
{	
	OS_ERR  err;
	CPU_TS 	Ts;
	uint8_t i;
	char 	buf[20];
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
		//��������ֱ�����������£�
		OSSemPend((OS_SEM   *)&SEM_SHOT,
						 (OS_TICK   )0,                     //��������������Ϊ0��һֱ�ȴ���ȥ
						 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //���û���ź������þ͵ȴ�
						 (CPU_TS   *)&Ts,             //���������ָ�����ź������ύ��ǿ�ƽ���ȴ�״̬�������ź�����ɾ����ʱ���        
						 (OS_ERR   *)&err);
		i=0;
		while(i<0xff)
		{
			sprintf((char *)buf,"0:/picture%d.bmp",i);
			result=f_open(&file,(const TCHAR*)buf,FA_READ);
			if(result==FR_NO_FILE)break;	
			else
				f_close(&file);
			i++;
		}
		OSSchedLock(&err);
		printf("start shot picture\n\r");
		/* ������ͼ */
		result = f_open(&file,buf, FA_WRITE|FA_CREATE_ALWAYS);
		/* ��SD������BMPͼƬ */
		GUI_BMP_Serialize(_WriteByte2File,&file);
		
		/* ������ɺ�ر�file */
		result = f_close(&file);
		printf("shot picture success\n\r");
		Pic_Name++;
		OSSchedUnlock(&err);	
	}  
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: LED4��˸
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����2
*********************************************************************************************************
*/
static void AppTaskUserapp(void *p_arg)
{
	(void)p_arg;		/* ����������澯 */
	if(IsCal!=0x55)
	{
		Touch_MainTask();
	}
	while (1) 
	{   		
		UserAPP();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppTaskCreate(void)
{
	OS_ERR      err;
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskUpdateTCB,             
                 (CPU_CHAR     *)"App Task Update",
                 (OS_TASK_PTR   )AppTaskUpdate, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_UPDATE_PRIO,
                 (CPU_STK      *)&AppTaskUpdateStk[0],
                 (CPU_STK_SIZE  )APP_TASK_UPDATE_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_UPDATE_STK_SIZE,
                 (OS_MSG_QTY    )1,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskShotTCB,            
                 (CPU_CHAR     *)"App Task SHOT",
                 (OS_TASK_PTR   )AppTaskShot, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_SHOT_PRIO,
                 (CPU_STK      *)&AppTaskCOMStk[0],
                 (CPU_STK_SIZE  )APP_TASK_SHOT_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_SHOT_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskUserappTCB,             
                 (CPU_CHAR     *)"App Task Userapp",
                 (OS_TASK_PTR   )AppTaskUserapp, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_USERAPP_PRIO,
                 (CPU_STK      *)&AppTaskUserappStk[0],
                 (CPU_STK_SIZE  )APP_TASK_USERAPP_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_USERAPP_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);				 
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
	OS_ERR      err;
	
  //����һ�������������ľ���ϸ��������Ȥ�ɲ���
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	//����һ���ź������������µ�ʱ��postһ���ź�������������
	OSSemCreate ((OS_SEM *)&SEM_SHOT,     //ָ���ź���������ָ��
			   (CPU_CHAR    *)"SEM_SHOT",    //�ź���������
			   (OS_SEM_CTR   )0,             //�ź���������ָʾ�¼����������Ը�ֵΪ0����ʾ�¼���û�з���
			   (OS_ERR      *)&err);         //��������
}

/*********************************************END OF FILE**********************/
