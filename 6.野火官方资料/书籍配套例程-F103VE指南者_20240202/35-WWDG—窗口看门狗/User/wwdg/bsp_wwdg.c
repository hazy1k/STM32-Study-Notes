 
#include "bsp_wwdg.h"   



// WWDG �ж����ȼ���ʼ��
static void WWDG_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	// ���õݼ���������ֵ
	WWDG_SetCounter( tr );
	
	// ����Ԥ��Ƶ����ֵ
	WWDG_SetPrescaler( prv );
	
	// �����ϴ���ֵ
	WWDG_SetWindowValue( wr );
	
	// ���ü�������ֵ��ʹ��WWDG
	WWDG_Enable(WWDG_CNT);	
	
	// �����ǰ�����жϱ�־λ
	WWDG_ClearFlag();	
	// ����WWDG�ж����ȼ�
	WWDG_NVIC_Config();	
	// ��WWDG �ж�
	WWDG_EnableIT();
}

// ι��
void WWDG_Feed(void)
{
	// ι����ˢ�µݼ���������ֵ�����ó����WDG_CNT=0X7F
	WWDG_SetCounter( WWDG_CNT );
}


/*********************************************END OF FILE**********************/
