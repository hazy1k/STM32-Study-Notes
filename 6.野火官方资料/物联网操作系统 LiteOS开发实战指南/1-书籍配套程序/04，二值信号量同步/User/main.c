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
#include "los_sem.h"
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
UINT32 Read_Task_Handle;
UINT32 Write_Task_Handle;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
/* �����ֵ�ź����ľ�� */
UINT32 BinarySem_Handle;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */

uint8_t ucValue [ 2 ] = { 0x00, 0x00 };


/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_Read_Task(void);
static UINT32 Creat_Write_Task(void);

static void Read_Task(void);
static void Write_Task(void);
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
	
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS��ֵ�ź���ͬ��ʵ�飡\n\n");
	printf("�����ڴ�ӡ��-Successful-����ʵ��ɹ���\n\n");

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
	
	/* ����һ����ֵ�ź���*/
	uwRet = LOS_BinarySemCreate(1,&BinarySem_Handle);
	if (uwRet != LOS_OK)
	{
		printf("BinarySem����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
	}
		
	uwRet = Creat_Read_Task();
  if (uwRet != LOS_OK)
  {
		printf("Read_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Write_Task();
  if (uwRet != LOS_OK)
  {
		printf("Write_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ ������  �� Creat_Read_Task
  * @ ����˵���� ����Read_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Read_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Read_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Read_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Read_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Write_Task
  * @ ����˵���� ����Write_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Write_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Write_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Write_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Write_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/******************************************************************
  * @ ������  �� Read_Task
  * @ ����˵���� Read_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Read_Task(void)
{
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			printf ( "\r\nSuccessful\r\n" );			
		}
		else
		{
			printf ( "\r\nFail\r\n" );			
		}
		
		LOS_SemPost( BinarySem_Handle );   //������ֵ�ź��� xSemaphore
		
  }
}
/******************************************************************
  * @ ������  �� Write_Task
  * @ ����˵���� Write_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Write_Task(void)
{
	/* ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ */
	UINT32 uwRet = LOS_OK;				
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  //��ȡ��ֵ�ź��� BinarySem_Handle��û��ȡ����һֱ�ȴ�		
		ucValue [ 0 ] ++;		
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */		
		ucValue [ 1 ] ++;		
		LOS_SemPost( BinarySem_Handle );    //������ֵ�ź��� xSemaphore		
		LOS_TaskYield();  									//����ʣ��ʱ��Ƭ������һ�������л�	
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

