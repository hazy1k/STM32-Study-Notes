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

extern _pid speed_pid,move_pid;
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
  * @brief  �������λ���ٶ�˫�ջ�����
  * @retval ��
  * @note   ������ʱ���ж��ڵ���
  */
void Stepper_Ctrl(void)
{
  /* ��������ر��� */
  static __IO float last_count = 0;
  __IO float capture_count = 0;
  __IO float capture_per_unit = 0;
  /* ����pid����������ֵ */
  static __IO float speed_cont_val = 0.0f;
  static __IO float move_cont_val = 0.0f;  
  static int cont_val = 0;  
	
  /* ������˶�ʱ������pid���� */
  if((sys_status.MSD_ENA == 1) && (sys_status.stepper_running == 1))
  {
    /* ��������������� */
    capture_count = (int)__HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (encoder_overflow_count * ENCODER_TIM_PERIOD);
		/* �����ٶȻ��Ĵ���ֵ */
		capture_per_unit = capture_count - last_count;
    last_count = capture_count;
		
		/* �����������ۼ�ֵ��Ϊʵ��ֵ����λ�û�pid������ */
    move_cont_val = PID_realize(&move_pid, (float)capture_count);// ���� PID ����
    /* �ж��˶����� */
    move_cont_val > 0 ? (MOTOR_DIR(CW)) : (MOTOR_DIR(CCW));
		/* �ж��Ƿ������ٶȻ� */
		if (fabsf(move_cont_val) >= MOVE_CTRL) 
		{
			/* ����λ�û�����ֵ�����ڼ���*/
			cont_val = move_cont_val;
			
			/* Ŀ���ٶ����޴��� */
			if (cont_val > TARGET_SPEED_MAX)
			{
				cont_val = TARGET_SPEED_MAX;
			}
			else if (cont_val < -TARGET_SPEED_MAX)
			{
				cont_val = -TARGET_SPEED_MAX;
			}
			
#if defined(PID_ASSISTANT_EN)
			int32_t temp = cont_val;
			set_computer_value(SEED_TARGET_CMD, CURVES_CH2, &temp, 1);     // ��ͨ�� 2 ����Ŀ��ֵ
#endif
			/* �趨�ٶȵ�Ŀ��ֵ */
			set_pid_target(&speed_pid, cont_val);    
			/* ��λʱ���ڵı�������������Ϊʵ��ֵ�����ٶȻ�pid������ */
			speed_cont_val = PID_realize(&speed_pid, (float)capture_per_unit);// ���� PID ����
			/* ����OC_Pulse_numΪuint16_t������ȡ�ٶȻ����ֵ�ľ���ֵ���к�������*/
			cont_val = fabsf(speed_cont_val);	
			/* ����Ƚϼ�������ֵ */
			OC_Pulse_num = ((uint16_t)(TIM_STEP_FREQ / (cont_val * PULSE_RATIO * SAMPLING_PERIOD))) >> 1;
		} 
		else
		{
			/* ����Ƚϼ�������ֵ */
			OC_Pulse_num = ((uint16_t)(TIM_STEP_FREQ / ((float)move_cont_val * PULSE_RATIO))) >> 1;
		}
#if PID_ASSISTANT_EN
     int Temp_ch2 = capture_per_unit;    // ��λ����Ҫ����������ת��һ��
		 int Temp_ch1 = capture_count;
     set_computer_value(SEED_FACT_CMD, CURVES_CH2, &Temp_ch2, 1);  // ��ͨ�� 1 ����ʵ��ֵ     // ��ͨ�� 2 ����ʵ��ֵ
     set_computer_value(SEED_FACT_CMD, CURVES_CH1, &Temp_ch1, 1);     // ��ͨ�� 1 ����ʵ��ֵ

#else
     printf("ʵ��ֵ��%d��Ŀ��ֵ��%.0f\r\n", capture_per_unit, pid.target_val);// ��ӡʵ��ֵ��Ŀ��ֵ 
#endif
  }
  else
  {
    /*ͣ��״̬���в�������*/
    last_count = 0;
    speed_cont_val = 0;
    move_cont_val = 0;
    speed_pid.actual_val = 0;
    speed_pid.err = 0;
    speed_pid.err_last = 0;
		move_pid.actual_val = 0;
    move_pid.err = 0;
    move_pid.err_last = 0;
  }
}
