/**
  ******************************************************************************
  * @file    bsp_stepper_S_speed.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   S�μӼ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./stepper/bsp_stepper_S_speed.h"
#include <math.h>
#include <string.h>

/*

��������õ�����ѧ�ȱ��ٵ���ѧģ�ͣ������ٶ�Ҳ��

�������ٱ仯�ģ�����׼ȷ��˵�Ǽ��ٶȵ��ȱ���ѧģ�͡�

a-t �������� ��V-t���� �뿴 �ĵ���

	|    .
 a|   /|\             
	|  / | \
	| /  |  \            
	|/___|___\_t 		
	0   t/2
		
���Ͼ��Ǽ��ٶȵ��ȱ�ģ�ͣ�Ĭ�ϳ�ʼ�ļ��ٶ�Ϊ0��

��ô�������ģ�;ͺ����׿������һЩ��Ϊ��֪�����ܣ�����

�����������ϵ�ģ�;Ϳ������ÿһʱ�̵ļ��ٶȣ������ȿ� 

0~t/2 ��� a=K*T ������KΪ���ٶ�a��б��

�����ٶȻ��־����ٶȣ����� V = a dt = K*T dt

�ã� V = K*T^2/2

���ٶȻ��־���λ�ƣ����� S = V dt = K*T^2/2 dt 

�ã� S = K*T^3/6

*/

/* �㷨��ؽṹ��������� */
SpeedCalc_TypeDef Speed;
Stepper_Typedef Stepper = {0};

/**
  * @brief  S�����ٶȱ����
  * @param  pScurve��S���߽ṹ��ָ��
  * @param  Vo�����ٶ�
  * @param  Vt��ĩ�ٶ�
  * @param  T������ʱ��
  * @retval 0��������1����������
	*	@note 	��
  */
static bool CalcSpeed(int32_t Vo, int32_t Vt, float T)
{
  int32_t i = 0;
  float Vm =0.0f;                // �м���ٶ�
  float K = 0.0f;                // �Ӽ��ٶ�
  float Ti = 0.0f;               // ʱ���� dt
  float Sumt = 0.0f;             // ʱ���ۼ���
  float DeltaV = 0.0f;           // �ٶȵ�����dv
  float temp = 0.0f;             // �м����

	/***************************************************************************/
	/* �������ĩ�ٶ� */
  Speed.Vo = CONVER(Vo);
  Speed.Vt = CONVER(Vt);
  
	/***************************************************************************/
	/* �����ʼ���� */
	T = T / 2.0f;						//�Ӽ��ٶε�ʱ�䣨���ٶ�б��>0��ʱ�䣩
	
  Vm = (Speed.Vo + Speed.Vt) / 2.0f;	//�����е���ٶ�

  K = fabsf((2.0f * (Vm - Speed.Vo)) / (T * T));// �����е��ٶȼ���Ӽ��ٶ�
		
  Speed.INC_AccelTotalStep = (int32_t)((Speed.Vo * T) + ((K * T * T* T) / 6.0f));// �Ӽ�����Ҫ�Ĳ���
	
  Speed.Dec_AccelTotalStep = (int32_t)(((Speed.Vt + Speed.Vo) * T - Speed.INC_AccelTotalStep));// ��������Ҫ�Ĳ��� S = Vt * Time - S1
  
	/***************************************************************************/
	/* ���㹲��Ҫ�Ĳ�������У���ڴ��С�������ڴ�ռ����ٶȱ� */
  Speed.AccelTotalStep = Speed.Dec_AccelTotalStep + Speed.INC_AccelTotalStep;              // ������Ҫ�Ĳ��� 
  if( Speed.AccelTotalStep  % 2 != 0)     // ���ڸ���������ת�����������ݴ��������,���������1
    Speed.AccelTotalStep  += 1;
	
	/* �ж��ڴ泤�� */
	if(FORM_LEN < Speed.AccelTotalStep)
	{
		printf("FORM_LEN ���泤�Ȳ���\r\n���뽫 FORM_LEN �޸�Ϊ %d \r\n", Speed.AccelTotalStep);
		return false;
	}

	/***************************************************************************/
	/* �����һ����ʱ�� */
		
	/* ���ݵ�һ����ʱ����㣬��һ�����ٶȺ�����ʱ���� */
	/* ����λ��Ϊ0��ʱ������������ʱ��Ĺ�ϵʽ -> ����λ�ƺ�ʱ��Ĺ�ʽS = 1/2 * K * Ti^3  �ɵ� Ti=6 * 1 / K��1/3�η� */
  Ti = pow(6.0f * 1.0f / K, 1.0f / 3.0f); //������� Ti ʱ�䳣��
  Sumt += Ti;//�ۼ�ʱ�䳣��
	/* ����V=1/2*K*T^2,���Լ����һ�����ٶ� */
  DeltaV = 0.5f * K * Sumt * Sumt;
	/* �ڳ�ʼ�ٶȵĻ����������ٶ� */
  Speed.Form[0] = Speed.Vo + DeltaV;
  
	/***************************************************************************/
	/* ��С�ٶ��޷� */
  if(Speed.Form[0] <= MIN_SPEED)//�Ե�ǰ��ʱ��Ƶ�����ܴﵽ������ٶ�
    Speed.Form[0] = MIN_SPEED;
	
  /***************************************************************************/
	/* ����S���ٶȱ� */
  for(i = 1; i < Speed.AccelTotalStep; i++)
  {
		/* ����ʱ��������Ƶ�ʳɷ��ȵĹ�ϵ�����Լ����Ti,������ÿ�μ�����һ��ʱ�䣬���ڻ��۵���ǰʱ�� */
		Ti = 1.0f / Speed.Form[i-1];
    if(i < Speed.INC_AccelTotalStep)
    {
			/* �ۻ�ʱ�� */
      Sumt += Ti;
			/* �ٶȵı仯�� dV = 1/2 * K * Ti^2 */
      DeltaV = 0.5f * K * Sumt * Sumt;
			/* ���ݳ�ʼ�ٶȺͱ仯������ٶȱ� */
      Speed.Form[i] = Speed.Vo + DeltaV;
			/* Ϊ�˱�֤�����һ������ʹ��ʱ���Ͻ�����Ԥ�ڼ����ʱ��һ�£������һ�����д��� */
      if(i == Speed.INC_AccelTotalStep - 1)
        {Sumt  = fabsf(Sumt - T);}
    }
    /* �����ٶȼ��� */
    else
    {
			/* ʱ���ۻ� */
      Sumt += Ti;
			/* �����ٶ� */
      temp = fabsf(T - Sumt);
      DeltaV = 0.5f * K * temp * temp;
      Speed.Form[i] = Speed.Vt - DeltaV;
    }
  }
  return true;
}


