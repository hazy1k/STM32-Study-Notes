#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H


#include "stm32f10x.h"

/************ͨ�ö�ʱ��TIM�������壬ֻ��TIM2��3��4��5************/
// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������Ĭ��ʹ��TIM5
#define            GENERAL_TIM                   TIM5                   // ͨ�ö�ʱ��ѡ��-TIM5
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd // ʹ��ʱ��
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM5    // ʱ��ѡ��
#define            GENERAL_TIM_PERIOD            0XFFFF                 // �Զ���װ��ֵ
#define            GENERAL_TIM_PSC               (72-1)                 // ʱ��Ԥ��Ƶϵ��

// TIM ���벶��ͨ��GPIO��غ궨��
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA  // ͨ��1 GPIOʱ��
#define            GENERAL_TIM_CH1_PORT          GPIOA                  // ͨ��1 GPIO�˿�
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_0             // ͨ��1 GPIO����
#define            GENERAL_TIM_CHANNEL_x         TIM_Channel_1          // ͨ��ѡ��

// �ж���غ궨��
#define            GENERAL_TIM_IT_CCx            TIM_IT_CC1             // ���벶���ж�
#define            GENERAL_TIM_IRQ               TIM5_IRQn              // ͨ�ö�ʱ���ж�
#define            GENERAL_TIM_INT_FUN           TIM5_IRQHandler        // ͨ�ö�ʱ���жϷ�����

// ��ȡ����Ĵ���ֵ�����궨��
#define            GENERAL_TIM_GetCapturex_FUN                 TIM_GetCapture1
// �����źż��Ժ����궨��
#define            GENERAL_TIM_OCxPolarityConfig_FUN           TIM_OC1PolarityConfig

// ��������ʼ����
#define            GENERAL_TIM_STRAT_ICPolarity                TIM_ICPolarity_Rising
// �����Ľ�������
#define            GENERAL_TIM_END_ICPolarity                  TIM_ICPolarity_Falling

// ��ʱ�����벶���û��Զ�������ṹ������
typedef struct
{   
	uint8_t   Capture_FinishFlag;   // ���������־λ
	uint8_t   Capture_StartFlag;    // ����ʼ��־λ
	uint16_t  Capture_CcrValue;     // ����Ĵ�����ֵ
	uint16_t  Capture_Period;       // �Զ���װ�ؼĴ������±�־ 
}TIM_ICUserValueTypeDef;

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;

void GENERAL_TIM_Init(void);

#endif