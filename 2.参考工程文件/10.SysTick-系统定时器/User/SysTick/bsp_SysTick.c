#include "bsp_SysTick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO u32 TimingDelay; // ��ʱ������
 
// ��ζ�ʱ����ʼ������
void SysTick_Init(void)
{
	/* 
	SystemFrequency / 1000    1ms�ж�һ��
	SystemFrequency / 100000	 10us�ж�һ��
	SystemFrequency / 1000000 1us�ж�һ��
	*/
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if (SysTick_Config(SystemCoreClock / 100000))	// ST3.5.0��汾
	{  
		while (1);
	}
}


// couter ��1��ʱ�� ���� 1/systick_clk
// ��counter �� reload ��ֵ��С��0��ʱ��Ϊһ��ѭ��������������ж���ִ���жϷ������
// ͬʱ CTRL �� countflag λ����1
// ��һ��ѭ����ʱ��Ϊ reload * (1/systick_clk)
void SysTick_Delay_Us( __IO uint32_t us) // us����ʱ
{
	uint32_t i;
	SysTick_Config(SystemCoreClock / 1000000);
	
	for(i = 0; i < us; i++)
	{
		// ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1	
		while( !((SysTick->CTRL)&(1<<16)) );
	}
	// �ر�SysTick��ʱ��
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Delay_Ms( __IO uint32_t ms) // ms����ʱ
{
	uint32_t i;	
	SysTick_Config(SystemCoreClock / 1000);
	
	for(i = 0; i < ms; i++)
	{
		// ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
		// ����1ʱ����ȡ��λ����0
		while( !((SysTick->CTRL)&(1<<16)) );
	}

	// �ر�SysTick��ʱ��
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}