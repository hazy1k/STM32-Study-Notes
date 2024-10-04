/**
  ******************************************************************************
  * @file    bsp_stepper_liner_interpolation.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ��������˳ʱ��Բ���岹-���ȽϷ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_circular_interpolation.h"
#include "./stepper/bsp_linear_interpolation.h"
#include <stdlib.h>

InterpolationMODE_TypeDef mode;
Axis_TypeDef axis;
Quadrant_TypeDef quadrant;
CircularInterpolation_TypeDef circular_para = {0};

/**
  * @brief  ���ý�������
  * @param  coord_x
  * @param  coord_y
  * @retval ��
  */
static void Set_Feed_DIR(int32_t coord_x, int32_t coord_y, uint8_t dir)
{
  /* ��¼�岹�˶����� */
  circular_para.dir_interpo = dir;
  
  if(dir == CW)
  {
    if(coord_x > 0)/* x������ */
    {
      if(coord_y > 0)/* ��һ���� */
      {
        circular_para.crood_pos = quadrant_1st;
        circular_para.dir_x = CW;
        circular_para.dir_y = CCW;
        circular_para.devi_sign[x_axis] = 1;
        circular_para.devi_sign[y_axis] = -1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
      }
      else/* �������� */
      {
        circular_para.crood_pos = quadrant_4th;
        circular_para.dir_x = CCW;
        circular_para.dir_y = CCW;
        circular_para.devi_sign[x_axis] = -1;
        circular_para.devi_sign[y_axis] = -1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
      }
    }
    else if(coord_x < 0)/* x������ */
    {
      if(coord_y >= 0)/* �ڶ����� */
      {
        circular_para.crood_pos = quadrant_2nd;
        circular_para.dir_x = CW;
        circular_para.dir_y = CW;
        circular_para.devi_sign[x_axis] = 1;
        circular_para.devi_sign[y_axis] = 1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
      }
      else/* �������� */
      {
        circular_para.crood_pos = quadrant_3rd;
        circular_para.dir_x = CCW;
        circular_para.dir_y = CW;
        circular_para.devi_sign[x_axis] = -1;
        circular_para.devi_sign[y_axis] = 1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
      }
    }
    else if(coord_x == 0)/* x=0����ǰ����Y���� */
    {
      if(coord_y > 0)/* ��һ���� */
      {
        circular_para.crood_pos = quadrant_1st;
        circular_para.dir_x = CW;
        circular_para.dir_y = CCW;
        circular_para.devi_sign[x_axis] = 1;
        circular_para.devi_sign[y_axis] = -1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
      }
      else if(coord_y < 0)/* �������� */
      {
        circular_para.crood_pos = quadrant_3rd;
        circular_para.dir_x = CCW;
        circular_para.dir_y = CW;
        circular_para.devi_sign[x_axis] = -1;
        circular_para.devi_sign[y_axis] = 1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
      }
    }
  }
  else
  {
    if(coord_x > 0)/* x������ */
    {
      if(coord_y >= 0)/* ��һ���� */
      {
        circular_para.crood_pos = quadrant_1st;
        circular_para.dir_x = CCW;
        circular_para.dir_y = CW;
        circular_para.devi_sign[x_axis] = -1;
        circular_para.devi_sign[y_axis] = 1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
      }
      else/* �������� */
      {
        circular_para.crood_pos = quadrant_4th;
        circular_para.dir_x = CW;
        circular_para.dir_y = CW;
        circular_para.devi_sign[x_axis] = 1;
        circular_para.devi_sign[y_axis] = 1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
      }
    }
    else if(coord_x < 0)/* x������ */
    {
      if(coord_y > 0)/* �ڶ����� */
      {
        circular_para.crood_pos = quadrant_2nd;
        circular_para.dir_x = CCW;
        circular_para.dir_y = CCW;
        circular_para.devi_sign[x_axis] = -1;
        circular_para.devi_sign[y_axis] = -1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
      }
      else/* �������� */
      {
        circular_para.crood_pos = quadrant_3rd;
        circular_para.dir_x = CW;
        circular_para.dir_y = CCW;
        circular_para.devi_sign[x_axis] = 1;
        circular_para.devi_sign[y_axis] = -1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
      }
    }
    else if(coord_x == 0)/* x=0����ǰ����Y���� */
    {
      if(coord_y > 0)/* �ڶ����� */
      {
        circular_para.crood_pos = quadrant_2nd;
        circular_para.dir_x = CCW;
        circular_para.dir_y = CCW;
        circular_para.devi_sign[x_axis] = -1;
        circular_para.devi_sign[y_axis] = -1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
      }
      else if(coord_y < 0)/* �������� */
      {
        circular_para.crood_pos = quadrant_4th;
        circular_para.dir_x = CW;
        circular_para.dir_y = CW;
        circular_para.devi_sign[x_axis] = 1;
        circular_para.devi_sign[y_axis] = 1;
        MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
        MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
      }
    }
  }
}
  
/**
  * @brief  ��������˳Բ�岹�˶�
  * @param  start_x��Բ���������X
  * @param  start_y��Բ���������Y
  * @param  stop_x��Բ���յ�����X
  * @param  stop_y��Բ���յ�����Y
  * @param  speed�������ٶ�
  * @param  dir����������
  * @retval ��
  */
