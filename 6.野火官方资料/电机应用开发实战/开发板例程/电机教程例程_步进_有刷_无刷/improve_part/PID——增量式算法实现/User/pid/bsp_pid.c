#include "./pid/bsp_pid.h"
#include "math.h"
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
    printf("PID_init begin \n");
    pid.target_val=0.0;				
    pid.actual_val=0.0;
		pid.err = 0.0;
		pid.err_last = 0.0;
		pid.err_next = 0.0;
//		pid.Kp = 0.21;
//		pid.Ki = 0.070;
//		pid.Kd = 0.32;
		
		pid.Kp = 0.20;
		pid.Ki = 0.80;
		pid.Kd = 0.01;
    printf("PID_init end \n");

}
/**
  * @brief  PID�㷨ʵ��
  * @param  val		Ŀ��ֵ
	*	@note 	��
  * @retval ͨ��PID���������
  */
float PID_realize(float temp_val) 
{
	/*����Ŀ��ֵ*/
	pid.target_val = temp_val;
	/*����Ŀ��ֵ��ʵ��ֵ�����*/
  pid.err=pid.target_val-pid.actual_val;
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
  * @brief  ��ʱ�����ڵ��ú���
  * @param  ��
	*	@note 	��
  * @retval ��
  */
void time_period_fun()
{
	static int flag=0;
	static int num=0;
	static int run_i=0;
		
	float set_point=200.0;
	if(!flag)
	{
		float val=PID_realize(set_point);
		printf("val,%f;act,%f\n",set_point,val);	
		run_i++;
		__IO int abs_val=val-set_point;
		if(abs(val-set_point)<=1)
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
