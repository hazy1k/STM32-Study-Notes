/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#include "main.h"
#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./IrDa/bsp_irda.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h" 

extern uint32_t frame_data;
extern uint8_t  frame_cnt;
extern uint8_t  frame_flag;

uint8_t isr_cnt;  /* 用于计算进了多少次中断 */   
/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
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
  //HAL_IncTick();
	TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
// IO 线中断, 接红外接收头的数据管脚
void IRDA_EXTI_IRQHANDLER_FUN(void)
{
	uint8_t pulse_time = 0;
  uint8_t leader_code_flag = 0; /* 引导码标志位，当引导码出现时，表示一帧数据开始 */
  uint8_t irda_data = 0;        /* 数据暂存位 */
  
  if(__HAL_GPIO_EXTI_GET_IT(IRDA_GPIO_PIN) != RESET) /* 确保是否产生了EXTI Line中断 */
	{   
		LED1_TOGGLE;
    while(1)
    {
      if( IrDa_DATA_IN()== SET )        /* 只测量高电平的时间 */
      {       
        pulse_time = Get_Pulse_Time();
        
        /* >=5ms 不是有用信号 当出现干扰或者连发码时，也会break跳出while(1)循环 */
        if( pulse_time >= 250 )                
        {
          break; /* 跳出while(1)循环 */
        }
        
        if(pulse_time>=200 && pulse_time<250)         /* 获得前导位 4ms~4.5ms */
        {
          leader_code_flag = 1;
        }
        else if(pulse_time>=10 && pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
        {
          irda_data = 0;
        }
        else if(pulse_time>=50 && pulse_time<100)     /* 1.68ms：1ms~2ms */
        {
          irda_data =1 ; 
        }        
        else if( pulse_time>=100 && pulse_time<=200 ) /* 2.1ms：2ms~4ms */
        {/* 连发码，在第二次中断出现 */
          frame_flag = 1;               /* 一帧数据接收完成 */
          frame_cnt++;                  /* 按键次数加1 */
          isr_cnt ++;                   /* 进中断一次加1 */
          break;                        /* 跳出while(1)循环 */
        }
        
        if( leader_code_flag == 1 )
        {/* 在第一次中断中完成 */
          frame_data <<= 1;
          frame_data += irda_data;
          frame_cnt = 0;
          isr_cnt = 1;
        }
      }      
    }// while(1)   
		__HAL_GPIO_EXTI_CLEAR_IT(IRDA_GPIO_PIN);     //清除中断标志位
		HAL_GPIO_EXTI_IRQHandler(IRDA_GPIO_PIN);
    //LED2_TOGGLE;
	}  
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
