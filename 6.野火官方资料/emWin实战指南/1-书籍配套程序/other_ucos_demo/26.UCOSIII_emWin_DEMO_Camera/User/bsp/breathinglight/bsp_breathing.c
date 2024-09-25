/**
  ******************************************************************************
  * @file    bsp_pwm_output.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   tim3�����·pwm bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "..\User\bsp\breathinglight\bsp_breathing.h" 

 /**
  * @brief  ����TIM3�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); 

  	/*IO����*/
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
  
  /* ���ú������õ���PB0���� */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5 ;
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/**
  * @brief  ����TIM3�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */

/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> �ж� & TIMxCNT ���¼���
 *                    TIMx_CCR(��ƽ�����仯)
 *
 * �ź�����=(TIMx_ARR +1 ) * ʱ������
 * 
 */

/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
static void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* ����TIM3CLK ʱ��Ϊ72MHZ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					//ʹ��TIM3ʱ��

  /* ������ʱ������ */		 
  TIM_TimeBaseStructure.TIM_Period = 255;       							  //����ʱ����0������255����Ϊ266�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 1999;	    							//����Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWMģʽ���� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
  TIM_OCInitStructure.TIM_Pulse = 0;										  			//���ó�ʼPWM������Ϊ0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	

  TIM_ARRPreloadConfig(TIM3, ENABLE);			 										//ʹ��TIM3���ؼĴ���ARR

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   										//ʹ�ܶ�ʱ��3		
}

/**
  * @brief  TIM3 �����Ƴ�ʼ��
  *         ����PWMģʽ��GPIO
  * @param  ��
  * @retval ��
  */
void TIM3_Breathing_Init(void)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config();	
}

//ֹͣpwm���
void TIM3_Breathing_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC2Config(TIM3,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(TIM3,TIM_ForcedAction_InActive);
	TIM_ForcedOC4Config(TIM3,TIM_ForcedAction_InActive);
	TIM_ARRPreloadConfig(TIM3, DISABLE);
	TIM_Cmd(TIM3, DISABLE);                   							//ʧ�ܶ�ʱ��3						
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); 	//ʧ�ܶ�ʱ��3ʱ��
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5);
}
//RGBLED��ʾ��ɫ
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	TIM3->CCR2 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	TIM3->CCR3 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	TIM3->CCR4 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}
//RGBLED��ʾ��ɫ
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	TIM3->CCR2 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	TIM3->CCR3 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	TIM3->CCR4 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}
/*********************************************END OF FILE**********************/
