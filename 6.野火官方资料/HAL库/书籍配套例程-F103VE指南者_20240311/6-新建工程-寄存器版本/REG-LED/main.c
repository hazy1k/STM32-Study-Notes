
#if 0

#include <rge52.h>

sbit LED  = P0^0

void main(void)
{
	P0 = 0XFE;     // ���߲���
	
	LED = 0;       // λ����
}

#endif



#include "stm32f10x.h"

int main (void)
{
	// �� GPIOB �˿ڵ�ʱ��
	*( unsigned int * )0x40021018 |=  ( (1) << 3 );	

	// ����IO��Ϊ���
	*( unsigned int * )0x40010C00 |=  ( (1) << (4*0) );
	
	// ���� ODR �Ĵ���
	*( unsigned int * )0x40010C0C &= ~(1<<0);
}


// ��λ   |=   ����0 &=~
//



// 1����������������LED��

// 2��дһ���򵥵���ʱ�������õ���˸


// 72M��HSI = 8M��

void SystemInit(void)
{
	// ������Ϊ�գ�Ŀ����Ϊ��ƭ��������������
}

