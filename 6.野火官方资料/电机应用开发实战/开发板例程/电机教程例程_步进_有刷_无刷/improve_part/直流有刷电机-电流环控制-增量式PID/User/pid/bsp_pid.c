#include "./pid/bsp_pid.h"

//����ȫ�ֱ���

_pid pid;

/**
  * @brief  PID������ʼ��
	*	@note 	��
  * @retval ��
  */
void PID_param_init()
{
		/* ��ʼ������ */
    pid.target_val=80;				
    pid.actual_val=0.0;
		pid.err = 0.0;
		pid.err_last = 0.0;
		pid.err_next = 0.0;
		
		pid.Kp = 0;
		pid.Ki = 2.8;
		pid.Kd = 0;

#if defined(PID_ASSISTANT_EN)
    float pid_temp[3] = {pid.Kp, pid.Ki, pid.Kd};
    set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // ��ͨ�� 1 ���� P I D ֵ
#endif
}

/**
  * @brief  ����Ŀ��ֵ
  * @param  val		Ŀ��ֵ
	*	@note 	��
  * @retval ��
  */
void set_pid_target(float temp_val)
{
  pid.target_val = temp_val;    // ���õ�ǰ��Ŀ��ֵ
}

/**
  * @brief  ��ȡĿ��ֵ
  * @param  ��
	*	@note 	��
  * @retval Ŀ��ֵ
  */
float get_pid_target(void)
{
  return pid.target_val;    // ���õ�ǰ��Ŀ��ֵ
}

/**
  * @brief  ���ñ��������֡�΢��ϵ��
  * @param  p������ϵ�� P
  * @param  i������ϵ�� i
  * @param  d��΢��ϵ�� d
	*	@note 	��
  * @retval ��
  */
void set_p_i_d(float p, float i, float d)
{
  	pid.Kp = p;    // ���ñ���ϵ�� P
		pid.Ki = i;    // ���û���ϵ�� I
		pid.Kd = d;    // ����΢��ϵ�� D
}

/**
  * @brief  PID�㷨ʵ��
  * @param  actual_val:ʵ��ֵ
	*	@note 	��
  * @retval ͨ��PID���������
  */
float PID_realize(float actual_val)
{
	/*����Ŀ��ֵ��ʵ��ֵ�����*/
  pid.err=pid.target_val-actual_val;
//	/* �����趨,������������,�����5ma�Ͳ��ۼ���� */
//	if(pid.err <= 5 && pid.err >= -5) 
//		pid.err = 0;
	/*PID�㷨ʵ��*/
	pid.actual_val += pid.Kp*(pid.err - pid.err_next) 
                 + pid.Ki*pid.err 
                 + pid.Kd*(pid.err - 2 * pid.err_next + pid.err_last);
	
	/* �޷����,��ֹ��ȱ��� */
	if(pid.actual_val > 3600) pid.actual_val = 3600;
	else if(pid.actual_val <= 0) pid.actual_val = 0;
	
	/*�������*/
	pid.err_last = pid.err_next;
	pid.err_next = pid.err;
	/*���ص�ǰʵ��ֵ*/
	return pid.actual_val;
}
