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

#include "./stepper/bsp_stepper_ctrl.h"
#include "./pid/bsp_pid.h"

extern _pid pid;
extern __IO uint16_t OC_Pulse_num;     //�Ƚ�����ļ���ֵ

/* ϵͳ״̬��ʼ�� */
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
  * @brief  ��������ٶȱջ�����
  * @retval ��
  * @note   ������ʱ���ж��ڵ���
  */
void Stepper_Speed_Ctrl(void)
{
  /* ��������ر��� */
  static __IO int32_t last_count = 0;
  __IO int32_t capture_count = 0;
  __IO int32_t capture_per_unit = 0;
  /* ����pid����������ֵ */
  static __IO float cont_val = 0.0f;
  
  __IO float timer_delay = 0.0f;
  
  /* ������˶�ʱ������pid���� */
  if((sys_status.MSD_ENA == 1) && (sys_status.stepper_running == 1))
  {
    /* ���㵥������ʱ���ڵı����������� */
    capture_count =(int)__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (encoder_overflow_count * ENCODER_TIM_PERIOD);
    capture_per_unit = capture_count - last_count;
    last_count = capture_count;
    
    /* ��λʱ���ڵı�������������Ϊʵ��ֵ����pid������ */
    cont_val += PID_realize((float)capture_per_unit);// ���� PID ����
    
    /* �ж��ٶȷ��� */
    cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));
    
    /* ����ó�������ֵȡ����ֵ */
    timer_delay = fabsf(cont_val);
    
    /* ����Ƚϼ�������ֵ */
    OC_Pulse_num = ((uint16_t)(TIM_STEP_FREQ / (timer_delay * PULSE_RATIO * SAMPLING_PERIOD))) >> 1;
    
    #if PID_ASSISTANT_EN
     int Temp = capture_per_unit;    // ��λ����Ҫ����������ת��һ��
     set_computer_value(SEED_FACT_CMD, CURVES_CH1, &Temp, 1);  // ��ͨ�� 1 ����ʵ��ֵ
    #else
     printf("ʵ��ֵ��%d��Ŀ��ֵ��%.0f\r\n", capture_per_unit, pid.target_val);// ��ӡʵ��ֵ��Ŀ��ֵ 
    #endif
  }
  else
  {
    /*ͣ��״̬���в�������*/
//    OC_Pulse_num = 0xFFFF;
    last_count = 0;
    cont_val = 0;
    pid.actual_val = 0;
    pid.err = 0;
    pid.err_last = 0;
    pid.err_next = 0;
  }
}
