/**
  ******************************************************************************
  * @file    rx_data_queue.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   ���λ������������ڽ����ⲿ����ʱ��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� IOT STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./usart/rx_data_queue.h"


RingBuff_t ringBuff;//����һ��ringBuff�Ļ�����
void RingBuff_Init(void)
  {
    //��ʼ�������Ϣ
     ringBuff.Head = 0;
     ringBuff.Tail = 0;
     ringBuff.Lenght = 0;
  }
	
uint8_t Read_RingBuff(uint8_t *rData)
{
	uint8_t i;
	 if(ringBuff.Lenght == 0)//�жϷǿ�
	{
	  return FALSE;
	}
	for(i=ringBuff.Lenght;i>0;i--)
	{
	 *rData++ = ringBuff.Ring_Buff[ringBuff.Head];//�Ƚ��ȳ�FIFO���ӻ�����ͷ��
	// printf("%d\n",*rData);
	 ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
	 ringBuff.Lenght--;
	}
	 return TRUE;
}


uint8_t Write_RingBuff(uint8_t data)
{
		if(ringBuff.Lenght >= RINGBUFF_LEN) //�жϻ������Ƿ�����
	{
		return FALSE;
	}
	ringBuff.Ring_Buff[ringBuff.Tail]=data;
	ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
	ringBuff.Lenght++;
	return TRUE;
}
