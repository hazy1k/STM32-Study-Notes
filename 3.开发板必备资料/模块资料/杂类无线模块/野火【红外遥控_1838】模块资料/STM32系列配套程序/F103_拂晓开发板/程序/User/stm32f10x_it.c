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
#include "./IrDa/bsp_irda.h" 



extern uint32_t frame_data;
extern uint8_t  frame_cnt;
extern uint8_t  frame_flag;

uint8_t isr_cnt;  /* ���ڼ�����˶��ٴ��ж� */   

extern void TimingDelay_Decrement(void);

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
	TimingDelay_Decrement();	
}




// IO ���ж�, �Ӻ������ͷ�����ݹܽ�
void IRDA_EXTI_IRQHANDLER_FUN(void)
{
	uint8_t pulse_time = 0;
  uint8_t leader_code_flag = 0; /* �������־λ�������������ʱ����ʾһ֡���ݿ�ʼ */
  uint8_t irda_data = 0;        /* �����ݴ�λ */
  
  if(EXTI_GetITStatus(IRDA_EXTI_LINE) != RESET) /* ȷ���Ƿ������EXTI Line�ж� */
	{   
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
        else if(pulse_time>=10 && pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
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
		EXTI_ClearITPendingBit(IRDA_EXTI_LINE);     //����жϱ�־λ
    //LED2_TOGGLE;
	}  
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
