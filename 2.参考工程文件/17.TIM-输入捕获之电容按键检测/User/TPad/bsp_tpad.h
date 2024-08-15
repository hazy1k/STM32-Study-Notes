#ifndef __BSP_TPADTIME_H
#define __BSP_TPADTIME_H


#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_usart.h"

/************ͨ�ö�ʱ��TIM�������壬ֻ��TIM2��3��4��5************/
// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������Ĭ��ʹ��TIM5
#define            TPAD_TIM                   TIM5                   // ���ݰ���ʹ�õĶ�ʱ��--TIM5
#define            TPAD_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd // ��ʱ��ʱ��ʹ�ܺ���--RCC_APB1PeriphClockCmd
#define            TPAD_TIM_CLK               RCC_APB1Periph_TIM5    // ��ʱ��ʱ��--RCC_APB1Periph_TIM5
#define            TPAD_TIM_Period            0XFFFF                 // ��ʱ������--0XFFFF
#define            TPAD_TIM_Prescaler         71                     // ��ʱ����Ƶϵ��--71

// TIM ���벶��ͨ��GPIO��غ궨��
#define            TPAD_TIM_CH_GPIO_CLK       RCC_APB2Periph_GPIOA  // ��ʱ�����벶��ͨ��GPIOʱ��--RCC_APB2Periph_GPIOA
#define            TPAD_TIM_CH_PORT           GPIOA                 // ��ʱ�����벶��ͨ��GPIO�˿�--GPIOA
#define            TPAD_TIM_CH_PIN            GPIO_Pin_1            // ��ʱ�����벶��ͨ��GPIO�˿ں�--GPIO_Pin_1
#define            TPAD_TIM_CHANNEL_x         TIM_Channel_2         // ��ʱ�����벶��ͨ��--TIM_Channel_2

// �ж���غ궨��
#define            TPAD_TIM_IT_CCx            TIM_IT_CC2           // ��ʱ�����벶���ж�--TIM_IT_CC2
#define            TPAD_TIM_IRQ               TIM5_IRQn            // ��ʱ���ж�����--TIM5_IRQn
#define            TPAD_TIM_INT_FUN           TIM5_IRQHandler      // ��ʱ���жϷ�����--TIM5_IRQHandler

// ��ȡ����Ĵ���ֵ�����궨��
#define            TPAD_TIM_GetCapturex_FUN                 TIM_GetCapture2
// �����źż��Ժ����궨��
#define            TPAD_TIM_OCxPolarityConfig_FUN           TIM_OC2PolarityConfig

// ���ݰ��������µ�ʱ������ֵ����Ҫ���ݲ�ͬ��Ӳ��ʵ�ʲ��ԣ���С�������ֵ�����ṩ��Ӧ�ٶ�
#define            TPAD_GATE_VAL              70

// ���ݰ������ص�ʱ���������С�ĳ��ʱ�䣬��ͬ��Ӳ����һ����ָ�����ȶ���76
#define            TPAD_DEFAULT_VAL_MIN       70
#define            TPAD_DEFAULT_VAL_MAX       80

#define            TPAD_ON                     1
#define            TPAD_OFF                    0

uint8_t TPAD_Init(void);
uint8_t TPAD_Scan(void);
uint8_t TPAD_Scan0(void);


#endif	
