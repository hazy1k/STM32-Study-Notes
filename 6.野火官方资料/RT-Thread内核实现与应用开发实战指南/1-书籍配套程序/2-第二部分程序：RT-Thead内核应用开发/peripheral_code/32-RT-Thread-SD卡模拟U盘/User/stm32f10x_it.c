/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "./sdio/bsp_sdio_sdcard.h"	

#include "usb_lib.h"
#include "usb_istr.h"

#include "board.h"
#include "rtthread.h"

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

/*
 * ��������USB_LP_CAN1_RX0_IRQHandler
 * ����  ��USB�����ȼ��ж�����
 * ����  ����		 
 * ���  ����
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  /* �����ж� */
	rt_interrupt_enter();

  USB_Istr();

	/* �뿪�ж� */
	rt_interrupt_leave();
}

/*
 * ��������USB_HP_CAN1_TX_IRQHandler
 * ����  ��USB�����ȼ��ж����� �����ж�
 * ����  ����		 
 * ���  ����
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
  /* �����ж� */
	rt_interrupt_enter();

  CTR_HP();
		
	/* �뿪�ж� */
	rt_interrupt_leave();
}
