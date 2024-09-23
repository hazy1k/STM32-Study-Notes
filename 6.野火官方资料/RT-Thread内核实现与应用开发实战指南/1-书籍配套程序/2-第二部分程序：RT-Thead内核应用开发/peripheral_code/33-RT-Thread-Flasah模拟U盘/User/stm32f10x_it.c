/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"

#include "usb_lib.h"
#include "usb_istr.h"

#include "board.h"
#include "rtthread.h"



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
