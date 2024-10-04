#ifndef __BSP_LINEAR_INTERPOLATION_H
#define	__BSP_LINEAR_INTERPOLATION_H

#include "stm32f1xx_hal.h"
#include "./stepper/bsp_stepper_init.h"

/* ������ö�� */
typedef enum{
  x_axis = 0U,
  y_axis
}Axis_TypeDef;

/* ֱ�߲岹�����ṹ�� */
typedef struct{
  __IO uint32_t endpoint_x;           //�յ�����X
  __IO uint32_t endpoint_y;           //�յ�����Y
  __IO uint32_t endpoint_pulse;       //�����յ�λ����Ҫ��������
  __IO uint32_t active_axis;          //��ǰ�˶�����
  __IO int32_t deviation;             //ƫ�����
}LinearInterpolation_TypeDef;

#define F       32

void InterPolation_Move(uint32_t inc_x, uint32_t inc_y, uint16_t speed);

#endif /* __BSP_STEP_LINEAR_INTERPOLATION_H */
