#include "stm32f10x.h" 

int main(void)
{	
	// ����GPIOB �˿�ʱ��
	RCC_APB2ENR |= (1<<3);

	//��տ���PB0�Ķ˿�λ
	GPIOB_CRL &= ~( 0x0F<< (4*0));	
	// ����PB0Ϊͨ������������ٶ�Ϊ10M
	GPIOB_CRL |= (1<<4*0);

	// PB0 ��� �͵�ƽ
	GPIOB_ODR &= ~(1<<0);
	
	//��տ���PB1�Ķ˿�λ
	GPIOB_CRL &= ~( 0x0F<< (4*1));	
	// ����PB1Ϊͨ������������ٶ�Ϊ10M
	GPIOB_CRL |= (1<<4*1);

	// PB1 ��� �͵�ƽ
	GPIOB_ODR &= ~(1<<1);
	
	//��տ���PB5�Ķ˿�λ
	GPIOB_CRL &= ~( 0x0F<< (4*5));	
	// ����PB5Ϊͨ������������ٶ�Ϊ10M
	GPIOB_CRL |= (1<<4*5);

	// PB5 ��� �͵�ƽ
	GPIOB_ODR &= ~(1<<5);
	while(1);
}



// ����Ϊ�գ�Ŀ����Ϊ��ƭ��������������
void SystemInit(void)
{	
	
}
