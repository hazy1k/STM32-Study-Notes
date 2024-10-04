/**
  ******************************************************************************
  * @file    bsp_stepper_liner_interpolation.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ��һ����˳ʱ��Բ���岹-���ȽϷ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_circular_interpolation.h"
#include <stdlib.h>
#include <math.h>

Axis_TypeDef axis;
CircularInterpolation_TypeDef interpolation_para = {0};

/**
  * @brief  ��һ����˳Բ�岹�˶�
  * @param  start_x��Բ����������Բ�ĵ�����X������
  * @param  inc_y���յ�����Y������
  * @param  speed�������ٶ�
  * @retval ��
  */
void Circular_InterPolation_CW(int32_t start_x, int32_t start_y, int32_t stop_x, int32_t stop_y, uint16_t speed)
{
  /* �жϵ�ǰ�Ƿ��������岹�˶� */
  if(interpolation_para.motionstatus != 0)
    return;
  
  /* �����㡢�յ������Ƿ���ͬһ��Բ�� */
  if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y)))
    return;
  
  /* ƫ������ */
  interpolation_para.deviation = 0;
  
  /* ������� */
  interpolation_para.startpoint_x = start_x;
  interpolation_para.startpoint_y = start_y;
  /* �յ����� */
  interpolation_para.endpoint_x = stop_x;
  interpolation_para.endpoint_y = stop_y;
  /* �����������Ǵ���㵽�յ��������֮�� */
  interpolation_para.endpoint_pulse = abs(stop_x - start_x) + abs(stop_y - start_y);
  
  /* ��һ������Բ��x����ת��y����ת */
  interpolation_para.dir_x = CW;
  interpolation_para.dir_y = CCW;
  MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
  MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
  
  /* �������x=0��˵�������y���ϣ�ֱ����x������ɼ�С��� */
  if(interpolation_para.startpoint_x == 0)
  {
    /* ��һ�����Ϊx�� */
    interpolation_para.active_axis = x_axis;
    /* ����ƫ�� */
    interpolation_para.deviation += (2 * interpolation_para.startpoint_x + 1);
  }
  else
  {
    /* ��һ�����ΪY�� */
    interpolation_para.active_axis = y_axis;
    /* ����ƫ�� */
    interpolation_para.deviation -= (2 * interpolation_para.startpoint_y + 1);
  }
  
  /* �����ٶ� */
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[x_axis].pul_channel, speed);
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[y_axis].pul_channel, speed);
  __HAL_TIM_SET_AUTORELOAD(&TIM_StepperHandle, speed * 2);
  
  /* ʹ������� */
  __HAL_TIM_MOE_ENABLE(&TIM_StepperHandle);
  /* ����X��Ƚ�ͨ����� */
  TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  HAL_TIM_Base_Start_IT(&TIM_StepperHandle);

  interpolation_para.motionstatus = 1;
}

/**
  * @brief  ��ʱ���Ƚ��жϻص�����
  * @param  htim����ʱ�����ָ��
	*	@note   ��
  * @retval ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint32_t last_axis = 0;
  
  /* ��¼��һ���Ľ������ */
  last_axis = interpolation_para.active_axis;
  
  /* ���ݽ�������ˢ������ */
  switch(last_axis)
  {
    case x_axis:
      switch(interpolation_para.dir_x)
      {
        case CCW: interpolation_para.startpoint_x--; break;
        case CW:  interpolation_para.startpoint_x++; break;
      }
      break;
    case y_axis:
      switch(interpolation_para.dir_y)
      {
        case CCW: interpolation_para.startpoint_y--; break;
        case CW:  interpolation_para.startpoint_y++; break;
      }
      break;
  }
  
  /* ������һ����ƫ��жϵĽ������򣬲�������һ����ƫ�� */
  if(interpolation_para.deviation >= 0)
  {
    /* ƫ��>=0����Բ����࣬Ӧ��Բ�ڽ���������ƫ�� */
    interpolation_para.active_axis = y_axis;
    interpolation_para.deviation -= (2 * interpolation_para.startpoint_y + 1);
  }
  else
  {
    /* ƫ��<0����Բ���ڲ࣬Ӧ��Բ�����������ƫ�� */
    interpolation_para.active_axis = x_axis;
    interpolation_para.deviation += (2 * interpolation_para.startpoint_x + 1);
  }
  
  /* ��һ���Ļ������һ���Ĳ�һ��ʱ����Ҫ���� */
  if(last_axis != interpolation_para.active_axis)
  {
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  }
  
  /* �����ܲ�����1 */
  interpolation_para.endpoint_pulse--;
  
  /* �ж��Ƿ���ɲ岹 */
  if(interpolation_para.endpoint_pulse == 0)
  {
    /* �رն�ʱ�� */
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_DISABLE);
    __HAL_TIM_MOE_DISABLE(htim);
    HAL_TIM_Base_Stop_IT(htim);
    interpolation_para.motionstatus = 0;
  }
}
