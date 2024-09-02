/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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


#define TASK_DELAY_NUM  2       //����������������Լ�����ʵ������޸�
#define TASK_DELAY_0    500    //����0��ʱ 500*10 �����ִ�У���������Ƿ�������
#define TASK_DELAY_1    50     //����1��ʱ 50*10 �����ִ�У�

uint32_t Task_Delay_Group[TASK_DELAY_NUM];  //�������飬������ʱ�����ж��Ƿ�ִ�ж�Ӧ����

/* ��־λ */
int hc05_inquery_connect; //ÿ5s��������Ƿ�����
int hc05_check_recvbuff; //ÿ500ms��������Ƿ�����



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
  int i;
  
  for(i=0; i<TASK_DELAY_NUM; i++)
  {
    Task_Delay_Group[i] ++;                   //�����ʱ��ʱ�䵽��ִ��
  }
  
  
  /* ��������0 */
  if(Task_Delay_Group[0] >= TASK_DELAY_0)     //�ж��Ƿ�ִ������0
  {
    Task_Delay_Group[0] = 0;                  //��0���¼�ʱ
    
    /* ����0����������Ƿ������� */
    
    hc05_inquery_connect = 1; //�������û�����ӣ���־λ��1
    
  }
  
  /* ��������1 */
  if(Task_Delay_Group[1] >= TASK_DELAY_1)     //�ж��Ƿ�ִ������1
  {
    Task_Delay_Group[1] = 0;                  //��0���¼�ʱ
    
    
    /* ����1��xxxxx */
    hc05_check_recvbuff = 1;
  }
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/


extern volatile    uint16_t uart_p;
extern uint8_t     uart_buff[UART_BUFF_SIZE];


void BLT_USART_IRQHandler(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(USART_GetITStatus(BLT_USARTx, USART_IT_RXNE) != RESET)
        {
            uart_buff[uart_p] = USART_ReceiveData(BLT_USARTx);
            uart_p++;
        }
    }
		else
		{
			USART_ClearITPendingBit(BLT_USARTx, USART_IT_RXNE);
			clean_rebuff();       
		}
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
