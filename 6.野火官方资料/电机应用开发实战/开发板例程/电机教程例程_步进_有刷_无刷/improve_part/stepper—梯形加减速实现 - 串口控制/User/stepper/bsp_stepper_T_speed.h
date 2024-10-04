#ifndef __BSP_STEPPER_T_SPEED_H
#define	__BSP_STEPPER_T_SPEED_H

#include "stm32f1xx.h"
#include "./stepper/bsp_stepper_init.h"
#include "math.h"


//���μӼ�����ر���
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


//ϵͳ״̬
struct GLOBAL_FLAGS {
  //�����������������ʱ��ֵΪ1
  unsigned char running:1;
  //�����ڽ��յ�����ʱ��ֵΪ1
  unsigned char cmd:1;
  //���������������ʱ,ֵΪ1
  unsigned char out_ena:1;
};



#define FALSE             0
#define TRUE              1

/*����ٶȾ����е��ĸ�״̬*/
#define STOP              0 // ֹͣ״̬
#define ACCEL             1 // ����״̬
#define DECEL             2 // ����״̬
#define RUN               3 // ����״̬

/*Ƶ����ز���*/
//��ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/(TIM_PRESCALER+1)
//72/(1+1)=36Mhz
//������Ҫ��Ƶ�ʿ����Լ�����
#define TIM_PRESCALER      1
#define T1_FREQ           (SystemCoreClock/(TIM_PRESCALER+1)) // Ƶ��ftֵ


/*�����Ȧ����*/
#define STEP_ANGLE				1.8									//��������Ĳ���� ��λ����
#define FSPR              (360.0/1.8)         //���������һȦ����������

#define MICRO_STEP        32          				//ϸ����ϸ���� 
#define SPR               (FSPR*MICRO_STEP)   //ϸ�ֺ�һȦ����������

/*��ѧ����,���ڼ򻯼���*/

#define ALPHA             ((float)(2*3.14159/SPR))       // ��= 2*pi/spr
#define A_T_x10           ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148       ((float)((T1_FREQ*0.676)/10)) // 0.676Ϊ�������ֵ
#define A_SQ              ((float)(2*100000*ALPHA)) 
#define A_x200            ((float)(200*ALPHA))

extern void speed_decision(void);
extern void MSD_ENA(FunctionalState NewState);
extern int stepPosition;
extern struct GLOBAL_FLAGS status;

#endif
