#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H


#include "stm32f1xx.h"

// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_PRESCALER               71

// ��ʱ������
#define GENERAL_TIM_PERIOD                  0xFFFF

/************ͨ�ö�ʱ��TIM�������壬ֻ��TIM2��3��4��5************/
// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������Ĭ��ʹ��TIM5

#define GENERAL_TIMx                        TIM5
#define GENERAL_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM5_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM5_CLK_DISABLE()
#define GENERAL_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE();
#define GENERAL_TIM_CHANNELx                TIM_CHANNEL_1
#define GENERAL_TIM_IT_CCx                  TIM_IT_CC1

#define GENERAL_TIM_IRQn                    TIM5_IRQn
#define GENERAL_TIM_IRQHANDLER              TIM5_IRQHandler

#define KEY1_RCC_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY1_GPIO_PIN                       GPIO_PIN_0
#define KEY1_GPIO                           GPIOA

//----------------------------------------------------------------
// ��������ʼ����
#define            GENERAL_TIM_STRAT_ICPolarity        TIM_INPUTCHANNELPOLARITY_RISING 
// �����Ľ�������
#define            GENERAL_TIM_END_ICPolarity          TIM_INPUTCHANNELPOLARITY_FALLING 


//----------------------------------------------------------------
// ��ʱ�����벶���û��Զ�������ṹ������
typedef struct              
{   
	uint8_t   ucFinishFlag;   // ���������־λ
	uint8_t   ucStartFlag;    // ����ʼ��־λ
	uint16_t  usCtr;          // ����Ĵ�����ֵ
	uint16_t  usPeriod;       // �Զ���װ�ؼĴ������±�־ 
}STRUCT_CAPTURE; 

//----------------------------------------------------------------
extern TIM_HandleTypeDef  TIM_TimeBase;
extern STRUCT_CAPTURE TIM_ICUserValueStructure;
//----------------------------------------------------------------

/**************************��������********************************/
void GENERAL_TIM_Mode_Config(void); 



#endif	/* __BSP_GENERALTIME_H */


