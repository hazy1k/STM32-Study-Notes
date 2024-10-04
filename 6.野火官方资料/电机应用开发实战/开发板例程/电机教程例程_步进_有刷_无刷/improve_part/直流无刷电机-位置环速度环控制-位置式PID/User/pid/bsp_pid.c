#include "./pid/bsp_pid.h"
#include ".\bldcm_control\bsp_bldcm_control.h"

//����ȫ�ֱ���

_pid pid_location;
_pid pid_speed;

/**
  * @brief  PID������ʼ��
	*	@note 	��
  * @retval ��
  */
void PID_param_init()
{
	/* λ����س�ʼ������ */
  pid_location.target_val=0.0;				
  pid_location.actual_val=0.0;
  pid_location.err=0.0;
  pid_location.err_last=0.0;
  pid_location.integral=0.0;

  pid_location.Kp = 104;
  pid_location.Ki = 0;
  pid_location.Kd = 120;
	
	/* �ٶ���س�ʼ������ */
	pid_speed.target_val=0.0;				
	pid_speed.actual_val=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.integral=0.0;

	pid_speed.Kp = 0.062;
	pid_speed.Ki = 0.011;
	pid_speed.Kd = 0.0;


#if defined(PID_ASSISTANT_EN)
  float pid_temp[3] = {pid_location.Kp, pid_location.Ki, pid_location.Kd};
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // ��ͨ�� 1 ���� P I D ֵ
	pid_temp[0] = pid_speed.Kp;
	pid_temp[1] = pid_speed.Ki;
	pid_temp[2] = pid_speed.Kd;
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, pid_temp, 3);     // ��ͨ�� 2 ���� P I D ֵ
#endif
}

/**
  * @brief  ����Ŀ��ֵ
  * @param  val		Ŀ��ֵ
	*	@note 	��
  * @retval ��
  */
void set_pid_target(_pid *pid, float temp_val)
{ 
  pid->target_val = temp_val;    // ���õ�ǰ��Ŀ��ֵ
}

/**
  * @brief  ��ȡĿ��ֵ
  * @param  ��
	*	@note 	��
  * @retval Ŀ��ֵ
  */
float get_pid_target(_pid *pid)
{
  return pid->target_val;    // ���õ�ǰ��Ŀ��ֵ
}
/**
  * @brief  ���ñ��������֡�΢��ϵ��
  * @param  p������ϵ�� P
  * @param  i������ϵ�� i
  * @param  d��΢��ϵ�� d
	*	@note 	��
  * @retval ��
  */
void set_p_i_d(_pid *pid, float p, float i, float d)
{
  	pid->Kp = p;    // ���ñ���ϵ�� P
		pid->Ki = i;    // ���û���ϵ�� I
		pid->Kd = d;    // ����΢��ϵ�� D
}

/**
  * @brief  λ��PID�㷨ʵ��
  * @param  actual_val:ʵ��ֵ
	*	@note 	��
  * @retval ͨ��PID���������
  */
float location_pid_realize(_pid *pid, float actual_val)
{
		/*����Ŀ��ֵ��ʵ��ֵ�����*/
    pid->err=pid->target_val-actual_val;
    pid->integral += pid->err;    // ����ۻ�

	  if((pid->err<1.0f )&& (pid->err>-1.0f))
		{
			pid->err = 0.0f;
		}	

		/*PID�㷨ʵ��*/
    pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);
  
		/*����*/
    pid->err_last=pid->err;
    
		/*���ص�ǰʵ��ֵ*/
    return pid->actual_val;
}

/**
  * @brief  �ٶ�PID�㷨ʵ��
  * @param  actual_val:ʵ��ֵ
	*	@note 	��
  * @retval ͨ��PID���������
  */
float speed_pid_realize(_pid *pid, float actual_val)
{
		/*����Ŀ��ֵ��ʵ��ֵ�����*/
    pid->err=pid->target_val-actual_val;

    if((pid->err<41.5f )&& (pid->err>-41.5f))
      pid->err = 0.0f;
		

    pid->integral += pid->err;    // ����ۻ�

		/*PID�㷨ʵ��*/
    pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);
  
		/*����*/
    pid->err_last=pid->err;
    
		/*���ص�ǰʵ��ֵ*/
    return pid->actual_val;
}




