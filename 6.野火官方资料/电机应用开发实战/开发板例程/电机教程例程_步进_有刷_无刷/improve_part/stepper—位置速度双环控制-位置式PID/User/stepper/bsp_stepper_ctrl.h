#ifndef __BSP_STEP_MOTOR_CTRL_H
#define	__BSP_STEP_MOTOR_CTRL_H

#include "./stepper/bsp_stepper_init.h"
#include "./Encoder/bsp_encoder.h"

/*�궨��*/
/*******************************************************/
#define TIM_STEP_FREQ     (SystemCoreClock/TIM_PRESCALER) // Ƶ��ftֵ

/*�����Ȧ����*/
#define STEP_ANGLE				1.8f                 //��������Ĳ���� ��λ����
#define FSPR              (360.0f/STEP_ANGLE)  //���������һȦ����������

#define MICRO_STEP        32          				 //ϸ����ϸ���� 
#define SPR               (FSPR*MICRO_STEP)    //ϸ�ֺ�һȦ����������

#define PULSE_RATIO       (float)(SPR/ENCODER_TOTAL_RESOLUTION)//���������Ȧ���������������Ȧ����ı�ֵ

#define SAMPLING_PERIOD   50                   //PID����Ƶ�ʣ���λHz

#define MOVE_CTRL         0.1f                   //�����ٶȻ�������
#define TARGET_DISP       100                   //��������˶�ʱ��Ŀ��Ȧ������λ��ת

#define TARGET_SPEED_MAX  200                // Ŀ���ٶȵ����ֵ

typedef struct {
  unsigned char stepper_dir : 1;               //�����������
  unsigned char stepper_running : 1;           //�����������״̬
  unsigned char MSD_ENA : 1;                   //������ʹ��״̬
}__SYS_STATUS;


void MSD_ENA(int NewState);
void Set_Stepper_Stop(void);
void Set_Stepper_Start(void);
void Stepper_Ctrl(void);

#endif /* __STEP_MOTOR_CTRL_H */
