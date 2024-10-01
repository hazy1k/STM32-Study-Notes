#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  


#define PWM_LEDR_GPIO_PORT         GPIOB
#define PWM_LEDR_GPIO_PIN          GPIO_Pin_5
#define PWM_LEDR_GPIO_CLK          (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
//#define PWM_LEDR_PINSOURCE         GPIO_PinSource10
//#define PWM_LEDR_AF                GPIO_AF_TIM5

#define PWM_LEDG_GPIO_PORT         GPIOB
#define PWM_LEDG_GPIO_PIN          GPIO_Pin_0
#define PWM_LEDG_GPIO_CLK          (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
//#define PWM_LEDG_PINSOURCE         GPIO_PinSource11
//#define PWM_LEDG_AF                GPIO_AF_TIM5

#define PWM_LEDB_GPIO_PORT         GPIOB
#define PWM_LEDB_GPIO_PIN          GPIO_Pin_1
#define PWM_LEDB_GPIO_CLK          (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
//#define PWM_LEDB_PINSOURCE         GPIO_PinSource12
//#define PWM_LEDB_AF                GPIO_AF_TIM5

#define PWM_LEDRGB_TIM             TIM3
#define PWM_LEDRGB_TIM_CLK         RCC_APB1Periph_TIM3

//��Ƶ�������Ҫ��ӳ��
#define   COLOR_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 		

 /**
  * @brief  ����TIM3�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIO clock enable */
  RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO), ENABLE); 
  
  	/*IO����*/
	COLOR_GPIO_REMAP_FUN();
  
  /* ����LED���õ������� */
	//��
  GPIO_InitStructure.GPIO_Pin = PWM_LEDR_GPIO_PIN;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(PWM_LEDR_GPIO_PORT , &GPIO_InitStructure);
	
	//��
	GPIO_InitStructure.GPIO_Pin =  PWM_LEDG_GPIO_PIN ;
  GPIO_Init(PWM_LEDG_GPIO_PORT, &GPIO_InitStructure);
	
	//��
	GPIO_InitStructure.GPIO_Pin =  PWM_LEDB_GPIO_PIN ;
  GPIO_Init(PWM_LEDB_GPIO_PORT, &GPIO_InitStructure);
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
void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	/* ����TIM3CLK ʱ��Ϊ72MHZ */
   RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, ENABLE); 					//ʹ��TIM5ʱ��

   /* ������ʱ������ */		 
   TIM_TimeBaseStructure.TIM_Period = 255;       							  //����ʱ����0������255����Ϊ266�Σ�Ϊһ����ʱ����
   TIM_TimeBaseStructure.TIM_Prescaler = 1999;	    							//����Ԥ��Ƶ��
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
   TIM_TimeBaseInit(PWM_LEDRGB_TIM, &TIM_TimeBaseStructure);

   /* PWMģʽ���� */
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
   TIM_OCInitStructure.TIM_Pulse = 0;										  			//���ó�ʼPWM������Ϊ0	
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
 
   TIM_OC2Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC2PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	

   TIM_OC3Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC3PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
   TIM_OC4Init(PWM_LEDRGB_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC4PreloadConfig(PWM_LEDRGB_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
   TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, ENABLE);			 										//ʹ��TIM5���ؼĴ���ARR
 
   /* TIM5 enable counter */
   TIM_Cmd(PWM_LEDRGB_TIM, ENABLE);                   										//ʹ�ܶ�ʱ��5
	
}

//RGBLED��ʾ��ɫ
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	PWM_LEDRGB_TIM->CCR2 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	PWM_LEDRGB_TIM->CCR3 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	PWM_LEDRGB_TIM->CCR4 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}

//RGBLED��ʾ��ɫ
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	PWM_LEDRGB_TIM->CCR2 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	PWM_LEDRGB_TIM->CCR3 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	PWM_LEDRGB_TIM->CCR4 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}

//ֹͣpwm���
void TIM_RGBLED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC1Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC2Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(PWM_LEDRGB_TIM,TIM_ForcedAction_InActive);
	TIM_ARRPreloadConfig(PWM_LEDRGB_TIM, DISABLE);
	TIM_Cmd(PWM_LEDRGB_TIM, DISABLE);                   							//ʧ�ܶ�ʱ��3						
	RCC_APB1PeriphClockCmd(PWM_LEDRGB_TIM_CLK, DISABLE); 	//ʧ�ܶ�ʱ��3ʱ��
	LED_GPIO_Config();
}


