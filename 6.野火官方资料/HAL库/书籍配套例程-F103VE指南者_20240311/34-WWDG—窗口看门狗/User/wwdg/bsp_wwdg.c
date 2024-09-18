 
#include "bsp_wwdg.h"   

WWDG_HandleTypeDef WWDG_Handle;

// WWDG �ж����ȼ���ʼ��
static void WWDG_NVIC_Config(void)
{
  HAL_NVIC_SetPriority(WWDG_IRQn,0,0);
	HAL_NVIC_EnableIRQ(WWDG_IRQn);
}

/* WWDG ���ú���
 * tr ���ݼ���ʱ����ֵ�� ȡֵ��ΧΪ��0x7f~0x40
 * wr ������ֵ��ȡֵ��ΧΪ��0x7f~0x40
 * prv��Ԥ��Ƶ��ֵ��ȡֵ������
 *      @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1(36MHZ)/4096)/1
 *      @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1(36mhz)/4096)/2
 *      @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1(36mhz)/4096)/4
 *      @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1(36mhz)/4096)/8
 */
void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{	
	// ���� WWDG ʱ��
	__HAL_RCC_WWDG_CLK_ENABLE();
	// ����WWDG�ж����ȼ�
	WWDG_NVIC_Config();
	// ����WWDG������Ĵ�������ַ
	WWDG_Handle.Instance = WWDG;
	// ����Ԥ��Ƶ��ֵ
	WWDG_Handle.Init.Prescaler = prv;
	// �����ϴ���ֵ
	WWDG_Handle.Init.Window = wr;	
	// ���ü�������ֵ
	WWDG_Handle.Init.Counter = tr;
	// ʹ����ǰ�����ж�
	WWDG_Handle.Init.EWIMode = WWDG_EWI_ENABLE;
	// ��ʼ��WWDG
	HAL_WWDG_Init(&WWDG_Handle);	
}

// ι��
void WWDG_Feed(void)
{
	// ι����ˢ�µݼ���������ֵ�����ó����WDG_CNT=0X7F
	HAL_WWDG_Refresh(&WWDG_Handle);
}

/*********************************************END OF FILE**********************/
