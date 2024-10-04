#ifndef __BSP_STEPPER_S_SPEED_H
#define	__BSP_STEPPER_S_SPEED_H

#include "stm32f1xx_hal.h"
#include "./usart/bsp_debug_usart.h"
#include "./stepper/bsp_stepper_init.h"
#include <stdbool.h>

#define FORM_LEN 	   8000

typedef struct {
	__IO uint8_t  status;             /* ״̬ */
	__IO uint8_t  dir;                /* ���� */
	__IO uint32_t pos;                /* λ�� */
  __IO uint16_t pluse_time;         /* ����ʱ�� */
}Stepper_Typedef;

/* S�Ӽ������õ��Ĳ��� */
typedef struct {
  __IO int32_t Vo;                  /* ��ʼ�ٶ� */
  __IO int32_t Vt;                  /* ĩ�ٶ� */
  __IO int32_t AccelTotalStep;      /* �����ܲ��� */
  __IO int32_t DecPoint;            /* ��ʼ���ٵĲ��� */
  __IO int32_t TotalStep;           /* ���������ܲ��� */
	__IO int32_t INC_AccelTotalStep;  /* �Ӽ��ٶȲ��� */
	__IO int32_t Dec_AccelTotalStep;  /* �����ٶȲ��� */
  __IO float   Form[FORM_LEN];/* S�Ӽ��� �ٶȱ� */
}SpeedCalc_TypeDef;

/* ����ٶȾ����е��ĸ�״̬ */
typedef enum {
  STOP = 0U,                        /* ֹͣ״̬ */
  ACCEL,                            /* ����״̬ */
  UNIFORM,                          /* ����״̬ */
  DECEL,                            /* ����״̬ */
}StateEnum_TypeDef;

/*Ƶ����ز���*/
#define T1_FREQ               (SystemCoreClock / TIM_PRESCALER)//Ƶ��ftֵ
/*�����Ȧ����*/
#define STEP_ANGLE						1.8f									           //��������Ĳ���� ��λ����
#define FSPR              		(360.0f / 1.8f)                  //���������һȦ����������
			
#define MICRO_STEP        		32          				             //ϸ����ϸ���� 
#define SPR               		(FSPR * MICRO_STEP)              //ϸ�ֺ�һȦ����������

#define CONVER(speed)         (float)(speed * SPR / 60.0f)     //���ݵ��ת�٣�r/min�������������٣�step/s��
  
#define MIN_SPEED             (float)(T1_FREQ / 65535U)        //���Ƶ��/�ٶ�

extern Stepper_Typedef Stepper;

bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step);

#endif 
