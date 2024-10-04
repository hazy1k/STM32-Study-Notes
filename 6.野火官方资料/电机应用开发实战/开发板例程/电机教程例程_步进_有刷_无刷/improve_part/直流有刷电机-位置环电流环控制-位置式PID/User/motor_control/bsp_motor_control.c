/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ������ƽӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include ".\motor_control\bsp_motor_control.h"
#include "./usart/bsp_debug_usart.h"
#include <math.h>
#include <stdlib.h>
#include "./Encoder/bsp_encoder.h"
#include "./adc/bsp_adc.h"
#include "./tim/bsp_basic_tim.h"

static motor_dir_t direction  = MOTOR_FWD;     // ��¼����
static uint16_t    dutyfactor = 0;             // ��¼ռ�ձ�
static uint8_t    is_motor_en = 0;             // ���ʹ��

#define TARGET_CURRENT_MAX    130    // Ŀ����������ֵ mA

static void sd_gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
	SHUTDOWN_GPIO_CLK_ENABLE();
  
  /* ����IO��ʼ�� */
	/*�����������*/
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	/*������������ */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*ѡ��Ҫ���Ƶ�GPIO����*/	
	GPIO_InitStruct.Pin = SHUTDOWN_PIN;
  
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
  HAL_GPIO_Init(SHUTDOWN_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  �����ʼ��
  * @param  ��
  * @retval ��
  */
void motor_init(void)
{
  Motor_TIMx_Configuration();     // ��ʼ����� 1
  sd_gpio_config();
}

/**
  * @brief  ���õ���ٶ�
  * @param  v: �ٶȣ�ռ�ձȣ�
  * @retval ��
  */
void set_motor_speed(uint16_t v)
{
  v = (v > PWM_PERIOD_COUNT) ? PWM_PERIOD_COUNT : v;     // ���޴���
  
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // �����ٶ�
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // �����ٶ�
  }
}

/**
  * @brief  ���õ������
  * @param  ��
  * @retval ��
  */
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;

  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // �����ٶ�
    SET_REV_COMPAER(0);              // �����ٶ�
  }
  else
  {
    SET_FWD_COMPAER(0);              // �����ٶ�
    SET_REV_COMPAER(dutyfactor);     // �����ٶ�
  }
}

/**
  * @brief  ʹ�ܵ��
  * @param  ��
  * @retval ��
  */
void set_motor_enable(void)
{
  is_motor_en = 1;	
  MOTOR_ENABLE_SD();
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

/**
  * @brief  ���õ��
  * @param  ��
  * @retval ��
  */
void set_motor_disable(void)
{
  is_motor_en = 0;
  MOTOR_DISABLE_SD();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

/**
  * @brief  ���λ��ʽ PID ����ʵ��(��ʱ����)
  * @param  ��
  * @retval ��
  */
void motor_pid_control(void)
{
  static uint32_t location_timer = 0;         // λ�û�����
  int32_t actual_current = get_curr_val();    // ��ȡ��ǰ����ֵ
  
  if (is_motor_en == 1)                  // �����ʹ��״̬�²Ž��п��ƴ���
  {
    static int32_t Capture_Count = 0;    // ��ǰʱ���ܼ���ֵ
    float cont_val = 0;                  // ��ǰ����ֵ
    
    /* ��ǰʱ���ܼ���ֵ = ������ֵ + ����������� * ENCODER_TIM_PERIOD  */
    Capture_Count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    
    if (location_timer++%2 == 0)
    {
      cont_val = location_pid_realize(&pid_location, Capture_Count);    // ���� PID ����

      if (cont_val > 0)    // �жϵ������
      {
        set_motor_direction(MOTOR_FWD);
      }
      else
      {
        cont_val = -cont_val;
        set_motor_direction(MOTOR_REV);
      }
      
      cont_val = (cont_val > TARGET_CURRENT_MAX) ? TARGET_CURRENT_MAX : cont_val;    // �������޴���
      set_pid_target(&pid_curr, cont_val);    // �趨�ٶȵ�Ŀ��ֵ
      
    #if defined(PID_ASSISTANT_EN)
      int32_t temp = cont_val;
      set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &temp, 1);     // ��ͨ�� 2 ����Ŀ��ֵ
    #endif
    }
    
    cont_val = curr_pid_realize(&pid_curr, actual_current);    // ���� PID ����
    
    if (cont_val < 0)
    {
      cont_val = 0;    // ���޴���
    }
    else if (cont_val > PWM_MAX_PERIOD_COUNT)
    {
      cont_val = PWM_MAX_PERIOD_COUNT;    // �ٶ����޴���
    }

    set_motor_speed(cont_val);                                                 // ���� PWM ռ�ձ�
    
  #if defined(PID_ASSISTANT_EN)
    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &Capture_Count,  1);          // ��ͨ�� 1 ����ʵ��ֵ
//    set_computer_value(SEND_FACT_CMD, CURVES_CH2, &actual_current, 1);         // ��ͨ�� 2 ����ʵ��ֵ
  #else
    printf("ʵ��ֵ��%d. Ŀ��ֵ��%.0f. ��ֵ��%.0f. ����ֵ��%.0f\n", Capture_Count, get_pid_target(&pid_location), (float)Capture_Count - get_pid_target(&pid_location), cont_val);      // ��ӡʵ��ֵ��Ŀ��ֵ
  #endif
  }
}

/*********************************************END OF FILE**********************/

