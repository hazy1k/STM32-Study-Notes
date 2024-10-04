#ifndef __BSP_CIRCULAR_INTERPOLATION_H
#define	__BSP_CIRCULAR_INTERPOLATION_H

#include "./stepper/bsp_stepper_init.h"

/* �岹ģʽö�� */
typedef enum{
  Circular = 0U,
  Linear
}InterpolationMODE_TypeDef;

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
  __IO int32_t startpoint[2];        //�������X��Y
  __IO int32_t endpoint_x;           //�յ�����X
  __IO int32_t endpoint_y;           //�յ�����Y
  __IO uint32_t endpoint_pulse;      //�����յ�λ����Ҫ��������
  __IO uint32_t active_axis;         //��ǰ�˶�����
  __IO int32_t deviation;            //ƫ�����F
  __IO int8_t devi_sign[2];          //ƫ��̵�������ţ�����
  __IO uint8_t motionstatus : 1;     //�岹�˶�״̬
  __IO uint8_t dir_x : 1;            //X���˶�����
  __IO uint8_t dir_y : 1;            //Y���˶�����
  __IO uint8_t dir_interpo : 1;      //�岹�����˶�����
  __IO uint8_t crood_pos : 2;        //����������ڵ�����
}CircularInterpolation_TypeDef;

extern InterpolationMODE_TypeDef mode;
extern CircularInterpolation_TypeDef circular_para;

void Circular_InterPolation(int32_t start_x, int32_t start_y, int32_t stop_x, int32_t stop_y, uint16_t speed, uint8_t dir);

#endif /* __BSP_STEP_CIRCULAR_INTERPOLATION_H */
