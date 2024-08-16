/**
  ******************************************************************************
  * @file    bsp_general_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ͨ�ö�ʱ������ͬ��PWM�������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ָ���� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_general_tim.h"

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /* ʹ��GPIOʱ�� */
  RCC_APB2PeriphClockCmd(GENERAL_MASTER_GPIO_CLK |GENERAL_SLAVE1_GPIO_CLK | GENERAL_SLAVE11_GPIO_CLK, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//ʹ��AFIOʱ��
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);//��ȫ��ӳ��TIM2_CH3->PB10
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);//��ȫ��ӳ��TIM3_CH1->PC6

  /* GPIO ����: PC6(TIM3 CH1) ��Ϊ���츴��������� ------*/
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;;
  GPIO_Init(GENERAL_SLAVE1_GPIO_PORT, &GPIO_InitStructure);

  /* GPIO ����: PB6(TIM4 CH1) ��Ϊ���츴��������� ------*/
  GPIO_InitStructure.GPIO_Pin = GENERAL_SLAVE11_PIN;
  GPIO_Init(GENERAL_SLAVE11_GPIO_PORT, &GPIO_InitStructure);

  /* GPIO ����: PB10(TIM2 CH3) ��Ϊ���츴��������� ------*/
  GPIO_InitStructure.GPIO_Pin = GENERAL_MASTER_PIN;
  GPIO_Init(GENERAL_MASTER_GPIO_PORT, &GPIO_InitStructure);

}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	  /* ʹ��TIMʱ��  */
  RCC_APB1PeriphClockCmd(GENERAL_TIM_MASTER_CLK | GENERAL_TIM_SLAVE1_CLK |
                         GENERAL_TIM_SLAVE11_CLK, ENABLE);
  /* Timers synchronisation in cascade mode ------------------------------------
     1/TIM2 is configured as Master Timer:
         - PWM Mode is used
         - The TIM2 Update event is used as Trigger Output  

     2/TIM3 is slave for TIM2 and Master for TIM4,
         - PWM Mode is used
         - The ITR1(TIM2) is used as input trigger 
         - Gated mode is used, so start and stop of slave counter
           are controlled by the Master trigger output signal(TIM2 update event).
         - The TIM3 Update event is used as Trigger Output. 

     3/TIM4 is slave for TIM3,
         - PWM Mode is used
         - The ITR2(TIM3) is used as input trigger
         - Gated mode is used, so start and stop of slave counter
           are controlled by the Master trigger output signal(TIM3 update event).

     In this example TIM2 input clock (TIM2CLK) is set to 2 * APB1 clock (PCLK1), 
     since APB1 prescaler is different from 1.   
     TIM2CLK = 2 * PCLK1  
     PCLK1 = HCLK / 4 
     => TIM2CLK = HCLK / 2 = SystemCoreClock /2

     The Master Timer TIM2 is running at TIM2 counter clock:
     TIM2 frequency = (TIM2 counter clock)/ (TIM2 period + 1) = 500 KHz 
     and the duty cycle = TIM2_CCR1/(TIM2_ARR + 1) = 25%.

     The TIM3 is running:
     - At (TIM2 frequency)/ (TIM3 period + 1) = 100 KHz and a duty cycle
       equal to TIM3_CCR1/(TIM3_ARR + 1) = 20%

     The TIM4 is running:
     - At (TIM3 frequency)/ (TIM4 period + 1) = 20 KHz and a duty cycle
       equal to TIM4_CCR1/(TIM4_ARR + 1) = 20%
  
     Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f10x.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect. 
  --------------------------------------------------------------------------- */

  /* ʱ������ */
  TIM_TimeBaseStructure.TIM_Period = 72;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(GENERAL_TIM_MASTER, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE1, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(GENERAL_TIM_SLAVE11, &TIM_TimeBaseStructure);

  /* �� ���ó�PWM1ģʽ */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 36;//ռ�ձ�
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(GENERAL_TIM_MASTER, &TIM_OCInitStructure);//��ʼ�� TIM2 ���ͨ��3
  /* ʹ�ܡ�����ģʽ�� */
  TIM_SelectMasterSlaveMode(GENERAL_TIM_MASTER, TIM_MasterSlaveMode_Enable);
	/* �������Դѡ�� */
  TIM_SelectOutputTrigger(GENERAL_TIM_MASTER, TIM_TRGOSource_Update);

	/* �� ���ó�PWM1ģʽ */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1;
  TIM_OC1Init(GENERAL_TIM_SLAVE1, &TIM_OCInitStructure);//��ʼ�� TIM3 ���ͨ��1
	TIM_OC1Init(GENERAL_TIM_SLAVE11, &TIM_OCInitStructure);//��ʼ�� TIM4 ���ͨ��1
  /* ʹ�ܡ�����ģʽ�� */
  TIM_SelectMasterSlaveMode(GENERAL_TIM_SLAVE1, TIM_MasterSlaveMode_Enable);
  /* �������Դѡ�� */
  TIM_SelectOutputTrigger(GENERAL_TIM_SLAVE1, TIM_TRGOSource_Update);
	
  TIM_SelectSlaveMode(GENERAL_TIM_SLAVE1, TIM_SlaveMode_Gated);//��ģʽ���� : TIM3
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE1, TIM_TS_ITR1);//�������봥��Դ
	
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);//��ģʽ���� : TIM4
  TIM_SelectInputTrigger(GENERAL_TIM_SLAVE11, TIM_TS_ITR2);//�������봥��Դ
  
  /* TIM ����������ж����� */
  TIM_Cmd(GENERAL_TIM_SLAVE1,ENABLE);
  TIM_Cmd(GENERAL_TIM_MASTER,ENABLE);
  TIM_Cmd(GENERAL_TIM_SLAVE11,ENABLE);
}

/**
  * @brief  ��ʼ��ͨ�ö�ʱ������ͬ��PWM���
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{  
	TIMx_GPIO_Config();
	
  TIM_Mode_Config();
}

/*********************************************END OF FILE**********************/
