/**
  ******************************************************************************
  * @file    bsp_stepper_liner_interpolation.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ��һ����ֱ�߲岹-���ȽϷ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_linear_interpolation.h"

Axis_TypeDef axis;
LinearInterpolation_TypeDef interpolation_para = {0};

/**
  * @brief  ֱ�������岹�˶�
  * @param  inc_x���յ�����X������
  * @param  inc_y���յ�����Y������
  * @param  speed�������ٶ�
  * @retval ��
  */
void InterPolation_Move(uint32_t inc_x, uint32_t inc_y, uint16_t speed)
{
  /* ƫ������ */
  interpolation_para.deviation = 0;
  
  /* �����յ����� */
  interpolation_para.endpoint_x = inc_x;
  interpolation_para.endpoint_y = inc_y;
  /* ����������ΪX��Y��������֮�� */
  interpolation_para.endpoint_pulse = inc_x + inc_y;
  
  /* ��һ�������Ļ��ΪX�� */
  interpolation_para.active_axis = x_axis;
  /* ����ƫ�� */
  interpolation_para.deviation -= interpolation_para.endpoint_y;
  
  /* �����ٶ� */
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[x_axis].pul_channel, speed);
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[y_axis].pul_channel, speed);
  __HAL_TIM_SET_AUTORELOAD(&TIM_StepperHandle, speed * 2);
  
  /* ʹ������� */
  __HAL_TIM_MOE_ENABLE(&TIM_StepperHandle);
  /* ����X��Ƚ�ͨ����� */
  TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[interpolation_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  HAL_TIM_Base_Start_IT(&TIM_StepperHandle);
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
  
  /* ������һ����ƫ��жϵĽ������򣬲�������һ����ƫ�� */
  if(interpolation_para.deviation >= 0)
  {
    /* ƫ��>0����ֱ���Ϸ�������X�ᣬ����ƫ�� */
    interpolation_para.active_axis = x_axis;
    interpolation_para.deviation -= interpolation_para.endpoint_y;
  }
  else
  {
    /* ƫ��<0����ֱ���·�������Y�ᣬ����ƫ�� */
    interpolation_para.active_axis = y_axis;
    interpolation_para.deviation += interpolation_para.endpoint_x;
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
  }
}
