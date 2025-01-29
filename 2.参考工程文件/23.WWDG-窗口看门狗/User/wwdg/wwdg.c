#include "wwdg.h"

static void WWDG_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
	tr���ݼ���������ֵ��ȡֵ��ΧΪ��0x7f~0x40
	wr������ֵ��ȡֵ��ΧΪ��0x7f~0x40
	prv��Ԥ��Ƶֵ
*/
void WWDG_Mode_Init(uint8_t tr, uint8_t wr, uint32_t prv)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_SetCounter(tr); // �ݼ���������ֵ
	WWDG_SetWindowValue(prv); // Ԥ��Ƶֵ
	WWDG_SetPrescaler(wr); // �����ϴ���ֵ
	WWDG_Enable(WWDG_CNT); // ���ü�������ֵ
	WWDG_ClearFlag();
	WWDG_NVIC_Init();
	WWDG_EnableIT();
}

void WWDG_Feed(void)
{
	WWDG_SetCounter(WWDG_CNT);
}
