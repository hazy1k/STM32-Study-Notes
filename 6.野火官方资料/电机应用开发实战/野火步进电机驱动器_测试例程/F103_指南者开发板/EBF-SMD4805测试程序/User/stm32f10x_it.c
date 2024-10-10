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
#include "MicroStepDriver.h"  
#include "bsp_usart.h"
#include "bsp_exti.h"
#include "bsp_led.h"

//��������ָ��
extern unsigned char UART_RxPtr;

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
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void KEY1_IRQHandler(void)
{

  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
       if(status.out_ena !=FALSE)
       {
        /*LED1��ת*/
        LED1_TOGGLE;
        SMD_ENA(ENABLE);
       }
        //����жϱ�־λ
        EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
	}  
}

void KEY2_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
       if(status.out_ena !=TRUE)
       {
        /*LED2��ת*/
        LED2_TOGGLE;
        SMD_ENA(DISABLE);
       }
        //����жϱ�־λ
        EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);         
	}  
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
// �����жϷ�����

void DEBUG_USART_IRQHandler(void)

{	
    unsigned char data;
    
	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE)!=RESET)

	{	

		data = USART_ReceiveData(DEBUG_USARTx);
        if(status.running == FALSE && status.out_ena == TRUE)
        {
            //���Ϊ�˸��
            if(data == '\b')
            {
              //���ָ�벻������Ŀ�ʼλ��
              if(UART_RxPtr)
              {
                Usart_SendByte(DEBUG_USARTx,'\b');
                Usart_SendByte(DEBUG_USARTx,' ');
                Usart_SendByte(DEBUG_USARTx,'\b');
                UART_RxPtr--;
                UART_RxBuffer[UART_RxPtr]=0x00;
              }
            }
            //��������˸��
            else
            {
              //��������������UART_RxBuffer
              //���ҽ������һ��Ԫ�������������������д�����һ��Ԫ��Ϊֹ
              if(UART_RxPtr < (UART_RX_BUFFER_SIZE - 1))
              {
                UART_RxBuffer[UART_RxPtr] = data;
                UART_RxBuffer[UART_RxPtr + 1]=0x00;
                UART_RxPtr++;
              }
              else
              {
                UART_RxBuffer[UART_RxPtr - 1] = data;
                Usart_SendByte(DEBUG_USARTx,'\b');
              }
              //���Ϊ�س�������ʼ����������
              if(data == 13||data == 10)
              {
                status.cmd = TRUE;
              }
              else
              {
                Usart_SendByte(DEBUG_USARTx,data);
              }
            }
        }

    }	 

}
/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
