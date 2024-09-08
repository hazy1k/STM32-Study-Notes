/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "stm32h7xx_it.h"
#include "./IrDa/bsp_irda.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h" 


extern uint32_t frame_data;
extern uint8_t  frame_cnt;
extern uint8_t  frame_flag;
uint8_t isr_cnt;  /* ���ڼ�����˶��ٴ��ж� */   
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*            Cortex Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
//  HAL_IncTick();
  TimingDelay_Decrement();
}
  


// IO ���ж�, �Ӻ������ͷ�����ݹܽ�
void IRDA_EXTI_IRQHANDLER_FUN(void)
{
	uint8_t pulse_time = 0;
  uint8_t leader_code_flag = 0; /* �������־λ�������������ʱ����ʾһ֡���ݿ�ʼ */
  uint8_t irda_data = 0;        /* �����ݴ�λ */
  
  if(__HAL_GPIO_EXTI_GET_IT(IRDA_GPIO_PIN) != RESET) /* ȷ���Ƿ������EXTI Line�ж� */
	{   
		LED1_TOGGLE;
    while(1)
    {
      if( IrDa_DATA_IN()== SET )        /* ֻ�����ߵ�ƽ��ʱ�� */
      {       
					pulse_time = Get_Pulse_Time();
					
					/* >=5ms ���������ź� �����ָ��Ż���������ʱ��Ҳ��break����while(1)ѭ�� */
					if( pulse_time >= 250 )                
					{
						break; /* ����while(1)ѭ�� */
					}
					
					if(pulse_time>=200 && pulse_time<250)         /* ���ǰ��λ 4ms~4.5ms */
					{
						leader_code_flag = 1;
					}
					else if(pulse_time>=10&& pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
					{
						irda_data = 0;
					}
					else if(pulse_time>=50 && pulse_time<100)     /* 1.68ms��1ms~2ms */
					{
						irda_data =1 ; 
					}        
					else if( pulse_time>=100 && pulse_time<=200 ) /* 2.1ms��2ms~4ms */
					{/* �����룬�ڵڶ����жϳ��� */
						frame_flag = 1;               /* һ֡���ݽ������ */
						frame_cnt++;                  /* ����������1 */
						isr_cnt ++;                   /* ���ж�һ�μ�1 */
						break;                        /* ����while(1)ѭ�� */
					}
					
					if( leader_code_flag == 1 )
					{/* �ڵ�һ���ж������ */
						frame_data <<= 1;
						frame_data += irda_data;
						frame_cnt = 0;
						isr_cnt = 1;
					}
      }      
    }// while(1)   
		__HAL_GPIO_EXTI_CLEAR_IT(IRDA_GPIO_PIN);     //����жϱ�־λ
		HAL_GPIO_EXTI_IRQHandler(IRDA_GPIO_PIN);
    //LED2_TOGGLE;
	}  
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
