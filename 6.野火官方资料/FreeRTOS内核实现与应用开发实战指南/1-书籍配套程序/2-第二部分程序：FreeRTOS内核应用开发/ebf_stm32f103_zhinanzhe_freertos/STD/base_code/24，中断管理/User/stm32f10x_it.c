/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_exti.h"

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
//systick�жϷ�����
void SysTick_Handler(void)
{	
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
    
    xPortSysTickHandler();
    
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  }
#endif  /* INCLUDE_xTaskGetSchedulerState */
}



/* ���������ⲿ���� & ��ֵ�ź��� */
extern QueueHandle_t Test_Queue;
extern SemaphoreHandle_t BinarySem_Handle;

static uint32_t send_data1 = 1;
static uint32_t send_data2 = 2;

/*********************************************************************************
  * @ ������  �� KEY1_IRQHandler
  * @ ����˵���� �жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void KEY1_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
  //ȷ���Ƿ������EXTI Line�ж�
  uint32_t ulReturn;
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
    /* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
		xQueueSendFromISR(Test_Queue, /* ��Ϣ���еľ�� */
											&send_data1,/* ���͵���Ϣ���� */
											&pxHigherPriorityTaskWoken);
		
		//�����Ҫ�Ļ�����һ�������л�
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
		
		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
  
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/*********************************************************************************
  * @ ������  �� KEY1_IRQHandler
  * @ ����˵���� �жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void KEY2_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
  uint32_t ulReturn;
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
    /* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
		xQueueSendFromISR(Test_Queue, /* ��Ϣ���еľ�� */
											&send_data2,/* ���͵���Ϣ���� */
											&pxHigherPriorityTaskWoken);
		
		//�����Ҫ�Ļ�����һ�������л�
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
		
		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
  
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/*********************************************************************************
  * @ ������  �� DEBUG_USART_IRQHandler
  * @ ����˵���� �����жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
  uint32_t ulReturn;
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();

	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_IDLE)!=RESET)
	{		
		Uart_DMA_Rx_Data();       /* �ͷ�һ���ź�������ʾ�����ѽ��� */
		USART_ReceiveData(DEBUG_USARTx); /* �����־λ */
	}	 
  
  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}


