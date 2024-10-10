#ifndef __BSP_ADVANCETIME_H
#define __BSP_ADVANCETIME_H


#include "stm32f10x.h"


#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20

//ϵͳ״̬
struct GLOBAL_FLAGS {
  //�����������������ʱ��ֵΪ1
  unsigned char running:1;
  //�����ڽ��յ�����ʱ��ֵΪ1
  unsigned char cmd:1;
  //���������������ʱ,ֵΪ1
  unsigned char out_ena:1;
};

extern struct GLOBAL_FLAGS status;
extern int stepPosition;

#define T1_FREQ 1000000     //��ʱ��Ƶ��
#define FSPR    200         //���������Ȧ����
#define SPR     (FSPR*2)  //4ϸ�ֵĲ���
// ��ѧ������ ����SMD_Move�����ļ򻯼���
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // 
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//�ٶ�����״̬
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

typedef struct {
  //�������״̬
  unsigned char run_state : 3;
  //������з���
  unsigned char dir : 1;
  //��һ��������ʱ���ڣ�����ʱΪ���ٶ�����
  unsigned int step_delay;
  //��ʼ���ٵ�λ��
  unsigned int decel_start;
  //���پ���
  signed int decel_val;
  //��С��ʱ��������ٶȣ�
  signed int min_delay;
  //���ٻ��߼��ټ�����
  signed int accel_count;
} speedRampData;



// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������ʹ�ö�ʱ��TIM8
#define            SMD_PULSE_TIM                    TIM8
#define            SMD_PULSE_TIM_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define            SMD_PULSE_TIM_CLK                RCC_APB2Periph_TIM8
#define            SMD_PULSE_TIM_CHANNEL            TIM_Channel_1
#define            SMD_PULSE_TIM_SETCOMPARE         TIM_SetCompare1
#define            SMD_PULSE_TIM_CC_ENABLE          (SMD_PULSE_TIM->CCER |= 1<<0)
#define            SMD_PULSE_TIM_CC_DISABLE         (SMD_PULSE_TIM->CCER &= ~(1<<0))

// ��ʱ�����PWMͨ����PA0��ͨ��1
#define            SMD_PULSE_OCx_Init               TIM_OC1Init
#define            SMD_PULSE_OCx_PreloadConfig      TIM_OC1PreloadConfig

// ��ʱ���ж�
#define            SMD_PULSE_TIM_IRQ                TIM8_UP_IRQn
#define            SMD_PULSE_TIM_IRQHandler         TIM8_UP_IRQHandler

// PWM �źŵ�Ƶ�� F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            SMD_PULSE_TIM_PERIOD             (1)
#define            SMD_PULSE_TIM_PSC                (72)

// ��������������ͨ��
#define            SMD_PULSE_GPIO_CLK               RCC_APB2Periph_GPIOC
#define            SMD_PULSE_PORT                   GPIOC
#define            SMD_PULSE_PIN                    GPIO_Pin_6

// ��������������
#define            SMD_DIR_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define            SMD_DIR_PORT                     GPIOB
#define            SMD_DIR_PIN                      GPIO_Pin_0

// ����������ʹ������
#define            SMD_ENA_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define            SMD_ENA_PORT                     GPIOB
#define            SMD_ENA_PIN                      GPIO_Pin_2



//// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
//// ��������ʹ�ö�ʱ��TIM2
//#define            SMD_PULSE_TIM                    TIM2
//#define            SMD_PULSE_TIM_APBxClock_FUN      RCC_APB1PeriphClockCmd
//#define            SMD_PULSE_TIM_CLK                RCC_APB1Periph_TIM2
//#define            SMD_PULSE_TIM_CHANNEL            TIM_Channel_4
//#define            SMD_PULSE_TIM_SETCOMPARE         TIM_SetCompare4
//#define            SMD_PULSE_TIM_CC_ENABLE          (SMD_PULSE_TIM->CCER |= 1<<12)
//#define            SMD_PULSE_TIM_CC_DISABLE         (SMD_PULSE_TIM->CCER &= ~(1<<12))
//// ��ʱ�����PWMͨ����PA0��ͨ��1
//#define            SMD_PULSE_OCx_Init               TIM_OC4Init
//#define            SMD_PULSE_OCx_PreloadConfig      TIM_OC4PreloadConfig
//// ��ʱ���ж�
//#define            SMD_PULSE_TIM_IRQ                TIM2_IRQn
//#define            SMD_PULSE_TIM_IRQHandler         TIM2_IRQHandler

//// PWM �źŵ�Ƶ�� F = TIM_CLK/{(ARR+1)*(PSC+1)}
//#define            SMD_PULSE_TIM_PERIOD             (1)
//#define            SMD_PULSE_TIM_PSC                (72)

//// ��������������ͨ��
//#define            SMD_PULSE_GPIO_CLK               RCC_APB2Periph_GPIOA
//#define            SMD_PULSE_PORT                   GPIOA
//#define            SMD_PULSE_PIN                    GPIO_Pin_3

//// ��������������
//#define            SMD_DIR_GPIO_CLK                 RCC_APB2Periph_GPIOB
//#define            SMD_DIR_PORT                     GPIOB
//#define            SMD_DIR_PIN                      GPIO_Pin_14

//// ����������ʹ������
//#define            SMD_ENA_GPIO_CLK                 RCC_APB2Periph_GPIOC
//#define            SMD_ENA_PORT                     GPIOB
//#define            SMD_ENA_PIN                      GPIO_Pin_12


#define DIR(a)	if (a == CW)	\
					GPIO_ResetBits(SMD_DIR_PORT,SMD_DIR_PIN);\
					else		\
					GPIO_SetBits(SMD_DIR_PORT,SMD_DIR_PIN)
                    
/**************************��������********************************/

void SMD_Init(void);
void SMD_ENA(FunctionalState NewState);
void SMD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

#endif	/* __BSP_ADVANCETIME_H */


