#include "./pid/bsp_pid.h"
#include "math.h"
#include "./key/bsp_key.h" 
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

		pid.target_val=0.0;				
		pid.actual_val=0.0;
		pid.err = 0.0;
		pid.err_last = 0.0;
		pid.err_next = 0.0;
		
//		pid.Kp = 0.20;
//		pid.Ki = 0.80;
//		pid.Kd = 0.01;
	
		pid.Kp = 0.05;
		pid.Ki = 0.10;
		pid.Kd = 0.01;
#if defined(PID_ASSISTANT_EN)
		float pid_temp[3] = {pid.Kp, pid.Ki, pid.Kd};
		set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // ��ͨ�� 1 ���� P I D ֵ
#endif
}
/**
  * @brief  PID�㷨ʵ��
  * @param  val		Ŀ��ֵ
	*	@note 	��
  * @retval ͨ��PID���������
  */
float PID_realize(float temp_val) 
{
	/*����Ŀ��ֵ��ʵ��ֵ�����*/
	pid.err=pid.target_val-temp_val;
	/*PID�㷨ʵ��*/
	float increment_val = pid.Kp*(pid.err - pid.err_next) + pid.Ki*pid.err + pid.Kd*(pid.err - 2 * pid.err_next + pid.err_last);
	/*�ۼ�*/
	pid.actual_val += increment_val;
	/*�������*/
	pid.err_last = pid.err_next;
	pid.err_next = pid.err;
	/*���ص�ǰʵ��ֵ*/
	return pid.actual_val;
	
}

/**
  * @brief  ����Ŀ��ֵ
  * @param  val		Ŀ��ֵ
  * @note 	��
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
  * @brief  ��ʱ�����ڵ��ú���
  * @param  ��
  *	@note 	��
  * @retval ��
  */
float set_point=0.0;
int pid_status=0;
void time_period_fun()
{
			
	if(!pid_status)
	{
		float val=PID_realize(pid.actual_val);
    
		int temp = val;    // ��λ����Ҫ����������ת��һ��
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &temp, 1);                // ��ͨ�� 1 ����ʵ��ֵ
//		printf("val,%f;act,%f\n",set_point,val);	
	}
}







