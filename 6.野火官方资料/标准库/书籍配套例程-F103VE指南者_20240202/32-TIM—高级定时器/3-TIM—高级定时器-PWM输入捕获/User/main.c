
// TIM���߼���ʱ��-PWM���벶��Ӧ�ã�ͨ�ö�ʱ������PWM�����߼���ʱ���򲶻����PWM�����������ں�ռ�ձ�

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_AdvanceTim.h" 
#include "bsp_GeneralTim.h" 
#include "bsp_usart.h"

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	/* ͨ�ö�ʱ����ʼ������������PWM�ź� */
	GENERAL_TIM_Init();
	
	/* �߼���ʱ����ʼ�� ���û�����PWM�ź�*/
	ADVANCE_TIM_Init();
	
  while(1)
  {      
  }
}
/*********************************************END OF FILE**********************/

