/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   Ұ��STM32ȫϵ�п����� + LiteOS
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
/* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_memory.h"
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
UINT32 Test_Task_Handle;


/* �������� */
extern LITE_OS_SEC_BSS UINT8* m_aucSysMem0;

static void AppTaskCreate(void);
static UINT32 Creat_Test_Task(void);
static void Test_Task(void);
static void BSP_Init(void);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
			�ڶ���������APPӦ������
			������������LiteOS����ʼ��������ȣ��������ɹ������������Ϣ
  */
int main(void)
{	
	UINT32 uwRet = LOS_OK;
	/* �弶��ʼ�������еĸ�������Ӳ����صĳ�ʼ�������Է�������������� */
	BSP_Init();
	/* ����һ���ַ��� */
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS��˫������ʵ�飡\n");
	/* LiteOS ���ĳ�ʼ�� */
	uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
		printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�\n");
		return LOS_NOK;
    }
	/* ����APPӦ���������е�Ӧ�����񶼿��Է�������������� */
	AppTaskCreate();
	
	/* ����LiteOS������� */
	LOS_Start();
}
static void AppTaskCreate(void)
{
	UINT32 uwRet = LOS_OK;
   	/* ����Test_Task���� */
	uwRet = Creat_Test_Task();
    if (uwRet != LOS_OK)
    {
		printf("Test_Task���񴴽�ʧ�ܣ�\n");
    }

}

/* ����Test_Task����*/
static UINT32 Creat_Test_Task(void)
{
    UINT32 uwRet = LOS_OK;				/* ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;				/* ���ȼ�����ֵԽС�����ȼ�Խ�� */
    task_init_param.pcName = "Test_Task";						/* ������ */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Test_Task;	/* �������� */
    task_init_param.uwStackSize = 1024;						/* ��ջ��С */

    uwRet = LOS_TaskCreate(&Test_Task_Handle, &task_init_param);/* �������� */
    return uwRet;
}



/*********************************************************************************
  * @ ������  �� Clear_Task
  * @ ����˵���� д���Ѿ���ʼ���ɹ����ڴ�ص�ַ����
  * @ ����    �� void
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Test_Task(void)
{
	UINT32 uwRet = LOS_OK;			/* ����һ����ʼ���ķ������ͣ���ʼ��Ϊ�ɹ��ķ���ֵ */
	printf("\n˫�������ʼ����......\n");
	
	LOS_DL_LIST *head;				/* ����һ��˫�������ͷ�ڵ� */
	head = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* ��̬����ͷ�����ڴ� */

	LOS_ListInit(head);				/* ��ʼ��˫������ */
	if (!LOS_ListEmpty(head))		/* �ж��Ƿ��ʼ���ɹ� */
	{
			printf("˫�������ʼ��ʧ��!\n\n");
	}
	else
	{
		printf("˫�������ʼ���ɹ�!\n\n");
	}
	
	printf("��ӽڵ��β�ڵ����......\n");			/* ����ڵ㣺˳��������ĩβ���� */

    LOS_DL_LIST *node1 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* ��̬�����һ�������ڴ� */
    LOS_DL_LIST *node2 = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));/* ��̬����ڶ��������ڴ� */
    LOS_DL_LIST *tail = (LOS_DL_LIST *)LOS_MemAlloc(m_aucSysMem0, sizeof(LOS_DL_LIST));	/* ��̬����β�����ڴ� */

	printf("��ӵ�һ���ڵ���ڶ����ڵ�.....\n");
    LOS_ListAdd(head,node1);					/* ��ӵ�һ���ڵ㣬������ͷ����� */
    LOS_ListAdd(node1,node2);					/* ��ӵڶ����ڵ㣬������һ���ڵ��� */
    if ((node1->pstPrev == head) || (node2->pstPrev == node1))	/* �ж��Ƿ����ɹ� */
    {
        printf("��ӽڵ�ɹ�!\n\n");
    }
	else
	{
		printf("��ӽڵ�ʧ��!\n\n");
	}
	printf("��β�ڵ����˫�������ĩβ.....\n");
	LOS_ListTailInsert(head, tail);				/* ��β�ڵ����˫�������ĩβ */
    if (tail->pstPrev == node2)					/* �ж��Ƿ����ɹ� */		
    {
        printf("����β�ڵ���ӳɹ�!\n\n");
    }
	else
	{
		printf("����β�ڵ����ʧ��!\n\n");
	}
	
    printf("ɾ���ڵ�......\n");				/* ɾ�����нڵ� */		
    LOS_ListDelete(node1);					/* ɾ����һ���ڵ� */	
    LOS_MemFree(m_aucSysMem0, node1);		/* �ͷŵ�һ���ڵ���ڴ棬��֤����ĳ�������ڴ�ռ� */	
    if (head->pstNext == node2)				/* �ж��Ƿ�ɾ���ɹ� */
    {
        printf("ɾ���ڵ�ɹ�\n\n");
    }
    else
    {
        printf("ɾ���ڵ�ʧ��\n\n");
        
    }
	
	while(1)
	{
		LED2_TOGGLE;			//LED2��ת
		printf("����������!\n");
		LOS_TaskDelay (2000);
	}
}


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