/**
  * @brief  �ٶȾ���
  * @param  pScurve��S���߽ṹ��ָ��
  * @retval ��
	*	@note 	���ж���ʹ�ã�ÿ��һ���жϣ�����һ��
  */
void Speed_Decision(void)
{
	/* ������� */
  static __IO uint8_t i = 0;
  static __IO uint32_t index = 0;
  
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) != RESET)
	{
		/* �����ʱ���ж� */
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);
		
		/******************************************************************/
		/* ����Ϊһ���������� */
		i++;
    if(i == 2)
    {
			/* ������������������ */
      i = 0;
      /* �жϵ�ǰ��״̬ */
      switch(Stepper.status)
      {
        case ACCEL:
          if(Stepper.pos >= (Speed.AccelTotalStep - 1))
          {
            Stepper.status = UNIFORM;
            index -= 1;
            break;
          }
          /* ��ȡÿһ���Ķ�ʱ������ֵ */
          Stepper.pluse_time = (uint16_t)(T1_FREQ / Speed.Form[index] / 2U);
          /* ����λ���������� */
          index++;
          break;
        case DECEL:
          if(Stepper.pos >= (Speed.TotalStep - 1))
          {
            /* ����ֹͣ״̬������ٶȱ��ҹر����ͨ�� */
            HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
            memset((void*)Speed.Form, 0, sizeof(float) * FORM_LEN);
            index = 0;
            Stepper.status = STOP;
            break;
          }
          /* ��ȡÿһ���Ķ�ʱ������ֵ */
          Stepper.pluse_time = (uint16_t)(T1_FREQ / Speed.Form[index] / 2U);
          /* ����λ���������� */
          index--;
          break;
        case UNIFORM:
          if(Stepper.pos >= Speed.DecPoint)
          {
            Stepper.status = DECEL;
          }
          break;
      }
      /* ����λ���������� */
      Stepper.pos++;
    }
		/**********************************************************************/
		/* ��ȡ��ǰ��������ֵ */
		uint32_t tim_count = __HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		/* ������һ��ʱ�� */
		uint16_t tmp = tim_count + Stepper.pluse_time;
		/* ���ñȽ�ֵ */
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, tmp);
	}
}

/**
  * @brief  �������S���߼Ӽ���
  * @param  axis����Ҫ���Ƶ�����
  * @param  start_speed�������ٶȣ���λ��ת/����
	* @param  end_speed��Ŀ���ٶȣ���λ��ת/����
	* @param  acc_time������ʱ�䣬��λ����
	* @param  step���˶���������λ�������迼��ϸ�֣�
  * @retval true������
  * @retval false���������ô�����ٶȱ�ռ䲻��
	*	@note   ��
  */
bool Stepper_Move_S(int16_t start_speed, int16_t end_speed, float acc_time, int32_t step)
{
  /* �ж��Ƿ������˶� */
  if(Stepper.status != STOP)
    return false;
  
	/* ������� */
	if(CalcSpeed(start_speed, end_speed, acc_time) != true)
    return false;
  
  if(step < 0)
  {
    step = -step;
    MOTOR_DIR(CCW);
  }
  else
  {
    MOTOR_DIR(CW);
  }
  
  /* ������ٵ㣬���˶�����С���������ٶ�ʱ�޷����s�Ӽ��� */
  if(step >= Speed.AccelTotalStep * 2)
  {
    Speed.TotalStep = step;
    Speed.DecPoint = Speed.TotalStep - Speed.AccelTotalStep;
  }
  else
  {
    printf("�Ӽ��ٲ������ô���\r\n");
    return false;
  }
  
	/* ��ʼ���ṹ�� */
  memset(&Stepper, 0, sizeof(Stepper_Typedef));
  
  /* ��ʼ�����״̬ */
	Stepper.status = ACCEL;
	Stepper.pos = 0;
  
  /* �����һ���Ķ�ʱ������ */
  Stepper.pluse_time = (uint16_t)(T1_FREQ / Speed.Form[0] / 2U);

  /* ��������� */
  __HAL_TIM_SET_COUNTER(&TIM_TimeBaseStructure, 0);
  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, Stepper.pluse_time);
  /* ʹ�ܶ�ʱ��ͨ�� */
	TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);
	/* ��������Ƚ��ж� */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
  
  return true;
}

/**
  * @brief  ��ʱ���жϷ�����
  * @param  ��
  * @retval ��
	*	@note   ��
  */
void MOTOR_PUL_IRQHandler(void)
{
  Speed_Decision();
}
