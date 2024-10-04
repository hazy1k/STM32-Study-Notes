#include "./pid/bsp_pid.h"
#include "math.h"
#include "./key/bsp_key.h" 
#include "./protocol/protocol.h"

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
//    printf("PID_init begin \n");
    pid.target_val=200.0;				
    pid.actual_val=0.0;
    pid.err=0.0;
    pid.err_last=0.0;
    pid.integral=0.0;
//		pid.Kp = 0.31;
//		pid.Ki = 0.070;
//		pid.Kd = 0.3;
//		pid.Kp = 0.21;
//		pid.Ki = 0.070;
//		pid.Kd = 0.3;
		pid.Kp = 0.01;//24
		pid.Ki = 0.80;
		pid.Kd = 0.04;

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
  * @param  val		ʵ��ֵ
  * @note 	��
  * @retval ͨ��PID���������
  */
float PID_realize(float temp_val)
{
	/*����Ŀ��ֵ��ʵ��ֵ�����*/
    pid.err=pid.target_val-temp_val;
	/*����ۻ�*/
    pid.integral+=pid.err;
	/*PID�㷨ʵ��*/
    pid.actual_val=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
	/*����*/
    pid.err_last=pid.err;
	/*���ص�ǰʵ��ֵ*/
    return pid.actual_val;
}
/**
  * @brief  ��ʱ�����ڵ��ú���
  * @param  ��
	*@note 	��
  * @retval ��
  */
void time_period_fun()
{
	static int flag=0;
	static int num=0;
	static int run_i=0;
		
	if(!flag)
	{
		float val=PID_realize(pid.actual_val);
		printf("val,%f;act,%f\n",pid.target_val,val);	
		run_i++;
		if(abs(val-pid.target_val)<=1)
		{
			num++;
		}
		else//����������������
		{
			num=0;
		}
		if(num>20)//�ȶ�����
		{
			printf("PID�㷨����%d �κ��ȶ�\r\n",run_i);
			flag=1;
		}
	}
}







