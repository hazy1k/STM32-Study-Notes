
// ͨ�ö�ʱ��TIMx,x[2,3,4]ͬ��Ӧ��
#include "stm32f10x.h"
#include "./tim/bsp_general_tim.h"


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
  /* ��ʼ��ͨ�ö�ʱ������ͬ��PWM��� */
	/* ʵ�֣�TIM2�����¼�����ʱ���ʹ����ź�����TIM3���� */
	/*       TIM3�����¼�����ʱ���ʹ����ź�����TIM4���� */
	TIMx_Configuration();
	
  while(1)
  {
        
  }
}
/*********************************************END OF FILE**********************/

