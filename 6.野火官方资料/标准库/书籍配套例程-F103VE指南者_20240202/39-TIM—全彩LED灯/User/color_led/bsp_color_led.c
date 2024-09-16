/**
  ******************************************************************************
  * @file    bsp_color_light.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ȫ��LED������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ָ���߿����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./color_led/bsp_color_led.h"


 /**
  * @brief  ����COLOR_TIMx�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void COLOR_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd(COLOR_TIM_GPIO_CLK, ENABLE); 

  	/*IO����*/
	COLOR_GPIO_REMAP_FUN();
  
  /* ����LED���õ������� */
	//��
  GPIO_InitStructure.GPIO_Pin =  COLOR_RED_TIM_LED_PIN ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(COLOR_RED_TIM_LED_PORT, &GPIO_InitStructure);
	
	//��
	GPIO_InitStructure.GPIO_Pin =  COLOR_GREEN_TIM_LED_PIN ;
  GPIO_Init(COLOR_GREEN_TIM_LED_PORT, &GPIO_InitStructure);
	
	//��
	GPIO_InitStructure.GPIO_Pin =  COLOR_BLUE_TIM_LED_PIN ;
  GPIO_Init(COLOR_BLUE_TIM_LED_PORT, &GPIO_InitStructure);

}

/**
  * @brief  ����COLOR_TIMx�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */
static void COLOR_TIMx_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* ����TIM CLK ʱ�� */
  COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK, ENABLE); 					//ʹ��COLOR_TIMxʱ��

  /* ������ʱ������ */		 
  TIM_TimeBaseStructure.TIM_Period = 255;       							  //����ʱ����0������255����Ϊ256�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 4000;	    							//����Ԥ��Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
  TIM_TimeBaseInit(COLOR_TIMx, &TIM_TimeBaseStructure);

  /* PWMģʽ���� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
  TIM_OCInitStructure.TIM_Pulse = 0;										  			//���ó�ʼPWM������Ϊ0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR_ValʱΪ�͵�ƽ

	//ʹ��ͨ����Ԥװ��
  COLOR_RED_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);	 							
  COLOR_RED_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);						
  
	//ʹ��ͨ����Ԥװ��
  COLOR_GREEN_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);	 									
  COLOR_GREEN_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);						
  
	//ʹ��ͨ����Ԥװ��
  COLOR_BLUE_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);	 									
  COLOR_BLUE_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);						

  TIM_ARRPreloadConfig(COLOR_TIMx, ENABLE);			 										//ʹ��COLOR_TIMx���ؼĴ���ARR

  /* COLOR_TIMx enable counter */
  TIM_Cmd(COLOR_TIMx, ENABLE);                   										//ʹ�ܶ�ʱ��		
}

/**
  * @brief  COLOR_TIMx �����Ƴ�ʼ��
  *         ����PWMģʽ��GPIO
  * @param  ��
  * @retval ��
  */
void COLOR_TIMx_LED_Init(void)
{
	COLOR_TIMx_GPIO_Config();
	COLOR_TIMx_Mode_Config();	
}

//ֹͣpwm���
void COLOR_TIMx_LED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_Cmd(COLOR_TIMx, DISABLE);                   							//ʧ�ܶ�ʱ��						
	COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK, DISABLE); 	//ʧ�ܶ�ʱ��ʱ��
	
	//�ر�LED��
	GPIO_SetBits(COLOR_RED_TIM_LED_PORT,COLOR_RED_TIM_LED_PIN);
	GPIO_SetBits(COLOR_GREEN_TIM_LED_PORT,COLOR_GREEN_TIM_LED_PIN);
	GPIO_SetBits(COLOR_BLUE_TIM_LED_PORT,COLOR_BLUE_TIM_LED_PIN);
}

 /**
  * @brief  ����RGB LED����ɫ
	* @param  rgb:Ҫ����LED��ʾ����ɫֵ��ʽRGB888
  * @retval ��
  */
void SetRGBColor(uint32_t rgb)
{
	//������ɫֵ�޸Ķ�ʱ���ıȽϼĴ���ֵ
	COLOR_TIMx->COLOR_RED_CCRx = (uint8_t)(rgb>>16);			//R
	COLOR_TIMx->COLOR_GREEN_CCRx = (uint8_t)(rgb>>8);	  //G     
	COLOR_TIMx->COLOR_BLUE_CCRx = (uint8_t)rgb;						//B
}


 /**
  * @brief  ����RGB LED����ɫ
	* @param  r\g\b:Ҫ����LED��ʾ����ɫֵ
  * @retval ��
  */
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	//������ɫֵ�޸Ķ�ʱ���ıȽϼĴ���ֵ
	COLOR_TIMx->COLOR_RED_CCRx = r;	
	COLOR_TIMx->COLOR_GREEN_CCRx = g;	        
	COLOR_TIMx->COLOR_BLUE_CCRx = b;	
}
/*********************************************END OF FILE**********************/
