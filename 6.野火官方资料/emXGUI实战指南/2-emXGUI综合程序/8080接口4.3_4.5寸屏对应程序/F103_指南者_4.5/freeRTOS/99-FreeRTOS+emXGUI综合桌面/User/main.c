/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 �������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h"   
#include "./usart/bsp_usart.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */


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


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void GUI_Thread_Entry(void* pvParameters);/* Test_Task����ʵ�� */
static void DEBUG_Thread_Entry(void* parameter);

static void BSP_Init(void);
  
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();  
  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* ������ں��� */
                        (const char*    )"gui",/* �������� */
                        (uint16_t       )1024/2,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )10, /* ��������ȼ� */
                        (TaskHandle_t*  )NULL);/* ������ƿ�ָ�� */ 
  xReturn = xTaskCreate((TaskFunction_t )DEBUG_Thread_Entry,  /* ������ں��� */
                        (const char*    )"debug",/* �������� */
                        (uint16_t       )256,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )NULL);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}

extern void GUI_Startup(void);

/**********************************************************************
  * @ ������  �� gui_thread_entry
  * @ ����˵���� gui_thread_entry��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void GUI_Thread_Entry(void* parameter)
{	
  printf("Ұ��emXGUI��ʾ����\n\n");

  /* ִ�б��������᷵�� */
	GUI_Startup();
  
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
  }
}

void FSMC_Init(void);
void FSMC_InitLCD(void);

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
extern void CPU_TS_TmrInit(void);
static void BSP_Init(void)
{
	/* Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* usart �˿ڳ�ʼ�� */
  Debug_USART_Config();
  CPU_TS_TmrInit();

  FSMC_Init();
	FSMC_InitLCD();

  /* KEY �˿ڳ�ʼ�� */
  Key_GPIO_Config();
}
 
static void DEBUG_Thread_Entry(void* parameter)
{
/* ����DEBUG */
	char tasks_buf[360] = {0};
	uint8_t tog=0;
	
	while(1)
{
	if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN))
	{
		memset(tasks_buf, 0, 360);
	#if 0
		strcat((char *)tasks_buf, "��������\t���м���\tʹ����\r\n" );

		strcat((char *)tasks_buf, "---------------------------------------------\r\n");

		/* displays the amount of time each task has spent in the Running state

		* in both absolute and percentage terms. */

		vTaskGetRunTimeStats((char *)(tasks_buf + strlen(tasks_buf)));

		strcat((char *)tasks_buf, "\r\n");
		printf("%s\r\n",tasks_buf);

		memset(tasks_buf, 0, 512);
	#endif
		strcat((char *)tasks_buf, "\n��������\t����״̬\t���ȼ�\tʣ���ջ\t�������\r\n" );

		strcat((char *)tasks_buf, "---------------------------------------------\r\n");

		vTaskList((char *)(tasks_buf + strlen(tasks_buf)));

		strcat((char *)tasks_buf, "\r\n---------------------------------------------\r\n");

		strcat((char *)tasks_buf, "B : ����,\t R : ����,\t D : ɾ��,\t S : ��ͣ\r\n");
		printf("%s\r\n",tasks_buf);
		
		printf("ʣ���ܵĶѿռ�Ϊ:%d Byte\n",xPortGetFreeHeapSize());
	//	unsigned portBASE_TYPE uxHighWaterMark;
	//	uxHighWaterMark=uxTaskGetStackHighWaterMark( pvCreatedTask_ToggleLed4 );
	//��printf("ʣ��ջ�ռ��Ƕ��٣�%ld.\r\n",uxHighWaterMark);
		vTaskDelay(1000);
	}else
	{
	vTaskDelay(1);
	}
}
}
/*********************************************END OF FILE**********************/

