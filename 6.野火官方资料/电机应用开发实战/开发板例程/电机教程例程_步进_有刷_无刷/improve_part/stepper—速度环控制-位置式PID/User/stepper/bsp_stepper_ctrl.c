/**
  ******************************************************************************
  * @file    bsp_stepper_init.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ���������ʼ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "math.h"
#include "./tim/bsp_basic_tim.h"
#include "./stepper/bsp_stepper_ctrl.h"
#include "./pid/bsp_pid.h"

extern _pid pid;
extern __IO uint16_t OC_Pulse_num;     //�Ƚ�����ļ���ֵ

__SYS_STATUS sys_status = {0};

/**
  * @brief  ����������ֹͣ
  * @param  NewState��ʹ�ܻ��߽�ֹ
  * @retval ��
  */
void MSD_ENA(int NewState)
{
    if(NewState)
    {
      //ENAʧ�ܣ���ֹ��������������ѻ�״̬����ʱ���Ϊ�ޱ�������״̬�������ֶ���ת���
      MOTOR_EN(OFF);
      sys_status.MSD_ENA = 0;			

    }
    else
    {
      //ENAʹ�ܣ���ʱ���Ϊ��������״̬
      MOTOR_EN(ON);
      sys_status.MSD_ENA = 1;			
    }
}

/**
  * @brief  �������ɲ��
  * @param  ��
  * @retval ��
  */
void Set_Stepper_Stop(void)
{
  /*ʧ�ܱȽ�ͨ��*/
	TIM_CCxChannelCmd(MOTOR_PUL_TIM,MOTOR_PUL_CHANNEL_x,TIM_CCx_DISABLE);
  sys_status.stepper_running = 0;
}

/**
  * @brief  �����������
  * @param  ��
  * @retval ��
  */
void Set_Stepper_Start(void)
{
  /*ʹ��������*/
  MSD_ENA(0);
  /*ʹ�ܱȽ�ͨ�����*/
	TIM_CCxChannelCmd(MOTOR_PUL_TIM,MOTOR_PUL_CHANNEL_x,TIM_CCx_ENABLE);
  sys_status.MSD_ENA = 1;
  sys_status.stepper_running = 1;
}

/**
  * @brief  �������λ��ʽPID����
  * @retval ��
  * @note   ������ʱ���ж��ڵ���
  */
void Stepper_Speed_Ctrl(void)
{
  /* ��������ر��� */
  __IO int16_t capture_per_unit = 0;
  __IO int32_t capture_count = 0;
  static __IO int32_t last_count = 0;
  
  /* ����pid����������ֵ */
  __IO float cont_val = 0;

  /* ������˶�ʱ������pid���� */
  if((sys_status.MSD_ENA == 1) && (sys_status.stepper_running == 1))
  {
    /* ���㵥������ʱ���ڵı����������� */
    capture_count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    capture_per_unit = capture_count - last_count;
    last_count = capture_count;
    
    /* ��λʱ���ڵı�������������Ϊʵ��ֵ����pid������ */
    cont_val = PID_realize((float)capture_per_unit);// ���� PID ����
    
    /* �ж��ٶȷ��� */
    cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));

    /* �Լ���ó�������ֵȡ����ֵ */
    cont_val = fabsf(cont_val);
    /* ����Ƚϼ�������ֵ */
    OC_Pulse_num = ((uint16_t)(T1_FREQ / (cont_val * PULSE_RATIO * SAMPLING_PERIOD))) >> 1;

   #if PID_ASSISTANT_EN
    int temp = capture_per_unit;
    set_computer_value(SEED_FACT_CMD, CURVES_CH1, &temp, 1);  // ��ͨ�� 1 ����ʵ��ֵ
   #else
    printf("ʵ��ֵ��%d��Ŀ��ֵ��%.0f\r\n", capture_per_unit, pid.target_val);// ��ӡʵ��ֵ��Ŀ��ֵ
   #endif
  }
  else
  {
    capture_per_unit = 0;
    cont_val = 0;
    pid.actual_val = 0;
    pid.err = 0;
    pid.err_last = 0;
    pid.integral = 0;
  }
}
