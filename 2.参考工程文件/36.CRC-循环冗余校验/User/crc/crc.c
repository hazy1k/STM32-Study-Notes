#include "crc.h"

__IO uint32_t CRCValue = 0; // ���ڴ�Ų�����CRCУ��ֵ

void CRC_Config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE); // ʹ��CRCʱ��
}