void Circular_InterPolation(int32_t start_x, int32_t start_y, int32_t stop_x, int32_t stop_y, uint16_t speed, uint8_t dir)
{
  /* ���ò岹ģʽΪֱ�߲岹 */
  mode = Circular;
  
  /* �жϵ�ǰ�Ƿ��������岹�˶� */
  if(circular_para.motionstatus != 0)
    return;
  
  /* �����㡢�յ������Ƿ���ͬһ��Բ�� */
  if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y)))
    return;
  
  /* ƫ������ */
  circular_para.deviation = 0;
  
  /* ������� */
  circular_para.startpoint[x_axis] = start_x;
  circular_para.startpoint[y_axis] = start_y;
  /* �յ����� */
  circular_para.endpoint_x = stop_x;
  circular_para.endpoint_y = stop_y;
  /* �����������Ǵ���㵽�յ��������֮�� */
  circular_para.endpoint_pulse = abs(stop_x - start_x) + abs(stop_y - start_y);
  
  /* ��������ȷ���岹�����X��Y�˶����� */
  Set_Feed_DIR(circular_para.startpoint[x_axis], circular_para.startpoint[y_axis], dir);
  
  /* �������x=0��˵�������y���ϣ�ֱ����x������ɼ�С��� */
  if(circular_para.startpoint[x_axis] == 0)
  {
    /* ƫ��̣�F = F �� 2 * x + 1*/
    circular_para.active_axis = x_axis;
    circular_para.deviation += 2 * circular_para.devi_sign[x_axis]
                                 * circular_para.startpoint[x_axis] + 1;
  }
  else
  {
    /* ƫ��̣�F = F �� 2 * y + 1*/
    circular_para.active_axis = y_axis;
    circular_para.deviation += 2 * circular_para.devi_sign[y_axis]
                                 * circular_para.startpoint[y_axis] + 1;
  }
  
  /* �����ٶ� */
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[x_axis].pul_channel, speed);
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[y_axis].pul_channel, speed);
  __HAL_TIM_SET_AUTORELOAD(&TIM_StepperHandle, speed * 2);
  
  /* ʹ������� */
  __HAL_TIM_MOE_ENABLE(&TIM_StepperHandle);
  /* �������Ƚ�ͨ����� */
  TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[circular_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  HAL_TIM_Base_Start_IT(&TIM_StepperHandle);
  
  circular_para.motionstatus = 1;
}

/**
  * @brief  Բ���岹������
  * @param  htim����ʱ�����ָ��
	*	@note   ֱ�߲岹��Բ���岹����һ���ص�����
  * @retval ��
  */
void CircularInterpolation_Handler(TIM_HandleTypeDef *htim)
{
  uint32_t last_axis = 0;
  
  /* ��¼��һ���Ľ������ */
  last_axis = circular_para.active_axis;
  
  /* ���ݽ�������ˢ������ */
  switch(last_axis)
  {
    case x_axis:
      switch(circular_para.dir_x)
      {
        case CCW: circular_para.startpoint[x_axis]--; break;
        case CW:  circular_para.startpoint[x_axis]++; break;
      }
      break;
    case y_axis:
      switch(circular_para.dir_y)
      {
        case CCW: circular_para.startpoint[y_axis]--; break;
        case CW:  circular_para.startpoint[y_axis]++; break;
      }
      break;
  }
  
  /* ������һ�ν�����ƫ��ж��µĽ������ */
  if(circular_para.deviation >= 0)
  {
    switch(circular_para.dir_interpo)
    {
      case CW:/* ˳ʱ�� */
        switch(circular_para.crood_pos)
        {
          case quadrant_1st:
          case quadrant_3rd:
            circular_para.active_axis = y_axis;
            break;
          case quadrant_2nd:
          case quadrant_4th:
            circular_para.active_axis = x_axis;
            break;
        }
        break;
      case CCW:/* ��ʱ�� */
        switch(circular_para.crood_pos)
        {
          case quadrant_1st:
          case quadrant_3rd:
            circular_para.active_axis = x_axis;
            break;
          case quadrant_2nd:
          case quadrant_4th:
            circular_para.active_axis = y_axis;
            break;
        }
        break;
    }
  }
  else /* ƫ��С��0����Բ����� */
  {
    switch(circular_para.dir_interpo)
    {
      case CW:/* ˳ʱ�� */
        switch(circular_para.crood_pos)
        {
          case quadrant_1st:
          case quadrant_3rd:
            circular_para.active_axis = x_axis;
            break;
          case quadrant_2nd:
          case quadrant_4th:
            circular_para.active_axis = y_axis;
            break;
        }
        break;
      case CCW:/* ��ʱ�� */
        switch(circular_para.crood_pos)
        {
          case quadrant_1st:
          case quadrant_3rd:
            circular_para.active_axis = y_axis;
            break;
          case quadrant_2nd:
          case quadrant_4th:
            circular_para.active_axis = x_axis;
            break;
        }
        break;
    }
  }
  /* ���ݲ岹�˶�����ͽ������������µ�ƫ�� */
  circular_para.deviation += 2 * circular_para.devi_sign[circular_para.active_axis]
                               * circular_para.startpoint[circular_para.active_axis] + 1;
  
  /* ��һ���Ļ������һ���Ĳ�һ��ʱ����Ҫ���� */
  if(last_axis != circular_para.active_axis)
  {
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[circular_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  }
  
  /* �����ܲ�����1 */
  circular_para.endpoint_pulse--;
  
  /* �ж��Ƿ���ɲ岹 */
  if(circular_para.endpoint_pulse == 0)
  {
    /* �رն�ʱ�� */
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[circular_para.active_axis].pul_channel, TIM_CCx_DISABLE);
    __HAL_TIM_MOE_DISABLE(htim);
    HAL_TIM_Base_Stop_IT(htim);
    circular_para.motionstatus = 0;
  }
}

/**
  * @brief  ��ʱ���жϻص�����
  * @param  htim����ʱ�����ָ��
	*	@note   ֱ�߲岹��Բ���岹����һ���ص�����
  * @retval ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  switch(mode)
  {
    case Circular:
      CircularInterpolation_Handler(htim);
      break;
    case Linear:
      LinearInterpolation_Handler(htim);
      break;
  }
}
