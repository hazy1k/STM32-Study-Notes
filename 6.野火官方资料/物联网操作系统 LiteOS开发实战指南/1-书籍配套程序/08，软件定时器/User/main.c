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
#include "los_swtmr.h"
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

/* ���嶨ʱ�������ID�� */
UINT16 Timer1_Handle;
UINT16 Timer2_Handle;

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

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
static UINT32 TmrCb_Count1 = 0;
static UINT32 TmrCb_Count2 = 0;


/* �������� */
static UINT32 AppTaskCreate(void);
static void Timer1_Callback(UINT32 arg);
static void Timer2_Callback(UINT32 arg);

static void LED_Task(void);
static void Key_Task(void);
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
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;  
	
	/* ������س�ʼ�� */
  BSP_Init();
	
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS�����ʱ��ʵ�飡\n\n");
	printf("Timer1_Callbackִֻ��һ�ξͱ�����\n");
	printf("Timer2_Callback��ѭ��ִ��\n");
	
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
	
	/* ����һ�������ʱ����ʱ��*/
	uwRet = LOS_SwtmrCreate(5000, 					/* �����ʱ���Ķ�ʱʱ��*/	
													LOS_SWTMR_MODE_ONCE, 	/* �����ʱ��ģʽ һ��ģʽ */
													(SWTMR_PROC_FUNC)Timer1_Callback,		/* �����ʱ���Ļص����� */
													&Timer1_Handle,					/* �����ʱ����id */
													0);		
	if (uwRet != LOS_OK)
    {
		printf("�����ʱ��Timer1����ʧ�ܣ�\n");
    }
	uwRet = LOS_SwtmrCreate(1000, 					/* �����ʱ���Ķ�ʱʱ�䣨ms��*/	
													LOS_SWTMR_MODE_PERIOD, 	/* �����ʱ��ģʽ ����ģʽ */
													(SWTMR_PROC_FUNC)Timer2_Callback,		/* �����ʱ���Ļص����� */
													&Timer2_Handle,			/* �����ʱ����id */
													0);		
	if (uwRet != LOS_OK) 
  {
		printf("�����ʱ��Timer2����ʧ�ܣ�\n");
		return uwRet;
  }
	
	/* ����һ�������ʱ����ʱ��*/
	uwRet = LOS_SwtmrStart(Timer1_Handle);
  if (LOS_OK != uwRet)
  {
		printf("start Timer1 failed\n");
		return uwRet;
  }
  else
  {
		printf("start Timer1 sucess\n");
  }
	/* ����һ�������ʱ����ʱ��*/
	uwRet = LOS_SwtmrStart(Timer2_Handle);
  if (LOS_OK != uwRet)
  {
		printf("start Timer2 failed\n");
		return uwRet;
  }
	else
	{
		printf("start Timer2 sucess\n");
	}
	
	return LOS_OK;
}

/*********************************************************************************
  * @ ������  �� Timer1_Callback
  * @ ����˵���� �����ʱ���ص�����1
  * @ ����    �� ����1����������δʹ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Timer1_Callback(UINT32 arg)
{
    UINT32 tick_num1;

    TmrCb_Count1++;						/* ÿ�ص�һ�μ�һ */
		LED1_TOGGLE;
    tick_num1 = (UINT32)LOS_TickCountGet();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	
    printf("Timer_CallBack_Count1=%d\n", TmrCb_Count1);
    printf("tick_num1=%d\n", tick_num1);
}
/*********************************************************************************
  * @ ������  �� Timer2_Callback
  * @ ����˵���� �����ʱ���ص�����2
  * @ ����    �� ����1����������δʹ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Timer2_Callback(UINT32 arg)
{
    UINT32 tick_num2;
	
    TmrCb_Count2++;				/* ÿ�ص�һ�μ�һ */
		LED2_TOGGLE;
    tick_num2 = (UINT32)LOS_TickCountGet();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	
    printf("Timer_CallBack_Count2=%d\n", TmrCb_Count2);
	
    printf("tick_num2=%d\n", tick_num2);
    
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

