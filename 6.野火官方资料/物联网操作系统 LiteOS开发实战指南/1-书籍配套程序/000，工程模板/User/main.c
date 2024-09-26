/********************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ����һ��[Ұ��]-STM32F103ȫϵ��LiteOS��ֲʵ�飡
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ȫϵ�� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
 /* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_typedef.h"
#include "los_task.ph"
/* �弶����ͷ�ļ� */
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"

/* ���������� */
UINT32 Test1_Task_Handle;
UINT32 Test2_Task_Handle;

/* �������� */
static UINT32 Creat_Test1_Task(void);
static UINT32 Creat_Test2_Task(void);
static void Test1_Task(void);
static void Test2_Task(void);
static void BSP_Init(void);


/***************************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
			�ڶ���������APPӦ������
			������������LiteOS����ʼ��������ȣ�����ʧ�������������Ϣ
  **************************************************************************/
int main(void)
{	
	UINT32 uwRet = LOS_OK;  //����һ�����񴴽��ķ���ֵ��Ĭ��Ϊ�����ɹ�
	/* ������س�ʼ�� */
  BSP_Init();
	/* ����һ���ַ��� */
	printf("����һ��[Ұ��]-STM32F103ȫϵ��LiteOS��ֲʵ�飡\n\n");
	/* LiteOS ���ĳ�ʼ�� */
	uwRet = LOS_KernelInit();
  if (uwRet != LOS_OK)
   {
	printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�\n");
	return LOS_NOK;
   }
	/* ����Test1_Task���񴴽�ʧ�� */
	uwRet = Creat_Test1_Task();
  if (uwRet != LOS_OK)
   {
	printf("Test1_Task���񴴽�ʧ�ܣ�\n");
       return LOS_NOK;
   }
	/* ����Test2_Task���񴴽�ʧ�� */
	uwRet = Creat_Test2_Task();
  if (uwRet != LOS_OK)
   {
	printf("Test2_Task���񴴽�ʧ�ܣ�\n");
       return LOS_NOK;
   }
   /* ����LiteOS������� */
   LOS_Start();
}
/*********************************************************************************
  * @ ������  �� Test1_Task
  * @ ����˵���� Test1_Task����ʵ��
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Test1_Task(void)
{
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LED2_TOGGLE;
    printf("����1������,ÿ1000ms��ӡһ����Ϣ��\r\n");
		LOS_TaskDelay(1000);		
		
	}
}
/*********************************************************************************
  * @ ������  �� Test2_Task
  * @ ����˵���� Test2_Task����ʵ��
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Test2_Task(void)
{
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
    LED1_TOGGLE;
		printf("����2������,ÿ500ms��ӡһ����Ϣ��\n");
		LOS_TaskDelay(500);
	}
}

/*********************************************************************************
  * @ ������  �� Creat_Test1_Task
  * @ ����˵���� ����Test1_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************************/
static UINT32 Creat_Test1_Task()
{
    UINT32 uwRet = LOS_OK;				/* ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 5;								/* ���ȼ�����ֵԽС�����ȼ�Խ�� */
    task_init_param.pcName = "Test1_Task";						/* ���������ַ�����ʽ��������� */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Test1_Task;	/* �������� */
    task_init_param.uwStackSize = 0x1000;						/* ջ��С����λΪ�֣���4���ֽ� */

    uwRet = LOS_TaskCreate(&Test1_Task_Handle, &task_init_param);/* �������� */
    return uwRet;
}
/*********************************************************************************
  * @ ������  �� Creat_Test2_Task
  * @ ����˵���� ����Test2_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************************/
static UINT32 Creat_Test2_Task()
{
    UINT32 uwRet = LOS_OK;				/* ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ */
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 4;								/* ���ȼ�����ֵԽС�����ȼ�Խ�� */
    task_init_param.pcName = "Test2_Task";						/* ���������ַ�����ʽ��������� */
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Test2_Task;	/* �������� */
    task_init_param.uwStackSize = 0x1000;						/* ջ��С����λΪ�֣���4���ֽ� */

    uwRet = LOS_TaskCreate(&Test2_Task_Handle, &task_init_param);/* �������� */

    return uwRet;
}
/*********************************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************************/
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

}


/*********************************************END OF FILE**********************/

