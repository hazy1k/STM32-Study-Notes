/**
  ******************************************************************************
  * @file    bsp_breathing.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ָ���߿����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */



#include "./breathinglight/bsp_breathing.h"

//����������ε�Ƶ��
__IO uint16_t period_class = 1;

/* LED���ȵȼ� PWM��,ָ������ ���˱�ʹ�ù���Ŀ¼�µ�python�ű�index_wave.py����*/
const uint16_t indexWave[] = {
1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 
	2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
	5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
	11, 12, 12, 13, 14, 15, 17, 18,
	19, 20, 22, 23, 25, 27, 29, 31, 
	33, 36, 38, 41, 44, 47, 51, 54, 
	58, 63, 67, 72, 77, 83, 89, 95, 
	102, 110, 117, 126, 135, 145, 156, 
	167, 179, 192, 206, 221, 237, 254,
	272, 292, 313, 336, 361, 387, 415,
	445, 477, 512, 512, 477, 445, 415, 
	387, 361, 336, 313, 292, 272, 254, 
	237, 221, 206, 192, 179, 167, 156, 
	145, 135, 126, 117, 110, 102, 95, 
	89, 83, 77, 72, 67, 63, 58, 54, 51, 
	47, 44, 41, 38, 36, 33, 31, 29, 27,
	25, 23, 22, 20, 19, 18, 17, 15, 14,
	13, 12, 12, 11, 10, 9, 9, 8, 8, 7, 7,
	6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 
	3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1
};

//����PWM���ж��ٸ�Ԫ��
uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]); 


 /**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd(BRE_TIM_GPIO_CLK, ENABLE); 

  	/*IO����*/
	BRE_GPIO_REMAP_FUN();
  
  /* ����LED���õ������� */
	//��
  GPIO_InitStructure.GPIO_Pin =  BRE_RED_TIM_LED_PIN ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(BRE_RED_TIM_LED_PORT, &GPIO_InitStructure);
	
	//��
	GPIO_InitStructure.GPIO_Pin =  BRE_GREEN_TIM_LED_PIN ;
  GPIO_Init(BRE_GREEN_TIM_LED_PORT, &GPIO_InitStructure);
	
	//��
	GPIO_InitStructure.GPIO_Pin =  BRE_BLUE_TIM_LED_PIN ;
  GPIO_Init(BRE_BLUE_TIM_LED_PORT, &GPIO_InitStructure);

}


/**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Config_PWM(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* ����TIM3_IRQ�ж�Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = BRE_TIMx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  ����TIM�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */

static void TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	
	/* ����TIMCLK ʱ�� */
	BRE_TIM_APBxClock_FUN ( BRE_TIM_CLK, ENABLE ); 

  /* ������ʱ������ ,���PWM��������жϷ������е�PERIOD_CLASS��AMPLITUDE_CLASSѭ����������*/	
	
	/* ����ʹ��������������Ϊ3�����Ҽ��ɴﵽ�ܺõ�Ч�� */	
	
	//Ҫ��
	//TIM_Period����PWM������ֵ��Χһ��
	//TIM_Prescaler��ԽСԽ�ã��ɼ�����˸����
	//PERIOD_CLASS���жϷ������п��Ƶ�����ѭ���Ĵ������������ɿ���������ߵ�����

	//AMPLITUDE_CLASS���жϷ������п��Ƶ�����ѭ���Ĵ������������ɿ��Ʒ�ֵ��
	//						�ڱ�ʵ������ΪLEDͨ��������ֵ����256���ȼ�����ӦRGB888��ʽ��ͨ������ɫ�ȼ�
	//POINT_NUM��PWM���Ԫ�أ�����PWM������ߵĲ�������

	/*************��ʵ���е�����***************/	
	/***********************************************
	#python����ű�	count.py
	#PWM����
	POINT_NUM = 180

	#��ֵ����ɫ���ȼ�
	AMPLITUDE_CLASS = 256
	
	#���ڱ���
	PERIOD_CLASS = 1

	#��ʱ����ʱ����
	TIMER_TIM_Period = 2**9
	#��ʱ����Ƶ
	TIMER_TIM_Prescaler = 10

	#STM32ϵͳʱ��Ƶ�ʺ�����
	f_pclk = 72000000
	t_pclk = 1/f_pclk

	#��ʱ��update�¼�����
	t_timer = t_pclk*TIMER_TIM_Prescaler*TIMER_TIM_Period

	#ÿ��PWM���ʱ��
	T_Point = t_timer * RGB_CLASS

	#������������
	T_Up_Down_Cycle = T_Point * POINT_NUM*PERIOD_CLASS

	print ("�������ڣ�",T_Up_Down_Cycle)
	
	#���н����
	
	�������ڣ�3.27679
	************************************************************/
	
  TIM_TimeBaseStructure.TIM_Period = (512-1);       							  //����ʱ����0������ TIM_Period+1 Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = (10-1);	    							//����Ԥ��Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
  TIM_TimeBaseInit(BRE_TIMx, &TIM_TimeBaseStructure);

  /* PWMģʽ���� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
  TIM_OCInitStructure.TIM_Pulse = 0;				 						  			//���ó�ʼPWM������Ϊ0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR_ValʱΪ�͵�ƽ

	//RGBͨ��
  BRE_RED_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	
  BRE_RED_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );						//ʹ��Ԥװ��	

  BRE_GREEN_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	
  BRE_GREEN_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );						//ʹ��Ԥװ��	

  BRE_BLUE_TIM_OCxInit ( BRE_TIMx, &TIM_OCInitStructure );	
  BRE_BLUE_TIM_OCxPreloadConfig ( BRE_TIMx, TIM_OCPreload_Enable );						//ʹ��Ԥװ��	

  TIM_ARRPreloadConfig(BRE_TIMx, ENABLE);			 										//ʹ��TIM���ؼĴ���ARR

  /* TIM3 enable counter */
  TIM_Cmd(BRE_TIMx, ENABLE);                   										//ʹ�ܶ�ʱ��	
	
	TIM_ITConfig(BRE_TIMx, TIM_IT_Update, ENABLE);										//ʹ��update�ж�
		
	NVIC_Config_PWM();																					//�����ж����ȼ�		
	
}

/**
  * @brief  TIM �����Ƴ�ʼ��
  *         ����PWMģʽ��GPIO
  * @param  ��
  * @retval ��
  */
void TIMx_Breathing_Init(void)
{
	TIMx_GPIO_Config();
	TIMx_Mode_Config();	
}

/*********************************************END OF FILE**********************/



