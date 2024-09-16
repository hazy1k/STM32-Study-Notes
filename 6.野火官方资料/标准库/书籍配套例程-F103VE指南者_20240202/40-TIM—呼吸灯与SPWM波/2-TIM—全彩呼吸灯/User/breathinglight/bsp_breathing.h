#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H


#include "stm32f10x.h"


/*��ѹ��ֵ�ȼ���*/
#define AMPLITUDE_CLASS 256

//����������ε�Ƶ��
extern __IO uint16_t period_class ;

/*PWM���еĵ���*/
extern uint16_t POINT_NUM;
/********************��ʱ��ͨ��**************************/

#define   BRE_TIMx                      TIM3
 
//�ж����
#define   BRE_TIMx_IRQn                TIM3_IRQn             
#define   BRE_TIMx_IRQHandler          TIM3_IRQHandler

//ʱ��
#define   BRE_TIM_APBxClock_FUN        RCC_APB1PeriphClockCmd
#define   BRE_TIM_CLK                   RCC_APB1Periph_TIM3
#define   BRE_TIM_GPIO_CLK              (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)

//��Ƶ�������Ҫ��ӳ��
#define   BRE_GPIO_REMAP_FUN()						GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 				

/************���***************/
#define  BRE_RED_TIM_LED_PORT               GPIOB
#define  BRE_RED_TIM_LED_PIN                GPIO_Pin_5

#define  BRE_RED_TIM_OCxInit                TIM_OC2Init            //ͨ����ʼ������
#define  BRE_RED_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig //ͨ���������ú���

//ͨ���ȽϼĴ�������TIMx->CCRx��ʽ�ɷ��ʸüĴ����������µıȽ�ֵ������ռ�ձ�
//�Ժ��װ��ʹ��������ʽ��BRE_TIMx->BRE_RED_CCRx ���ɷ��ʸ�ͨ���ıȽϼĴ���
#define  BRE_RED_CCRx                       	CCR2		

/************�̵�***************/

#define  BRE_GREEN_TIM_LED_PORT               GPIOB
#define  BRE_GREEN_TIM_LED_PIN                GPIO_Pin_0

#define  BRE_GREEN_TIM_OCxInit                TIM_OC3Init            //ͨ����ʼ������
#define  BRE_GREEN_TIM_OCxPreloadConfig       TIM_OC3PreloadConfig //ͨ���������ú���

//ͨ���ȽϼĴ�������TIMx->CCRx��ʽ�ɷ��ʸüĴ����������µıȽ�ֵ������ռ�ձ�
//�Ժ��װ��ʹ��������ʽ��BRE_TIMx->BRE_GREEN_CCRx ���ɷ��ʸ�ͨ���ıȽϼĴ���
#define  BRE_GREEN_CCRx                       CCR3

/************����***************/
#define   BRE_BLUE_TIM_LED_PORT             GPIOB
#define   BRE_BLUE_TIM_LED_PIN              GPIO_Pin_1

#define   BRE_BLUE_TIM_OCxInit              TIM_OC4Init            //ͨ����ʼ������
#define   BRE_BLUE_TIM_OCxPreloadConfig    TIM_OC4PreloadConfig  //ͨ���������ú���

//ͨ���ȽϼĴ�������TIMx->CCRx��ʽ�ɷ��ʸüĴ����������µıȽ�ֵ������ռ�ձ�
//�Ժ��װ��ʹ��������ʽ��BRE_TIMx->BRE_BLUE_CCRx ���ɷ��ʸ�ͨ���ıȽϼĴ���
#define   BRE_BLUE_CCRx                      CCR4




void      TIMx_Breathing_Init          (void);


#endif /* __PWM_BREATHING_H */
