#include "stm32f10x.h" 

int main(void)
{	
	// 点亮LED1
	// 1.开启GPIOB 端口时钟
	RCC_APB2ENR |= (1<<3);
	// 2.清空控制PB0的端口位
	GPIOB_CRL &= ~( 0x0F<< (4*0));	
	// 3.配置PB0为通用推挽输出，速度为10M
	GPIOB_CRL |= (1<<4*0);
	// 4.使PB0输出低电平 - 点亮LED1
	GPIOB_ODR &= ~(1<<0);
	
	// 点亮LED2
	// 1.清空控制PB1的端口位
	GPIOB_CRL &= ~( 0x0F<< (4*1));	
	// 2.配置PB1为通用推挽输出，速度为10M
	GPIOB_CRL |= (1<<4*1);
	// 3.PB1输出低电平 - 点亮LED2
	GPIOB_ODR &= ~(1<<1);
	
	// 点亮LED3
	// 1.清空控制PB5的端口位
	GPIOB_CRL &= ~( 0x0F << (4*5));
	// 2.配置PB5为通用推挽输出，速度为10M
	GPIOB_CRL |= (1<<4*5);
	// 3.PB5输出低电平
	GPIOB_ODR &= ~(1<<5);
	while(1)
	{
		
	}
}

// 函数为空，目的是为了骗过编译器不报错
void SystemInit(void)
{	
	
}
