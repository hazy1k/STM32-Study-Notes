#include "stm32f10x.h"

int main (void)
{	
	// ����GPIOB�˿�ʱ��
	RCC_APB2ENR |= (1<<3);
	// ��տ���PB0�Ķ˿�λ
	GPIOB_CRL &= ~(0x0F << (4*0));
	// ����PB0λͨ���������
	GPIOB_CRL |= (1<<4*0);
	// ʹPB0���Ϊ�͵�ƽ
	GPIOB_ODR |= (0<<0);
	while(1)
	{
		
	}
}

void SystemInit(void)
{
	// ������Ϊ�գ�Ŀ����Ϊ��ƭ��������������
}
