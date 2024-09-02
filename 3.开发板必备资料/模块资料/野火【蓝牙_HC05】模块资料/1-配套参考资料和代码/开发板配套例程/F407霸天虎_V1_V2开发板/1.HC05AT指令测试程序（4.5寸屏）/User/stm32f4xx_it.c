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
#include "./systick/bsp_SysTick.h" 
#include "./usart/bsp_blt_usart.h"
#include "./usart/bsp_debug_usart.h"


/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

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
{

}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * @}
  */ 


ReceiveData DEBUG_USART_ReceiveData;
// 串口中断服务函数
void DEBUG_USART_IRQHandler(void)
{
		uint8_t ucCh; 
    if(USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET)
    {
      ucCh = USART_ReceiveData(DEBUG_USART);
      if(DEBUG_USART_ReceiveData.datanum < UART_BUFF_SIZE)
        {
          if((ucCh != 0x0a) && (ucCh != 0x0d))
          {
            DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = ucCh;                 //不接收换行回车
            DEBUG_USART_ReceiveData.datanum++;
          }

        }         
     }
		if(USART_GetITStatus( DEBUG_USART, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
    {
        DEBUG_USART_ReceiveData.receive_data_flag = 1;
        USART_ReceiveData( DEBUG_USART );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
    }	
} 

//蓝牙串口中断缓存串口数据
ReceiveData BLT_USART_ReceiveData;

void BLT_USART_IRQHandler(void)
{
    uint8_t ucCh; 
    if(USART_GetITStatus(BLT_USARTx, USART_IT_RXNE) != RESET)
    {
      ucCh = USART_ReceiveData(BLT_USARTx);
      if(BLT_USART_ReceiveData.datanum < UART_BUFF_SIZE)
        {
          if((ucCh != 0x0a) && (ucCh != 0x0d))
          {
            BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = ucCh;                 //不接收换行回车
            BLT_USART_ReceiveData.datanum++;
          }
        }         
     }
		if(USART_GetITStatus( BLT_USARTx, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
    {
        BLT_USART_ReceiveData.receive_data_flag = 1;
        USART_ReceiveData( BLT_USARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
    }	

}


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
