/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "board.h" 
/* RT-Thread���ͷ�ļ� */
#include <rthw.h>
#include <rtthread.h>
/*
 * ��������SDIO_IRQHandler
 * ����  ����SDIO_ITConfig(���������������sdio�ж�	��
 			���ݴ������ʱ�����ж�
 * ����  ����		 
 * ���  ����
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */
	
		/* �����ж� */
	rt_interrupt_enter();

  SD_ProcessIRQSrc();

	/* �뿪�ж� */
	rt_interrupt_leave();
	

}



