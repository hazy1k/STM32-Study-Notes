#ifndef __BSP_CIRCULAR_INTERPOLATION_H
#define	__BSP_CIRCULAR_INTERPOLATION_H

#include "./stepper/bsp_stepper_init.h"

/* ������ö�� */
typedef enum{
  x_axis = 0U,
  y_axis
}Axis_TypeDef;

/* ����������ö�� */
typedef enum{
  quadrant_1st = 0U,
  quadrant_2nd,
  quadrant_3rd,
  quadrant_4th
}Quadrant_TypeDef;

/* Բ���岹�����ṹ�� */
typedef struct{
  __IO int32_t startpoint_x;         //�������X
  __IO int32_t startpoint_y;         //�������Y
  __IO int32_t endpoint_x;           //�յ�����X
  __IO int32_t endpoint_y;           //�յ�����Y
  __IO uint32_t endpoint_pulse;       //�����յ�λ����Ҫ��������
  __IO uint32_t active_axis;          //��ǰ�˶�����
  __IO int32_t deviation;             //ƫ�����
  __IO uint8_t motionstatus : 1;      //�岹�˶�״̬
  __IO uint8_t dir_x : 1;             //X���˶�����
  __IO uint8_t dir_y : 1;             //Y���˶�����
  __IO uint8_t crood_pos : 2;         //����������ڵ�����
}CircularInterpolation_TypeDef;


extern CircularInterpolation_TypeDef interpolation_para;

void Circular_InterPolation_CW(int32_t start_x, int32_t start_y, int32_t stop_x, int32_t stop_y, uint16_t speed);

#endif /* __BSP_STEP_CIRCULAR_INTERPOLATION_H */
