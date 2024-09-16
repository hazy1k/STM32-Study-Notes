/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��ȡоƬ��ID
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

// ͨ��ADC1ͨ��16��ȡоƬ�ڲ��¶ȣ���ͨ�����ڴ�ӡ�����Դ��ڵ���������ʾ

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./chipid/bsp_chipid.h"

extern uint32_t ChipUniqueID[3];

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{	
	/* USART1 config 115200 8-N-1 */
	USART_Config();

	/* to get the chipid and put it in ChipUniqueID[3] */
	Get_ChipID();	
	
	/* printf the chipid */
	printf("\r\nоƬ��ΨһIDΪ: 0x%08X-%08X-%08X\r\n",
	        ChipUniqueID[0],ChipUniqueID[1],ChipUniqueID[2]);
	
	/* printf the flash memory amount */
	printf("\r\nоƬflash������Ϊ: %dK \r\n", *(__IO u16 *)(0X1FFFF7E0));

	for(;;)
	{
		
	}
}
