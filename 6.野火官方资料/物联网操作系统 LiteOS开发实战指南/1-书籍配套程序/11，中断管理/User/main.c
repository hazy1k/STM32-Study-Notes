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
#include "los_queue.h"
#include "los_sem.h"
/* �弶����ͷ�ļ� */
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_key.h" 
#include "bsp_exti.h"

/* ��׼��ͷ�ļ� */
#include <string.h>

/********************************** ������ *************************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */

/* ���������� */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

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
 /* ������Ϣ���еľ�� */
UINT32 Test_Queue_Handle;
/* �����ֵ�ź����ľ�� */
UINT32 BinarySem_Handle;

/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
/* ������Ϣ���г��� */
#define  TEST_QUEUE_LEN  	10
#define  TEST_QUEUE_SIZE  10

/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_LED_Task(void);
static UINT32 Creat_Key_Task(void);

static void LED_Task(void);
static void Key_Task(void);
static void BSP_Init(void);

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 
extern char Usart_Rx_Buf[USART_RBUFF_SIZE];
	
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
	
	/* �弶��ʼ�������еĸ�������Ӳ����صĳ�ʼ�������Է�������������� */
  BSP_Init();
	
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS�жϹ���ʵ�飡\n");
	printf("����KEY1����KEY2���Ͷ�����Ϣ\n");
	printf("Receive_Task������յ���Ϣ�ڴ��ڻ���\n\n");
	
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
	
	/* ����һ�����Զ���*/
	uwRet = LOS_QueueCreate("Test_Queue",				/* ���е����� */
													TEST_QUEUE_LEN,			/* ���еĳ��� */
													&Test_Queue_Handle,	/* ���е�ID(���) */
													0,						/* ����ģʽ���ٷ���ʱδʹ�� */
													TEST_QUEUE_SIZE);	/* �ڵ��С����λΪ�ֽ� */
	if (uwRet != LOS_OK)
  {
		printf("Test_Queue���д���ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	/* ����һ����ֵ�ź���*/
	uwRet = LOS_BinarySemCreate(0,&BinarySem_Handle);
	if (uwRet != LOS_OK)
	{
		printf("BinarySem����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
	}
	
	uwRet = Creat_LED_Task();
  if (uwRet != LOS_OK)
  {
		printf("LED_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Key_Task();
  if (uwRet != LOS_OK)
  {
		printf("Key_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ ������  �� Creat_LED_Task
  * @ ����˵���� ����LED_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_LED_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "LED_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&LED_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Key_Task
  * @ ����˵���� ����Key_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Key_Task()
{
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Key_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Key_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Key_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/******************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void LED_Task(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;
	UINT32 *r_queue;
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
		uwRet = LOS_QueueRead(Test_Queue_Handle,	/* ��ȡ�����գ����е�ID(���) */
													&r_queue,			/* ��ȡ�����գ������ݱ���λ�� */
													TEST_QUEUE_SIZE,		/* ��ȡ�����գ������ݵĳ��� */
													LOS_WAIT_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */
		if(LOS_OK == uwRet)
		{
			printf("�����жϵ��� KEY%d !\n",*r_queue);
		}
		else
		{
			printf("���ݽ��ճ���,�������0x%X\n",uwRet);
		}
	}
}
/******************************************************************
  * @ ������  �� Key_Task
  * @ ����˵���� Key_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Key_Task(void)
{			
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			

	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
    //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		uwRet = LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); 
    if(LOS_OK == uwRet)
    {
      printf("�յ�����:%s\n",Usart_Rx_Buf);
      memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);/* ���� */
      LED2_TOGGLE;
    }
		
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

	/* DMA��ʼ��	*/
	USARTx_DMA_Config();
	
	/* ���ڳ�ʼ��	*/
	USART_Config();
  
  /* ������ʼ��	*/
  Key_GPIO_Config();
	
	/* ������ʼ��	*/
	EXTI_Key_Config();
}


/*********************************************END OF FILE**********************/

