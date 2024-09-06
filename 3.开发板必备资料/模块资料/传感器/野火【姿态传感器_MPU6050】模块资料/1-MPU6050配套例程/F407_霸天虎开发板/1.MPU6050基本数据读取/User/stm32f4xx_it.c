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
#include "./led/bsp_led.h"
#include "./mpu6050/mpu6050.h"

#define TASK_DELAY_NUM  2       //����������������Լ�����ʵ������޸�
#define TASK_DELAY_0    1000    //����0��ʱ 1000*1 �����ִ�У���תLED
#define TASK_DELAY_1    500     //����1��ʱ 500*1 �����ִ�У�MPU6050����

uint32_t Task_Delay_Group[TASK_DELAY_NUM];  //�������飬������ʱ�����ж��Ƿ�ִ�ж�Ӧ����


/* ִ�������־����ȡMPU6050���� */
// - ��־�� 1��ʾ��ȡMPU6050������ɣ���Ҫ����ѭ������MPU6050����
// - ��־�� 0��ʾδ��ɶ�ȡMPU6050���ݣ���Ҫ���ж��ж�ȡMPU6050����
int task_readdata_finish;


// �����ⲿ����
extern short Acel[3];
extern short Gyro[3];
extern float Temp;


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
    
    /* ����0����תLED */
    LED2_TOGGLE;
  }
  
  /* ��������1 */
  if(Task_Delay_Group[1] >= TASK_DELAY_1)     //�ж��Ƿ�ִ������1
  {
    Task_Delay_Group[1] = 0;                  //��0���¼�ʱ
    
    /* ����1��MPU6050���� */
    if( ! task_readdata_finish )
    {
      MPU6050ReadAcc(Acel);
      MPU6050ReadGyro(Gyro);
      MPU6050_ReturnTemp(&Temp);
      
      task_readdata_finish = 1; //��־λ��1����ʾ��Ҫ����ѭ������MPU6050����
    }
  }
  
  /* ��������2 */
  //���������Ҫ�޸����������ĺ궨�� TASK_DELAY_NUM
  //������Ӷ��������ִ�����ں궨�� TASK_DELAY_x��x����һ����ţ������� TASK_DELAY_2
  
  
  
  TimingDelay_Decrement(); //���ʹ��Delay_ms��ʱ��������Ҫ�������
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

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
