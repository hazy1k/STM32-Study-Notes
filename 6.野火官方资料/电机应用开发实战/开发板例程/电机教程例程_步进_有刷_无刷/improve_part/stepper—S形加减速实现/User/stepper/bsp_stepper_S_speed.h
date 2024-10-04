#ifndef __BSP_STEPPER_S_SPEED_H
#define	__BSP_STEPPER_S_SPEED_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "./usart/bsp_debug_usart.h"
#include "./stepper/bsp_stepper_init.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"


#define CONVER(speed)  ((speed) * FSPR * MICRO_STEP / 60)  // ���ݵ��ת�٣�r/min�������������٣�step/s��

#define FORM_LEN 	   8000

typedef struct {
	uint8_t 	status;			/*״̬*/
	uint8_t 	dir;				/*����*/
	uint32_t 	pos;				/*λ��*/
	uint32_t  pluse_time; /*����ʱ��*/
}Stepper_Typedef;

/*S�Ӽ������õ��Ĳ���*/
typedef struct {
  int32_t   Vo;                	/*��ʼ�ٶ�*/
  int32_t   Vt;               	/*ĩ�ٶ�*/
  int32_t 	AccelTotalStep;   	/*�����ܲ���*/  
	int32_t		INC_AccelTotalStep;	/*�Ӽ��ٶȲ���*/
	int32_t 	Dec_AccelTotalStep;	/*�����ٶȲ���*/
  float   	Form[FORM_LEN];     /*S�Ӽ��� �ٶȱ�*/ 
}SpeedCalc_TypeDef;

extern SpeedCalc_TypeDef Speed ;

/*����ٶȾ����е��ĸ�״̬*/
#define ACCEL                 1   //  ����״̬
#define AVESPEED              2   //  ����״̬
#define DECEL                 3   //  ����״̬
#define STOP                  0   //  ֹͣ״̬
																												  

/*Ƶ����ز���*/
//��ʱ��ʵ��ʱ��Ƶ��Ϊ��168MHz/(TIM_PRESCALER+1)
//���� �߼���ʱ���� Ƶ��Ϊ168MHz,������ʱ��Ϊ84MHz
//168/(168)=1Mhz
//������Ҫ��Ƶ�ʿ����Լ�����
#define TIM_PRESCALER         72 
#define T1_FREQ               (SystemCoreClock/(TIM_PRESCALER+1)) // Ƶ��ftֵ


/*�����Ȧ����*/
#define STEP_ANGLE						1.8									//��������Ĳ���� ��λ����
#define FSPR              		(360.0f/1.8f)         //���������һȦ����������
			
#define MICRO_STEP        		32          				//ϸ����ϸ���� 
#define SPR               		(FSPR*MICRO_STEP)   //ϸ�ֺ�һȦ����������

#define CONVER(speed)  ((speed) * FSPR * MICRO_STEP / 60)  // ���ݵ��ת�٣�r/min�������������٣�step/s��

#define TRUE                  1
#define FALSE                 0

#define MIN_SPEED                              (T1_FREQ / (65535.0f))// ���Ƶ��/�ٶ�

extern uint8_t print_flag;
void CalcSpeed(int32_t Vo, int32_t Vt, float T);
void stepper_move_S(int start_speed,int end_speed,float time);
void speed_decision(void);
#endif 
