#ifndef __BSP_PID_H
#define	__BSP_PID_H
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
  float target_val;     //Ŀ��ֵ
	float actual_val;     //ʵ��ֵ
	float err;            //���嵱ǰƫ��ֵ
	float err_next;       //������һ��ƫ��ֵ
	float err_last;       //�������һ��ƫ��ֵ
	float Kp, Ki, Kd;     //������������֡�΢��ϵ��
}_pid;

extern void PID_param_init(void);
extern void set_pid_target(float temp_val);
extern float get_pid_target(void);
extern void set_p_i_d(float p, float i, float d);
extern float PID_realize(float actual_val);
extern void time_period_fun(void);

#endif
