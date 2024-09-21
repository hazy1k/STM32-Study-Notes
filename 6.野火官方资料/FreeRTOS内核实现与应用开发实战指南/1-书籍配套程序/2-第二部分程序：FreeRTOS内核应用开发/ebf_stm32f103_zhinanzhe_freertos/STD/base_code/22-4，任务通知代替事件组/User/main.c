/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0 + STM32 ����֪ͨ�����¼���
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32ȫϵ�п����� 
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
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "limits.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t LED_Task_Handle = NULL;/* LED_Task������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY_Task������ */

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


/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
#define KEY1_EVENT  (0x01 << 0)//�����¼������λ0
#define KEY2_EVENT  (0x01 << 1)//�����¼������λ1

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void LED_Task(void* pvParameters);/* LED_Task ����ʵ�� */
static void KEY_Task(void* pvParameters);/* KEY_Task ����ʵ�� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

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
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS����֪ͨ�����¼���ʵ�飡\n");
  printf("����KEY1|KEY2���������¼�֪ͨ��\n");
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
    
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
  
  /* ����KEY_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,  /* ������ں��� */
                        (const char*    )"KEY_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&KEY_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED_Task(void* parameter)
{	
  uint32_t r_event = 0;  /* ����һ���¼����ձ��� */
  uint32_t last_event = 0;/* ����һ�������¼��ı��� */
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  /* ������һ������ѭ�������ܷ��� */
  while (1)
	{
    /* BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                                  uint32_t ulBitsToClearOnExit, 
                                  uint32_t *pulNotificationValue, 
                                  TickType_t xTicksToWait ); 
     * ulBitsToClearOnEntry����û�н��յ�����֪ͨ��ʱ������ֵ֪ͨ��˲�����ȡ
       ��ֵ���а�λ�����㣬���˲���ΪOxfffff����ULONG_MAX��ʱ��ͻὫ����ֵ֪ͨ���㡣
     * ulBits ToClearOnExit��������յ�������֪ͨ����������Ӧ�Ĵ����˳�����֮ǰ��
       ����ֵ֪ͨ��˲�����ȡ��ֵ���а�λ�����㣬���˲���Ϊ0xfffff����ULONG MAX��ʱ��
       �ͻὫ����ֵ֪ͨ���㡣
     * pulNotification Value���˲���������������ֵ֪ͨ��
     * xTick ToWait������ʱ�䡣
     *
     * ����ֵ��pdTRUE����ȡ��������֪ͨ��pdFALSE������֪ͨ��ȡʧ�ܡ�
     */
    //��ȡ����֪ͨ ,û��ȡ����һֱ�ȴ�
		xReturn = xTaskNotifyWait(0x0,			//���뺯����ʱ���������bit
                              ULONG_MAX,	  //�˳�������ʱ��������е�bitR
                              &r_event,		  //��������ֵ֪ͨ                    
                              portMAX_DELAY);	//����ʱ��
    if( pdTRUE == xReturn )
    { 
      last_event |= r_event;
      /* ���������ɲ�����ȷ */      
      if(last_event == (KEY1_EVENT|KEY2_EVENT)) 
      {
        last_event = 0;     /* ��һ�ε��¼����� */
        printf ( "Key1��Key2������\n");		
        LED1_TOGGLE;       //LED1	��ת 
      }
      else  /* ����͸����¼� */
        last_event = r_event;   /* ������һ�δ������¼� */
    }
    
  }
}

/**********************************************************************
  * @ ������  �� KEY_Task
  * @ ����˵���� KEY_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{	 
    /* ������һ������ѭ�������ܷ��� */
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )       
		{
      printf ( "KEY1������\n" );
      /* ԭ��:BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify, 
                                      uint32_t ulValue, 
                                      eNotifyAction eAction ); 
       * eNoAction = 0��֪ͨ�������������ֵ֪ͨ��
       * eSetBits��     ��������ֵ֪ͨ�е�λ��
       * eIncrement��   ���������ֵ֪ͨ��
       * eSetvaluewithoverwrite�����ǵ�ǰ֪ͨ
       * eSetValueWithoutoverwrite �����ǵ�ǰ֪ͨ
       * 
       * pdFAIL��������eAction����ΪeSetValueWithoutOverwrite��ʱ��
       * �������ֵ֪ͨû�и��³ɹ��ͷ���pdFAIL��
       * pdPASS: eAction ����Ϊ����ѡ���ʱ��ͳһ����pdPASS��
      */
			/* ����һ���¼�1 */
			xTaskNotify((TaskHandle_t	)LED_Task_Handle,//��������֪ͨ��������
                  (uint32_t		)KEY1_EVENT,			//Ҫ�������¼�
                  (eNotifyAction)eSetBits);			//��������ֵ֪ͨ�е�λ
									
		}
    
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )       
		{
      printf ( "KEY2������\n" );	
			/* ����һ���¼�2 */
			xTaskNotify((TaskHandle_t	)LED_Task_Handle,//��������֪ͨ��������
                  (uint32_t		)KEY2_EVENT,			//Ҫ�������¼�
                  (eNotifyAction)eSetBits);			//��������ֵ֪ͨ�е�λ				
		}
		vTaskDelay(20);     //ÿ20msɨ��һ��		
  }
}

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
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
  
  /* ������ʼ��	*/
  Key_GPIO_Config();

}

/********************************END OF FILE****************************/
