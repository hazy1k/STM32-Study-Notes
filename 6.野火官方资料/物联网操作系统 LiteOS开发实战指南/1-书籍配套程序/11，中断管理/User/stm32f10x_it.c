/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "los_sem.h"
#include "los_queue.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_exti.h"

 /* ������Ϣ���еľ�� */
extern UINT32 Test_Queue_Handle;

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
	UINTPTR uvIntSave;
	
	uvIntSave = LOS_IntLock();		//���ж�
	
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
			/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
			LOS_QueueWrite(	Test_Queue_Handle,	/* д�루���ͣ����е�ID(���) */
											&send_data1,				/* д�루���ͣ������� */
											sizeof(send_data1),	/* ���ݵĳ��� */
											0);									/* �ȴ�ʱ��Ϊ 0  */
		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
    
	LOS_IntRestore(uvIntSave);	//���ж�
}
/*********************************************************************************
  * @ ������  �� KEY1_IRQHandler
  * @ ����˵���� �жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void KEY2_IRQHandler(void)
{
	UINTPTR uvIntSave;
	
	uvIntSave = LOS_IntLock();		//���ж�
	
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
		LOS_QueueWrite(	Test_Queue_Handle,	/* д�루���ͣ����е�ID(���) */
										&send_data2,				/* д�루���ͣ������� */
										sizeof(send_data2),	/* ���ݵĳ��� */
										0);									/* �ȴ�ʱ��Ϊ 0  */
		//����жϱ�־λ
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
		
	LOS_IntRestore(uvIntSave);	//���ж�
}

/*********************************************************************************
  * @ ������  �� DEBUG_USART_IRQHandler
  * @ ����˵���� �����жϷ�����
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
	UINTPTR uvIntSave;
	
	uvIntSave = LOS_IntLock();		//���ж�

	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_IDLE)!=RESET)
	{		
		Uart_DMA_Rx_Data();       /* �ͷ�һ���ź�������ʾ�����ѽ��� */
		USART_ReceiveData(DEBUG_USARTx); /* �����־λ */
	}	 
  
	LOS_IntRestore(uvIntSave);	//���ж�
}



