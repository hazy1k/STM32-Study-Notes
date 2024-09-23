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


void  ADVANCE_TIM_IRQHandler (void)
{
	if ( TIM_GetITStatus( ADVANCE_TIM, TIM_IT_Update) != RESET ) 
	{	
		time_num++;
		TIM_ClearITPendingBit(ADVANCE_TIM , TIM_FLAG_Update);  		 
	}		 	
	if(time_num == 1000)
	{
		time_num = 0;
		rt_sem_release(test_sem);   //�ͷŶ�ֵ�ź���
	}
}
