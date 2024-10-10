/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./SMD/MicroStepDriver.h"  
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_exti.h"
#include "./led/bsp_led.h"

//接收数组指针
extern unsigned char UART_RxPtr;

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
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
  {}
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
  {}
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
  {}
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
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

    
/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void KEY1_IRQHandler(void)
{

  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
       if(status.out_ena !=FALSE)
       {
        /*LED1反转*/
        LED1_TOGGLE;
        SMD_ENA(ENABLE);
       }
        //清除中断标志位
        EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
	}  
}

void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
       if(status.out_ena !=TRUE)
       {
        /*LED2反转*/
        LED2_TOGGLE;
        SMD_ENA(DISABLE);
       }
        //清除中断标志位
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
// 串口中断服务函数

void DEBUG_USART_IRQHandler(void)

{	
    unsigned char data;
    
	if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)

	{	

		data = USART_ReceiveData(DEBUG_USART);
        if(status.running == FALSE && status.out_ena == TRUE)
        {
            //如果为退格键
            if(data == '\b')
            {
              //如果指针不在数组的开始位置
              if(UART_RxPtr)
              {
                Usart_SendByte(DEBUG_USART,'\b');
                Usart_SendByte(DEBUG_USART,'\b');
                Usart_SendByte(DEBUG_USART,'\b');
                UART_RxPtr--;
                UART_RxBuffer[UART_RxPtr]=0x00;
              }
            }
            //如果不是退格键
            else
            {
              //将数据填入数组UART_RxBuffer
              //并且将后面的一个元素清零如果数组满了则写入最后一个元素为止
              if(UART_RxPtr < (UART_RX_BUFFER_SIZE - 1))
              {
                UART_RxBuffer[UART_RxPtr] = data;
                UART_RxBuffer[UART_RxPtr + 1]=0x00;
                UART_RxPtr++;
              }
              else
              {
                UART_RxBuffer[UART_RxPtr++] = data;
                Usart_SendByte(DEBUG_USART,'\b');
              }
              //如果为回车键，则开始处理串口数据
              if(data == 13||data == 10)
              {
                status.cmd = TRUE;
              }
              else
              {
                Usart_SendByte(DEBUG_USART,data);
              }
            }
        }

    }	 

}
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
