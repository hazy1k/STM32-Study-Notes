/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "board.h" 

/* RT-Thread���ͷ�ļ� */
#include <rthw.h>
#include <rtthread.h>

extern uint32_t TimeDisplay;

/* �����ź������ƿ� */
extern rt_sem_t test_sem;

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	  {
	    /* Clear the RTC Second interrupt */
	    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	    /* Enable time update */
	    rt_sem_release(test_sem);   //�ͷŶ�ֵ�ź���
	
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	  }
}
