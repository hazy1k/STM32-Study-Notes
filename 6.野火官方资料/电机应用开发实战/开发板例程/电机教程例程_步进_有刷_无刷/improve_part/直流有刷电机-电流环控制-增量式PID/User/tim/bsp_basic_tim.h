#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f1xx.h"

#define BASIC_TIM           		  TIM6
#define BASIC_TIM_CLK_ENABLE()   	__HAL_RCC_TIM6_CLK_ENABLE()

#define BASIC_TIM_IRQn				    TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	TIM6_DAC_IRQHandler

/* �ۼ� TIM_Period�������һ�����»����ж�*/		
//����ʱ����0������BASIC_PERIOD_COUNT-1����ΪBASIC_PERIOD_COUNT�Σ�Ϊһ����ʱ����

//PID��������:25ms����һ��--Ƶ��40Hz
#define BASIC_PERIOD_MS    (25)//25ms

#define BASIC_PERIOD_COUNT    (BASIC_PERIOD_MS*100)

//��ʱ��ʱ��ԴTIMxCLK = 2 * APB1_CLK
//				���APB1_CLK = 72MHz = TIMxCLK
//				����TIMxCLK=APB1_CLK * 2
// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
#define BASIC_PRESCALER_COUNT   (720)//100Khz

/* ��ȡ��ʱ�������ڣ���λms */
//#define __HAL_TIM_GET_PRESCALER(__HANDLE__)      ((__HANDLE__)->Instance->PSC)    // Get TIM Prescaler.
//#define GET_BASIC_TIM_PERIOD(__HANDLE__)    (1.0/(HAL_RCC_GetPCLK2Freq()/(__HAL_TIM_GET_PRESCALER(__HANDLE__)+1)/(__HAL_TIM_GET_AUTORELOAD(__HANDLE__)+1))*1000)

/* ��������������ڶ�ʱ��ʱ��ԴTIMxCLK=72MHz��Ԥ��Ƶ��Ϊ��7200-1 ����� */
#define SET_BASIC_TIM_PERIOD(T)     __HAL_TIM_SET_AUTORELOAD(&TIM_TimeBaseStructure, (T)*100 - 1)    // ���ö�ʱ�������ڣ�1~1000ms��
#define GET_BASIC_TIM_PERIOD()      ((__HAL_TIM_GET_AUTORELOAD(&TIM_TimeBaseStructure)+1)/100.0f)     // ��ȡ��ʱ�������ڣ���λms

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
void TIMx_Configuration(void);

#endif /* __BASIC_TIM_H */

