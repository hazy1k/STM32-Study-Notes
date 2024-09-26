/***************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   STM32ȫϵ�п�����-LiteOS��
  **************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ȫϵ�� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ***************************************************************
  */ 
 /* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_mux.h"
/* �弶����ͷ�ļ� */
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h" 

/********************************** ������ *************************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */

/* ���������� */
UINT32 HighPriority_Task_Handle;
UINT32 MidPriority_Task_Handle;
UINT32 LowPriority_Task_Handle;

/********************************** �ں˶����� *********************************/
/*
 * ����������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
/* ���廥�����ľ�� */
UINT32 Mutex_Handle;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_HighPriority_Task(void);
static UINT32 Creat_MidPriority_Task(void);
static UINT32 Creat_LowPriority_Task(void);

static void HighPriority_Task(void);
static void MidPriority_Task(void);
static void LowPriority_Task(void);
static void BSP_Init(void);


/***************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
			�ڶ���������APPӦ������
			������������LiteOS����ʼ��������ȣ�����ʧ�������������Ϣ
  **************************************************************/
int main(void)
{	
	UINT32 uwRet = LOS_OK;  //����һ�����񴴽��ķ���ֵ��Ĭ��Ϊ�����ɹ�
	
	/* ������س�ʼ�� */
  BSP_Init();
	
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS������ʵ�飡\n\n");

	/* LiteOS �ں˳�ʼ�� */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }
	
	 /* ����APPӦ���������е�Ӧ�����񶼿��Է�������������� */
	uwRet = AppTaskCreate();
	if (uwRet != LOS_OK)
  {
		printf("AppTaskCreate��������ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }

  /* ����LiteOS������� */
  LOS_Start();
	
	//��������²���ִ�е�����
	while(1);
}


/*********************************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� ���񴴽���Ϊ�˷���������е����񴴽����������Է��������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static UINT32 AppTaskCreate(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;
	
	/* ����һ��������*/
	uwRet = LOS_MuxCreate(&Mutex_Handle);
	if (uwRet != LOS_OK)
	{
		printf("Mutex����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
	}
		
	uwRet = Creat_HighPriority_Task();
  if (uwRet != LOS_OK)
  {
		printf("HighPriority_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_MidPriority_Task();
  if (uwRet != LOS_OK)
  {
		printf("MidPriority_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_LowPriority_Task();
  if (uwRet != LOS_OK)
  {
		printf("LowPriority_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	return LOS_OK;
}


/******************************************************************
  * @ ������  �� Creat_HighPriority_Task
  * @ ����˵���� ����HighPriority_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_HighPriority_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 3;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "HighPriority_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)HighPriority_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&HighPriority_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_MidPriority_Task
  * @ ����˵���� ����MidPriority_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_MidPriority_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "MidPriority_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)MidPriority_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&MidPriority_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/*******************************************************************
  * @ ������  �� Creat_MidPriority_Task
  * @ ����˵���� ����MidPriority_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_LowPriority_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "LowPriority_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LowPriority_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&LowPriority_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/******************************************************************
  * @ ������  �� HighPriority_Task
  * @ ����˵���� HighPriority_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void HighPriority_Task(void)
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;	
	
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		//��ȡ������,û��ȡ����һֱ�ȴ�
		uwRet = LOS_MuxPend( Mutex_Handle , LOS_WAIT_FOREVER ); 
		if (uwRet == LOS_OK)
			printf("HighPriority_Task Runing\n");
		
		LED1_TOGGLE;
		LOS_MuxPost( Mutex_Handle );   //����������
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */	
  }
}
/******************************************************************
  * @ ������  �� MidPriority_Task
  * @ ����˵���� MidPriority_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void MidPriority_Task(void)
{	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		printf("MidPriority_Task Runing\n");
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */		
	}
}

/******************************************************************
  * @ ������  �� LowPriority_Task
  * @ ����˵���� LowPriority_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void LowPriority_Task(void)
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;		
	
	static uint32_t i;
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		//��ȡ��������û��ȡ����һֱ�ȴ�		
		uwRet = LOS_MuxPend( Mutex_Handle , LOS_WAIT_FOREVER );  
		if (uwRet == LOS_OK)
			printf("LowPriority_Task Runing\n");
		
		LED2_TOGGLE;
		
    for(i=0;i<2000000;i++)//ģ������ȼ�����ռ�û�����
		{
			//����ʣ��ʱ��Ƭ������һ�������л�
			LOS_TaskYield();
		}
    printf("LowPriority_Task �ͷŻ�����!\r\n");
		LOS_MuxPost( Mutex_Handle );    //����������		
		
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */		
	}
}

/*******************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	USART_Config();

  /* ������ʼ�� */
	Key_GPIO_Config();
}


/*********************************************END OF FILE**********************/

