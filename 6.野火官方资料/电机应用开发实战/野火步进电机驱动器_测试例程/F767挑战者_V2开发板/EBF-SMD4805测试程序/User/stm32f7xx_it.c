/**
  ******************************************************************************
  * @file    Templates/stm32f7xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    18-November-2015 
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_it.h"
#include "main.h"
#include "./usart/bsp_usart.h"
#include "./SMD/MicroStepDriver.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h" 

extern TIM_HandleTypeDef TIM_Base;
extern unsigned char UART_RxPtr;
/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
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
    HAL_IncTick();
}
  

/**
  * @brief  This function KEY1 Handler.
  * @param  None
  * @retval None
  */
void KEY1_IRQHandler(void)
{

  //确保是否产生了EXTI Line中断
if(__HAL_GPIO_EXTI_GET_IT(KEY1_INT_GPIO_PIN) != RESET)  
	{
       if(status.out_ena !=FALSE)
       {
        /*LED1反转*/
        LED1_ON;
        SMD_ENA(ENABLE);
       }
        //清除中断标志位
     __HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);    
	}  
}


/**
  * @brief  This function KEY2 Handler.
  * @param  None
  * @retval None
  */
void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET) 
	{
       if(status.out_ena !=TRUE)
       {
        /*LED1反转*/
        LED1_OFF;
        SMD_ENA(DISABLE);
       }
        //清除中断标志位
     	__HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);         
	}  
}




/**
  * @brief  This function USART1 Handler.
  * @param  None
  * @retval None
  */

void DEBUG_USART_IRQHandler(void)

{	
    unsigned char data;
	if(__HAL_UART_GET_IT(&UartHandle,USART_IT_RXNE)!=RESET)

	{	
        data = UartHandle.Instance->RDR;
        if(status.running == FALSE && status.out_ena == TRUE)
        {
					 if(UART_RxPtr < (UART_RX_BUFFER_SIZE - 1))
              {
                UART_RxBuffer[UART_RxPtr++] = data;
              }
        }

         
    }	 
	else if(UartHandle.Instance->ISR >> 4 & 1)		 
			{
				status.cmd = TRUE;
				  data = UartHandle.Instance->RDR;
				/*清除空闲标志位*/
				UartHandle.Instance->ICR |= ((1<<4));
			}	
	HAL_UART_IRQHandler(&UartHandle);
}


/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
