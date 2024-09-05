/**
  ******************************************************************************
  * @file    rx_data_queue.c
  * @author  fire
  * @version V1.0
  * @date    2015-01-xx
  * @brief   环形缓冲区，适用于接收外部数据时用作缓冲
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 IOT STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./usart/rx_data_queue.h"


RingBuff_t ringBuff;//创建一个ringBuff的缓冲区
void RingBuff_Init(void)
  {
    //初始化相关信息
     ringBuff.Head = 0;
     ringBuff.Tail = 0;
     ringBuff.Lenght = 0;
  }
	
uint8_t Read_RingBuff(uint8_t *rData)
{
	uint8_t i;
	 if(ringBuff.Lenght == 0)//判断非空
	{
	  return FALSE;
	}
	for(i=ringBuff.Lenght;i>0;i--)
	{
	 *rData++ = ringBuff.Ring_Buff[ringBuff.Head];//先进先出FIFO，从缓冲区头出
	// printf("%d\n",*rData);
	 ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//防止越界非法访问
	 ringBuff.Lenght--;
	}
	 return TRUE;
}


uint8_t Write_RingBuff(uint8_t data)
{
		if(ringBuff.Lenght >= RINGBUFF_LEN) //判断缓冲区是否已满
	{
		return FALSE;
	}
	ringBuff.Ring_Buff[ringBuff.Tail]=data;
	ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//防止越界非法访问
	ringBuff.Lenght++;
	return TRUE;
}
