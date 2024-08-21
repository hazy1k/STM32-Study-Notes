#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "stm32f10x.h"

// �߼���ʱ��TIM1��ز����궨��
#define            ADVANCE_TIM                   TIM1                   // �߼���ʱ��ѡ��
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd // �߼���ʱ��ʱ��ʹ�ܺ���
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1    // �߼���ʱ��ʱ��

// PWM �źŵ�Ƶ�� F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            ADVANCE_TIM_PERIOD            (8-1)                  // �߼���ʱ����������
#define            ADVANCE_TIM_PSC               (9-1)                  // �߼���ʱ����Ƶ����
#define            ADVANCE_TIM_PULSE             4                      // �߼���ʱ�����������

// �ж���������
#define            ADVANCE_TIM_IRQ               TIM1_UP_IRQn           // �߼���ʱ�������ж�
#define            ADVANCE_TIM_IRQHandler        TIM1_UP_IRQHandler     // �߼���ʱ�������жϴ�����

// TIM1 ����Ƚ�ͨ��
#define            ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA   // ͨ��1 GPIOʱ��ʹ�ܺ���
#define            ADVANCE_TIM_CH1_PORT          GPIOA                  // ͨ��1 GPIO�˿�
#define            ADVANCE_TIM_CH1_PIN           GPIO_Pin_8             // ͨ��1 GPIO����

// TIM1 ����Ƚ�ͨ���Ļ���ͨ��
#define            ADVANCE_TIM_CH1N_GPIO_CLK      RCC_APB2Periph_GPIOB  // ͨ��1N GPIOʱ��ʹ�ܺ���
#define            ADVANCE_TIM_CH1N_PORT          GPIOB                 // ͨ��1N GPIO�˿�
#define            ADVANCE_TIM_CH1N_PIN           GPIO_Pin_13           // ͨ��1N GPIO����

// TIM1 ����Ƚ�ͨ����ɲ��ͨ��
#define            ADVANCE_TIM_BKIN_GPIO_CLK      RCC_APB2Periph_GPIOB  // ɲ��ͨ�� GPIOʱ��ʹ�ܺ���
#define            ADVANCE_TIM_BKIN_PORT          GPIOB                 // ɲ��ͨ�� GPIO�˿�
#define            ADVANCE_TIM_BKIN_PIN           GPIO_Pin_12           // ɲ��ͨ�� GPIO����

void ADVANCE_TIM_Init(void);


#endif	