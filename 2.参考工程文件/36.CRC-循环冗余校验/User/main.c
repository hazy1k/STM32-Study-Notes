/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   CRC���㡣
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ָ���߿�����
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./crc/bsp_crc.h"

extern __IO uint32_t CRCValue;

/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void)
{
	uint8_t i = 0;
		
	/* USART1 config 115200 8-N-1 */
	USART_Config();
	
	/* ʹ��CRCʱ�� */
	CRC_Config();
	
	printf("\r\n ����һ�� CRC(ѭ������У��)ʵ�� \r\n");
	
	/* Compute the CRC of "DataBuffer" */
	for(i=0; i<BUFFER_SIZE; i++ ) 
	{
		//�ԸüĴ�������д����ʱ����Ϊ����Ĵ�������������Ҫ����CRC����������ݡ�
		//�ԸüĴ������ж�����ʱ��������һ��CRC����Ľ����
		//ÿһ��д�����ݼĴ��������������ǰһ��CRC���������¼����������(������32λ��
		//����CRC���㣬���������ֽڵؼ���)��
		CRCValue = CRC_CalcBlockCRC((uint32_t *)DataBuffer, BUFFER_SIZE);
		printf("\r\n32-bit CRC У����Ϊ:0X%X\r\n", CRCValue);
	}
	printf("\r\nCRC(ѭ������У��)���Գɹ�\r\n");

	for(;;)
	{
		
	}
}
