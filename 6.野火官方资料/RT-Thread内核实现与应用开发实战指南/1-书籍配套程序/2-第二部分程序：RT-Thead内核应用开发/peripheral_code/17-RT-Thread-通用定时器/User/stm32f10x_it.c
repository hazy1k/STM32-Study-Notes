/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "board.h" 
#include "rtthread.h"

/*
*************************************************************************
*                               ����
*************************************************************************
*/

extern volatile uint32_t time_num;
/* �����ź������ƿ� */
extern rt_sem_t test_sem;

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void  GENERAL_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( GENERAL_TIM, TIM_IT_Update) != RESET ) 
	{	
		time_num++;
		TIM_ClearITPendingBit(GENERAL_TIM , TIM_FLAG_Update);  
		if(time_num == 1000)
		{
			time_num = 0;
			rt_sem_release(test_sem);   //�ͷŶ�ֵ�ź���
		}
	}		 	
}

