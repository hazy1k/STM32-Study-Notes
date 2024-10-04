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
#include "./stepper/bsp_circular_interpolation.h"

//Axis_TypeDef axis;
LinearInterpolation_TypeDef linear_para = {0};

/**
  * @brief  ��һ����ֱ�߲岹�˶�
  * @param  inc_x���յ�����X������
  * @param  inc_y���յ�����Y������
  * @param  speed�������ٶ�
  * @retval ��
  */
static void InterPolation_Move(uint32_t inc_x, uint32_t inc_y, uint16_t speed)
{
  /* ƫ������ */
  linear_para.deviation = 0;
  
  /* �����յ����� */
  linear_para.endpoint_x = inc_x;
  linear_para.endpoint_y = inc_y;
  /* ����������ΪX��Y��������֮�� */
  linear_para.endpoint_pulse = inc_x + inc_y;
  
  /* ��һ�������Ļ��ΪX�� */
  linear_para.active_axis = x_axis;
  /* ����ƫ�� */
  linear_para.deviation -= linear_para.endpoint_y;
  
  /* �����ٶ� */
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[x_axis].pul_channel, speed);
  __HAL_TIM_SET_COMPARE(&TIM_StepperHandle, step_motor[y_axis].pul_channel, speed);
  __HAL_TIM_SET_AUTORELOAD(&TIM_StepperHandle, speed * 2);
  
  /* ʹ������� */
  __HAL_TIM_MOE_ENABLE(&TIM_StepperHandle);
  /* ����X��Ƚ�ͨ����� */
  TIM_CCxChannelCmd(MOTOR_PUL_TIM, step_motor[linear_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  HAL_TIM_Base_Start_IT(&TIM_StepperHandle);
  
  linear_para.motionstatus = 1;
}

/**
  * @brief  ����ֱ�߲岹�˶�
  * @param  coordi_x���յ�����X������
  * @param  coordi_y���յ�����Y������
  * @param  speed�������ٶȣ���ʱ������ֵ
  * @retval ��
  */
void Linear_Interpolation(int32_t coordi_x, int32_t coordi_y, uint16_t speed)
{
  /* ���ò岹ģʽΪֱ�߲岹 */
  mode = Linear;
  
  /* �жϵ�ǰ�Ƿ��������岹�˶� */
  if(linear_para.motionstatus != 0)
    return;
  
  /* �ж������������Դ˾���������˶����� */
  if(coordi_x < 0)
  {
    linear_para.dir_x = CCW;
    coordi_x = -coordi_x;
    MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CCW);
  }
  else
  {
    linear_para.dir_x = CW;
    MOTOR_DIR(step_motor[x_axis].dir_port, step_motor[x_axis].dir_pin, CW);
  }
  
  if(coordi_y < 0)
  {
    linear_para.dir_y = CCW;
    coordi_y = -coordi_y;
    MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CCW);
  }
  else
  {
    linear_para.dir_y = CW;
    MOTOR_DIR(step_motor[y_axis].dir_port, step_motor[y_axis].dir_pin, CW);
  }
  
  /* ��ʼ�岹�˶� */
  InterPolation_Move(coordi_x, coordi_y, speed);
}

/**
  * @brief  ֱ�߲岹������
  * @param  htim����ʱ�����ָ��
	*	@note   ��ʱ���ж������
  * @retval ��
  */
void LinearInterpolation_Handler(TIM_HandleTypeDef *htim)
{
  uint32_t last_axis = 0;
  
  /* ��¼��һ���Ľ������ */
  last_axis = linear_para.active_axis;

  
  /* ������һ����ƫ��жϵĽ������򣬲�������һ����ƫ�� */
  if(linear_para.deviation >= 0)
  {
    /* ƫ��>0����ֱ���Ϸ�������X�ᣬ����ƫ�� */
    linear_para.active_axis = x_axis;
    linear_para.deviation -= linear_para.endpoint_y;
  }
  else
  {
    /* ƫ��<0����ֱ���·�������Y�ᣬ����ƫ�� */
    linear_para.active_axis = y_axis;
    linear_para.deviation += linear_para.endpoint_x;
  }
  
  /* ��һ���Ļ������һ���Ĳ�һ��ʱ����Ҫ���� */
  if(last_axis != linear_para.active_axis)
  {
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[linear_para.active_axis].pul_channel, TIM_CCx_ENABLE);
  }
  
  /* �����ܲ�����1 */
  linear_para.endpoint_pulse--;
    
  /* �ж��Ƿ���ɲ岹 */
  if(linear_para.endpoint_pulse == 0)
  {
    /* �رն�ʱ�� */
    TIM_CCxChannelCmd(htim->Instance, step_motor[last_axis].pul_channel, TIM_CCx_DISABLE);
    TIM_CCxChannelCmd(htim->Instance, step_motor[linear_para.active_axis].pul_channel, TIM_CCx_DISABLE);
    __HAL_TIM_MOE_DISABLE(htim);
    HAL_TIM_Base_Stop_IT(htim);
    linear_para.motionstatus = 0;
  }
}
