#include "stm32f10x.h" 

int main(void)
{	
	// ����LED1
	// 1.����GPIOB �˿�ʱ��
	RCC_APB2ENR |= (1<<3);
	// 2.��տ���PB0�Ķ˿�λ
	GPIOB_CRL &= ~( 0x0F<< (4*0));	
	// 3.����PB0Ϊͨ������������ٶ�Ϊ10M
	GPIOB_CRL |= (1<<4*0);
	// 4.ʹPB0����͵�ƽ - ����LED1
	GPIOB_ODR &= ~(1<<0);
	
	// ����LED2
	// 1.��տ���PB1�Ķ˿�λ
	GPIOB_CRL &= ~( 0x0F<< (4*1));	
	// 2.����PB1Ϊͨ������������ٶ�Ϊ10M
	GPIOB_CRL |= (1<<4*1);
	// 3.PB1����͵�ƽ - ����LED2
	GPIOB_ODR &= ~(1<<1);
	
	// ����LED3
	// 1.��տ���PB5�Ķ˿�λ
	GPIOB_CRL &= ~( 0x0F << (4*5));
	// 2.����PB5Ϊͨ������������ٶ�Ϊ10M
	GPIOB_CRL |= (1<<4*5);
	// 3.PB5����͵�ƽ
	GPIOB_ODR &= ~(1<<5);
	while(1)
	{
		
	}
}

// ����Ϊ�գ�Ŀ����Ϊ��ƭ��������������
void SystemInit(void)
{	
	
}
