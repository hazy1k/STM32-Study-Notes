#include "iwdg.h"

// ǰ��֪ʶ��iwdg��ʱʱ�����
/*
	T = prv/40*rlv (s)
	prvΪԤ��Ƶ��ֵ��prvȡֵ������[4,8,16,32,64,128,256]
	rlvΪ���ؼĴ�����ֵ��ȡֵ��ΧΪ��0~0xFFF
	������T = 64/40*625 = 1s
*/
void IWDG_Config(uint8_t prv, uint16_t rlv)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); // ʹ�ܼĴ�����д
	IWDG_SetPrescaler(prv); // ����Ԥ��Ƶ��
	IWDG_SetReload(rlv); // �������ؼĴ���
	IWDG_Enable(); // ʹ��IWDG
}

void IWDG_Feed(void)
{
	IWDG_ReloadCounter(); // ���ؼ�����
}
