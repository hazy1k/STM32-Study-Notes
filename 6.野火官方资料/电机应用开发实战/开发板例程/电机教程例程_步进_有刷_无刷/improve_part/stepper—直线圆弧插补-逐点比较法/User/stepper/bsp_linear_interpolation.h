#ifndef __BSP_LINEAR_INTERPOLATION_H
#define	__BSP_LINEAR_INTERPOLATION_H

#include "stm32f1xx_hal.h"
#include "./stepper/bsp_stepper_init.h"

///* ������ö�� */
//typedef enum{
//  x_axis = 0U,
//  y_axis
//}Axis_TypeDef;

/* ֱ�߲岹�����ṹ�� */
typedef struct{
  __IO uint32_t endpoint_x;           //�յ�����X
  __IO uint32_t endpoint_y;           //�յ�����Y
  __IO uint32_t endpoint_pulse;       //�����յ�λ����Ҫ��������
  __IO uint32_t active_axis;          //��ǰ�˶�����
  __IO int32_t deviation;             //ƫ�����
  __IO uint8_t motionstatus : 1;      //�岹�˶�״̬
  __IO uint8_t dir_x : 1;             //X���˶�����
  __IO uint8_t dir_y : 1;             //Y���˶�����
}LinearInterpolation_TypeDef;

extern LinearInterpolation_TypeDef linear_para;

void Linear_Interpolation(int32_t coordi_x, int32_t coordi_y, uint16_t speed);
void LinearInterpolation_Handler(TIM_HandleTypeDef *htim);

#endif /* __BSP_STEP_LINEAR_INTERPOLATION_H */
