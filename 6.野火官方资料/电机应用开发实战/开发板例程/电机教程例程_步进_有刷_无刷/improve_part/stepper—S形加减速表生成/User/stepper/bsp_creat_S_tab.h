#ifndef __BSP_CREAT_S_TAB_H
#define	__BSP_CREAT_S_TAB_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "./usart/bsp_debug_usart.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

#define FORM_LEN 	   1000
#define MICRO_STEP     32    // ������ϸ����
#define FSPR           200   // ���������Ȧ����
#define CONVER(speed)  ((speed) * FSPR * MICRO_STEP / 60)  // ���ݵ��ת�٣�r/min�������������٣�step/s��


typedef struct 
{
	int		Vo;  //���ٶ� ��λ Step/s
	int		Vt;  //ĩ�ٶ� ��λ Step/s
	int		AccelHalfStep;   //���ٽ׶ΰ�·��	��λ Step   
	int		AccelTotalStep;  //��·�̣����ٽ׶��ܲ��� Step
	float   Form[FORM_LEN]; // �ٶȱ�� ��λ Step/s
	
}Speed_s;


extern uint8_t print_flag;
void CalculateSpeedTab(int Vo, int Vt, float Time);

#endif 
