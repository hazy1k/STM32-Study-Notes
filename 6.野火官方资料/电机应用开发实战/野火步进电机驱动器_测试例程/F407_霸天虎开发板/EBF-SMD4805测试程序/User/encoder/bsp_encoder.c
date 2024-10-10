/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   �������ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./Encoder/bsp_encoder.h"

/* ��ʱ��������� */
__IO int16_t Encoder_Overflow_Count = 0;

TIM_TimeBaseInitTypeDef TIM_EncoderHandle;

/**
  * @brief  �������ӿ����ų�ʼ��
  * @param  ��
  * @retval ��
  */
static void Encoder_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* ��ʱ��ͨ�����Ŷ˿�ʱ��ʹ�� */
  ENCODER_TIM_CH1_GPIO_CLK_ENABLE();
  ENCODER_TIM_CH2_GPIO_CLK_ENABLE();
  
  /**TIM3 GPIO Configuration    
  PC6     ------> TIM3_CH1
  PC7     ------> TIM3_CH2 
  */
  /* ������������ */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  /* �������� */
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  /* ������������ */
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  
  /* ѡ��Ҫ���Ƶ�GPIO���� */	
  GPIO_InitStruct.GPIO_Pin = ENCODER_TIM_CH1_PIN;
  /* ���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO */
  GPIO_Init(ENCODER_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(ENCODER_TIM_CH1_GPIO_PORT, ENCODER_TIM_CH1_GPIO_AF, GPIO_AF_TIM3);
  /* ѡ��Ҫ���Ƶ�GPIO���� */	
  GPIO_InitStruct.GPIO_Pin = ENCODER_TIM_CH2_PIN;
  /* ���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO */
  GPIO_Init(ENCODER_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(ENCODER_TIM_CH1_GPIO_PORT, ENCODER_TIM_CH2_GPIO_AF, GPIO_AF_TIM3);
}

/**
  * @brief  ����TIMx������ģʽ
  * @param  ��
  * @retval ��
  */
static void TIM_Encoder_Init(void)
{ 
  TIM_ICInitTypeDef Encoder_ConfigStructure = {0};
  
  /* ʹ�ܱ������ӿ�ʱ�� */
  ENCODER_TIM_CLK_ENABLE();
  
  /* ��ʱ����ʼ������ */
  TIM_EncoderHandle.TIM_Prescaler = ENCODER_TIM_PRESCALER;
  TIM_EncoderHandle.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_EncoderHandle.TIM_Period = ENCODER_TIM_PERIOD;
  TIM_EncoderHandle.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(ENCODER_TIM, &TIM_EncoderHandle); 
  
  TIM_EncoderInterfaceConfig(ENCODER_TIM, ENCODER_MODE,
                             TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);	//���ñ�����ģʽ����Դ�ͼ���
  /* �������ӿ�ͨ��1���� */
  Encoder_ConfigStructure.TIM_Channel = TIM_Channel_1;
  Encoder_ConfigStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  Encoder_ConfigStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  Encoder_ConfigStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  Encoder_ConfigStructure.TIM_ICFilter = 0;
  /* ��ʼ���������ӿ� */
  TIM_ICInit(ENCODER_TIM, &Encoder_ConfigStructure);
  
  /* �������ӿ�ͨ��2���� */
  Encoder_ConfigStructure.TIM_Channel = TIM_Channel_2;
  /* ��ʼ���������ӿ� */
  TIM_ICInit(ENCODER_TIM, &Encoder_ConfigStructure);
  
  NVIC_InitTypeDef NVIC_InitStructure; 
  // �����ж���Ϊ0
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
  // �����ж���Դ
  NVIC_InitStructure.NVIC_IRQChannel = ENCODER_TIM_IRQn; 	
  // ������ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
  // ���������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
  
  /* ��������� */
  ENCODER_TIM->CNT = 0;

  /* ���ø����¼�����ԴΪ����������� */
  TIM_UpdateRequestConfig(ENCODER_TIM,TIM_UpdateSource_Regular);
  /* ʹ�ܶ�ʱ���ĸ����¼��ж� */
  TIM_ITConfig(ENCODER_TIM, TIM_IT_Update, ENABLE);
  
  NVIC_Init(&NVIC_InitStructure);
  
  /* ʹ�ܱ������ӿ� */
  // ʹ�ܼ�����
	TIM_Cmd(ENCODER_TIM, ENABLE);
}

/**
  * @brief  �������ӿڳ�ʼ��
  * @param  ��
  * @retval ��
  */
void Encoder_Init(void)
{
  Encoder_GPIO_Init();    /* ���ų�ʼ�� */
  TIM_Encoder_Init();     /* ���ñ������ӿ� */
}

/*********************************************END OF FILE**********************/

