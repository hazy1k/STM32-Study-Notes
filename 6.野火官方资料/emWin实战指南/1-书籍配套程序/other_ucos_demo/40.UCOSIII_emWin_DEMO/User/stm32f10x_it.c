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
#include "includes.h"

#include "usb_lib.h"
#include "usb_istr.h"
#include "..\User\bsp\ESP8266\bsp_esp8266.h"
//wifi
uint8_t ucTcpClosedFlag = 0;
//Clock
char TimeDisplay=0;

//Camera
extern u8 Ov7725_vsync;
//SIM900A
void bsp_USART2_IRQHandler(void);
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	printf("HardFault!!!");
#ifdef SOFTRESET
	Soft_Reset();
#endif
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);		
		TimeDisplay = 1;
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/*
 * ��������USB_LP_CAN1_RX0_IRQHandler
 * ����  ��USB�����ȼ��ж�����
 * ����  ����		 
 * ���  ����
 */

void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
	USB_Istr();
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}
/*
 * ��������USB_HP_CAN1_TX_IRQHandler
 * ����  ��USB�����ȼ��ж����� �����ж�
 * ����  ����		 
 * ���  ����
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
	CTR_HP();
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

void USART3_IRQHandler(void)
{
	uint8_t ucCh;
	
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
	if ( USART_GetITStatus ( macESP8266_USARTx, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( macESP8266_USARTx );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //Ԥ��1���ֽ�д������
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if ( USART_GetITStatus( macESP8266_USARTx, USART_IT_IDLE ) == SET )                                         //����֡�������
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macESP8266_USARTx );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)
	
		ucTcpClosedFlag = strstr(strEsp8266_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
		
  }	
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/* ov7725 ���ж� ������� */
void macOV7725_VSYNC_EXTI_INT_FUNCTION ( void )
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
	if ( EXTI_GetITStatus(macOV7725_VSYNC_EXTI_LINE) != RESET ) 	//���EXTI_Line0��·�ϵ��ж������Ƿ��͵���NVIC 
	{
			if( Ov7725_vsync == 0 )
			{
					FIFO_WRST_L(); 	                      //����ʹFIFOд(����from����ͷ)ָ�븴λ
					FIFO_WE_H();	                        //����ʹFIFOд����
					
					Ov7725_vsync = 1;	   	
					FIFO_WE_H();                          //ʹFIFOд����
					FIFO_WRST_H();                        //����ʹFIFOд(����from����ͷ)ָ���˶�
			}
			else if( Ov7725_vsync == 1 )
			{
					FIFO_WE_L();                          //����ʹFIFOд��ͣ
					Ov7725_vsync = 2;
			}        
			EXTI_ClearITPendingBit(macOV7725_VSYNC_EXTI_LINE);		    //���EXTI_Line0��·�����־λ
	}    
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}


/*
*����2�жϣ�����SIM900Aģ��
*/
void USART2_IRQHandler(void)
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
    bsp_USART2_IRQHandler();
	
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
