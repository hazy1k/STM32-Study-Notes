#include "stm32f10x.h"
#include "usart.h"
#include "crc.h"

extern __IO uint32_t CRCValue;

int main(void)
{
	uint8_t i = 0;
	USART_Config();
	CRC_Config();
	/* Compute the CRC of "DataBuffer" */
	for(i=0; i<BUFFER_SIZE; i++ ) 
	{
		// �ԸüĴ�������д����ʱ����Ϊ����Ĵ�������������Ҫ����CRC����������ݡ�
		// �ԸüĴ������ж�����ʱ��������һ��CRC����Ľ����
		// ÿһ��д�����ݼĴ��������������ǰһ��CRC���������¼����������(������32λ��
		// ����CRC���㣬���������ֽڵؼ���)��
		CRCValue = CRC_CalcBlockCRC((uint32_t *)DataBuffer, BUFFER_SIZE);
		printf("\r\n32-bit CRC У����Ϊ:0X%X\r\n", CRCValue);
	}
	printf("\r\nCRC(ѭ������У��)���Գɹ�\r\n");
	while(1);
}
